/*
* Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com)
* All rights reserved.
* 
* 文件名称：SocketSource.h
* 摘    要：Socket源接口类
* 作    者：张保元 bolidezhang@gmail.com
* 完成日期：2005/07/16
*
*/

#pragma once
#include "AppLog.h"
#include "SocketRunner.h"
#include "SocketLib.h"
#include <string>

namespace SL
{

class CSocketEvent;
class SOCKETLIBRARY_API CSocketSource
{
public:
	CSocketSource(void);
	virtual ~CSocketSource(void);

	virtual BOOL Open(const char *szServerName, unsigned short nPort, int nOption) = 0;
	virtual BOOL Close() = 0;
	virtual BOOL PostSend(void *pPerIoData) { return TRUE; };

	//TCP时用到
	virtual BOOL PostRecv(SOCKET hSocket, void *pPerIoData) { return TRUE; };
	virtual BOOL Disconnect(SOCKET hSocket, bool bForce=false) { return TRUE; };

	//设置接口
	virtual void SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner){ return; };
	//设置缓冲区大小
	virtual void SetBufferSize(int nRecvBufferSize, int nSendBufferSize){ return; };
	//设置socket的附加信息指针
	virtual BOOL SetAttachInfo(SOCKET hSocket, void *pAttachInfo) { return TRUE; };

	//TCPServer
	virtual int  Send(SOCKET hSocket, const char *szData, int nLen, int nOptval=0) { return 0; };

	//TCPClient/UDP
	virtual int  Send(const char *szData, int nLen, int nOptval=0) { return 0; };

	//UDP用到
	virtual BOOL PostRecv(void *pPerIoRecv) { return TRUE; };
	virtual int  Send(const char *szData, int nLen, sockaddr *pTo, int nToLen, int nOptval=0) { return 0; };
	virtual int  Send(const char *szData, int nLen, const char *szServerIP, unsigned short nPort, int nOptval=0) { return 0; };

public:
	CAppLog		   *m_pAppLog;
	CSocketEvent   *m_pSocketEvent;
	CSocketRunner  *m_pSocketRunner; 

protected:
	//接收缓冲区的大小
	//(一般为8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536)
	int				m_nRecvBufferSize;
	//发送缓冲区的大小
	int				m_nSendBufferSize;

};

};
