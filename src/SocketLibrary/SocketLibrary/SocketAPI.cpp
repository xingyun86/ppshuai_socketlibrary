//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "socketapi.h"
#include <mstcpip.h>
namespace SL
{

CSocketAPI::CSocketAPI(void)
{
}

CSocketAPI::~CSocketAPI(void)
{
}

BOOL CSocketAPI::LoadSocketLib(int version_high, int version_low)
{
	WSADATA wsaData;
    WORD version_requested = MAKEWORD(version_high, version_low);
	return (WSAStartup(version_requested,&wsaData) == 0);
}

BOOL CSocketAPI::UnloadSocketLib()
{
	WSACleanup();
	return TRUE; 
}

//取得客户端信息
BOOL CSocketAPI::GetSocketInfo(SOCKET hSocket, LPSTR szIPAddr, USHORT *nPort)
{
	sockaddr_in in;
	int nLen = sizeof(sockaddr);
	if (getpeername(hSocket,(sockaddr*)&in,&nLen) == SOCKET_ERROR)
	{
		return FALSE;
	}
	strcpy(szIPAddr,inet_ntoa(in.sin_addr));
	*nPort = ntohs(in.sin_port);
	return TRUE;
}

BOOL CSocketAPI::GetSocketKeepAlive(SOCKET hSocket)
{
	BOOL bRet = 0;
	int  nOptLen = sizeof(BOOL);
	return (SOCKET_ERROR != getsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bRet, &nOptLen));
}

BOOL CSocketAPI::SetSocketKeepAlive(SOCKET hSocket, BOOL bKeepAlive)
{
	return (SOCKET_ERROR != setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive)));
}

BOOL CSocketAPI::SetSocketKeepAliveValue(SOCKET hSocket, unsigned long nKeepAliveTime, unsigned long nKeepAliveInterval)
{
	tcp_keepalive inKeepAlive	= {0};
    tcp_keepalive outKeepAlive	= {0};
	unsigned long nInLen	= sizeof(tcp_keepalive);
	unsigned long nOutLen	= sizeof(tcp_keepalive);
	unsigned long nBytesReturn = 0;
     
    //设置socket的keepalivetime,keepaliveinterval
	inKeepAlive.onoff = 1;
    inKeepAlive.keepalivetime = nKeepAliveTime;
    inKeepAlive.keepaliveinterval = nKeepAliveInterval;
                   
    //为选定的SOCKET设置Keep Alive，成功后SOCKET可通过KeepAlive自动检测连接是否断开 
	return (SOCKET_ERROR != WSAIoctl(hSocket, SIO_KEEPALIVE_VALS, (LPVOID)&inKeepAlive, nInLen, (LPVOID)&outKeepAlive, nOutLen, &nBytesReturn, NULL, NULL));
}

BOOL CSocketAPI::SetSocketLinger(SOCKET hSocket, unsigned short nOnoff, unsigned short nLinger)
{
	linger l;
	l.l_onoff  = nOnoff;
	l.l_linger = nLinger;
	return (SOCKET_ERROR != setsockopt(hSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&l), sizeof(l)));
}

BOOL CSocketAPI::SetTimeOut(SOCKET hSocket,int nRecvTimeOut,int nSendTimeOut)
{
	setsockopt(hSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&nRecvTimeOut,sizeof(int));
	setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char*)&nSendTimeOut,sizeof(int));
	return TRUE;
}

BOOL CSocketAPI::SetBufferSize(SOCKET hSocket,int nRecvBufferSize,int nSendBufferSize)
{
	setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvBufferSize, sizeof(int));
	setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nSendBufferSize, sizeof(int));
	return TRUE;
}

BOOL CSocketAPI::SetTCPNODELAY(SOCKET hSocket,int nOpt)
{
	setsockopt(hSocket,IPPROTO_TCP,TCP_NODELAY,(char*)&nOpt,sizeof(int));
	return TRUE;
}

BOOL CSocketAPI::SetFIONBIO(SOCKET hSocket, int nOpt)
{
	ioctlsocket(hSocket, FIONBIO, (unsigned long*)&nOpt);
	return TRUE;
}

}