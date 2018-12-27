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

class SOCKETLIBRARY_API CSemaphore
{
private:
	// = Prevent copying.
	CSemaphore (const CSemaphore& oSemaphore);
	CSemaphore &operator= (const CSemaphore &oSemaphore);

public:
	CSemaphore(void);
	CSemaphore(LONG nInitialCount, LONG nMaxCount);
	CSemaphore(LPSECURITY_ATTRIBUTES pSecurity, LONG nInitialCount, LONG nMaxCount, LPCTSTR pszName);
	virtual ~CSemaphore(void);
	BOOL  Clear();

	// Create a new semaphore
	BOOL  Create(LPSECURITY_ATTRIBUTES pSecurity, LONG nInitialCount, LONG nMaxCount, LPCTSTR pszName);
	// Open an existing named semaphore
	BOOL  Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName);
	// Increase the count of the semaphore
	BOOL  Release(LONG nReleaseCount = 1, LONG* pnOldCount = NULL);

	// Wait Semaphore
	DWORD Wait(DWORD nWaitTime);

	// Ëø
	BOOL  Lock(DWORD nTimeOut);
	BOOL  Lock();
	BOOL  UnLock();
	BOOL  TryLock();

	HANDLE GetHandle() const;

protected:
	HANDLE m_hSemaphore;
};

};