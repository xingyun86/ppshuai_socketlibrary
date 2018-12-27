//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#pragma once
#include <list>

namespace SLEXT
{

template <typename T, typename TMutex>
class CSmallObjectPool
{
private:
    CSmallObjectPool(const CSmallObjectPool &);
    void operator= (const CSmallObjectPool &);

public:
	CSmallObjectPool()
		: m_nMaxCount(100)
		, m_nGrowCount(1)
	{
	};
	virtual ~CSmallObjectPool()
	{
		clear();
	};
	void clear()
	{
		m_mtx.Lock();
		for (std::list<T*>::iterator itr=m_lstFreeObj.begin(); itr!=m_lstFreeObj.end(); itr++)
		{
			delete *itr;
		}
		m_lstFreeObj.clear();
		m_mtx.UnLock();
	};
	void init(unsigned int nMaxCount=100, unsigned int nInitCount=0, unsigned int nGrowCount=1)
	{
		if (nGrowCount<1)
			nGrowCount = 1;
		clear();

		m_mtx.Lock();
		m_nMaxCount  = nMaxCount;
		m_nGrowCount = nGrowCount;
		Alloc(nInitCount);
		m_mtx.UnLock();
	};

	T* pop()
	{
        T *pObj = NULL;
		m_mtx.Lock();
		if (!m_lstFreeObj.empty())
        {
            pObj = m_lstFreeObj.front();
            m_lstFreeObj.pop_front();
        }
		else
		{
			pObj = new T();
			Alloc(m_nGrowCount);
		};
		m_mtx.UnLock();
		return pObj;
	};
    void push(T* pObj)
    {
		m_mtx.Lock();
		if (m_lstFreeObj.size() < m_nMaxCount)
			m_lstFreeObj.push_back(pObj);
		else
			delete pObj;
		m_mtx.UnLock();
    };

private:
	bool Alloc(unsigned int nCount)
	{
		T *pObj = NULL;
		for (unsigned int i = 0; i<nCount; i++)
		{			
			pObj = new T();
			m_lstFreeObj.push_back(pObj);
		}
		return true;
	};

private:
	unsigned int  m_nMaxCount;   //对象池最大容量
	unsigned int  m_nGrowCount;  //增长幅度

	TMutex		  m_mtx;
	std::list<T*> m_lstFreeObj;
};

template <typename T, typename TMutex>
class CSmallObjectPoolEx
{
private:
    CSmallObjectPoolEx(const CSmallObjectPoolEx &);
    void operator= (const CSmallObjectPoolEx &);

public:
	CSmallObjectPoolEx()
		: m_nMaxCount(100)
		, m_nGrowCount(1)
	{
	};
	virtual ~CSmallObjectPoolEx()
	{
		clear();
	};
	void clear()
	{
		m_mtx.Lock();
		m_lstFreeObj.clear();
		if (m_lstAllObj.size() > 0)
		{
			for(std::list<T*>::iterator itr=m_lstAllObj.begin(); itr!=m_lstAllObj.end(); itr++)
			{
				delete [](*itr);
			}
			m_lstAllObj.clear();
		}
        m_mtx.UnLock();
	};
	void init(unsigned int nMaxCount=100, unsigned int nInitCount=0, unsigned int nGrowCount=1)
	{
		if (nGrowCount<1)
			nGrowCount = 1;
		clear();
		m_mtx.Lock();
		m_nMaxCount  = nMaxCount;
		m_nInitCount = nInitCount;
		m_nGrowCount = nGrowCount;
		Alloc(nInitCount);
		m_mtx.UnLock();
	};

	T* pop()
	{
		T* pObj = NULL;
		m_mtx.Lock();
		if (!m_lstFreeObj.empty())
		{
			pObj = m_lstFreeObj.front();
			m_lstFreeObj.pop_front();
			m_mtx.UnLock();
			return pObj;
		}
		if (!Alloc(m_nGrowCount))
		{
			m_mtx.UnLock();
			return NULL;
		}
		pObj = m_lstFreeObj.front();
		m_lstFreeObj.pop_front();
		m_mtx.UnLock();
		return pObj;
	};
    void push(T* pObj)
    {
		m_mtx.Lock();
		m_lstFreeObj.push_back(pObj);
		m_mtx.UnLock();
    };

private:
	bool Alloc(unsigned int nCount)
	{
		T *pObj = NULL;
		pObj	= new T[nCount];
		if (NULL == pObj)
			return false;
		for(unsigned int i = 0; i<nCount; i++)
		{
			m_lstFreeObj.push_back(&pObj[i]);
		}
        m_lstAllObj.push_back(pObj);
		return true;
	};

private:
	unsigned int  m_nMaxCount;   //对象池最大容量
	unsigned int  m_nInitCount;  //初始对象数
	unsigned int  m_nGrowCount;  //增长幅度

	TMutex		  m_mtx;
	std::list<T*> m_lstFreeObj;
	std::list<T*> m_lstAllObj;

};

};