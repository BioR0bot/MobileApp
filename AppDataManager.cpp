#include "AppDataManager.h"

AppDataManager::AppDataManager() :
    mLocalId("Mobil"),
    mExternalId("Robot"),
    mLogData(""),
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

bool AppDataManager::GetIsConnectToRobot()
{
    std::lock_guard<std::mutex> lock(mIsConnectToRobotMutex);
    return mIsConnectToRobot;
}

void AppDataManager::SetIsConnectToRobot(bool val)
{
    std::lock_guard<std::mutex> lock(mIsConnectToRobotMutex);
    mIsConnectToRobot = val;
}
