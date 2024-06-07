#pragma once
#include <Windows.h>
#include <string>

class MainInfo {
public:
    MainInfo();
    void Update();
    std::wstring GetAppName() const;
    std::wstring GetAppVersion() const;
    std::wstring GetInjectTime() const;
    std::wstring GetParentProcessName() const;
    DWORD GetParentProcessID() const;
    std::wstring GetWindowTitle() const;
    RECT GetWindowRectangle() const;
    bool IsFullScreen() const;

    friend std::wostream& operator<<(std::wostream& os, const MainInfo& info);

private:
    void FetchParentProcessName();
    void FetchWindowInfo();

    std::wstring appName_;
    std::wstring appVersion_;
    std::wstring injectTime_;
    std::wstring parentProcessName_;
    DWORD parentProcessID_;
    std::wstring windowTitle_;
    RECT windowRect_;
    bool fullScreen_;
};
