#include "pch.h"
#include "MainInfo.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <ctime>
#include <iomanip>
#include <sstream>

MainInfo::MainInfo() {
    // Initialization and fetching initial data
    FetchParentProcessName();
    FetchWindowInfo();

    // Set appName_, appVersion_, injectTime_ ...
}

void MainInfo::Update() {
    FetchWindowInfo();
}

void MainInfo::FetchParentProcessName() {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            WCHAR szProcessName[MAX_PATH];
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(WCHAR));
            parentProcessName_ = szProcessName;
        }
        CloseHandle(hProcess);
    }
}

void MainInfo::FetchWindowInfo() {
    HWND hwnd = GetForegroundWindow();

    WCHAR title[256];
    GetWindowText(hwnd, title, sizeof(title) / sizeof(WCHAR));
    windowTitle_ = title;

    GetWindowRect(hwnd, &windowRect_);

    // Check if fullscreen
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(monitor, &monitorInfo);
    fullScreen_ = (windowRect_.left == monitorInfo.rcMonitor.left &&
        windowRect_.top == monitorInfo.rcMonitor.top &&
        windowRect_.right == monitorInfo.rcMonitor.right &&
        windowRect_.bottom == monitorInfo.rcMonitor.bottom);
}

// Getters implementation
std::wstring MainInfo::GetAppName() const {
    return appName_;
}

std::wstring MainInfo::GetAppVersion() const {
    return appVersion_;
}

std::wstring MainInfo::GetInjectTime() const {
    return injectTime_;
}

std::wstring MainInfo::GetParentProcessName() const {
    return parentProcessName_;
}

DWORD MainInfo::GetParentProcessID() const {
    return parentProcessID_;
}

std::wstring MainInfo::GetWindowTitle() const {
    return windowTitle_;
}

RECT MainInfo::GetWindowRectangle() const {
    return windowRect_;
}

bool MainInfo::IsFullScreen() const {
    return fullScreen_;
}

std::wostream& operator<<(std::wostream& os, const MainInfo& info) {
    os << L"App Name: " << info.GetAppName() << L"\n"
        << L"App Version: " << info.GetAppVersion() << L"\n"
        << L"Inject Time: " << info.GetInjectTime() << L"\n"
        << L"Parent Process Name: " << info.GetParentProcessName() << L"\n"
        << L"Parent Process ID: " << info.GetParentProcessID() << L"\n"
        << L"Window Title: " << info.GetWindowTitle() << L"\n"
        << L"Window Rect: (" << info.GetWindowRectangle().left << L", " << info.GetWindowRectangle().top << L", "
        << info.GetWindowRectangle().right << L", " << info.GetWindowRectangle().bottom << L")\n"
        << L"Full Screen: " << (info.IsFullScreen() ? L"Yes" : L"No") << L"\n";
    return os;
}
