#include "SLBlockTcpClient.h"
#include "SocketAPI.h"

namespace SL
{

CSLBlockTcpClient::CSLBlockTcpClient(void)
{
	m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}

CSLBlockTcpClient::CSLBlockTcpClient(SOCKET hSocket)
	: m_hSocket(hSocket)
{
}

CSLBlockTcpClient::~CSLBlockTcpClient(void)
{
	Close();
}

inline bool CSLBlockTcpClient::Close()
{
	if (m_hSocket != NULL)
	{
		shutdown(m_hSocket,SD_BOTH);
		closesocket(m_hSocket);
		m_hSocket = NULL;
	}
	return true;
}

bool CSLBlockTcpClient::Open(const char *szServerName, unsigned short nServerPort, int nLocalPort)
{
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;	
	saddr.sin_addr.S_un.S_addr = inet_addr(szServerName);
	saddr.sin_port = htons(nServerPort); 
	int nRet = connect(m_hSocket,(SOCKADDR*)&saddr,sizeof(saddr));
	if (SOCKET_ERROR == nRet)
		return false;
	return true;
}

bool CSLBlockTcpClient::SetTimeOut(int nRecvTimeOut,int nSendTimeOut)
{
	CSocketAPI::SetTimeOut(m_hSocket,nRecvTimeOut,nSendTimeOut);
	return true;
}

bool CSLBlockTcpClient::SetBufferSize(int nRecvBufferSize,int nSendBufferSize)
{
	CSocketAPI::SetBufferSize(m_hSocket,nRecvBufferSize,nSendBufferSize);
	return true;
}

int CSLBlockTcpClient::Recv(char *szData,int nLen)
{
	int nRecvLen = recv(m_hSocket,szData,nLen,0);
	return nRecvLen;
}

int CSLBlockTcpClient::Recv(char *szData, int nLen, int nRecvLen)
{
	if (nLen < nRecvLen)
		return 0;

	int nSingle = 0;
	int nSum = 0;
	while (nSum < nRecvLen)
	{
		nSingle = recv(m_hSocket,szData,nLen,0);
		if (SOCKET_ERROR == nSingle)
			return nSum;
		else
			nSum += nSingle;
	}
	return nSum;
}

int CSLBlockTcpClient::Send(const char *szData, int nLen)
{
	int nSingle	= 0;
	int nSum	= 0;
	while (nSum < nLen)
	{
		nSingle = send(m_hSocket,szData+nSum,nLen-nSum,0);
		if (SOCKET_ERROR == nSingle)
			return nSum;
		else
			nSum += nSingle;
	}
	return nLen;
}

void CSLBlockTcpClient::Attach(SOCKET hSocket)
{
	Close();
	m_hSocket = hSocket;
}

SOCKET CSLBlockTcpClient::Detach()
{	
	SOCKET hSocket = m_hSocket;
	m_hSocket = NULL;
	return hSocket;
}

SOCKET CSLBlockTcpClient::GetSocketHandle() const
{
	return m_hSocket;
}

}