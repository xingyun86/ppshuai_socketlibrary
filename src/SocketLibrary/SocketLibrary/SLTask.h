//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SLTASK_H
#define SLTASK_H

#pragma once
#include "config.h"
#include "slmessagequeue.h"
#include "sync/threadgroup.h"
#include "sync/semaphore.h"

namespace SL
{

template <class TMutex>
class CSLTask
{
public:
	CSLTask(void)
	{
	};
	virtual ~CSLTask(void)
	{
		Close();
	};
	uint32 GetMsgQueueSize()
	{
		return m_msgQueue.GetSize();
	};
	int GetMessage(CSLMessageQueueNode& oNode)
	{
		return m_msgQueue.Dequeue(oNode);
	};
	int PostMessage(const char *szMsg, int nLen, int nAttachID, void *pAttachInfo)
	{
		if (m_msgQueue.Enqueue(szMsg, nLen, nAttachID, pAttachInfo) >= 0)
			m_semaphore.UnLock();
		else
			return -1;
		return 1;
	};
	int Pop(CSLMessageQueueNode& oNode)
	{
		return m_msgQueue.Dequeue(oNode);
	};
	int Push(const char *szMsg, int nLen, int nAttachID, void *pAttachInfo)
	{
		return m_msgQueue.Enqueue(szMsg, nLen, nAttachID, pAttachInfo);
	};
	virtual bool Open(void *p) 
	{
		return true;
	};
	virtual bool Close()
	{
		int nThreadCount = m_threadGroup.GetThreadCount();
		for (int i=0;i<nThreadCount;i++)
			m_semaphore.UnLock();
		m_semaphore.Clear();
		m_threadGroup.Stop(200);
		m_threadGroup.Kill();
		m_msgQueue.Clear();
		return true;
	};
	int Activate(int nThreadCount=1, uint32 nQueueMaxSize=1000)
	{
		m_msgQueue.Init(nQueueMaxSize);
		m_semaphore.Create(NULL, 0, nQueueMaxSize, NULL);
		return m_threadGroup.Start(CSLTask<TMutex>::Run, this, nThreadCount);
	};
	bool Resume()
	{
		return m_threadGroup.Resume();
	};
	bool Suspend()
	{
		return m_threadGroup.Suspend();
	};
	int GetThreadCount() const
	{
		return m_threadGroup.GetThreadCount();
	};

	virtual int ProcessMessage(CSLMessageQueueNode& oNode)
	{
		return 0;
	};

private:
	// Routine that runs the task routine as a daemon thread.
	static unsigned int WINAPI Run(void *p)
	{
		CSLTask<TMutex> *pTask =(CSLTask<TMutex>*)p;
		CSLMessageQueueNode oNode;
		while (1)
		{
			if (pTask->m_threadGroup.IsStop())
				return -1;

			pTask->m_semaphore.Lock();
			if (pTask->GetMessage(oNode) >= 0)
			{//取得处理消息时
				pTask->ProcessMessage(oNode);
			}
		};
		return 0;
	};

protected:
	CSLMessageQueue<TMutex> m_msgQueue;
	SYNC::CThreadGroup		m_threadGroup;
	SYNC::CSemaphore		m_semaphore;
};

};

#endif