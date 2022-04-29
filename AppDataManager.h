#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <vector>

class AppDataManager
{
public:
    AppDataManager();

    void SetLogData(const std::string& val);
    void GetLogData(std::string& val);

    void SetReceivedBuffer(const std::vector<unsigned char>& buffer);
    bool GetReceivedBuffer(std::vector<unsigned char>& buffer);


    bool GetIsConnectToRobot();
    void SetIsConnectToRobot(bool val);

public:
    const std::string mLocalId;
    const std::string mExternalId;

private:
    std::mutex mLogDataMutex;
    std::string mLogData;

    std::mutex mReceivedBufferMutex;
    std::vector<unsigned char> mReceivedBuffer;
    bool mReceivedBufferChanged;


    std::atomic_bool mIsConnectToRobot;
};
