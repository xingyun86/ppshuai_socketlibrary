//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "event.h"
namespace SYNC
{

inline CEvent::CEvent(void)
	: m_hEvent(NULL)
{
}

inline CEvent::CEvent(BOOL bManualReset, BOOL bInitialState)
	: m_hEvent(NULL)

{
	Create(NULL, bManualReset, bInitialState, NULL);
};

inline CEvent::CEvent(LPSECURITY_ATTRIBUTES pSecurity, BOOL bManualReset, BOOL bInitialState, LPCTSTR pszName)
	: m_hEvent(NULL)
{
	Create(pSecurity, bManualReset, bInitialState, pszName);
};

inline CEvent::~CEvent(void)
{
	Clear();
}

inline BOOL CEvent::Create( LPSECURITY_ATTRIBUTES pSecurity, BOOL bManualReset, BOOL bInitialState, LPCTSTR pszName)
{
	Clear();
	m_hEvent = ::CreateEvent(pSecurity, bManualReset, bInitialState, pszName);
	return (m_hEvent != NULL);
};

inline BOOL CEvent::Open( DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
{
	HANDLE hEvent = ::OpenEvent(dwAccess, bInheritHandle, pszName);
	if (hEvent == NULL)
		return FALSE;

	if (hEvent != m_hEvent)
	{
		Clear();
		m_hEvent = hEvent;
	}
	return TRUE;
};

inline BOOL CEvent::Pulse()
{
	return (::PulseEvent(m_hEvent));
};

inline BOOL CEvent::Reset()
{
	return (::ResetEvent(m_hEvent));
};

inline BOOL CEvent::Signal()
{
	return (::SetEvent(m_hEvent));
};

inline BOOL CEvent::Clear()
{
	if (m_hEvent != NULL)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	return TRUE;
};

inline DWORD CEvent::Wait(DWORD nTimeOut)
{
	return WaitForSingleObject(m_hEvent, nTimeOut);
};

inline BOOL CEvent::Lock(DWORD nTimeOut)
{
	return (WAIT_OBJECT_0 == Wait(nTimeOut));
};

inline BOOL CEvent::Lock()
{
	return (WAIT_OBJECT_0 == Wait(INFINITE));
};

inline BOOL CEvent::UnLock()
{
	return Signal();
};

inline BOOL CEvent::TryLock()
{
	return FALSE;
};

HANDLE CEvent::GetHandle() const
{
	return m_hEvent;
}

};