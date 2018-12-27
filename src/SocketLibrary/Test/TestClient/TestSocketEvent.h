#pragma once
#include "../../socketlibrary/socketevent.h"
#include "../../socketlibrary/socketsource.h"

using namespace SL;
class CTestSocketEvent : public CSocketEvent
{
public:
	CTestSocketEvent(void);
	virtual ~CTestSocketEvent(void);

	BOOL OnAccept(CSocketSource *pSource, SOCKET hClientSocket);
	BOOL OnDisconnect(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo);
	int  OnRecv(CSocketSource *pSource, SOCKET hClientSocket, void *pAttachInfo, const char *szData, int nLen);

	int  OnRecv(CSocketSource *pSource, const char *szData, int nLen, sockaddr *pFrom, int nFromLen);

private:
	int nUdpRecv,nTcpRecv;
};
