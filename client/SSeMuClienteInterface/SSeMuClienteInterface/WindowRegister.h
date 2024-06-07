#ifndef WINDOWREGISTER_H
#define WINDOWREGISTER_H

#include "IWindow.h"
#include "AccountInfo.h"

class WindowRegister {
public:
    bool RegisterWindow(std::unique_ptr<IWindow> window);
    virtual ~WindowRegister();
    void ShowAll();
    void UpdateAll();
    void CloseAll();
    bool IsWindowRegistered(BYTE windowId) const;

private:
    std::vector<std::unique_ptr<IWindow>> windows;
};

#endif // WINDOWREGISTER_H
