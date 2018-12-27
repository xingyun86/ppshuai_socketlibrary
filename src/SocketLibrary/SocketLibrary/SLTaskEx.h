//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlibrary is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SLTASKEX_H
#define SLTASKEX_H

//基于完成端口实现消息队列通知

#pragma once
#include "config.h"
#include "slmessagequeue.h"
#include "sync/threadgroup.h"

namespace SL
{

template <class TMutex>
class CSLTaskEx
{
public:

	CSLTaskEx(void)
		: m_hCompletionPort(NULL)
	{
	};
	virtual ~CSLTaskEx(void)
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
		{
			if ( PostQueuedCompletionStatus(m_hCompletionPort, nLen, 1, NULL) )
				return 1;
			else
				return -1;
		}
		return -1;
	};
	int Pop(CSLMessageQueueNode& oNode)
	{
		return m_msgQueue.Dequeue(oNode);
	};
	int Push(const char *szMsg, int nLen, int nAttachID, void *pAttachInfo)
	{
		if (m_msgQueue.Enqueue(szMsg, nLen, nAttachID, pAttachInfo) >= 0)
		{
			//if ( PostQueuedCompletionStatus(m_hCompletionPort,nLen,1,NULL) )
			//	return 1;
			//else
			//	return -1;
			return 1;
		}
		return -1;
	};
	virtual bool Open(void *p) 
	{
		return true;
	};
	virtual bool Close() 
	{
		if (NULL == m_hCompletionPort)
			return false;

		//使工作线程退出
		for (int i=0; i<m_threadGroup.GetThreadCount(); i++)
		{
			//发出退出消息
			::PostQueuedCompletionStatus(m_hCompletionPort,-1,-1,NULL);
		}
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
		m_threadGroup.Stop(200);
		m_threadGroup.Kill();
		m_msgQueue.Clear();

		return true;
	};
	int Activate(int nThreadCount=1, int nQueueMaxCount=1000)
	{
		//创建完成端口句柄
		m_hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,nThreadCount);
		if (INVALID_HANDLE_VALUE == m_hCompletionPort)
		{		
			return -1;
		}
		m_msgQueue.Init(nQueueMaxCount);
		return m_threadGroup.Start(CSLTaskEx<TMutex>::Run, this, nThreadCount);
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
		CSLTaskEx<TMutex> *pTaskEx =(CSLTaskEx<TMutex>*)p;
		CSLMessageQueueNode oNode;

		BOOL	bSuccess;
		DWORD	nByteTransferred;
		DWORD   dwCompletionKey;
		OVERLAPPED *pOverlapped;
		while (1)
		{
			if (pTaskEx->m_threadGroup.IsStop())
				return -1;

			//nByteTransferred = 0;
			//dwCompletionKey  = 0;
			//pOverlapped	   = NULL;
			bSuccess = ::GetQueuedCompletionStatus(pTaskEx->m_hCompletionPort,
												&nByteTransferred,
												&dwCompletionKey,
												(LPOVERLAPPED*)&pOverlapped,
												INFINITE);
			//退出信号到达，退出线程
			if ( -1==nByteTransferred )
				return -1;
			if (pTaskEx->GetMessage(oNode) >= 0)
			{//取得处理消息时
				pTaskEx->ProcessMessage(oNode);
			}
		};
		return 0;
	};

protected:
	CSLMessageQueue<TMutex> m_msgQueue;
	SYNC::CThreadGroup		m_threadGroup;
	HANDLE					m_hCompletionPort;
};

};

#endif
