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

// windows api: Timer-queue timer function
typedef void (CALLBACK *TimerQueueProc)(PVOID lpParameter,BOOLEAN bTimerOrWaitFired);

class SOCKETLIBRARY_API CTimerQueue
{
private:
	// = Prevent copying.
	CTimerQueue (const CTimerQueue& oEvent);
	CTimerQueue &operator= (const CTimerQueue &oEvent);
public:
	CTimerQueue(void);
	virtual ~CTimerQueue(void);
	BOOL Clear();

	//timerqueue
	BOOL Create();
	BOOL Release(HANDLE CompletionEvent = NULL);

	//timer
	BOOL CreateTimer(PHANDLE phNewTimer,TimerQueueProc timerQueueProc, PVOID pParameter, DWORD nDueTime, DWORD nPeriod, ULONG nFlags);
	BOOL DeleteTimer(HANDLE hTimer,HANDLE hCompletionEvent = NULL);
	BOOL UpdateTimer(HANDLE hTimer,ULONG nDueTime,ULONG nPeriod);

	//È¡µÃ¾ä±ú
	HANDLE GetHandle() const;

private:
	HANDLE m_hTimerQueue;

};

};