//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "threadgroup.h"
#include "guard.h"
#include <process.h>
namespace SYNC
{

CThreadGroup::CThreadGroup(void)
	: m_ThreadCmd(CThread::ThreadCmd::nothing)
	, m_threadProc(NULL)
	, m_pParameter(NULL)
{
	m_vecThreads.reserve(16);
};

CThreadGroup::CThreadGroup(ThreadProc threadProc, void *pParameter)
	: m_threadProc(threadProc)
	, m_pParameter(pParameter)
 	, m_ThreadCmd(CThread::ThreadCmd::nothing)
{
	m_vecThreads.reserve(16);
}

CThreadGroup::~CThreadGroup(void)
{
	Clear();
};

bool CThreadGroup::Start(ThreadProc threadProc, void *pParameter, int nThreadCount, int nPriority)
{	
	if (NULL == threadProc)
		return false;

	if (nThreadCount > 0)
	{
		Clear();
		m_vecThreads.reserve(nThreadCount);
		unsigned int nThreadID = 0;
		HANDLE hThread = NULL;
		for (int i=0; i<nThreadCount; i++)
		{
			hThread = (HANDLE)_beginthreadex(NULL,0,threadProc,pParameter,0,&nThreadID);
			if (NULL == hThread)
				return false;
			::SetThreadPriority(hThread, nPriority);
			m_vecThreads.push_back(hThread);
		}
	}
	m_threadProc = threadProc;
	m_pParameter = pParameter;

	return true;
}

bool CThreadGroup::Start(int nThreadCount)
{	
	if (NULL == m_threadProc)
		return false;

	if (nThreadCount > 0)
	{
		Clear();
		m_vecThreads.reserve(nThreadCount);
		unsigned int nThreadID = 0;
		HANDLE hThread = NULL;
		for (int i=0; i<nThreadCount; i++)
		{
			hThread = (HANDLE)_beginthreadex(NULL,0,m_threadProc,m_pParameter,0,&nThreadID);
			if (NULL == hThread)
				return false;
			m_vecThreads.push_back(hThread);
		}
	}
	return true;
}

inline DWORD CThreadGroup::Stop(DWORD nTimeOut)
{
	m_ThreadCmd = CThread::ThreadCmd::stop;
	std::vector<HANDLE>::iterator itThread;
	for (itThread = m_vecThreads.begin(); itThread != m_vecThreads.end(); itThread++)
	{
		WaitForSingleObject(*itThread, nTimeOut);
	}
	return 0;
}

inline bool CThreadGroup::IsStop() const
{
	if (m_ThreadCmd != CThread::ThreadCmd::stop)
		return false;
	else
		return true;
};

bool CThreadGroup::Kill()
{
	return Clear();
}

bool CThreadGroup::Clear()
{
	HANDLE hThread;
	std::vector<HANDLE>::iterator itThread;
	for (itThread = m_vecThreads.begin(); itThread != m_vecThreads.end(); itThread++)
	{
		hThread = *itThread;
		TerminateThread(hThread,0);
		CloseHandle(hThread);
	}
	m_vecThreads.clear();
	m_ThreadCmd = CThread::ThreadCmd::nothing;
	return true;
}

inline int CThreadGroup::GetThreadCount() const
{
	return m_vecThreads.size();
};

int CThreadGroup::AddThread(int nThreadCount)
{
	if (NULL == m_threadProc)
		return 0;
	if (nThreadCount <= 0)
		return 0;

	int nRet = 0;
	unsigned int nThreadID = 0;
	HANDLE hThread = NULL;
	for (int i=0; i<nThreadCount; i++)
	{
		hThread = (HANDLE)_beginthreadex(NULL,0,m_threadProc,m_pParameter,0,&nThreadID);
		if (NULL != hThread)
		{
			nRet++;
			m_vecThreads.push_back(hThread);
		}
	}
	return nRet;
};

int CThreadGroup::RemoveThread(int nThreadCount)
{
	if (nThreadCount>0)
	{
		int nRet = GetThreadCount();
		if (nThreadCount >= nRet)
			Clear();
		else
		{
			nRet = nThreadCount;
			for (int i=0; i<nThreadCount; i++)
			{
				const HANDLE &hThread = m_vecThreads.back();
				TerminateThread(hThread,0);
				CloseHandle(hThread);
				m_vecThreads.pop_back();
			}
		}
		return nRet;
	}
	return 0;
};

bool CThreadGroup::Resume()
{
	std::vector<HANDLE>::iterator itThread;
	for (itThread = m_vecThreads.begin(); itThread != m_vecThreads.end(); itThread++)
	{
		ResumeThread(*itThread);
	};
	return true;
};

bool CThreadGroup::Suspend()
{
	std::vector<HANDLE>::iterator itThread;
	for (itThread = m_vecThreads.begin(); itThread != m_vecThreads.end(); itThread++)
	{
		SuspendThread(*itThread);
	};
	return true;
};

};