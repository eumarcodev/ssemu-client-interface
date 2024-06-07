#pragma once

class CriticalSection
{
public:
	CriticalSection();
	virtual ~CriticalSection();
	void lock();
	void unlock();
private:
	CRITICAL_SECTION m_critical;
};