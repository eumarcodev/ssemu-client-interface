#include "pch.h"
#include "AccountInfo.h"
#include "WindowRegister.h"
#include "SocketClient.h"
#include "Resource.h"
#include "Protocol.h"
#include "MainInfo.h"

#define TIMER_ID 1
#define ACCOUNT_INFO_VALID (WM_APP + 0x01)

void GetAccountInfoFromWindowsTitle();
void GetMainInfo();
void RedirectIOToConsole();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HINSTANCE hins;
HWND hwndMainWindow = NULL;
HWND hwndCtrl = NULL;

std::wstring title = TEXT("");

std::unique_ptr<AccountInfo> accountInfo;
WindowRegister windowRegistrar;
std::unique_ptr<SocketClient> socketClient;
std::unique_ptr<MainInfo> mainInfo;

void RedirectIOToConsole() {
    static std::wofstream fileStream("debug.log");
    if (fileStream.is_open()) {
        std::wcout.rdbuf(fileStream.rdbuf());
        std::wcerr.rdbuf(fileStream.rdbuf());
    }
}

void GetMainInfo() {
    HWND hwnd = GetForegroundWindow();

    RECT rect;
    GetWindowRect(hwnd, &rect);

    SetWindowPos(hwndMainWindow, HWND_TOPMOST, rect.left + (rect.right - rect.left) / 2 - 200, rect.top + (rect.bottom - rect.top) / 2 - 100, 0, 0, SWP_NOSIZE);

    SetForegroundWindow(hwndMainWindow);
    ShowWindow(hwndMainWindow, SW_SHOW);
}

void GetAccountInfoFromWindowsTitle() {
    HWND hwnd = GetForegroundWindow();
    WCHAR titleWindow[512];
    GetWindowText(hwnd, titleWindow, sizeof(titleWindow) / sizeof(WCHAR));

    std::wstring titleWindowStr(titleWindow);
    if (titleWindowStr.find(L"ExclusiveMu - ") != std::wstring::npos && !accountInfo->IsSet()) {
        std::wstring account = titleWindowStr.substr(titleWindowStr.find(L"ExclusiveMu - ") + 13, titleWindowStr.find(L":") - 13);
        std::wstring character = titleWindowStr.substr(titleWindowStr.find(L":") + 2, titleWindowStr.length() - 1);

        accountInfo->SetAccount(std::string(account.begin(), account.end()));
        accountInfo->SetCharacter(std::string(character.begin(), character.end()));
        accountInfo->SetIsSet(true);
        PostMessage(hwndCtrl, ACCOUNT_INFO_VALID, 0x00, NULL);
    }
}

DWORD WINAPI EntryProcThread(LPVOID lpParam) {
    accountInfo = std::make_unique<AccountInfo>();
    socketClient = std::make_unique<SocketClient>();
    mainInfo = std::make_unique<MainInfo>();

    // Log the MainInfo
    std::wcout << *mainInfo << std::endl;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hins;
    wc.lpszClassName = L"HiddenWindowClass";

    RegisterClass(&wc);

    hwndCtrl = CreateWindowEx(0, L"HiddenWindowClass", NULL, 0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hins, NULL);

    if (!hwndCtrl) {
        return 1;
    }

    SetTimer(hwndCtrl, TIMER_ID, 1000, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == TIMER_ID) {
            if (!accountInfo->IsSet()) {
                GetAccountInfoFromWindowsTitle();
            }
            mainInfo->Update();
            std::wcout << *mainInfo << std::endl;
        }
        break;
    case ACCOUNT_INFO_VALID:
        if (wParam == 0x00) {
            if (!socketClient->Connect("127.0.0.1", 55900)) {
                MessageBox(NULL, L"Failed to connect to server", L"Error", MB_OK | MB_ICONERROR);
                return 1;
            }
            AccountInfoPacket packet;
            packet.h.type = 0xC1;
            packet.h.size = sizeof(AccountInfoPacket);
            packet.h.head = static_cast<uint8_t>(Command::ACCOUNT_INFO);
            packet.command = Command::ACCOUNT_INFO;
            strncpy_s(packet.account, accountInfo->GetAccount().c_str(), sizeof(packet.account) - 1);
            strncpy_s(packet.character, accountInfo->GetCharacter().c_str(), sizeof(packet.character) - 1);

            if (socketClient && socketClient->IsConnected() && !socketClient->Send(reinterpret_cast<const char*>(&packet), sizeof(packet))) {
                MessageBox(NULL, L"Failed to send message to server", L"Error", MB_OK | MB_ICONERROR);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

extern "C" __declspec(dllexport) void EntryProc() {
    CreateThread(NULL, 0, EntryProcThread, NULL, 0, NULL);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        hins = hModule;
#if DEBUG
        RedirectIOToConsole();
#endif
        EntryProc();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
