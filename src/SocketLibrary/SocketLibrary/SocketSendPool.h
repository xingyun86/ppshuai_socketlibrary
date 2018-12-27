#pragma once
#include <list>
#include "socketlib.h"
#include "sync/threadmutex.h"

namespace SL
{

template <class TMutex>
class CSocketSendPool
{
public:
	CSocketSendPool(void)
	{
		init();
	}
	virtual ~CSocketSendPool(void)
	{
		clear();
	}
	void clear()
	{
		m_lock.Lock();
		std::list<PPER_IO_OPERATION_DATA>::iterator itr = m_objFreeList.begin();
        for (; itr != m_objFreeList.end(); ++itr)
        {
			HeapFree(m_hProcessHeap, 0, (void*)*itr);
        }
		m_objFreeList.clear();
		m_lock.UnLock();
	};
    void init(unsigned int nMaxCount=100, unsigned int nInitCount=100, unsigned int nGrowCount=4)
	{
		clear();
		m_hProcessHeap	= GetProcessHeap();
		m_nMaxCount		= nMaxCount;
		m_nGrowCount	= nGrowCount;
	};
	PPER_IO_OPERATION_DATA pop(unsigned int nBufferSize=0)
	{
        PPER_IO_OPERATION_DATA pObj;
		char *p;
		m_lock.Lock();
		if (!m_objFreeList.empty())
        {
            pObj = m_objFreeList.front();
            m_objFreeList.pop_front();
			m_lock.UnLock();

			if (nBufferSize > 0)
			{
				if (pObj->nLen < nBufferSize)
				{
					p = (char*)HeapReAlloc(m_hProcessHeap, 
						HEAP_ZERO_MEMORY, 
						(void*)pObj,
						(sizeof(PER_IO_OPERATION_DATA)+nBufferSize));
					if (NULL == p)
						return NULL;

					pObj = (PPER_IO_OPERATION_DATA)p;
					pObj->OperType = SEND_POSTED;
					pObj->szBuffer = (p+sizeof(PER_IO_OPERATION_DATA));
					pObj->nLen	   = nBufferSize;
				}
			}
        }
		else
		{
			m_lock.UnLock();

			p = (char*)HeapAlloc(m_hProcessHeap, 
				HEAP_ZERO_MEMORY, 
				(sizeof(PER_IO_OPERATION_DATA)+nBufferSize));
			if (NULL == p)
				return NULL;
			else
			{
				pObj = (PPER_IO_OPERATION_DATA)p;
				pObj->OperType = SEND_POSTED;
				if (nBufferSize > 0)
				{
					pObj->szBuffer = (p+sizeof(PER_IO_OPERATION_DATA));
					pObj->nLen	   = nBufferSize;
				}
				else
					pObj->szBuffer = NULL;
			}
		};
		return pObj;
	};
    void push(PPER_IO_OPERATION_DATA pObj)
    {
		m_lock.Lock();
		if (m_objFreeList.size() < m_nMaxCount)
		{
			m_objFreeList.push_back(pObj);
			m_lock.UnLock();
		}
		else
		{
			m_lock.UnLock();
			HeapFree(m_hProcessHeap, 0, (void*)pObj);
		}
    };

private:
	std::list<PPER_IO_OPERATION_DATA> m_objFreeList;
	unsigned int	m_nMaxCount;
	unsigned int	m_nGrowCount;

	HANDLE			m_hProcessHeap;
	TMutex			m_lock;
};

};