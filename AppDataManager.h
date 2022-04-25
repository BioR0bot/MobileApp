#pragma once

#include <string>
#include <mutex>

class AppDataManager
{
public:
    AppDataManager();

    void SetLogData(const std::string& val);
    void GetLogData(std::string& val);
    bool GetIsConnectToRobot();
    void SetIsConnectToRobot(bool val);

public:
    const std::string mLocalId;
    const std::string mExternalId;

private:
    std::mutex mLogDataMutex;
    std::string mLogData;

    std::mutex mIsConnectToRobotMutex;
    bool mIsConnectToRobot;
};
