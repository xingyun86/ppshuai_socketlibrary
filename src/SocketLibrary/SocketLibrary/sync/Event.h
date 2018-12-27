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

class SOCKETLIBRARY_API CEvent
{
private:
	// = Prevent copying.
	CEvent (const CEvent& oEvent);
	CEvent &operator= (const CEvent &oEvent);
public:
	CEvent(void);
	CEvent(BOOL bManualReset, BOOL bInitialState);
	CEvent(LPSECURITY_ATTRIBUTES pSecurity, BOOL bManualReset, BOOL bInitialState, LPCTSTR pszName);
	virtual ~CEvent(void);
	BOOL  Clear();

	// Create a new event
	BOOL  Create(LPSECURITY_ATTRIBUTES pSecurity, BOOL bManualReset, BOOL bInitialState, LPCTSTR pszName);
	// Open an existing named event
	BOOL  Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName);
	// Pulse the event (signals waiting objects, then resets)
	BOOL  Pulse();
	// Set the event to the non-signaled state
	BOOL  Reset();
	// Set the event to the signaled state
	BOOL  Signal();

	// Wait event
	DWORD Wait(DWORD nTimeOut);	

	// Ëø
	BOOL  Lock(DWORD nTimeOut);
	BOOL  Lock();
	BOOL  UnLock();
	BOOL  TryLock();

	//È¡µÃ¾ä±ú
	HANDLE GetHandle() const;

protected:
	HANDLE m_hEvent;

};

};