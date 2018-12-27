/*
* Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com)
* All rights reserved.
* 
* 文件名称：TcpClient.h
* 摘    要：TcpClient
* 作    者：张保元 bolidezhang@gmail.com
* 完成日期：2005/07/16
*
*/

#pragma once
#include <string>
#include "SocketSource.h"
#include "SocketSendPool.h"
#include "sync/threadmutex.h"
#include "sync/event.h"
#include "sync/thread.h"
#include <vector>
#include <utility>

namespace SL
{

class SOCKETLIBRARY_API CTcpClient : public CSocketSource
{
public:
	CTcpClient(void);
	virtual ~CTcpClient(void);

	void SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner);
	void SetBufferSize(int nRecvBufferSize, int nSendBufferSize);
	BOOL SetAutoConnect(BOOL bAutoConnect, int nCheckInterval=300000, int nReconnectSleep=1000);

	virtual BOOL DoOpen();
	BOOL AppendServerAddr(const char *szServerName, unsigned short nServerPort);
	BOOL RemoveServerAddr(int nPos);
	BOOL Open(const char *szServerName, unsigned short nServerPort, int nLocalPort=0);
	BOOL Close();

	BOOL IsConnected() const;
	int  Send(const char *szData, int nLen, int nOptval=0);

	SOCKET GetSocketHandle() const;
protected:
	BOOL PostRecv(SOCKET hSocket, void *p);
	BOOL PostSend(void *pPerIoData);

	BOOL Connect();
	BOOL Disconnect(SOCKET hSocket, BOOL bForce = FALSE);
	BOOL IsRealConnected();

protected:
	typedef std::pair<std::string,unsigned short> SERVER_ADDR;

	SOCKET	m_hSocket;
	//连接状态
	BOOL	m_bIsConnected;

	//自动重连
	BOOL				m_bAutoConnect;
	int					m_nCheckIntervalTime;	//检测连接的状态(ms)
	int					m_nReconnectSleepTime;  //连接断开时，重连的时间间隔(ms)
	SYNC::CThread		m_threadAutoConnect;
	SYNC::CEvent		m_eventAutoConnect;

	SYNC::CThreadMutex	m_lock;

	//发送缓冲池
	CSocketSendPool<SYNC::CThreadMutex>	m_sendPool;
	SOCKET_SESSION						m_SocketSession;

	//远程服务器名字:可以是IP地址(218.1.65.74)或域名(www.cga.com.cn)
	std::vector<SERVER_ADDR>	m_vectServerAddr;
	unsigned short				m_nLocalPort;
	unsigned short				m_nVectorPos;

private:
	static unsigned WINAPI AutoConnect(LPVOID lParam);

};

};
 