//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "../config.h"
namespace SYNC
{

class SOCKETLIBRARY_API CProcessMutex
{
private:
    CProcessMutex(const CProcessMutex &);
    void operator= (const CProcessMutex &);

public:
	CProcessMutex();
	CProcessMutex(BOOL bInitialOwner);
	CProcessMutex(LPSECURITY_ATTRIBUTES pSecurity, BOOL bInitialOwner, LPCTSTR pszName);
	~CProcessMutex();
	BOOL  Clear();

	// Create a new mutex
	BOOL  Create(LPSECURITY_ATTRIBUTES pSecurity, BOOL bInitialOwner, LPCTSTR pszName );
	// Open an existing named mutex
	BOOL  Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName);
	// Release ownership of the mutex
	BOOL  Release();

	// Wait mutex
	DWORD Wait(DWORD nTimeOut);	

	// Ëø
	BOOL  Lock(DWORD nTimeOut);
	BOOL  Lock();
	BOOL  UnLock();
	BOOL  TryLock();

	HANDLE GetHandle() const;

private:
    HANDLE m_hMutex;

};

};