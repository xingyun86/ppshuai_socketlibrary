//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "udpsource.h"
#include "SocketLib.h"
#include "SocketEvent.h"
#include "sync/guard.h"
#include <assert.h>

namespace SL
{

CUdpSource::CUdpSource()
	: m_hSocket(NULL)
	, m_nMaxFreeRecvPoolNum(100)
	, m_nMaxFreeSendPoolNum(100)
{
	m_perHandle.pSocketSource = this;
}

CUdpSource::CUdpSource(int nMaxFreeRecvPoolNum, int nMaxFreeSendPoolNum)
	: m_hSocket(NULL)
{
	if (nMaxFreeRecvPoolNum<1)
		m_nMaxFreeRecvPoolNum = 100;
	else
		nMaxFreeRecvPoolNum = nMaxFreeRecvPoolNum;
	if (nMaxFreeSendPoolNum<1)
		m_nMaxFreeSendPoolNum = 100;
	else
		m_nMaxFreeSendPoolNum = nMaxFreeRecvPoolNum;
	m_perHandle.pSocketSource = this;
}

CUdpSource::~CUdpSource(void)
{
	Close();
}

inline BOOL CUdpSource::Close()
{
	//清除空闲池
	{
		SYNC::CGuard<SYNC::CThreadMutex> guard(m_lockRecv);
		std::list<PPER_IO_OPERATION_UDP_RECV>::iterator itRecv;
		for (itRecv = m_lstFreeRecvPool.begin(); itRecv != m_lstFreeRecvPool.end(); itRecv++)
		{
			HeapFree(GetProcessHeap(),0,*itRecv);
		}
		m_lstFreeRecvPool.clear();
	}
	{
		SYNC::CGuard<SYNC::CThreadMutex> guard(m_lock);
		if (m_hSocket != NULL)
		{
			shutdown(m_hSocket,0);
			closesocket(m_hSocket);
			m_hSocket = NULL;
		}
	}
	m_sendPool.clear();
	return TRUE;
}

BOOL CUdpSource::Open(const char *szServerName, unsigned short nServerPort, int nLocalPort, const char *szLocalIP)
{
	Close();

	m_sendPool.init(m_nMaxFreeSendPoolNum);
	SYNC::CGuard<SYNC::CThreadMutex> guard(m_lock);

	//创建Socket
	m_hSocket = WSASocket(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET==m_hSocket)
	{
#ifdef SHOW_PRINTFINFO
		printf("Create socket failure!\r\n");
#endif
		return FALSE;
	}

	//主机名解释
	memset(&m_ServerAddr,0,sizeof(SOCKADDR_IN));
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port	= htons(nServerPort);
	unsigned long nAddr		= inet_addr(szServerName);
	if (nAddr == INADDR_NONE)
	{
		struct hostent *hp = NULL;
		hp = gethostbyname(szServerName);
		if (hp != NULL)
			memcpy(&(m_ServerAddr.sin_addr),hp->h_addr,hp->h_length);
		else
			return FALSE;
	}
	else
  		m_ServerAddr.sin_addr.s_addr = nAddr;

	if ( (nLocalPort>0) && (nLocalPort<65535) )
	{
		SOCKADDR_IN LoacalAddr;
		memset(&LoacalAddr,0,sizeof(LoacalAddr));
		LoacalAddr.sin_family = AF_INET;
		LoacalAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		//if (szLocalIP == NULL)
		//	LoacalAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		//else
		//	LoacalAddr.sin_addr.S_un.S_addr = inet_addr(szLocalIP);
		LoacalAddr.sin_port = htons(nLocalPort);
		if (bind(m_hSocket,(SOCKADDR*)&LoacalAddr,sizeof(LoacalAddr)) == SOCKET_ERROR)
		{
#ifdef SHOW_PRINTFINFO
			_tprintf(TEXT("UDP bind port socket failure!\r\n"));
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			m_pAppLog->WriteLog(TEXT("UDP bind port socket failure!"), TEXT(__FILE__), __LINE__, CAppLog::LOG_LEVEL_5);
	#endif
#endif
			return FALSE;
		}
	}
	else
	{
		int nRet = connect(m_hSocket,(SOCKADDR*)&m_ServerAddr,sizeof(m_ServerAddr));
		if (SOCKET_ERROR == nRet)
		{
#ifdef SHOW_PRINTFINFO
			printf("UdpSource port invalid!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			m_pAppLog->WriteLog(TEXT("UdpSource port invalid!"), TEXT(__FILE__), __LINE__, CAppLog::LOG_LEVEL_5);
	#endif
#endif
			return FALSE;
		}
	}

    //Turn off UDP errors resulting from ICMP messages (port/host unreachable, etc)
	DWORD nBytes;
    int optval = 0;
    int nRet = WSAIoctl(m_hSocket,SIO_UDP_CONNRESET,&optval,sizeof(optval),NULL,0,&nBytes,NULL,NULL);
	if (SOCKET_ERROR == nRet)
		return FALSE;

	//关联Socket到完成端口，句柄数据在此时被绑定到完成端口
	m_perHandle.hSocket = m_hSocket;
	m_pSocketRunner->AssociateSocket(m_hSocket, &m_perHandle);
	int nPostRecv = m_pSocketRunner->GetWorkThreadNum()*2+2;
	for (int i=0; i<nPostRecv; i++)
	{
		PPER_IO_OPERATION_UDP_RECV p = GetRecvObj();
		if (NULL == p)
			return FALSE;
		PostRecv(p);
	}

#ifdef SHOW_PRINTFINFO
	printf("UdpSource::Open success, socket:%ld localport:%d!\r\n", m_hSocket, nLocalPort);
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	TCHAR szLog[MAX_LOGBUFFER_LENGTH]={0};
	sprintf(szLog, "UDPSource::Open success, socket:%ld localport:%ld!", m_hSocket, nLocalPort);
	m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	return TRUE;
}

inline BOOL CUdpSource::Open(const char *szServerName, unsigned short nServerPort, int nLocalPort)
{
	return Open(szServerName, nServerPort, nLocalPort, NULL);
}

BOOL CUdpSource::Open(unsigned short nLocalPort, const char *szLocalIP)
{
	Close();

	m_sendPool.init(m_nMaxFreeSendPoolNum);
	SYNC::CGuard<SYNC::CThreadMutex> guard(m_lock);

	//创建Socket
	m_hSocket = WSASocket(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET==m_hSocket)
	{
#ifdef SHOW_PRINTFINFO
		printf("Create socket failure!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
		m_pAppLog->WriteLog("Create socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return FALSE;
	}

	SOCKADDR_IN LoacalAddr;
	LoacalAddr.sin_family = AF_INET;
	if (szLocalIP == NULL)
		LoacalAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	else
		LoacalAddr.sin_addr.S_un.S_addr = inet_addr(szLocalIP);
	LoacalAddr.sin_port = htons(nLocalPort);
	if (bind(m_hSocket,(SOCKADDR*)&LoacalAddr,sizeof(LoacalAddr)) == SOCKET_ERROR)
	{
#ifdef SHOW_PRINTFINFO
		printf("UDP bind port socket failure!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
		m_pAppLog->WriteLog("UDP bind port socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_5);
	#endif
#endif
		return FALSE;
	}

    //Turn off UDP errors resulting from ICMP messages (port/host unreachable, etc)
	DWORD nBytes;
    int optval = 0;
    int nRet = WSAIoctl(m_hSocket, SIO_UDP_CONNRESET,&optval,sizeof(optval),NULL,0,&nBytes,NULL,NULL);
	if (SOCKET_ERROR == nRet)
		return FALSE;

	//关联Socket到完成端口，句柄数据在此时被绑定到完成端口
	m_perHandle.hSocket = m_hSocket;
	m_pSocketRunner->AssociateSocket(m_hSocket, &m_perHandle);
	int nPostRecv = m_pSocketRunner->GetWorkThreadNum()*2+2;
	for (int i=0; i<nPostRecv; i++)
	{
		PPER_IO_OPERATION_UDP_RECV p = GetRecvObj();
		if (NULL == p)
			return FALSE;
		PostRecv(p);
	}

#ifdef SHOW_PRINTFINFO
	printf("UDPSource::Open success, socket:%ld, localport:%ld!\r\n", m_hSocket, nLocalPort);
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	char szLog[MAX_LOGBUFFER_LENGTH]={0};
	sprintf(szLog, "UDPSource::Open success, socket:%ld, localport:%ld!", m_hSocket, nLocalPort);
	m_pAppLog->WriteLog("UDPSource::Open success!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	return TRUE;
}

void CUdpSource::SetBufferSize(int nRecvBufferSize, int nSendBufferSize)
{
	if (nRecvBufferSize > 0)
		m_nRecvBufferSize = nRecvBufferSize;
	if (nSendBufferSize > 0)
		m_nSendBufferSize = nSendBufferSize;
	return;
}

void CUdpSource::SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner)
{
	assert(pAppLog != NULL);
	assert(pSocketEvent != NULL);
	assert(pSocketRunner != NULL);
	m_pAppLog		= pAppLog;
	m_pSocketEvent	= pSocketEvent;
	m_pSocketRunner = pSocketRunner;
	return;
}

inline BOOL CUdpSource::PostRecv(void *p)
{
	if (p == NULL)
		return FALSE;
	PPER_IO_OPERATION_UDP_RECV pPerIoRecv = (PPER_IO_OPERATION_UDP_RECV)p;

	//重置IO操作数据
	DWORD  nFlag = 0;
	DWORD  nRecvByte = 0;
	WSABUF wsabuf;
	ZeroMemory(&pPerIoRecv->Overlapped, sizeof(OVERLAPPED));
	pPerIoRecv->OperType = RECV_POSTED;
	wsabuf.len = m_nRecvBufferSize;
	wsabuf.buf = pPerIoRecv->szBuffer;

	//提交PostRecv请求
	int nRet = WSARecvFrom(m_hSocket, &wsabuf, 1, &nRecvByte, &nFlag, 
		(sockaddr*)&pPerIoRecv->addr, &pPerIoRecv->addrlen, &pPerIoRecv->Overlapped, NULL);

	if (SOCKET_ERROR == nRet) 
	{
		int nErrorID = WSAGetLastError();
		if (nErrorID != WSA_IO_PENDING)
		{//接收失败,最有可能是Socket已断开,所以按断开处理
			FreeRecvObj(pPerIoRecv);
#ifdef SHOW_PRINTFINFO
			printf("UDPSource::PostRecv: error:%d!\r\n", nErrorID);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			char szLog[MAX_LOGBUFFER_LENGTH] = {0};
			sprintf(szLog,"PostRead error:%ld!", nErrorID);
			m_pAppLog->WriteLog(szLog,__FILE__,__LINE__,CAppLog::LOG_LEVEL_7);
	#endif
#endif
			return FALSE;
		}
	}
	return TRUE;
}

inline BOOL CUdpSource::PostSend(void *p)
{
	PPER_IO_OPERATION_DATA pPerIoData = (PPER_IO_OPERATION_DATA)p;
	m_sendPool.push(pPerIoData);
	return TRUE;
}

int CUdpSource::Send(const char *szData, int nLen, sockaddr *pTo, int nToLen, int nOptval)
{
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
			nRet = WSASendTo(m_hSocket, &wsabuf, 1, &nSendByte, nFlag, pTo, nToLen, NULL, NULL);
			if (SOCKET_ERROR == nRet)
			{
				nRet = WSAGetLastError();
				if (nRet != WSA_IO_PENDING)
				{//发送失败,一律按Socket断开处理
#ifdef SHOW_PRINTFINFO
				printf("UDPSource::Send error:%d!\r\n",nRet);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
				TCHAR szLog[MAX_LOGBUFFER_LENGTH] = {0};
				_stprintf(szLog,TEXT("UDPSource::Send error:%d!"), nRet);
				m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
					return nSendByteSum;
				}
				else
				{//WSASendTo返回SOCKET_ERROR并且错误码WSA_IO_PENDING；
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
				memcpy(pPerIoData->szBuffer, szData+nSendByteSum, nPacketSize);
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
			nRet = WSASendTo(m_hSocket, &wsabuf, 1, &nSendByte, nFlag, pTo, nToLen, &pPerIoData->Overlapped, NULL);
			if (SOCKET_ERROR == nRet)
			{
				nRet = WSAGetLastError();
				if (nRet != WSA_IO_PENDING)
				{//发送失败,一律按Socket断开处理
					m_sendPool.push(pPerIoData);
#ifdef SHOW_PRINTFINFO
				printf("UDPSource::Send error:%d!\r\n",nRet);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
				TCHAR szLog[MAX_LOGBUFFER_LENGTH] = {0};
				_stprintf(szLog,TEXT("UDPSource::Send error:%d!"), nRet);
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

inline int CUdpSource::Send(const char *szData, int nLen, const char *szIP, unsigned short nPort,int nOptval)
{
	sockaddr_in ServerAddr;
	ServerAddr.sin_family	= AF_INET;
	ServerAddr.sin_port		= htons(nPort);
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(szIP);
	return Send(szData, nLen, (sockaddr*)&ServerAddr, sizeof(sockaddr_in), nOptval);
}

inline int CUdpSource::Send(const char *szData, int nLen, int nOptval)
{
	return Send(szData, nLen, (sockaddr*)&m_ServerAddr, sizeof(sockaddr_in), nOptval);
}

PPER_IO_OPERATION_UDP_RECV CUdpSource::GetRecvObj()
{	
	PPER_IO_OPERATION_UDP_RECV p = NULL;
	m_lockRecv.Lock();
	if (!m_lstFreeRecvPool.empty())
	{
		p = m_lstFreeRecvPool.front();
		m_lstFreeRecvPool.pop_front();
		m_lockRecv.UnLock();
	}
	else
	{
		m_lockRecv.UnLock();
		p = (PPER_IO_OPERATION_UDP_RECV)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(sizeof(PER_IO_OPERATION_UDP_RECV)+m_nRecvBufferSize));
		if (NULL == p)
		{
			return NULL;
		}
		p->szBuffer		 = ((char*)p+sizeof(PER_IO_OPERATION_UDP_RECV));
		p->nLen			 = m_nRecvBufferSize;
		p->OperType		 = RECV_POSTED;
		p->addrlen		 = sizeof(sockaddr_in);
	}
	return p;
}

void CUdpSource::FreeRecvObj(SL::PPER_IO_OPERATION_UDP_RECV p)
{
	m_lockRecv.Lock();
	if (m_lstFreeRecvPool.size() >= m_nMaxFreeRecvPoolNum)
		HeapFree(GetProcessHeap(),0,(void*)p);
	else
		m_lstFreeRecvPool.push_back(p);
	m_lockRecv.UnLock();

}

};