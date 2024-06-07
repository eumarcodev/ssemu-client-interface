#ifndef JAWELBANK_H
#define JAWELBANK_H

#include <Windows.h>
#include "IWindow.h"
#include "AccountInfo.h"
#include "SocketClient.h"

class JawelBank : public IWindow {
public:
    JawelBank();
    ~JawelBank();

    bool Create(BYTE wId, HINSTANCE hInstance, AccountInfo* accountInfo, SocketClient* socketClient) override;
    void Show() override;
    void Update() override;
    void Close() override;
    void Hide() override;
    HWND GetHwnd() const override;
    BYTE GetWindowId() const override;
    std::wstring GetWindowTitle() const override;

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE hInstance;
    HWND hwnd;
    BYTE wId;
    SocketClient* socketClient;
};

#endif // JAWELBANK_H
