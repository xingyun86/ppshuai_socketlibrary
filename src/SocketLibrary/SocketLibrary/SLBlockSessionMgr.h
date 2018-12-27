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
#include "config.h"
#include "sync/threadmutex.h"
#include <map>

namespace SL
{

//组塞式调用的会话管理类
class CSLBlockSession;
class SOCKETLIB_API CSLBlockSessionMgr
{
public:
	CSLBlockSessionMgr(void)
	{
		m_bOwner	= false;
		m_nMaxKeyID = 0;
	};
	CSLBlockSessionMgr(bool bOwner)
	{
		m_bOwner	= bOwner;
		m_nMaxKeyID = 0;
	};
	virtual ~CSLBlockSessionMgr(void)
	{
		Clear();
	};
	void Clear()
	{
		m_lock.Lock();
		if (m_bOwner)
		{
			std::map<ulong, CSLBlockSession*>::iterator itItem;
			for (itItem = m_mapSession.begin(); itItem != m_mapSession.end(); ++itItem)
			{
				CSLBlockSession *pSession = itItem->second;
				delete pSession;
			}
		}
		m_mapSession.clear();
		m_lock.UnLock();
	};
	ulong PushSession(CSLBlockSession *pSession)
	{		
		ulong nKeyID;
		m_lock.Lock();
		nKeyID = m_nMaxKeyID;
		m_mapSession[nKeyID] = pSession;
		m_nMaxKeyID++;
		m_lock.UnLock();
		return nKeyID;
	};
	void RemoveSession(ulong nKeyID)
	{
		m_lock.Lock();
		m_mapSession.erase(nKeyID);
		m_lock.UnLock();
		return;
	};
	CSLBlockSession* Find(ulong nKeyID)
	{
		CSLBlockSession *pSession = NULL;
		m_lock.Lock();
		std::map<ulong, CSLBlockSession* >::iterator itItem;
		itItem = m_mapSession.find(nKeyID);
		if (itItem != m_mapSession.end())
		{
			pSession = itItem->second;
		}
		m_lock.UnLock();
		return pSession;
	}
private:
	std::map<ulong, CSLBlockSession*>	m_mapSession;
	SYNC::CThreadMutex					m_lock;
	ulong								m_nMaxKeyID; //当前最大键值

	//true:表示拥有所有权
	bool	m_bOwner;
};

};
