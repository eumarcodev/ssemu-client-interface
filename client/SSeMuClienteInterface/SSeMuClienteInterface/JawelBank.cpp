#include "pch.h"
#include "JawelBank.h"
#include "Resource.h"

JawelBank::JawelBank() : hInstance(NULL), hwnd(NULL), wId(0x00), socketClient(nullptr) {}

JawelBank::~JawelBank() {
    DestroyWindow(hwnd);
}

bool JawelBank::Create(BYTE wId, HINSTANCE hInstance, AccountInfo* accountInfo, SocketClient* socketClient) {
    this->hInstance = hInstance;
    this->wId = wId;
    this->socketClient = socketClient;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = JawelBank::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"JawelBankClass";

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        L"JawelBankClass",
        L"Jawel Bank",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL,
        NULL,
        hInstance,
        this
    );

    return hwnd != NULL;
}

void JawelBank::Show() {
    ShowWindow(hwnd, SW_SHOW);
}

void JawelBank::Update() {
    // Placeholder function. Actual message loop runs in a separate thread.
}

void JawelBank::Close() {
    DestroyWindow(hwnd);
}

void JawelBank::Hide() {
    ShowWindow(hwnd, SW_HIDE);
}

HWND JawelBank::GetHwnd() const {
    return hwnd;
}

BYTE JawelBank::GetWindowId() const {
    return wId;
}

std::wstring JawelBank::GetWindowTitle() const {
    return L"Jawel Bank";
}

LRESULT CALLBACK JawelBank::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    JawelBank* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (JawelBank*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->hwnd = hwnd;
    }
    else {
        pThis = (JawelBank*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT JawelBank::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
