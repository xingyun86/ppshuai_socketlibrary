//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "tcpclient.h"
#include "SocketEvent.h"
#include <assert.h>
#include "sync/guard.h"

namespace SL
{

CTcpClient::CTcpClient(void)
	: CSocketSource()
	, m_bIsConnected(false)
	, m_bAutoConnect(false)
	, m_hSocket(INVALID_SOCKET)
	, m_nLocalPort(0)
	, m_nReconnectSleepTime(100)
	, m_nVectorPos(0)
{
	ZeroMemory(&m_SocketSession, sizeof(SOCKET_SESSION));
	m_SocketSession.PerHandle.pSocketSource = this;
	m_SocketSession.PerHandle.pAttachInfo   = NULL;
	m_SocketSession.PerIoRecv.szBuffer		= NULL;
	m_sendPool.init();
	m_vectServerAddr.reserve(3);
}

CTcpClient::~CTcpClient(void)
{
	SetAutoConnect(false);
	Close();
	m_vectServerAddr.clear();
	HeapFree(GetProcessHeap(), 0, m_SocketSession.PerIoRecv.szBuffer);
	m_SocketSession.PerIoRecv.szBuffer = NULL;
}

inline BOOL CTcpClient::IsConnected() const 
{
	return m_bIsConnected;
}

inline BOOL CTcpClient::Disconnect(SOCKET hSocket, BOOL bForce)
{
	Close();
#ifdef SHOW_PRINTFINFO
	printf("tcpClient disconnect, server:%s port:%d!\r\n", m_vectServerAddr[m_nVectorPos].first.c_str(),m_vectServerAddr[m_nVectorPos].second);
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	char szLog[MAX_LOGBUFFER_LENGTH] = {0};
	sprintf(szLog, "tcpClient disconnect, server:%s port:%d!\r\n", m_vectServerAddr[m_nVectorPos].first.c_str(),m_vectServerAddr[m_nVectorPos].second);
	m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	if (!bForce)
	{
		if (m_bAutoConnect)
			m_eventAutoConnect.Signal();
		//m_pSocketEvent->OnDisconnect(this, hSocket, m_SocketSession.PerHandle.pAttachInfo);
		//m_SocketSession.PerHandle.pAttachInfo = NULL;
	}
	return TRUE;
}

inline BOOL CTcpClient::PostRecv(SOCKET hSocket, void *p)
{
	PPER_IO_OPERATION_DATA pPerIoData = (PPER_IO_OPERATION_DATA)p;

	//重置IO操作数据
	DWORD  nFlag = 0;
	DWORD  nRecvByte = 0;
	WSABUF wsabuf;
	ZeroMemory(&pPerIoData->Overlapped, sizeof(OVERLAPPED));
	pPerIoData->OperType = RECV_POSTED;
	wsabuf.len	= m_nRecvBufferSize;
	wsabuf.buf	= pPerIoData->szBuffer;

	//提交PostRecv请求
	int nRet = WSARecv(hSocket, &wsabuf, 1, &nRecvByte, &nFlag, &pPerIoData->Overlapped, NULL);

	if (SOCKET_ERROR == nRet) 
	{
		nRet = WSAGetLastError();
		if (nRet != WSA_IO_PENDING)
		{//接收失败,最有可能是Socket已断开,所以按断开处理
			Disconnect(hSocket);
#ifdef SHOW_PRINTFINFO
			printf("CTcpClient::PostRecv: error:%d!\r\n", nRet);
#endif
			return FALSE;
		}
	}
	return TRUE;
}

inline BOOL CTcpClient::PostSend(void *pPerIoData)
{
	m_sendPool.push((PER_IO_OPERATION_DATA*)pPerIoData);
	return TRUE;
}

int CTcpClient::Send(const char *szData, int nLen, int nOptval)
{
	if ( (m_hSocket == INVALID_SOCKET) || (!m_bIsConnected) )
		return -1;

	BOOL bBlock = nOptval & SEND_OPTION_BLOCK;
	BOOL bSegment = nOptval & SEND_OPTION_SEGMENT;
	BOOL bNeedBuffer = nOptval & SEND_OPTION_USERBUFFER;

	WSABUF wsabuf;
	DWORD  nFlag=0, nSendByte=0;
	int	   nRet, nPacketSize, nSegmentSize;
	int    nSendByteSum = 0;

	//控制分段
	if (bSegment)
		nSegmentSize = m_nSendBufferSize;
	else
		nSegmentSize = nLen;

	SOCKET hSocket = m_hSocket;
	if (bBlock)
	{
		while (nSendByteSum < nLen)
		{
			if ((nLen-nSendByteSum) > nSegmentSize)
				nPacketSize = nSegmentSize;
			else
				nPacketSize	= nLen-nSendByteSum;

			nSendByte  = 0;
			wsabuf.len = nPacketSize;
			wsabuf.buf = (char*)szData+nSendByteSum;
			nRet = WSASend(hSocket, &wsabuf, 1, &nSendByte, nFlag, NULL, NULL);
			if (SOCKET_ERROR == nRet)
			{
				nRet = WSAGetLastError();
				if (nRet != WSA_IO_PENDING)
				{//发送失败,一律按Socket断开处理
					if (hSocket == m_hSocket)
						Disconnect(hSocket);
#ifdef SHOW_PRINTFINFO
					printf("TcpClient::Send error:%d, Socket:%ld!\r\n", nRet, hSocket);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
					char szLog[MAX_LOGBUFFER_LENGTH] = {0};
					sprintf(szLog, "TcpClient::Send error:%d, Socket:%ld!", nRet, hSocket);
					m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
					return nSendByteSum;
				}
				else
				{//WSASend返回SOCKET_ERROR并且错误码WSA_IO_PENDING；
				 //在这种情况下,lpNumberOfBytesSent不更新。
					nSendByteSum += nPacketSize;
				}
			}
			else
				nSendByteSum += nSendByte;
		}
	}
	else
	{
		PPER_IO_OPERATION_DATA pPerIoData;
		while (nSendByteSum < nLen)
		{
			if ((nLen-nSendByteSum) > nSegmentSize)
				nPacketSize = nSegmentSize;
			else
				nPacketSize	= nLen-nSendByteSum;

			if (bNeedBuffer)
			{
				pPerIoData = m_sendPool.pop(nPacketSize);
				if (NULL == pPerIoData)
					return nSendByteSum;
				::CopyMemory(pPerIoData->szBuffer, szData+nSendByteSum, nPacketSize);
			}
			else
			{
				pPerIoData = m_sendPool.pop();
				if (NULL == pPerIoData)
					return nSendByteSum;
				pPerIoData->szBuffer = (char*)szData+nSendByteSum;
			}
			nSendByte = 0;
			ZeroMemory(&pPerIoData->Overlapped,sizeof(OVERLAPPED));
			pPerIoData->OperType = SEND_POSTED;
			wsabuf.len			 = nPacketSize;
			wsabuf.buf			 = pPerIoData->szBuffer;
			nRet = WSASend(hSocket, &wsabuf, 1, &nSendByte, nFlag, &pPerIoData->Overlapped, NULL);
			if (SOCKET_ERROR == nRet)
			{
				nRet = WSAGetLastError();
				if (nRet != WSA_IO_PENDING)
				{//发送失败,一律按Socket断开处理
					m_sendPool.push(pPerIoData);
					if (hSocket == m_hSocket)
						Disconnect(hSocket);
#ifdef SHOW_PRINTFINFO
					printf("TcpClient::Send error:%d, Socket:%ld!\r\n", nRet, hSocket);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
					char szLog[MAX_LOGBUFFER_LENGTH] = {0};
					sprintf(szLog, "TcpClient::Send error:%d, Socket:%ld!", nRet, hSocket);
					m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
					return nSendByteSum;
				}
				else
				{//WSASend返回SOCKET_ERROR并且错误码WSA_IO_PENDING；
				 //在这种情况下,lpNumberOfBytesSent不更新。
					nSendByteSum += nPacketSize;
				}
			}
			else
				nSendByteSum += nSendByte;
		}
	}
	return nSendByteSum;
}

BOOL CTcpClient::SetAutoConnect(BOOL bAutoConnect, int nCheckInterval, int nReconnectSleep)
{
	if (bAutoConnect)
	{
		if (!m_bAutoConnect)
		{
			m_eventAutoConnect.Create(NULL, FALSE, FALSE, NULL);
			m_threadAutoConnect.Start(CTcpClient::AutoConnect,this);
		}
	}
	else
	{
		if (m_bAutoConnect)
		{
			m_eventAutoConnect.Signal();
			m_threadAutoConnect.Stop(WAIT_TIMEOUT);
			m_threadAutoConnect.Kill();
		}
	}
	m_bAutoConnect			= bAutoConnect;
	m_nCheckIntervalTime	= nCheckInterval;
	m_nReconnectSleepTime	= nReconnectSleep;
	return TRUE;
}

void CTcpClient::SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner)
{
	assert(pAppLog != NULL);
	assert(pSocketEvent != NULL);
	assert(pSocketRunner != NULL);
	m_pAppLog		= pAppLog;
	m_pSocketEvent	= pSocketEvent;
	m_pSocketRunner = pSocketRunner;
	return;
}

inline void CTcpClient::SetBufferSize(int nRecvBufferSize, int nSendBufferSize)
{
	if (nRecvBufferSize > 0)
	{
		m_nRecvBufferSize = nRecvBufferSize;
	}
	if (nSendBufferSize > 0)
	{
		m_nSendBufferSize = nSendBufferSize;
	}
	return;
}

BOOL CTcpClient::Open(const char *szServerName, unsigned short nServerPort, int nLocalPort)
{
	m_nLocalPort = nLocalPort;
	if (m_vectServerAddr.empty())
		AppendServerAddr(szServerName,nServerPort);

	if ( Connect() )
	{
		return TRUE;
	}
	else
	{
		if (m_bAutoConnect)
			m_eventAutoConnect.Signal();
	}
	return FALSE;
}

BOOL CTcpClient::Connect()
{
	{
		SYNC::CGuard<SYNC::CThreadMutex> guard(m_lock);
		Close();

		//创建Socket
		SOCKET hSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == hSocket)
		{
	#ifdef SHOW_PRINTFINFO
			printf("Create socket failure!\r\n");
	#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			m_pAppLog->WriteLog("TcpClient create socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
	#endif
			return FALSE;
		}
		if ( (m_nLocalPort>0) && (m_nLocalPort<65535) )
		{
			SOCKADDR_IN LoacalAddr;
			memset(&LoacalAddr,0,sizeof(SOCKADDR_IN));
			LoacalAddr.sin_family = AF_INET;
			LoacalAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			LoacalAddr.sin_port = htons(m_nLocalPort);
			if (bind(m_hSocket,(SOCKADDR*)&LoacalAddr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
			{
	#ifdef SHOW_PRINTFINFO
				printf("TcpClient bind port socket failure!\r\n");
	#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
				m_pAppLog->WriteLog("TcpClient bind port socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
	#endif
				return FALSE;
			}
		}

		SOCKADDR_IN ServerAddr;
		memset(&ServerAddr,0,sizeof(SOCKADDR_IN));
		ServerAddr.sin_family	= AF_INET;
		ServerAddr.sin_port		= htons(m_vectServerAddr[m_nVectorPos].second);
		unsigned long nAddr		= inet_addr(m_vectServerAddr[m_nVectorPos].first.c_str());
		if (nAddr == INADDR_NONE)
		{
			struct hostent *hp = NULL;
			hp = gethostbyname(m_vectServerAddr[m_nVectorPos].first.c_str());
			if (hp != NULL)
				memcpy(&(ServerAddr.sin_addr),hp->h_addr,hp->h_length);
			else
				return FALSE;
		}
		else
  			ServerAddr.sin_addr.s_addr = nAddr;
		int nRet = connect(hSocket,(SOCKADDR*)&ServerAddr,sizeof(ServerAddr));
		if (SOCKET_ERROR == nRet)
		{
	#ifdef SHOW_PRINTFINFO
			printf("TcpClient::Open connect error!\r\n");
	#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			m_pAppLog->WriteLog("TcpClient::Open connect error!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
	#endif
			return FALSE;
		}
		if (m_SocketSession.PerIoRecv.szBuffer != NULL)
		{
			m_SocketSession.PerIoRecv.szBuffer = (char*)HeapReAlloc(
				GetProcessHeap(), 
				HEAP_ZERO_MEMORY, 
				m_SocketSession.PerIoRecv.szBuffer, 
				m_nRecvBufferSize);
		}
		else
		{
			m_SocketSession.PerIoRecv.szBuffer = (char*)HeapAlloc(
				GetProcessHeap(), 
				HEAP_ZERO_MEMORY, 
				m_nRecvBufferSize);
		}
		m_SocketSession.PerIoRecv.nLen		= m_nRecvBufferSize;
		m_SocketSession.PerIoRecv.OperType	= RECV_POSTED;

		//关联Socket到完成端口，句柄数据在此时被绑定到完成端口
		m_SocketSession.PerHandle.hSocket = hSocket;
		m_pSocketRunner->AssociateSocket(hSocket, &m_SocketSession.PerHandle);

		//由上层初始化Socket(一般为设置Socket附加信息的指针)
		m_pSocketEvent->OnInitSocket(&m_SocketSession);

		//重置IO操作数据
		DWORD  nFlag = 0;
		DWORD  nRecvByte = 0;
		WSABUF wsabuf;
		ZeroMemory(&m_SocketSession.PerIoRecv.Overlapped,sizeof(OVERLAPPED));
		wsabuf.len	= m_nRecvBufferSize;
		wsabuf.buf	= m_SocketSession.PerIoRecv.szBuffer;

		//提交PostRecv请求
		nRet = WSARecv(hSocket, &wsabuf, 1, 
			&nRecvByte, &nFlag, &m_SocketSession.PerIoRecv.Overlapped, NULL);

		m_hSocket		= hSocket;
		m_bIsConnected	= TRUE;
	}
	DoOpen();

#ifdef SHOW_PRINTFINFO
	printf("TcpClient::Open success socket:%ld, servername=%s, serverport:%d, localport=%d!\r\n",
		m_hSocket, m_vectServerAddr[m_nVectorPos].first.c_str(), m_vectServerAddr[m_nVectorPos].second, m_nLocalPort);
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	char szLog[MAX_LOGBUFFER_LENGTH] = {0};
	sprintf(szLog,"TcpClient::Open success socket:%ld, servername=%s, serverport:%d, localport=%d!",
		m_hSocket, m_vectServerAddr[m_nVectorPos].first.c_str(), m_vectServerAddr[m_nVectorPos].second, m_nLocalPort);
	m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	return TRUE;
}

inline BOOL CTcpClient::Close()
{	
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		m_bIsConnected = FALSE;

#ifdef SHOW_PRINTFINFO
		printf("TcpClient::Close serverip=%s, serverport:%d, localport=%d!\r\n",
			m_vectServerAddr[m_nVectorPos].first.c_str(), m_vectServerAddr[m_nVectorPos].second, m_nLocalPort);
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
		char szLog[MAX_LOGBUFFER_LENGTH]={0};
		sprintf(szLog,"TcpClient::Close serverip=%s, serverport:%d, localport=%d!",
			m_vectServerAddr[m_nVectorPos].first.c_str(), m_vectServerAddr[m_nVectorPos].second, m_nLocalPort);
		m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif
	}
	return TRUE;
}

inline SOCKET CTcpClient::GetSocketHandle() const
{
	return m_hSocket;
}

inline BOOL CTcpClient::DoOpen()
{
	return TRUE;
}

unsigned WINAPI CTcpClient::AutoConnect(LPVOID lParam)
{
	BOOL bRet = FALSE;
	CTcpClient *pThis = (CTcpClient*)lParam;
	int nSize = pThis->m_vectServerAddr.size();
	while (1)
	{
		pThis->m_eventAutoConnect.Wait(pThis->m_nCheckIntervalTime);
		if (pThis->m_threadAutoConnect.IsStop())
			break;

		if ( !pThis->IsRealConnected() )
		{	
			do
			{
				if (pThis->m_threadAutoConnect.IsStop())
					return -1;

				Sleep(pThis->m_nReconnectSleepTime);
				if (!bRet)
				{
					if (nSize > 1)
						pThis->m_nVectorPos = ((pThis->m_nVectorPos+1) % nSize);
					else
						pThis->m_nVectorPos = 0;
				}
				bRet = pThis->Connect();
			} while(!bRet);
		}
	}
	return 0;
}

BOOL CTcpClient::IsRealConnected()
{
	if (m_hSocket == INVALID_SOCKET)
	{
		m_bIsConnected = FALSE;
		return FALSE;
	}

	fd_set fdread,fdwrite,fderror;
	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fderror);
	FD_SET(m_hSocket, &fdread);
	FD_SET(m_hSocket, &fdwrite);
	FD_SET(m_hSocket, &fderror);
	timeval tv = {1,0};
	int nRet = select(0,&fdread,&fdwrite,&fderror,&tv);
	if ( (nRet<0) || FD_ISSET(m_hSocket,&fderror) )
	{
		Close();
		return FALSE;
	}
	m_bIsConnected = TRUE;
	return TRUE;
}

BOOL CTcpClient::AppendServerAddr(const char *szServerName, unsigned short nServerPort)
{
	m_vectServerAddr.push_back(std::make_pair(szServerName,nServerPort));
	return TRUE;
}

BOOL CTcpClient::RemoveServerAddr(int nPos)
{
	if ( (nPos < 0) || (nPos >= m_vectServerAddr.size()) )
		return FALSE;

	m_vectServerAddr.erase(m_vectServerAddr.begin()+nPos);
	return TRUE;
}

}
