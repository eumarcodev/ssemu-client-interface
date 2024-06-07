#pragma once

#include <windows.h>
#include <string>
#include <ostream>

class MainInfo {
public:
    MainInfo();
    friend std::wostream& operator<<(std::wostream& os, const MainInfo& info);

private:
    void GetParentProcessInfo(HANDLE processHandle);
    void CheckFullScreen();

    std::wstring appName_;
    std::wstring appVersion_ = L"1.0.0";
    std::wstring injectTime_;
    std::wstring parentProcessName_;
    DWORD parentProcessID_ = 0;
    std::wstring windowTitle_;
    RECT windowRect_;
    bool fullScreen_ = false;

    HWND hwnd;
};
