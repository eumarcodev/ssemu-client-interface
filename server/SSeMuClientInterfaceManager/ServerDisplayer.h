#pragma once
#include "LogDisplayInfo.h"

class ServerDisplayer
{
public:
    static ServerDisplayer& GetInstance();
    void Init(HWND hWnd);
    void Run();
    void SetWindowName();
    void PaintAllInfo();
    void LogTextPaint();
    void PaintInfos();
    void LogAddText(eLogColor color, const std::string& text);
    std::string ConvertMemory(uint64_t bytes);
private:
    ServerDisplayer();
    ~ServerDisplayer();
    ServerDisplayer(const ServerDisplayer&) = delete;
    ServerDisplayer& operator=(const ServerDisplayer&) = delete;

    HWND m_hwnd;
    HFONT m_font[2];
    HBRUSH m_brush[3];
    std::array<LogDisplayInfo, MAX_LOG_TEXT_LINE> m_log;
    int m_count;
    std::array<std::string, 2> m_DisplayerText;
    static std::mutex m_mutex;
};
