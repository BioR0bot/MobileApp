#include "DatachannelThread.h"
#include "AppDataManager.h"

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
    mAppDataManager->SetIsConnectToRobot(!mNeedToRestart.load());

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

}
