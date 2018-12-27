//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

// -------------------------------------------------------------------------
// class CFixedObjectPool
// 固定大小的对象池
#ifndef FIXED_OBJECTPOOL_H
#define FIXED_OBJECTPOOL_H

namespace SLEXT
{

template <typename T, typename TMutex>
class CFixedObjectPool
{
private:
    CFixedObjectPool(const CFixedObjectPool &);
    void operator= (const CFixedObjectPool &);

public:
	struct NODE
	{
		T				Object;
		struct NODE	   *pPrev;
		struct NODE	   *pNext;
		bool			bFree;
	};
    CFixedObjectPool()
		: m_ObjectPool(NULL)
		, m_pFreeListHead(NULL)
		, m_pUserListHead(NULL)
		, m_nUserSize(0)
	{
	}
    CFixedObjectPool(size_t nMaxSize)
		: m_pFreeListHead(NULL)
		, m_pUserListHead(NULL)
		, m_nUserSize(0)
		, m_nCapacity(nMaxSize)
	{
		init(nCapacity);
	}
	~CFixedObjectPool()
	{
		clear();
	}
	void clear()
	{
		m_mtx.Lock();
		if (m_ObjectPool != NULL)
		{
			delete []m_ObjectPool;
			m_ObjectPool = NULL;
		}
		m_pFreeListHead = NULL;
		m_pUserListHead = NULL;
		m_nCapacity = 0;
		m_nUserSize = 0;
		m_mtx.UnLock();
	}

	void init(unsigned int nMaxSize=100, unsigned int nInitSize=100, unsigned int nGrowSize=4)
	{
		if (nMaxSize < 3)
			nMaxSize = 3;
		clear();

		m_mtx.Lock();
		m_ObjectPool	= new NODE[nMaxSize];
		if (m_ObjectPool == NULL)
			return;
		m_nCapacity		= nMaxSize;
		m_pFreeListHead = m_ObjectPool;
		m_ObjectPool[0].pPrev = NULL;
		m_ObjectPool[0].pNext = &m_ObjectPool[1];
		m_ObjectPool[0].bFree = true;
		m_ObjectPool[nMaxSize-1].pPrev = &m_ObjectPool[nMaxSize-2];
		m_ObjectPool[nMaxSize-1].pNext = NULL;
		m_ObjectPool[nMaxSize-1].bFree = true;
		for (size_t i=1; i<nMaxSize-1;i++)
		{
			m_ObjectPool[i].pPrev = &m_ObjectPool[i-1];
			m_ObjectPool[i].pNext = &m_ObjectPool[i+1];
			m_ObjectPool[i].bFree = true;
		}
		m_mtx.UnLock();

		return;
	}

	T* pop()
	{
		NODE *pNode;
        T	 *pObj = NULL;

		m_mtx.Lock();
		if (m_pFreeListHead != NULL)
		{	
			pNode			= m_pFreeListHead;
			pNode->bFree	= false;
			pObj			= &pNode->Object;
			m_pFreeListHead	= m_pFreeListHead->pNext;
			if (m_pUserListHead != NULL)
				m_pUserListHead->pPrev = pNode;
			pNode->pPrev	= NULL;
			pNode->pNext    = m_pUserListHead;
			m_pUserListHead = pNode;
			m_nUserSize++;
		}
		m_mtx.UnLock();

		return pObj;
	};
	void push(T *pObj)
	{
		NODE *pNode = (NODE*)pObj;
		m_mtx.Lock();
		pNode->bFree = true;
		if (pNode != m_pUserListHead)
		{
			pNode->pPrev->pNext = pNode->pNext;
			pNode->pNext->pPrev = pNode->pPrev;
		}
		else
			m_pUserListHead = m_pUserListHead->pNext;
		pNode->pPrev = NULL;
		pNode->pNext = m_pFreeListHead;
		if (m_pFreeListHead)
			m_pFreeListHead->pPrev = pNode;
		m_pFreeListHead = pNode;
		m_nUserSize--;
		m_mtx.UnLock();
		return;
	}

	T* at(size_t nPos)
	{
		if (nPos <= m_nCapacity)
		{
			T *pObj = &m_ObjectPool[nPos].Object;
			return pObj;
		}
		return NULL;
	}
	T* operator[](size_t nPos)
	{	
		T *pObj = &m_ObjectPool[nPos].Object;
		return pObj;
	}
	size_t capacity()
	{
		return m_nCapacity;
	}
	size_t size()
	{
		return m_nUserSize;
	}
private:
	TMutex	m_mtx;

	size_t  m_nCapacity;		//容量大小	
	size_t  m_nUserSize;		//当前已被使用对象数量
	NODE   *m_ObjectPool;		//对象池
	NODE   *m_pFreeListHead;	//空闲对象链头
	NODE   *m_pUserListHead;	//使用对象链头

};

};

#endif
