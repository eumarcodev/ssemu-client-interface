#include "pch.h"
#include "WindowRegister.h"

bool WindowRegister::RegisterWindow(std::unique_ptr<IWindow> window) {
    if (window) {
        if(IsWindowRegistered(window->GetWindowId())) {
			return true;
		}
        windows.push_back(std::move(window));
        return true;
    }
    return false;
}

WindowRegister::~WindowRegister() {
    windows.clear();
}

void WindowRegister::ShowAll() {
    for (const auto& window : windows) {
        window->Show();
    }
}

void WindowRegister::UpdateAll() {
    for (const auto& window : windows) {
        window->Update();
    }
}

void WindowRegister::CloseAll() {
    for (const auto& window : windows) {
        window->Close();
    }
}

bool WindowRegister::IsWindowRegistered(BYTE windowId) const {
    for (const auto& window : windows) {
        if (window->GetWindowId() == windowId) {
            return true;
        }
    }
    return false;
}
