#include "StdAfx.h"
#include ".\testsocketevent.h"
#include "..\..\socketlibrary\slcommondef.h"
//#include <>

CTestSocketEvent::CTestSocketEvent(void)
{
	nUdpRecv = 0;
	nTcpRecv = 0;
}

CTestSocketEvent::~CTestSocketEvent(void)
{
}

BOOL CTestSocketEvent::OnAccept(CSocketSource *pSource, SOCKET hClientSocket)
{
	return TRUE;
}

BOOL CTestSocketEvent::OnDisconnect(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo)
{
	return TRUE;
}

int CTestSocketEvent::OnRecv(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo, const char *szData, int nLen)
{
	//printf("Server return msg:(%s) len:%d\r\n", szData, nLen);
	nTcpRecv++;
	printf("tcp Srv return:(%s,%ld),totalrecv:%ld\n",szData,nLen,nTcpRecv);
	return 1;
}

int CTestSocketEvent::OnRecv(CSocketSource *pSource, const char *szData, int nLen, 
							 sockaddr *pFrom, int nFromLen)
{
	nUdpRecv++;
	printf("udp Srv return:(%s,%ld),totalrecv:%ld\n",szData,nLen,nUdpRecv);
	return 1;
}
