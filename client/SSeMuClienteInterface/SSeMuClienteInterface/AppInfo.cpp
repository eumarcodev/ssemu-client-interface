#include "pch.h"
#include "AppInfo.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iomanip>
#include <sstream>
#include <locale>
#include <codecvt>
#include <psapi.h>

AppInfo::AppInfo() {
    WCHAR buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    appName = buffer;

    appVersion = L"1.0.0"; // Example version

    SYSTEMTIME st;
    GetLocalTime(&st);
    std::wstringstream wss;
    wss << std::setw(4) << std::setfill(L'0') << st.wYear << L"-"
        << std::setw(2) << std::setfill(L'0') << st.wMonth << L"-"
        << std::setw(2) << std::setfill(L'0') << st.wDay << L" "
        << std::setw(2) << std::setfill(L'0') << st.wHour << L":"
        << std::setw(2) << std::setfill(L'0') << st.wMinute << L":"
        << std::setw(2) << std::setfill(L'0') << st.wSecond;
    injectTime = wss.str();

    HANDLE hProcess = GetCurrentProcess();
    DWORD processID = GetProcessId(hProcess);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe)) {
            do {
                if (pe.th32ProcessID == processID) {
                    parentProcessID = pe.th32ParentProcessID;
                    HANDLE hParent = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, parentProcessID);
                    if (hParent) {
                        HMODULE hMod;
                        DWORD cbNeeded;
                        if (EnumProcessModules(hParent, &hMod, sizeof(hMod), &cbNeeded)) {
                            GetModuleFileNameEx(hParent, hMod, buffer, sizeof(buffer) / sizeof(WCHAR));
                            parentProcessName = buffer;
                        }
                        CloseHandle(hParent);
                    }
                    break;
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
}

std::wstring AppInfo::getAppName() const {
    return appName;
}

std::wstring AppInfo::getAppVersion() const {
    return appVersion;
}

std::wstring AppInfo::getInjectTime() const {
    return injectTime;
}

std::wstring AppInfo::getParentProcessName() const {
    return parentProcessName;
}

DWORD AppInfo::getParentProcessID() const {
    return parentProcessID;
}

std::wostream& operator<<(std::wostream& os, const AppInfo& appInfo) {
    os << L"App Name: " << appInfo.getAppName() << std::endl;
    os << L"App Version: " << appInfo.getAppVersion() << std::endl;
    os << L"Inject Time: " << appInfo.getInjectTime() << std::endl;
    os << L"Parent Process Name: " << appInfo.getParentProcessName() << std::endl;
    os << L"Parent Process ID: " << appInfo.getParentProcessID() << std::endl;
    return os;
}
