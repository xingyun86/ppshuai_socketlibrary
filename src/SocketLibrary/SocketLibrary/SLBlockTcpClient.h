#pragma once

#include "config.h"
namespace SL
{

class SOCKETLIB_API CSLBlockTcpClient
{
public:
	CSLBlockTcpClient(void);
	CSLBlockTcpClient(SOCKET hSocket);
	virtual ~CSLBlockTcpClient(void);

	bool Close();
	bool Open(const char *szServerName, unsigned short nServerPort, int nLocalPort);
	bool SetTimeOut(int nRecvTimeOut,int nSendTimeOut);
	bool SetBufferSize(int nRecvBufferSize,int nSendBufferSize);

	int  Recv(char *szData, int nLen);
	int  Recv(char *szData, int nLen, int nRecvLen);
	int  Send(const char *szData, int nLen);

	void	Attach(SOCKET hSocket);
	SOCKET	Detach();

	SOCKET  GetSocketHandle() const;
private:
	SOCKET	m_hSocket;

};

};