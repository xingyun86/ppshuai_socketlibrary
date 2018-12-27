//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include ".\thread.h"
#include <process.h>
namespace SYNC
{

CThread::CThread(void)
	: m_hThread(NULL)
	, m_ThreadCmd(ThreadCmd::nothing)
	, m_ThreadState(ThreadState::norun)
{
}

CThread::CThread(ThreadProc threadProc, void *pParameter, bool bSuspend)
	: m_hThread(NULL)
	, m_ThreadCmd(ThreadCmd::nothing)
	, m_ThreadState(ThreadState::norun)
{
	Start(threadProc,pParameter,bSuspend);
}

CThread::~CThread(void)
{
	Clear();
}

BOOL CThread::Start(ThreadProc threadProc, void *pParameter, bool bSuspend, int nPriority)
{
	Clear();
	unsigned int nThreadID = 0;
	if (bSuspend) 
		m_hThread = (HANDLE)_beginthreadex(NULL,0,threadProc,pParameter,CREATE_SUSPENDED,&nThreadID);
	else
		m_hThread = (HANDLE)_beginthreadex(NULL,0,threadProc,pParameter,0,&nThreadID);
	
	if (m_hThread == NULL)
		return FALSE;
	else
	{
		SetPriority(nPriority);
		m_threadProc  = threadProc;
		m_pParameter  = pParameter;
		if (bSuspend)
			m_ThreadState = ThreadState::suspend;
		else
			m_ThreadState = ThreadState::normal;
		return TRUE;
	}
}

BOOL CThread::Start()
{
	Clear();
	unsigned int nThreadID = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL,0,m_threadProc,m_pParameter,0,&nThreadID);
	if (NULL == m_hThread)
		return FALSE;
	else
	{
		m_ThreadState = ThreadState::normal;
		return TRUE;
	}
}

inline BOOL CThread::Resume()
{
	DWORD nRet = ResumeThread(m_hThread);
	if (nRet == -1)
		return FALSE;
	else
	{
		m_ThreadState = ThreadState::normal;
		return TRUE;
	}
}

inline BOOL CThread::Suspend()
{
	DWORD nRet = SuspendThread(m_hThread);
	if (nRet == -1)
		return FALSE;
	else
	{
		m_ThreadState = ThreadState::suspend;
		return TRUE;
	}
}

inline DWORD CThread::Stop(DWORD nTimeOut)
{
	m_ThreadCmd = ThreadCmd::stop;
	DWORD nRet = WaitForSingleObject(m_hThread, nTimeOut);
	if (nRet == WAIT_OBJECT_0)
		m_ThreadState = ThreadState::norun;
	return nRet;
}

inline BOOL CThread::Kill()
{
	BOOL bRet = TerminateThread(m_hThread,0);
	if (bRet)
	{
		m_ThreadState = ThreadState::norun;
		return TRUE;
	}
	return FALSE;
}

inline BOOL CThread::Clear()
{
	if (m_ThreadState != ThreadState::norun)
		Kill();
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	m_ThreadCmd   = ThreadCmd::nothing;
	m_ThreadState = ThreadState::norun;
	return TRUE;
}

inline HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

inline BOOL CThread::IsStop() const
{
	return (m_ThreadCmd == ThreadCmd::stop);
};

inline int CThread::GetPriority()
{
	return ::GetThreadPriority(m_hThread);
};

inline BOOL CThread::SetPriority(int nPriority)
{
	return ::SetThreadPriority(m_hThread, nPriority);
};

};