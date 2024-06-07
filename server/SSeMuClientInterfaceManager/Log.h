#pragma once
#define MAX_LOG 13

#include "LogInfo.h"

class CLog
{
public:
    static CLog& GetInstance();
    void AddLog(BOOL active, const std::string& directory);
    void Output(eLogType type, const std::string& text);

private:
    CLog();
    ~CLog();
    CLog(const CLog&) = delete;
    CLog& operator=(const CLog&) = delete;

    LogInfo m_LogInfo[MAX_LOG];
    int m_count;
    static std::mutex m_mutex;
};