#ifndef JEWELBANKBUTTONWINDOW_H
#define JEWELBANKBUTTONWINDOW_H

#include <Windows.h>
#include <string>
#include <memory>
#include "IWindow.h"
#include "SocketClient.h"
#include "JawelBank.h"  
class JewelBankButtonWindow : public IWindow {
public:
    JewelBankButtonWindow();
    ~JewelBankButtonWindow();

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
    int imageResourceId;
    BYTE wId;
    SocketClient* socketClient;
    std::unique_ptr<JawelBank> jawelBank;
    AccountInfo* accountInfo;
};

#endif // JEWELBANKBUTTONWINDOW_H
