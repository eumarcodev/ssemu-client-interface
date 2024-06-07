#include "pch.h"
#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&this->m_critical);
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&this->m_critical);
}

void CriticalSection::lock()
{
	EnterCriticalSection(&this->m_critical);
}

void CriticalSection::unlock()
{
	LeaveCriticalSection(&this->m_critical);
}