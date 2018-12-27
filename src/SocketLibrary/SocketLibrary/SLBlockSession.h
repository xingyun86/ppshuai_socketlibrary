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
#include "sync/event.h"
#include "SLBlockSessionMgr.h"

namespace SL
{

//组塞式调用的会话类
class SOCKETLIB_API CSLBlockSession
{
protected:
	virtual bool DoResult(const char *szMsg, int nLen)
	{
		return true;
	};
public:
	CSLBlockSession(CSLBlockSessionMgr *pSessionMgr)
		: m_oEvent(true, false)
		, m_pSessionMgr(pSessionMgr)
	{
		m_nKeyID = pSessionMgr->PushSession(this);
	};
	virtual ~CSLBlockSession(void)
	{
		m_pSessionMgr->RemoveSession(m_nKeyID);
	};
	bool Wait(DWORD nTimeOut)
	{
		return m_oEvent.Lock(nTimeOut);
	};
	bool Result(const char *szMsg, int nLen)
	{
		if (DoResult(szMsg, nLen))
			return m_oEvent.Signal();
		else
			return false;
	};
	ulong GetKeyID()
	{
		return m_nKeyID;
	}
protected:
	SYNC::CEvent	m_oEvent;
	ulong			m_nKeyID;  //唯一键值

	CSLBlockSessionMgr *m_pSessionMgr;
};

};
