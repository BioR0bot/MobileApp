#include "AppDataManager.h"

AppDataManager::AppDataManager() :
    mLocalId("Mobil"),
    mExternalId("Robot"),
    mLogData(""),
    mReceivedBufferChanged(false),
    mIsConnectToRobot(false)
{

}

void AppDataManager::SetLogData(const std::string &val)
{
    std::lock_guard<std::mutex> lock(mLogDataMutex);
    mLogData = val;
}

void AppDataManager::GetLogData(std::string &val)
{
    std::lock_guard<std::mutex> lock(mLogDataMutex);
    val = mLogData;
}

void AppDataManager::SetReceivedBuffer(const std::vector<unsigned char>& buffer)
{
    std::lock_guard<std::mutex> lock(mReceivedBufferMutex);
    mReceivedBuffer = buffer;
    mReceivedBufferChanged = true;
}

bool AppDataManager::GetReceivedBuffer(std::vector<unsigned char>& buffer)
{
    std::lock_guard<std::mutex> lock(mReceivedBufferMutex);
    bool receivedBufferChangedTemp = mReceivedBufferChanged;
    if(receivedBufferChangedTemp)
    {
        buffer = mReceivedBuffer;
        mReceivedBufferChanged = false;
    }
    return receivedBufferChangedTemp;
}

bool AppDataManager::GetIsConnectToRobot()
{
    return mIsConnectToRobot.load();
}

void AppDataManager::SetIsConnectToRobot(bool val)
{
    mIsConnectToRobot.store(val);
}
