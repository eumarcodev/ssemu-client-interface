#pragma once
#include "pch.h"
#include <Windows.h>
#include <memory>
#define _WINSOCK_DEPRECATED_NO_WARNINGS



class DllMain {
public:
	DllMain(HINSTANCE hins);
	void GetAccountInfoFromWindowsTitle();
	void GetMainInfo();
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE hins;
	HWND hwndMessage = NULL;
	HWND hwndMainWindow = NULL; // Adicionado para armazenar o identificador da janela principal

	std::wstring title = TEXT("");

	std::unique_ptr<AccountInfo> accountInfo;
};