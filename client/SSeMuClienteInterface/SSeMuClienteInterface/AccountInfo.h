#pragma once
#include "pch.h"
#include <mutex>

class AccountInfo {
public:
	AccountInfo() : m_Account(""), m_Character(""), m_IsSet(false) {}
	virtual ~AccountInfo() {};

	void SetAccount(const std::string& account) 
	{ 
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Account = account; 
	}

	void SetCharacter(const std::string& character) 
	{ 
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Character = character;
	}

	void SetIsSet(bool isSet) 
	{ 
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_IsSet = isSet; 
	}

	const std::string& GetAccount() const 
	{ 
		std::lock_guard<std::mutex> lock(m_Mutex); 
		return m_Account; 
	}

	const std::string& GetCharacter() const 
	{ 
		std::lock_guard<std::mutex> lock(m_Mutex); 
		return m_Character; 
	}

	bool IsSet() const 
	{ 
		std::lock_guard<std::mutex> lock(m_Mutex); 
		return m_IsSet; 
	}

private:
	std::string m_Account;
	std::string m_Character;
	bool m_IsSet;
	mutable std::mutex m_Mutex;
};