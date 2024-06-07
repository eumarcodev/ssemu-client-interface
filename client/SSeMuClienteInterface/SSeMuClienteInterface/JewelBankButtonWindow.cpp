#include "pch.h"
#include "JewelBankButtonWindow.h"
#include "JawelBank.h"
#include "Resource.h"

JewelBankButtonWindow::JewelBankButtonWindow() : hInstance(NULL), hwnd(NULL), imageResourceId(IDB_BUTTON_JEWEL_BANK), wId(0x01), socketClient(nullptr) {}

JewelBankButtonWindow::~JewelBankButtonWindow() {
    DestroyWindow(hwnd);
}

bool JewelBankButtonWindow::Create(BYTE wId, HINSTANCE hInstance, AccountInfo* accountInfo, SocketClient* socketClient) {
    this->hInstance = hInstance;
    this->wId = wId;
    this->socketClient = socketClient;
    this->accountInfo = accountInfo;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = JewelBankButtonWindow::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"JewelBankButtonWindowClass";

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        L"JewelBankButtonWindowClass",
        NULL,
        WS_POPUP,
        50, 50, 50, 50,
        NULL,
        NULL,
        hInstance,
        this
    );

    return hwnd != NULL;
}

void JewelBankButtonWindow::Show() {
    ShowWindow(hwnd, SW_SHOW);
}

void JewelBankButtonWindow::Update() {
    // Implementação futura
}

void JewelBankButtonWindow::Close() {
    DestroyWindow(hwnd);
}

void JewelBankButtonWindow::Hide() {
    ShowWindow(hwnd, SW_HIDE);
}

HWND JewelBankButtonWindow::GetHwnd() const {
    return hwnd;
}

BYTE JewelBankButtonWindow::GetWindowId() const {
    return wId;
}

std::wstring JewelBankButtonWindow::GetWindowTitle() const {
    return L"Jewel Bank Button";
}

LRESULT CALLBACK JewelBankButtonWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    JewelBankButtonWindow* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (JewelBankButtonWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->hwnd = hwnd;
    }
    else {
        pThis = (JewelBankButtonWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT JewelBankButtonWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(imageResourceId));
        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, hBitmap);

            BITMAP bitmap;
            GetObject(hBitmap, sizeof(bitmap), &bitmap);
            BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

            DeleteDC(hdcMem);
            DeleteObject(hBitmap);
        }

        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_LBUTTONDOWN:
    {
       

        jawelBank = std::make_unique<JawelBank>();
        if (jawelBank->Create(0x00, hInstance, accountInfo, socketClient)) {
            jawelBank->Show();
        }
    }
    return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
