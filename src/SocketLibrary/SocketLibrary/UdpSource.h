//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#pragma once
#include <list>
#include "SocketSource.h"
#include "SocketSendPool.h"
#include "sync/threadmutex.h"

namespace SL
{

class SOCKETLIBRARY_API CUdpSource : public CSocketSource
{
public:
	CUdpSource();
	CUdpSource(int nMaxFreeRecvPoolNum, int nMaxFreeSendPoolNum);
	virtual ~CUdpSource(void);

	void SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner);
	void SetBufferSize(int nRecvBufferSize, int nSendBufferSize);

	BOOL Open(const char *szServerName, unsigned short nServerPort, int nLocalPort, const char *szLocalIP);
	BOOL Open(const char *szServerName, unsigned short nServerPort, int nLocalPort);
	BOOL Open(unsigned short nLocalPort, const char *szLocalIP = NULL);
	BOOL Close();

	int  Send(const char *szData, int nLen, sockaddr *pTo, int nToLen, int nOptval=0);
	int  Send(const char *szData, int nLen, const char *szIP, unsigned short nPort, int nOptval=0);
	int  Send(const char *szData, int nLen, int nOptval=0);

protected:
	PPER_IO_OPERATION_UDP_RECV GetRecvObj();
	void FreeRecvObj(PPER_IO_OPERATION_UDP_RECV p);

	BOOL PostRecv(void *p);
	BOOL PostSend(void *p);

protected:
	SOCKET			 m_hSocket;
	sockaddr_in		 m_ServerAddr;   //远程服务器地址
	PER_HANDLE_DATA  m_perHandle;

	//最大空闲池的数量
	int m_nMaxFreeRecvPoolNum;
	int m_nMaxFreeSendPoolNum;

	//存储池
	std::list<PPER_IO_OPERATION_UDP_RECV> m_lstFreeRecvPool;  //数据接收内存池
	CSocketSendPool<SYNC::CThreadMutex>	  m_sendPool;

	//锁
	SYNC::CThreadMutex m_lock;
	SYNC::CThreadMutex m_lockRecv;
};

};
