#include "pch.h"
#include "Log.h"

std::mutex CLog::m_mutex;

CLog::CLog() : m_count(0)
{
}

CLog::~CLog()
{
	for (int i = 0; i < this->m_count; i++)
	{
		if (this->m_LogInfo[i].isActive())
		{
			CloseHandle(this->m_LogInfo[i].getFile());
		}
	}
}

CLog& CLog::GetInstance()
{
	static CLog instance;
	return instance;
}

void CLog::AddLog(BOOL active, const std::string& directory)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (this->m_count >= MAX_LOG || this->m_count < 0)
	{
		return;
	}

	LogInfo& logInfo = this->m_LogInfo[this->m_count++];

	logInfo.setActive(active);
	logInfo.setDirectory(directory);


	if (logInfo.isActive())
	{
		
		CreateDirectory(std::wstring(logInfo.getDirectory().begin(), logInfo.getDirectory().end()).c_str(), NULL);
		SYSTEMTIME st;
		GetLocalTime(&st);


		logInfo.setDay(st.wDay);
		logInfo.setMonth(st.wMonth);
		logInfo.setYear(st.wYear);

		logInfo.setFilename(logInfo.getDirectory() + std::to_string(logInfo.getYear()) + "-" + std::to_string(logInfo.getMonth()) + "-" + std::to_string(logInfo.getDay()) + ".log");
		
		logInfo.setFile(CreateFile(std::wstring(logInfo.getFilename().begin(), logInfo.getFilename().end()).c_str(), 
			GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0));

		if (logInfo.getFile()  == INVALID_HANDLE_VALUE)
		{
			logInfo.setActive(false);
			return;
		}

		if (SetFilePointer(logInfo.getFile(), 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
		{
			logInfo.setActive(false);
			CloseHandle(logInfo.getFile());
			return;
		}
	}
}	

void CLog::Output(eLogType type, const std::string& text)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (type < 0 || type >= this->m_count)
	{
		return;
	}

	LogInfo& logInfo = this->m_LogInfo[type];

	if (logInfo.isActive()  == 0)
	{
		return;
	}
	SYSTEMTIME st;
	GetLocalTime(&st);


	if (logInfo.getDay() != st.wDay || logInfo.getMonth() != st.wMonth || logInfo.getYear() != st.wYear)
	{
		CloseHandle(logInfo.getFile());

		logInfo.setDay(st.wDay);
		logInfo.setMonth(st.wMonth);
		logInfo.setYear(st.wYear);


		logInfo.setFilename(logInfo.getDirectory() + std::to_string(logInfo.getYear()) + "-" + std::to_string(logInfo.getMonth()) + "-" + std::to_string(logInfo.getDay()) + ".log");
		logInfo.setFile(CreateFile(std::wstring(logInfo.getFilename().begin(), logInfo.getFilename().end()).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

		if (logInfo.getFile() == INVALID_HANDLE_VALUE)
		{
			logInfo.setActive(false);
			return;
		}

		if (SetFilePointer(logInfo.getFile(), 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
		{
			logInfo.setActive(false);
			CloseHandle(logInfo.getFile());
			return;
		}
	}

	DWORD dwWritten;
	WriteFile(logInfo.getFile(), text.c_str(), text.length(), &dwWritten, NULL);
}