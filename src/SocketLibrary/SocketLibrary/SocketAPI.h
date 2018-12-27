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
namespace SL
{

class SOCKETLIBRARY_API CSocketAPI
{
private:
	CSocketAPI(void);
	~CSocketAPI(void);

public:
	//通过Socket值取得IP地址和Port端口
	static BOOL	GetSocketInfo(SOCKET hSocket, LPSTR szIPAddr, USHORT *nPort);

	static BOOL LoadSocketLib(int version_high, int version_low);
	static BOOL UnloadSocketLib();

	/*
	从windows2000开始引入一个新的I/O控制命令SIO_KEEPALIVE_VALS，可通过WSAIoctl()针对
	单个套接字设置是否启用保活机制、KeepAliveTime、KeepAliveInterval。
	Argument structure for SIO_KEEPALIVE_VALS
	struct tcp_keepalive {
		u_long  onoff;
		u_long  keepalivetime;
		u_long  keepaliveinterval;
	};
	*/
	static BOOL GetSocketKeepAlive(SOCKET hSocket);
	static BOOL SetSocketKeepAlive(SOCKET hSocket, BOOL bKeepAlive);
	static BOOL SetSocketKeepAliveValue(SOCKET hSocket, unsigned long nKeepAliveTime=7200000, unsigned long nKeepAliveInterval=1000);

	//SO_LINGER
	static BOOL SetSocketLinger(SOCKET hSocket, unsigned short nOnoff, unsigned short nLinger);

	//timeout and buffer
	static BOOL SetTimeOut(SOCKET hSocket,int nRecvTimeOut,int nSendTimeOut);
	static BOOL SetBufferSize(SOCKET hSocket,int nRecvBufferSize,int nSendBufferSize);

	//设置是否启用Nagle算法
	//Set the nodelay TCP option for real-time games
	static BOOL SetTCPNODELAY(SOCKET hSocket,int nOpt);

	//设置是否将Socket置入非阻塞式模式
	static BOOL SetFIONBIO(SOCKET hSocket,int nOpt);
};

};
