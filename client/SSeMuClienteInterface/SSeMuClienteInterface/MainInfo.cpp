#include "pch.h"
#include "MainInfo.h"
#include <psapi.h>
#include <tchar.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

// Usar o namespace padrão de conversão para wstring
std::wstring s2ws(const std::string& str) {
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(str);
}

MainInfo::MainInfo() {
    // Getting the main window handle
    hwnd = GetForegroundWindow();

    // Getting the process ID
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);

    // Getting the process handle
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (processHandle) {
        TCHAR buffer[MAX_PATH];
        if (GetModuleFileNameEx(processHandle, NULL, buffer, MAX_PATH)) {
            appName_ = buffer;
        }

        // Get the window title
        TCHAR windowTitle[256];
        if (GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(TCHAR))) {
            windowTitle_ = windowTitle;
        }
        else {
            windowTitle_ = _T("Unknown");
        }

        // Get the parent process info
        GetParentProcessInfo(processHandle);

        CloseHandle(processHandle);
    }

    // Getting the window position and size
    GetWindowRect(hwnd, &windowRect_);

    // Check if the window is in full screen
    CheckFullScreen();

    // Getting the current time
    auto t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);
    std::wostringstream oss;
    oss << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
    injectTime_ = oss.str();
}

void MainInfo::GetParentProcessInfo(HANDLE processHandle) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    if (Process32First(snapshot, &pe32)) {
        DWORD parentPID = 0;
        DWORD currentPID = GetProcessId(processHandle);
        do {
            if (pe32.th32ProcessID == currentPID) {
                parentPID = pe32.th32ParentProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));

        if (parentPID != 0) {
            HANDLE parentProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, parentPID);
            if (parentProcessHandle) {
                TCHAR buffer[MAX_PATH];
                if (GetModuleFileNameEx(parentProcessHandle, NULL, buffer, MAX_PATH)) {
                    parentProcessName_ = buffer;
                }
                parentProcessID_ = parentPID;
                CloseHandle(parentProcessHandle);
            }
        }
    }
    CloseHandle(snapshot);
}

void MainInfo::CheckFullScreen() {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    fullScreen_ = (windowRect_.left == 0 && windowRect_.top == 0 &&
        windowRect_.right == desktop.right && windowRect_.bottom == desktop.bottom);
}

std::wostream& operator<<(std::wostream& os, const MainInfo& info) {
    os << L"App Name: " << info.appName_ << std::endl;
    os << L"App Version: " << info.appVersion_ << std::endl;
    os << L"Inject Time: " << info.injectTime_ << std::endl;
    os << L"Parent Process Name: " << info.parentProcessName_ << std::endl;
    os << L"Parent Process ID: " << info.parentProcessID_ << std::endl;
    os << L"Window Title: " << info.windowTitle_ << std::endl;
    os << L"Window Rect: (" << info.windowRect_.left << L", " << info.windowRect_.top << L", " << info.windowRect_.right << L", " << info.windowRect_.bottom << L")" << std::endl;
    os << L"Full Screen: " << (info.fullScreen_ ? L"Yes" : L"No") << std::endl;
    return os;
}
