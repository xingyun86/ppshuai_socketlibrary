//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "processmutex.h"
namespace SYNC
{

inline CProcessMutex::CProcessMutex(void)
	: m_hMutex(NULL)
{
};

inline CProcessMutex::CProcessMutex(BOOL bInitialOwner)
{
	Create(NULL, bInitialOwner, NULL);
};

inline CProcessMutex::CProcessMutex(LPSECURITY_ATTRIBUTES pSecurity, BOOL bInitialOwner, LPCTSTR pszName)
{
	Create(pSecurity, bInitialOwner, pszName);
};

inline CProcessMutex::~CProcessMutex(void)
{
	Clear();
};

inline BOOL CProcessMutex::Clear()
{
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
	return TRUE;
};

inline BOOL CProcessMutex::Create( LPSECURITY_ATTRIBUTES pSecurity, BOOL bInitialOwner, LPCTSTR pszName)
{
	Clear();
	m_hMutex = ::CreateMutex(pSecurity, bInitialOwner, pszName);
	return (m_hMutex != NULL);
};

inline BOOL CProcessMutex::Open( DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
{
	HANDLE hMutex = ::OpenMutex(dwAccess, bInheritHandle, pszName);
	if (hMutex == NULL)
		return FALSE;

	if (hMutex != m_hMutex)
	{
		Clear();
		m_hMutex = hMutex;
	}
	return TRUE;
};

inline BOOL CProcessMutex::Release()
{
	return (::ReleaseMutex(m_hMutex));
};

inline DWORD CProcessMutex::Wait(DWORD nTimeOut)
{
	return WaitForSingleObject(m_hMutex, nTimeOut);
};

inline BOOL CProcessMutex::Lock(DWORD nTimeOut)
{
	return (Wait(nTimeOut) == WAIT_OBJECT_0);
};

inline BOOL CProcessMutex::Lock()
{
	return (Wait(INFINITE) == WAIT_OBJECT_0);
};

inline BOOL CProcessMutex::UnLock()
{
	return FALSE;
};

inline BOOL CProcessMutex::TryLock()
{
	return FALSE;
};

inline HANDLE CProcessMutex::GetHandle() const
{
	return m_hMutex;
};

};