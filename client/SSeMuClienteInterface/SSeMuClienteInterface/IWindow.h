#ifndef IWINDOW_H
#define IWINDOW_H

#include "SocketClient.h"

class AccountInfo;

class IWindow {
public:
    virtual ~IWindow() {}
    virtual bool Create(BYTE wId, HINSTANCE hInstance, AccountInfo* accountInfo, SocketClient* socketClient) = 0;
    virtual void Show() = 0;
    virtual void Update() = 0;
    virtual void Close() = 0;
    virtual void Hide() = 0;
    virtual HWND GetHwnd() const = 0;
    virtual BYTE GetWindowId() const = 0;
    virtual std::wstring GetWindowTitle() const = 0;
};

#endif // IWINDOW_H
