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
#include "sync/event.h"
#include "SLBlockObjectMgr.h"

namespace SL
{

//组塞式调用的对象类
class SOCKETLIBRARY_API CSLBlockObject
{
protected:
	virtual bool DoResult(const char *szMsg, int nLen)
	{
		return true;
	};
public:
	CSLBlockObject(CSLBlockObjectMgr *pMgr)
		: m_oEvent(true, false)
		, m_pMgr(pMgr)
	{
		m_nKeyID = pMgr->Push(this);
	};
	CSLBlockObject()
		: m_oEvent(true, false)
	{
	};
	virtual ~CSLBlockObject(void)
	{
		m_oEvent.Reset();
		m_pMgr->Remove(m_nKeyID);
	};
	void SetBlockObjectMgr(CSLBlockObjectMgr *pMgr)
	{
		m_pMgr = pMgr;
		m_nKeyID = m_pMgr->Push(this);
	}
	BOOL Wait(DWORD nTimeOut)
	{
		return m_oEvent.Lock(nTimeOut);
	};
	BOOL Result(const char *szMsg, int nLen)
	{
		if (DoResult(szMsg, nLen))
			return m_oEvent.Signal();
		else
			return FALSE;
	};
	ulong GetKeyID()
	{
		return m_nKeyID;
	}
protected:
	SYNC::CEvent	m_oEvent;
	ulong			m_nKeyID;  //唯一键值

	CSLBlockObjectMgr  *m_pMgr;
};

};
