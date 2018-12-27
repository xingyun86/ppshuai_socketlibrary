//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SLMESSAGE_QUEUE_H
#define SLMESSAGE_QUEUE_H

#pragma once
#include "config.h"
#include "slext/bytebuffer.h"
#include "slext/smallobjectpool.h"
#include "sync/guard.h"
#include "sync/nullmutex.h"

namespace SL
{

class SOCKETLIBRARY_API CSLMessageQueueNode
{
public:
	CSLMessageQueueNode();
	~CSLMessageQueueNode();

public:
	int					 attachID;
	void			    *attachInfo;
	CSLMessageQueueNode *next;
	SLEXT::CByteBuffer	 msg;
};

template <class TMutex>
class CSLMessageQueue
{
public:
	CSLMessageQueue(void)
		: m_pHead(NULL)
		, m_pTail(NULL)
		, m_nSize(0)
		, m_nMaxSize(0)
	{
	};
	virtual ~CSLMessageQueue(void)
	{
		Clear();
		m_nodePool.clear();
	};
	void Init(uint32 nQueueMaxSize)
	{
		m_nMaxSize = nQueueMaxSize;
		m_nodePool.init(nQueueMaxSize, nQueueMaxSize, 4);
	}
	uint32 GetSize()
	{
		SYNC::CGuard<TMutex> guard(m_lock);
		return m_nSize;
	};
	bool IsEmpty()
	{
		SYNC::CGuard<TMutex> guard(m_lock);
		return m_nSize==0;
	}
	bool Clear()
	{
		m_lock.Lock();
		if (m_pHead == NULL)
		{
			m_lock.UnLock();
			return false;
		}

		CSLMessageQueueNode *pNode = m_pHead;
		CSLMessageQueueNode *pNext;
		while (pNode != NULL)
		{
			pNext = pNode->next;
			m_nodePool.push(pNode);
			pNode = pNext;
		};
		m_pHead = NULL;
		m_pTail = NULL;
		m_nSize = 0;
		m_lock.UnLock();

		return true;
	};

	int Enqueue(const char *szMsg, int nLen, int nAttachID, void *pAttachInfo)
	{
		m_lock.Lock();
		if ((m_nMaxSize>0) && (m_nSize>=m_nMaxSize))
		{
			m_lock.UnLock();
			return -1;
		}
		CSLMessageQueueNode *pNode = m_nodePool.pop();
		if (pNode == NULL)
		{
			m_lock.UnLock();
			return -1;
		}

		pNode->msg.data(szMsg, nLen, true);
		pNode->attachID		= nAttachID;
		pNode->attachInfo	= pAttachInfo;
		if (m_pTail != NULL)
		{
			m_pTail->next	= pNode;
			m_pTail			= pNode;
		}
		else
		{
			m_pHead = pNode;
			m_pTail = pNode;
		}
		m_nSize++;
		m_lock.UnLock();
		return 0;
	};
	int Dequeue(CSLMessageQueueNode &oNode)
	{
		m_lock.Lock();
		if (m_pHead == NULL)
		{
			m_lock.UnLock();
			return -1;
		}

		//oNode.msg.clear();
		oNode.attachID	 = m_pHead->attachID;
		oNode.attachInfo = m_pHead->attachInfo;
		oNode.msg.data(m_pHead->msg.data(),m_pHead->msg.size(),false);
		oNode.msg.owner(true);
		oNode.next = 0;

		m_pHead->msg.owner(false);
		CSLMessageQueueNode *pTmp = m_pHead;
		m_pHead = m_pHead->next;
		m_nodePool.push(pTmp);
		m_nSize--;
		if (m_nSize == 0)
		{
			m_pHead = NULL;
			m_pTail = NULL;
		}
		m_lock.UnLock();
		return 0;
	};
	
protected:
	CSLMessageQueueNode	*m_pHead;
	CSLMessageQueueNode	*m_pTail;
	uint32				 m_nSize;
	uint32				 m_nMaxSize;

	//线程安全锁
	TMutex	m_lock;

	//对象池
	SLEXT::CSmallObjectPool<CSLMessageQueueNode, SYNC::CNullMutex> m_nodePool;

};

};

#endif