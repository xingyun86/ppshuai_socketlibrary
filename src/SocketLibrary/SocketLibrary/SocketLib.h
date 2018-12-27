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

namespace SL
{

#define DEFAULT_BUFFER_SIZE       8192
#define RECV_BUFFER_SIZE          8192
#define SEND_BUFFER_SIZE          8192
#define MAX_SOCKET_CONNECTNUM   500000
#define MAX_SOCKET_FREENUM      500000
#define MAX_LOGBUFFER_LENGTH      1024

//IPV4地址的最大长度,如192.168.128.210
#define MAX_IPADDR_LENGTH		   15

enum SEND_OPTION
{
	SEND_OPTION_BLOCK = 1,         //阻塞式发送
	SEND_OPTION_SEGMENT = 2,	   //分段发送
	SEND_OPTION_USERBUFFER = 4	   //使用IOCP缓冲
};

enum IOCP_OPERTYPE
{
	RECV_POSTED = 1,
	SEND_POSTED = 2,
	ACCEPT_POSTED = 3
};

//单句柄数据
class CSocketSource;
typedef struct _PER_HANDLE_DATA
{
	SOCKET		    hSocket;
	CSocketSource  *pSocketSource;  //SocketSource指针
	void		   *pAttachInfo;	//上层附加信息指针
}PER_HANDLE_DATA,*PPER_HANDLE_DATA;

//单I/O操作数据
typedef struct _PER_IO_OPERATION_DATA
{
	//重叠结构
	OVERLAPPED Overlapped;

	//操作类型表示
	IOCP_OPERTYPE OperType;

	//数据缓冲区
	int	   nLen;
	char  *szBuffer;
}PER_IO_OPERATION_DATA,*PPER_IO_OPERATION_DATA;

//单个Socket会话结构
typedef struct _SOCKET_SESSION
{
	PER_HANDLE_DATA		   PerHandle;   //单句柄数据
	PER_IO_OPERATION_DATA  PerIoRecv;   //接收(I/O)
}SOCKET_SESSION,*PSOCKET_SESSION;


// UDP完成端口的用到结构体

//单I/O操作数据
typedef struct _PER_IO_OPERATION_UDP_RECV
{
	//重叠结构
	OVERLAPPED Overlapped;

	//操作类型表示
	IOCP_OPERTYPE OperType;

	//数据缓冲区
	int	   nLen;
	char  *szBuffer;

	//发送方地址
    sockaddr_in		 addr;
    int              addrlen;
}PER_IO_OPERATION_UDP_RECV,*PPER_IO_OPERATION_UDP_RECV;

};
