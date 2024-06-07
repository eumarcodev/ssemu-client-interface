#include "pch.h"
#include "ServerDisplayer.h"
#include "Log.h"

std::mutex ServerDisplayer::m_mutex;
ServerDisplayer::ServerDisplayer() : m_count(0)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
    {
        m_log[n].setColor(eLogColor::LOG_BLACK);
        m_log[n].setText("");
    }

    m_font[0] = CreateFont(50, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Times");
    m_font[1] = CreateFont(18, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Times");
    m_brush[0] = CreateSolidBrush(RGB(105, 105, 105));
    m_brush[1] = CreateSolidBrush(RGB(110, 240, 120));
    m_brush[2] = CreateSolidBrush(RGB(50, 50, 50));
    m_DisplayerText[0] = "STANDBY MODE";
    m_DisplayerText[1] = "ACTIVE MODE";
}

ServerDisplayer::~ServerDisplayer()
{
    DeleteObject(m_font[0]);
    DeleteObject(m_font[1]);
    DeleteObject(m_brush[0]);
    DeleteObject(m_brush[1]);
    DeleteObject(m_brush[2]);
}

ServerDisplayer& ServerDisplayer::GetInstance()
{
    
    static ServerDisplayer instance;
    return instance;
}

void ServerDisplayer::Init(HWND hWnd)
{
    m_hwnd = hWnd;

    CreateDirectory(L"LOGS", 0);
    CLog& log = CLog::GetInstance();
    log.AddLog(1, "LOGS\\GENERAL\\");
}

void ServerDisplayer::Run()
{
    SetWindowName();
    PaintAllInfo();
    LogTextPaint();
    PaintInfos();
}

void ServerDisplayer::SetWindowName()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::wstring str = L"SSeMuClienteInterfaceManager";
    SetWindowText(m_hwnd, str.c_str());
}

void ServerDisplayer::PaintAllInfo()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    rect.top = 0;
    rect.bottom = 80;
    HDC hdc = GetDC(m_hwnd);
    int OldBkMode = SetBkMode(hdc, TRANSPARENT);
    HFONT OldFont = (HFONT)SelectObject(hdc, m_font[0]);

    std::wstring active = L"ACTIVE MODE";

    SetTextColor(hdc, RGB(250, 250, 250));
    FillRect(hdc, &rect, this->m_brush[1]);
    TextOut(hdc, 120, 20, active.c_str(), active.length());

    SelectObject(hdc, OldFont);
    SetBkMode(hdc, OldBkMode);
    ReleaseDC(this->m_hwnd, hdc);
}

std::string ServerDisplayer::ConvertMemory(uint64_t bytes)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    static const char* suffix[] = { "B", "KB", "MB", "GB", "TB" };
    int length = sizeof(suffix) / sizeof(suffix[0]);
    int i = 0;
    double dblBytes = static_cast<double>(bytes);
    if (bytes > 1024) {
        for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024) {
            dblBytes = bytes / 1024.0;
        }
    }
    char output[200];
    sprintf_s(output, "%.02lf %s", dblBytes, suffix[i]);
    return std::string(output);
}

void ServerDisplayer::PaintInfos()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RECT rect;

    GetClientRect(this->m_hwnd, &rect);

    rect.top = 570;

    rect.bottom = 600;

    HDC hdc = GetDC(this->m_hwnd);

    int OldBkMode = SetBkMode(hdc, TRANSPARENT);

    HFONT OldFont = (HFONT)SelectObject(hdc, this->m_font[1]);

    SetTextColor(hdc, RGB(255, 255, 255));

    FillRect(hdc, &rect, this->m_brush[2]);

	std::wstring str = L"Memory: ";

    TextOut(hdc, 25, 573, str.c_str(), str.length());

    SelectObject(hdc, OldFont);
    SetBkMode(hdc, OldBkMode);
    ReleaseDC(this->m_hwnd, hdc);
}

void ServerDisplayer::LogTextPaint() // OK
{
    RECT rect;

    GetClientRect(this->m_hwnd, &rect);

    rect.top = 80;

    HDC hdc = GetDC(this->m_hwnd);

    FillRect(hdc, &rect, (HBRUSH)GetStockObject(0));

    int OldBkMode = SetBkMode(hdc, TRANSPARENT);

    HFONT OldFont = (HFONT)SelectObject(hdc, this->m_font);

    int line = MAX_LOG_TEXT_LINE;

    int count = (((this->m_count - 1) >= 0) ? (this->m_count - 1) : (MAX_LOG_TEXT_LINE - 1));

    for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
    {
        switch (this->m_log[count].getColor())
        {
        case eLogColor::LOG_BLACK:
            SetTextColor(hdc, RGB(0, 0, 0));
            break;
        case eLogColor::LOG_RED:
            SetTextColor(hdc, RGB(255, 0, 0));
            break;
        case eLogColor::LOG_GREEN:
            SetTextColor(hdc, RGB(0, 255, 0));
            break;
        case eLogColor::LOG_BLUE:
            SetTextColor(hdc, RGB(0, 0, 255));
            break;
        }

        int size = this->m_log[count].getText().length();

        if (size > 1)
        {
            SYSTEMTIME st;
            GetLocalTime(&st);
			std::wstring strText = std::wstring(this->m_log[count].getText().begin(), this->m_log[count].getText().end());
            TextOut(hdc, 0, (65 + (line * 15)), strText.c_str(), size);
            line--;
        }

        count = (((--count) >= 0) ? count : (MAX_LOG_TEXT_LINE - 1));
    }

    ReleaseDC(this->m_hwnd, hdc);
}

void ServerDisplayer::LogAddText(eLogColor color, const std::string& text) // OK
{
    SYSTEMTIME st;
    GetLocalTime(&st);
	int size = text.length();
    size = ((size >= MAX_LOG_TEXT_SIZE) ? (MAX_LOG_TEXT_SIZE - 1) : size);

    //[%02d:%02d:%02d] %s
    std::stringstream ss;
    ss << "[" << std::setw(2) << std::setfill('0') << st.wHour << ":" 
        << std::setw(2) << std::setfill('0') << st.wMinute << ":" 
        << std::setw(2) << std::setfill('0') << st.wSecond << "] " 
        << text << std::endl;


	this->m_log[this->m_count].setText(ss.str());
    this->m_log[this->m_count].setColor(color);

    this->m_count = (((++this->m_count) >= MAX_LOG_TEXT_LINE) ? 0 : this->m_count);
    CLog& log = CLog::GetInstance();
    log.Output(LOG_GENERAL, ss.str());
}
