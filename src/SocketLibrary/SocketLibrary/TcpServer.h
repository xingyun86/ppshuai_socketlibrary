/*
* Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com)
* All rights reserved.
* 
* 文件名称：TcpServer.h
* 摘    要：TcpServer
* 作    者：张保元 bolidezhang@gmail.com
* 完成日期：2005/07/16
*
*/

#pragma once

#ifndef USE_HASHMAP
	#include <map>
#else
	#include <hash_map>
#endif
#include <list>

#include "SocketSource.h"
#include "SocketSendPool.h"
#include "sync/Thread.h"
#include "sync/threadmutex.h"

namespace SL
{

class SOCKETLIBRARY_API CTcpServer : public CSocketSource
{
public:

#ifndef USE_HASHMAP
	typedef std::map<SOCKET,PSOCKET_SESSION> SocketSessionMap;
	typedef std::map<SOCKET,PSOCKET_SESSION>::iterator SocketSessionIterator;
#else
	typedef stdext::hash_map<SOCKET,PSOCKET_SESSION> SocketSessionMap;
	typedef stdext::hash_map<SOCKET,PSOCKET_SESSION>::iterator SocketSessionIterator;
#endif

public:
	CTcpServer(void);
	virtual ~CTcpServer(void);

	void SetConfig(int nMaxSocketNum, int nMaxFreePoolNum, int nDefaultFreePoolNum);
	void SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner);
	void SetBufferSize(int nRecvBufferSize, int nSendBufferSize);

	BOOL Open(const char *szLocalIP, unsigned short nPort, int nBacklog = 10);
	BOOL Close();

	BOOL IsConnected(SOCKET hClientSocket);
	BOOL Disconnect(SOCKET hClientSocket, bool bForce = false);
	int  Send(SOCKET hClientSocket, const char *szData, int nLen, int nOptval=0);

	//socket的附加信息指针
	BOOL  SetAttachInfo(SOCKET hClientSocket, void *pAttachInfo);
	void* GetAttachInfo(SOCKET hClientSocket);

protected:
	BOOL PostRecv(SOCKET hClientSocket, void *p);
	BOOL PostSend(void *pPerIoData);

	static unsigned int WINAPI ListenProc(LPVOID lParam);

protected:

	SOCKET			m_hListenSocket;

	//临界保护量
	SYNC::CThreadMutex			m_lock;

	//当前进程堆
	HANDLE						m_hProcessHeap;
	//空闲对象池
	std::list<PSOCKET_SESSION>	m_lstFreePool;
	//发送缓冲池
	CSocketSendPool<SYNC::CThreadMutex>	m_sendPool;

	//Socket最大连接数
	int	m_nMaxSocketNum;
	//最大空闲池的数量
	int m_nMaxFreePoolNum;
	//默认空闲池的数量
	int m_nDefaultFreePoolNum;

	//listen线程
	SYNC::CThread m_threadListen;

	//当前在线连接
	SocketSessionMap			m_mapSocketSession;
};

};
