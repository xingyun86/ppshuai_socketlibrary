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
#include "socketevent.h"
#include "sync/threadmutex.h"
#include "slext/smallobjectpool.h"

namespace SL
{

template <class TSession>
class CSLTcpSessionEvent : public CSocketEvent
{
public:
	CSLTcpSessionEvent(void)
	{
		m_sessionPool.init(1000,0,1);
	};

	virtual ~CSLTcpSessionEvent(void)
	{
		m_sessionPool.clear();
	};

	virtual bool OnAccept(CSocketSource *pSource, SOCKET hClientSocket)
	{
		return true;
	};

	virtual bool OnInitSocket(PSOCKET_SESSION pSocketSession)
	{
		TSession *p = m_sessionPool.pop();
		if (p == NULL)
			return false;

		p->Init(pSocketSession, pSocketSession->PerHandle.hSocket, 1024, 4);
		return true;
	};
	virtual bool OnUninitSocket(void *pAttachInfo) 
	{ 
		TSession *p = (TSession*)pAttachInfo;
		p->Uninit();
		m_sessionPool.push(p);
		return true; 
	};

	virtual bool OnDisconnect(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo)
	{
		OnUninitSocket(pAttachInfo);
		return true;
	};

	virtual int OnRecv(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo, const char *szData, int nLen)
	{
		TSession *p = (TSession*)pAttachInfo;
		return p->OnRecv(szData, nLen);
	};

protected:
	SLEXT::CSmallObjectPool<TSession,SYNC::CThreadMutex> m_sessionPool;

};

};
