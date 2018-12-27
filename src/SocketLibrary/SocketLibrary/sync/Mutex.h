//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#pragma once
#include "../config.h"

namespace SYNC
{

class SOCKETLIBRARY_API CMutex
{
public:
	inline CMutex()
		: m_hMutex(NULL)
	{
	};

	inline ~CMutex()
	{
		Clear();
	};

	inline DWORD Wait(DWORD nTimeOut)
	{
		return WaitForSingleObject(m_hMutex, nTimeOut);
	}

	inline bool Lock(DWORD nTimeOut)
	{
		DWORD nRet = Wait(nTimeOut);
		if (WAIT_OBJECT_0 == nRet)
			return true;
		return false;
	};

	inline bool Lock()
	{
		return Lock(INFINITE);
	};

	inline bool UnLock()
	{
		return false;
	};

	inline bool TryLock()
	{
		return false;
	};

	inline HANDLE GetHandle() const	
	{
		return m_hMutex;
	};

protected:
	inline bool Clear()
	{
		if (m_hMutex != NULL)
		{
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		};
		return true;
	};

protected:
	HANDLE  m_hMutex;

};

};