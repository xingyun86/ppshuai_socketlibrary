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
#include "thread.h"
#include "threadmutex.h"
#include <vector>
namespace SYNC
{

class SOCKETLIBRARY_API CThreadGroup
{
public:
	CThreadGroup(void);
	CThreadGroup(ThreadProc threadProc, void *pParameter);
	virtual ~CThreadGroup(void);
	bool  Clear();

	bool  Kill();
	bool  Start(ThreadProc threadProc, void *pParameter, int nThreadCount = 2, int nPriority = THREAD_PRIORITY_NORMAL);
	bool  Start(int nThreadCount = 2);
	bool  IsStop() const;
	DWORD Stop(DWORD nTimeOut = INFINITE);

	//继续线程
	bool  Resume(void);
	//挂起线程
	bool  Suspend(void);

	//动态增减线程数
	int	  AddThread(int nThreadCount);
	int   RemoveThread(int nThreadCount);
	int   GetThreadCount() const;

protected:
	CThreadMutex		m_lock;
	std::vector<HANDLE> m_vecThreads;

	ThreadProc  m_threadProc;
	void	   *m_pParameter;

	CThread::ThreadCmd m_ThreadCmd;

};

};