#include "StdAfx.h"
#include ".\testsocketevent.h"
#include "..\..\socketlibrary\slcommondef.h"
#include "..\..\socketlibrary\socketapi.h"
#include ".\testserverapp.h"

CTestSocketEvent::CTestSocketEvent(void)
{
}

CTestSocketEvent::~CTestSocketEvent(void)
{
}

BOOL CTestSocketEvent::OnAccept(CSocketSource *pSource, SOCKET hClientSocket)
{
	SL::CSocketAPI::SetSocketKeepAlive(hClientSocket,1);
	SL::CSocketAPI::SetSocketKeepAliveValue(hClientSocket,900000,10000);
	return TRUE;
}

BOOL CTestSocketEvent::OnDisconnect(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo)
{
	return TRUE;
}

int CTestSocketEvent::OnRecv(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo, const char *szData, int nLen)
{
	//printf("tcprecv Socket:%ld msg:(%s) len:%d\r\n", hClientSocket, szData, nLen);
	//int nRet = pSource->Send(hClientSocket, szData, nLen);
	//printf("Socket:%ld sendmsg:(%s) len:%d\r\n", hClientSocket, szData, nRet);

	m_pApp->m_taskTest.PostMessage(szData, nLen, hClientSocket, pAttachInfo);
	return 1;
}

int CTestSocketEvent::OnRecv(CSocketSource *pSource, const char *szData, int nLen, 
							 sockaddr *pFrom, int nFromLen)
{
	printf("udprecv msg:(%s) len:%d\r\n", szData, nLen);

	//将收到的数据直接发回客户
	int nRet = pSource->Send(szData, nLen, pFrom, nFromLen);
	return 1;
}
