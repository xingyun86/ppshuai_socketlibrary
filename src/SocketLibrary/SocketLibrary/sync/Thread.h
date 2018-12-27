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

typedef unsigned (__stdcall *ThreadProc)(void *pParameter);

class SOCKETLIBRARY_API CThread
{
public:

	enum ThreadCmd{nothing = 0, stop};
	enum ThreadState{norun = 0, normal, suspend};

	CThread(void);
	CThread(ThreadProc threadProc, void *pParameter, bool bSuspend = false);
	virtual ~CThread(void);
	BOOL  Clear();

	//强制线程停止(这时线程不作清理资源或内存,可能会引起资源或内存泄漏)
	BOOL  Kill(void);

	BOOL  Start(ThreadProc threadProc, void *pParameter, bool bSuspend = false, int nPriority = THREAD_PRIORITY_NORMAL);
	BOOL  Start(void);

	//继续线程
	BOOL  Resume(void);
	//挂起线程
	BOOL  Suspend(void);
	//停止线程(友好退出)
	BOOL  IsStop() const;
	DWORD Stop(DWORD nTimeOut = INFINITE);

	int   GetPriority();
	BOOL  SetPriority(int nPriority);

	HANDLE GetHandle() const;

protected:
	//线程句柄
	HANDLE		m_hThread;

	ThreadProc  m_threadProc;
	void	   *m_pParameter;

	ThreadCmd   m_ThreadCmd;
	ThreadState	m_ThreadState;
};

};