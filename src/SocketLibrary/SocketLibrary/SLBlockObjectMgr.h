//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlibrary is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#pragma once
#include "config.h"
#include "sync/threadmutex.h"
#ifndef USE_HASHMAP
	#include <map>
#else
	#include <hash_map>
#endif

namespace SL
{

//组塞式调用的会话管理类
class CSLBlockObject;
class SOCKETLIBRARY_API CSLBlockObjectMgr
{
public:

#ifndef USE_HASHMAP
	typedef std::map<ulong, CSLBlockObject*> BlockMap;
	typedef std::map<ulong, CSLBlockObject*>::iterator BlockIterator;
#else
	typedef stdext::hash_map<ulong, CSLBlockObject*> BlockMap;
	typedef stdext::hash_map<ulong, CSLBlockObject*>::iterator BlockIterator;
#endif

	CSLBlockObjectMgr(void)
	{
		m_bOwner	= false;
		m_nMaxKeyID = 0;
	};
	CSLBlockObjectMgr(bool bOwner)
	{
		m_bOwner	= bOwner;
		m_nMaxKeyID = 0;
	};
	virtual ~CSLBlockObjectMgr(void)
	{
		Clear();
	};
	void Clear()
	{
		m_lock.Lock();
		if (m_bOwner)
		{
			BlockIterator itItem;
			for (itItem = m_map.begin(); itItem != m_map.end(); ++itItem)
			{
				CSLBlockObject *p = itItem->second;
				delete p;
			}
		}
		m_map.clear();
		m_lock.UnLock();
	};
	ulong Push(CSLBlockObject *p)
	{		
		ulong nKeyID;
		m_lock.Lock();
		nKeyID = m_nMaxKeyID;
		m_map.insert(BlockMap::value_type(nKeyID,p));
		m_nMaxKeyID++;
		m_lock.UnLock();
		return nKeyID;
	};
	void Remove(ulong nKeyID)
	{
		m_lock.Lock();
		m_map.erase(nKeyID);
		m_lock.UnLock();
		return;
	};
	CSLBlockObject* Find(ulong nKeyID)
	{
		CSLBlockObject *p = NULL;
		m_lock.Lock();
		BlockIterator itItem;
		itItem = m_map.find(nKeyID);
		if (itItem != m_map.end())
		{
			p = itItem->second;
		}
		m_lock.UnLock();
		return p;
	}
private:
	bool				m_bOwner;    //true:表示拥有所有权
	ulong				m_nMaxKeyID; //当前最大键值
	SYNC::CThreadMutex	m_lock;
	BlockMap			m_map;
};

};
