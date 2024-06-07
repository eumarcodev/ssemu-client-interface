#ifndef APPINFO_H
#define APPINFO_H

#include <string>
#include <iostream>

class AppInfo {
public:
    AppInfo();
    std::wstring getAppName() const;
    std::wstring getAppVersion() const;
    std::wstring getInjectTime() const;
    std::wstring getParentProcessName() const;
    DWORD getParentProcessID() const;

    friend std::wostream& operator<<(std::wostream& os, const AppInfo& appInfo);

private:
    std::wstring appName;
    std::wstring appVersion;
    std::wstring injectTime;
    std::wstring parentProcessName;
    DWORD parentProcessID;
};

#endif // APPINFO_H
