//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "../stdafx.h"
#include "timerqueue.h"
namespace SYNC
{

inline CTimerQueue::CTimerQueue(void)
	: m_hTimerQueue(NULL)
{
	Create();
};

inline CTimerQueue::~CTimerQueue(void)
{
	Clear();
};

inline BOOL CTimerQueue::Clear()
{
	return Release(NULL);
};

inline BOOL CTimerQueue::Create()
{
	Clear();
	m_hTimerQueue = ::CreateTimerQueue();
	return (m_hTimerQueue != NULL);
};

inline BOOL CTimerQueue::Release(HANDLE hCompletionEvent)
{
	if (m_hTimerQueue != NULL)
	{
		BOOL bRet = ::DeleteTimerQueueEx(m_hTimerQueue,hCompletionEvent);
		m_hTimerQueue = NULL;
		return bRet;
	};
	return TRUE;
};

//È¡µÃ¾ä±ú
inline HANDLE CTimerQueue::GetHandle() const
{
	return m_hTimerQueue;
};

inline BOOL CTimerQueue::CreateTimer(PHANDLE phNewTimer,TimerQueueProc timerQueueProc, PVOID pParameter, DWORD nDueTime, DWORD nPeriod, ULONG nFlags)
{
	return ::CreateTimerQueueTimer(phNewTimer,m_hTimerQueue,timerQueueProc,pParameter,nDueTime,nPeriod,nFlags);
};

BOOL CTimerQueue::DeleteTimer(HANDLE hTimer,HANDLE hCompletionEvent)
{
	return ::DeleteTimerQueueTimer(m_hTimerQueue,hTimer,hCompletionEvent);
};

BOOL CTimerQueue::UpdateTimer(HANDLE hTimer,ULONG nDueTime,ULONG nPeriod)
{
	return ::ChangeTimerQueueTimer(m_hTimerQueue,hTimer,nDueTime,nPeriod);
};

};