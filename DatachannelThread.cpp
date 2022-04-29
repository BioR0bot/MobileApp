#include "DatachannelThread.h"
#include "AppDataManager.h"
#include "Utils.h"

#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>

DatachannelThread::DatachannelThread(std::shared_ptr<AppDataManager> &appDataManager) :
    mIsStopped(true),
    mAppDataManager(appDataManager),
    mNeedToRestart(false)
{

}

DatachannelThread::~DatachannelThread()
{
    Stop();
}

void DatachannelThread::Start()
{
    Stop();
    mIsStopped.store(false);
    mThread = std::thread(&DatachannelThread::ThreadFunction, this);
}

void DatachannelThread::Stop()
{
    mIsStopped.store(true);
    if(mThread.joinable())
    {
        mThread.join();
    }
}

void DatachannelThread::RestartIfNeed()
{
    if(mNeedToRestart.load())
    {
        mNeedToRestart.store(false);
        Start();
    }
}

void DatachannelThread::ThreadFunction()
{
    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    rtc::Configuration peerConnectConfig;
    const rtc::IceServer turnServer("handsome.edgeconsult.me", 3478, "guest", "somepassword");
    peerConnectConfig.iceServers.emplace_back(turnServer);

    rtc::InitLogger(rtc::LogLevel::None);

    rtc::WebSocket::Configuration websockConfig;
    websockConfig.disableTlsVerification = true;

    auto ws = std::make_shared<rtc::WebSocket>(websockConfig);

    ConnectStatus connectStatus = ConnectStatus::UNDEFINED;

    ws->onOpen([&]()
    {
        connectStatus = ConnectStatus::CONNECTED;
    });

    ws->onError([&](std::string s)
    {
        connectStatus = ConnectStatus::ERROR;
    });

    ws->onClosed([&]()
    {
        connectStatus = ConnectStatus::CLOSED;
    });

    ws->onMessage([&](std::variant<rtc::binary, std::string> data)
    {
        if (!std::holds_alternative<std::string>(data))
        {
            return;
        }

        nlohmann::json message = nlohmann::json::parse(std::get<std::string>(data));

        auto it = message.find("id");
        if (it == message.end())
        {
            return;
        }
        std::string id = it->get<std::string>();

        it = message.find("type");
        if (it == message.end())
        {
            return;
        }
        std::string type = it->get<std::string>();

        std::shared_ptr<rtc::PeerConnection> pc;
        if (auto jt = peerConnectionMap.find(id); jt != peerConnectionMap.end())
        {
            pc = jt->second;
        }
        else if (type == "offer")
        {
            //std::cout << "Answering to " + id << std::endl;
            pc = CreatePeerConnection(peerConnectConfig, ws, id, mAppDataManager, peerConnectionMap, dataChannelMap);
        }
        else
        {
            return;
        }

        if (type == "offer" || type == "answer")
        {
            auto sdp = message["description"].get<std::string>();
            pc->setRemoteDescription(rtc::Description(sdp, type));
        }
        else if (type == "candidate")
        {
            auto sdp = message["candidate"].get<std::string>();
            auto mid = message["mid"].get<std::string>();
            pc->addRemoteCandidate(rtc::Candidate(sdp, mid));
        }
    });

    const std::string externalId = mAppDataManager->mExternalId;
    const std::string localId = mAppDataManager->mLocalId;

    const std::string url = "wss://handsome.edgeconsult.me/" + localId;

    try
    {
        ws->open(url);
    }
    catch (...)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        mNeedToRestart.store(true);
        mAppDataManager->SetIsConnectToRobot(false);
        return;
    }

    WaitForWebSockConnection(5000, connectStatus);

    if(connectStatus != ConnectStatus::CONNECTED)
    {
        mNeedToRestart.store(true);
        mAppDataManager->SetIsConnectToRobot(false);
        return;
    }

     auto pc = CreatePeerConnection(peerConnectConfig, ws, externalId, mAppDataManager, peerConnectionMap, dataChannelMap);

     const std::string label = "test";
     auto dc = pc->createDataChannel(label);

     dc->onOpen([externalId, localId, wdc = make_weak_ptr(dc)]()
     {
         //std::cout << "DataChannel from " << id << " open" << std::endl;
         if (auto dc = wdc.lock())
         {
             dc->send("Hello from " + localId);
         }
     });

     dc->onClosed([externalId]()
     {
         //std::cout << "DataChannel from " << id << " closed" << std::endl;
     });


     dc->onMessage([externalId, &appDataManager = mAppDataManager](std::variant<rtc::binary, std::string> data)
     {
         if (std::holds_alternative<std::string>(data))
         {
             appDataManager->SetLogData(std::get<std::string>(data));
             //std::cout << "Message from " << id << " received: " << std::get<std::string>(data) << std::endl;
         }
         else
         {
             std::vector<unsigned char> buffer = reinterpret_cast<std::vector<unsigned char> &&>(std::get<rtc::binary>(data));
             appDataManager->SetReceivedBuffer(buffer);
             //std::cout << "Binary message from " << id << " received, size=" << std::get<rtc::binary>(data).size() << std::endl;
         }
     });

     dataChannelMap.emplace(externalId, dc);


     int millisecPast = 0;
     const int millisecStep = 10;

     while(!mIsStopped.load())
     {
         if(connectStatus != ConnectStatus::CONNECTED)
         {
             std::this_thread::sleep_for(std::chrono::milliseconds(500));
             mNeedToRestart.store(true);
             mAppDataManager->SetIsConnectToRobot(false);
             return;
         }

         if(dc->isOpen())
         {
             millisecPast = 0;

             try
             {
                 dc->send("1");
             }
             catch (...)
             {
                 mNeedToRestart.store(true);
                 mAppDataManager->SetIsConnectToRobot(false);
                 return;
             }

             mAppDataManager->SetIsConnectToRobot(true);
         }
         else
         {
             millisecPast += millisecStep;
             if(millisecPast > 4000)
             {
                 mNeedToRestart.store(true);
                 mAppDataManager->SetIsConnectToRobot(false);
                 return;
             }
         }

         std::this_thread::sleep_for(std::chrono::milliseconds(millisecStep));
     }
}


void DatachannelThread::WaitForWebSockConnection(int millisecWait, const ConnectStatus& connectStatus) const
{
    int millisecPast = 0;
    int millisecStep = 10;

    while(!mIsStopped.load() && connectStatus == ConnectStatus::UNDEFINED && millisecPast < millisecWait)
    {
        millisecPast += millisecStep;

        std::this_thread::sleep_for(std::chrono::milliseconds(millisecStep));
    }
}

std::shared_ptr<rtc::PeerConnection> DatachannelThread::CreatePeerConnection(const rtc::Configuration &config, std::weak_ptr<rtc::WebSocket> wws, std::string id, std::shared_ptr<AppDataManager> appDataManager,
                                                                             std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>>& peerConnectionMap,
                                                                             std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>>& dataChannelMap)
{
    auto pc = std::make_shared<rtc::PeerConnection>(config);

    pc->onStateChange([](rtc::PeerConnection::State state)
    {
        //std::cout << "State: " << state << std::endl;
    });

    pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state)
    {
        //std::cout << "Gathering State: " << state << std::endl;
    });

    pc->onLocalDescription([wws, id](rtc::Description description)
    {
        nlohmann::json message =
        {
            {"id", id}, {"type", description.typeString()}, {"description", std::string(description)}
        };

        if (auto ws = wws.lock())
        {
            ws->send(message.dump());
        }
    });

    pc->onLocalCandidate([wws, id](rtc::Candidate candidate)
    {
        nlohmann::json message = {{"id", id},
                                 {"type", "candidate"},
                                 {"candidate", std::string(candidate)},
                                 {"mid", candidate.mid()}};

        if (auto ws = wws.lock())
        {
            ws->send(message.dump());
        }
    });

    pc->onDataChannel([id, appDataManager, &dataChannelMap](std::shared_ptr<rtc::DataChannel> dc)
    {
        dc->onOpen([wdc = make_weak_ptr(dc), appDataManager]()
        {
            if (auto dc = wdc.lock())
            {
                dc->send("Hello from " + appDataManager->mLocalId);
            }
        });

        dc->onClosed([id]()
        {
            //std::cout << "DataChannel from " << id << " closed" << std::endl;
        });

        dc->onMessage([id, appDataManager](std::variant<rtc::binary, std::string> data)
        {
            if (std::holds_alternative<std::string>(data))
            {
                appDataManager->SetLogData(std::get<std::string>(data));
                //std::cout << "Message from " << id << " received: " << std::get<std::string>(data) << std::endl;
            }
            else
            {
                std::vector<unsigned char> buffer = reinterpret_cast<std::vector<unsigned char> &&>(std::get<rtc::binary>(data));
                appDataManager->SetReceivedBuffer(buffer);
                //std::cout << "Binary message from " << id << " received, size=" << std::get<rtc::binary>(data).size() << std::endl;
            }
        });

        dataChannelMap.emplace(id, dc);
    });

    peerConnectionMap.emplace(id, pc);
    return pc;
}
