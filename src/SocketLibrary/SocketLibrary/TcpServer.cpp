//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include ".\tcpserver.h"
#include "SocketEvent.h"
#include "SocketAPI.h"
#include <assert.h>
namespace SL
{

CTcpServer::CTcpServer(void)
	: m_nMaxSocketNum(MAX_SOCKET_CONNECTNUM)
	, m_nMaxFreePoolNum(MAX_SOCKET_FREENUM)
	, m_nDefaultFreePoolNum(10)
	, m_hListenSocket(NULL)
{
}

CTcpServer::~CTcpServer(void)
{
	Close();
}

inline BOOL CTcpServer::PostRecv(SOCKET hClientSocket, void *p)
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
	int nRet = WSARecv(hClientSocket, &wsabuf, 1,
		&nRecvByte, &nFlag, &pPerIoData->Overlapped, NULL);

	if (nRet == SOCKET_ERROR) 
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{//接收失败,一律Socket断开处理
			Disconnect(hClientSocket);
			return FALSE;
		}
	}
	return TRUE;
}

inline BOOL CTcpServer::PostSend(void *pPerIoData)
{
	m_sendPool.push((PER_IO_OPERATION_DATA*)pPerIoData);
	return TRUE;
}

void CTcpServer::SetConfig(int nMaxSocketNum, int nMaxFreePoolNum, int nDefaultFreePoolNum)
{
	m_nMaxSocketNum = nMaxSocketNum;
	m_nMaxFreePoolNum = nMaxFreePoolNum;
	m_nDefaultFreePoolNum = nDefaultFreePoolNum;
	return;
}

void CTcpServer::SetInterface(CAppLog *pAppLog, CSocketEvent *pSocketEvent, CSocketRunner *pSocketRunner)
{
	m_pAppLog = pAppLog;
	m_pSocketEvent  = pSocketEvent;
	m_pSocketRunner = pSocketRunner;
	return;
}

inline void CTcpServer::SetBufferSize(int nRecvBufferSize, int nSendBufferSize)
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

unsigned int WINAPI CTcpServer::ListenProc(LPVOID lParam)
{
	CTcpServer *pSrv = (CTcpServer*)lParam;
	SOCKET      hClientSocket;
	SOCKADDR_IN siClientSocketAddr;
	int         nClientSocketLen;

	char        szIPAddr[MAX_IPADDR_LENGTH];
	USHORT      nPort = 0;
#ifdef SOCKETLIBRARY_WRITE_LOG
	char        szLog[MAX_LOGBUFFER_LENGTH];
#endif

	nClientSocketLen   = sizeof(siClientSocketAddr);
	while (1)
	{
		if (pSrv->m_threadListen.IsStop())
		{//退出线程
#ifdef SHOW_PRINTFINFO
			printf("ListenProc thread exit!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			pSrv->m_pAppLog->WriteLog("ListenProc thread exit!",__FILE__,__LINE__,CAppLog::LOG_LEVEL_7);
	#endif
#endif
			break;
		}

		//置0
		memset(szIPAddr, 0, sizeof(szIPAddr));
#ifdef SOCKETLIBRARY_WRITE_LOG
		memset(szLog, 0, sizeof(szLog));
#endif

		//接收客户的请求
		hClientSocket = WSAAccept(pSrv->m_hListenSocket,(SOCKADDR*)&siClientSocketAddr,&nClientSocketLen,NULL,0);
		if (INVALID_SOCKET == hClientSocket)
		{
#ifdef SHOW_PRINTFINFO
			printf("WSAAccept invalid socket!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			pSrv->m_pAppLog->WriteLog("WSAAccept invalid socket!",__FILE__,__LINE__,CAppLog::LOG_LEVEL_7);
	#endif
#endif
			continue;
		}

		//取得客户端IP信息
		//CSocketAPI::GetSocketInfo(hClientSocket, szIPAddr, &nPort);
		strncpy(szIPAddr,inet_ntoa(siClientSocketAddr.sin_addr),MAX_IPADDR_LENGTH);
		nPort = ntohs(siClientSocketAddr.sin_port);

		//调SocketEvent::OnAccept
		if (!pSrv->m_pSocketEvent->OnAccept(pSrv, hClientSocket))
		{
			closesocket(hClientSocket);
#ifdef SHOW_PRINTFINFO
			printf("ListenSocket:%ld, clientip:%s, port:%d, acceptconnect error!\r\n", 
				pSrv->m_hListenSocket, szIPAddr, nPort);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
			sprintf(szLog,"ListenSocket:%ld, clientip:%s, port:%d, acceptconnect error!", 
				pSrv->m_hListenSocket, szIPAddr, nPort);
			pSrv->m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
			continue;
		}

		if (pSrv->m_mapSocketSession.size() >= pSrv->m_nMaxSocketNum)
		{
			closesocket(hClientSocket);
#ifdef SHOW_PRINTFINFO
			printf("Socket: %d, clientip:%s, port: %d, refuse; current online total:%ld \r\n", 
				hClientSocket, szIPAddr, nPort, pSrv->m_mapSocketSession.size());
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
			sprintf(szLog,"ListenSocket:%ld, clientip:%s, port: %d, refuse; current online total:%ld",
				pSrv->m_hListenSocket, szIPAddr, nPort, pSrv->m_mapSocketSession.size());
			pSrv->m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif
			continue;
		}

		//设置新socket的选项(如接收/发送缓冲区的大小或关闭缓冲区等)
		//int nBufferLen = RECV_BUFFER_SIZE;
		//setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBufferLen, sizeof(int));
		//setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBufferLen, sizeof(int));

		PSOCKET_SESSION pSocketSession = NULL;
		//取得空闲Socket的内存
		pSrv->m_lock.Lock();
		if (!pSrv->m_lstFreePool.empty())
		{
			pSocketSession = pSrv->m_lstFreePool.front();
			pSrv->m_lstFreePool.pop_front();
			pSocketSession->PerHandle.hSocket		  = hClientSocket;
			pSocketSession->PerHandle.pSocketSource   = pSrv;
		}
		pSrv->m_lock.UnLock();
		if (NULL == pSocketSession)
		{//分配Socket I/O操作数据的内存
			char *p = (char*)HeapAlloc(pSrv->m_hProcessHeap, 
				HEAP_ZERO_MEMORY,
				(sizeof(SOCKET_SESSION)+pSrv->m_nRecvBufferSize));
			if (NULL == p)
			{
				closesocket(hClientSocket);
				continue;
			}
			pSocketSession = (PSOCKET_SESSION)p;
			pSocketSession->PerHandle.hSocket		= hClientSocket;
			pSocketSession->PerHandle.pSocketSource = pSrv;
			pSocketSession->PerIoRecv.szBuffer		= (p+sizeof(SOCKET_SESSION));
			pSocketSession->PerIoRecv.nLen			= pSrv->m_nRecvBufferSize;
			pSocketSession->PerIoRecv.OperType		= RECV_POSTED;
		}

		//将Socket关联到完成端口，句柄数据在此时被绑定到完成端口
		pSrv->m_pSocketRunner->AssociateSocket(hClientSocket, &pSocketSession->PerHandle);

		//加入Socket集合
		pSrv->m_lock.Lock();
		pSrv->m_mapSocketSession.insert(SocketSessionMap::value_type(hClientSocket,pSocketSession));
		pSrv->m_lock.UnLock();

		//由上层初始化Socket(一般为设置Socket附加信息的指针)
		pSrv->m_pSocketEvent->OnInitSocket(pSocketSession);

		//提交首个接收数据请求
		//如果这时客户端断开连接,则也可以得到退出通知
		DWORD  nFlag = 0;
		DWORD  nRecvByte = 0;
		WSABUF wsabuf;
		wsabuf.len = pSrv->m_nRecvBufferSize;
		wsabuf.buf = pSocketSession->PerIoRecv.szBuffer;
		ZeroMemory(&pSocketSession->PerIoRecv.Overlapped, sizeof(OVERLAPPED));
		pSocketSession->PerIoRecv.OperType = RECV_POSTED;
		WSARecv(hClientSocket, &wsabuf, 1,
			&nRecvByte, &nFlag, &pSocketSession->PerIoRecv.Overlapped, NULL);

#ifdef SHOW_PRINTFINFO
		printf("ListenSocket:%ld, clientip:%s, port:%d, connected; total:%ld\r\n",
			pSrv->m_hListenSocket, szIPAddr, nPort, pSrv->m_mapSocketSession.size());
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
		sprintf(szLog,"ListenSocket:%ld, clientip:%s, port:%d, connected; total:%ld",
			pSrv->m_hListenSocket, szIPAddr, nPort, pSrv->m_mapSocketSession.size());
		pSrv->m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_5);
	#endif
#endif

	}

	ExitThread(0);
	return 0;
}

BOOL CTcpServer::Open(const char *szLocalIP, unsigned short nPort, int nBacklog)
{
	//m_mapSocketSession.clear();

	//创建监听Socket
	m_hListenSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET==m_hListenSocket)
	{
#ifdef SHOW_PRINTFINFO
		printf("Create listen socket failure!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
		m_pAppLog->WriteLog("Create listen socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return FALSE;
	}

	SOCKADDR_IN InternetAddr;
	memset(&InternetAddr,0,sizeof(InternetAddr));
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//if (szLocalIP == NULL)
	//	InternetAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//else
	//	InternetAddr.sin_addr.S_un.S_addr = inet_addr(szLocalIP);
	InternetAddr.sin_port = htons(nPort);
	if (bind(m_hListenSocket,(SOCKADDR*)&InternetAddr,sizeof(InternetAddr)) == SOCKET_ERROR)
	{
#ifdef SHOW_PRINTFINFO
		printf("TcpServer bind socket failure!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
		m_pAppLog->WriteLog("TcpServer bind socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return FALSE;
	}

	//监听端口上的连接请求
	if (listen(m_hListenSocket,nBacklog) == SOCKET_ERROR)
	{
#ifdef SHOW_PRINTFINFO
		printf("Listening socket failure!\r\n");
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
		m_pAppLog->WriteLog("Listening socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return FALSE;
	}

	m_hProcessHeap = GetProcessHeap();
	//创建空闲Soket的内存池
	for (int i=0; i<m_nDefaultFreePoolNum; i++)
	{
		//分配IO操作数据的内存
		char *p = (char*)HeapAlloc(m_hProcessHeap, 
			HEAP_ZERO_MEMORY, 
			(sizeof(SOCKET_SESSION)+m_nRecvBufferSize));
		if (NULL == p)
		{
			return FALSE;
		}
		PSOCKET_SESSION pSocketSession			= (PSOCKET_SESSION)p;
		pSocketSession->PerHandle.hSocket		= 0;
		pSocketSession->PerHandle.pSocketSource = this;
		pSocketSession->PerIoRecv.szBuffer		= (p+sizeof(SOCKET_SESSION));
		m_lstFreePool.push_back(pSocketSession);
	}

	//启动监听线程
	m_threadListen.Start(CTcpServer::ListenProc, this, false);
	//初始化发送缓冲池
	m_sendPool.init(m_nMaxSocketNum);

#ifdef SHOW_PRINTFINFO
	printf("TcpServer::Open success, socket:%ld port:%d!\r\n", m_hListenSocket, nPort);
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	TCHAR szLog[MAX_LOGBUFFER_LENGTH]={0};
	_stprintf(szLog, TEXT("TcpServer::Open success, socket:%ld port:%d!"), m_hListenSocket, nPort);
	m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	return TRUE;
}

BOOL CTcpServer::Close()
{
	if (m_hListenSocket == NULL)
		return FALSE;

	//关闭监听句柄
	shutdown(m_hListenSocket,0);
	closesocket(m_hListenSocket);
	m_hListenSocket = NULL;
	m_threadListen.Stop(200);
	m_threadListen.Kill();

	//清除在线Socket
	SocketSessionIterator itClientSocket;
	for (itClientSocket = m_mapSocketSession.begin(); itClientSocket != m_mapSocketSession.end(); itClientSocket++)
	{
		shutdown(itClientSocket->first,SD_BOTH);
		closesocket(itClientSocket->first);
		HeapFree(m_hProcessHeap, 0, (void*)itClientSocket->second);
	}
	m_mapSocketSession.clear();

	//清除空闲池
	std::list<PSOCKET_SESSION>::iterator itItem;
	for (itItem = m_lstFreePool.begin(); itItem != m_lstFreePool.end(); itItem++)
	{
		HeapFree(m_hProcessHeap, 0, *itItem);
	}
	m_lstFreePool.clear();
	m_sendPool.clear();

#ifdef SHOW_PRINTFINFO
	printf("TcpServer::Close()!\r\n");
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	m_pAppLog->WriteLog("TcpServer::Close()!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	return TRUE;
}

inline BOOL CTcpServer::Disconnect(SOCKET hClientSocket, bool bForce)
{
	//取得Socket操作数据
	void *pAttachInfo;
	m_lock.Lock();
	SocketSessionIterator itClientSocket;
	itClientSocket = m_mapSocketSession.find(hClientSocket);
	if (itClientSocket == m_mapSocketSession.end())
	{
		m_lock.UnLock();
		return FALSE;
	}
	PSOCKET_SESSION pSocketSession = itClientSocket->second;
	pAttachInfo = pSocketSession->PerHandle.pAttachInfo;

	char   szIPAddr[MAX_IPADDR_LENGTH];
	USHORT nPort = 0;
	CSocketAPI::GetSocketInfo(hClientSocket, szIPAddr, &nPort);
#ifdef SHOW_PRINTFINFO
	printf("ListenSocket:%ld, socket:%ld, clientip:%s, client close; total:%ld\r\n", 
		m_hListenSocket, hClientSocket, szIPAddr, m_mapSocketSession.size()-1 );
#else
#ifdef SOCKETLIBRARY_WRITE_LOG
	char szLog[MAX_LOGBUFFER_LENGTH];
	sprintf(szLog, "ListenSocket:%ld, socket:%ld, clientip:%s, client close; total:%ld", 
		m_hListenSocket, hClientSocket, szIPAddr, m_mapSocketSession.size()-1 );
	m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_5);
#endif
#endif

	//关闭套接字
	shutdown(hClientSocket,SD_BOTH);
	closesocket(hClientSocket);

	if (m_lstFreePool.size() >= m_nMaxFreePoolNum)
	{
		HeapFree(m_hProcessHeap, 0, (void*)pSocketSession);
	}
	else
	{
		m_lstFreePool.push_back(pSocketSession);
	}
	m_mapSocketSession.erase(itClientSocket);
	m_lock.UnLock();

	if (!bForce)
		m_pSocketEvent->OnDisconnect(this, hClientSocket, pAttachInfo);

	return TRUE;
}

inline int CTcpServer::Send(SOCKET hClientSocket, const char *szData, int nLen, int nOptval)
{
	BOOL bBlock	= nOptval & SEND_OPTION_BLOCK;
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
			nRet = WSASend(hClientSocket, &wsabuf, 1, &nSendByte, nFlag, NULL, NULL);
			if (SOCKET_ERROR == nRet)
			{
				nRet = WSAGetLastError();
				if (nRet != WSA_IO_PENDING)
				{//发送失败,一律按Socket断开处理
					Disconnect(hClientSocket);
#ifdef SHOW_PRINTFINFO
					printf("TcpServer::Send error:%d, Socket:%ld!\r\n", nRet, hClientSocket);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
					char szLog[MAX_LOGBUFFER_LENGTH] = {0};
					sprintf(szLog, "TcpServer::Send error:%d, Socket:%ld!", nRet, hClientSocket);
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
			nRet = WSASend(hClientSocket, &wsabuf, 1, &nSendByte, nFlag, &pPerIoData->Overlapped, NULL);
			if (SOCKET_ERROR == nRet)
			{
				nRet = WSAGetLastError();
				if (nRet != WSA_IO_PENDING)
				{//发送失败,一律按Socket断开处理
					m_sendPool.push(pPerIoData);
					Disconnect(hClientSocket);
#ifdef SHOW_PRINTFINFO
					printf("TcpServer::Send error:%d, Socket:%ld!\r\n", nRet, hClientSocket);
#else
	#ifdef SOCKETLIBRARY_WRITE_LOG
					char szLog[MAX_LOGBUFFER_LENGTH] = {0};
					sprintf(szLog, "TcpServer::Send error:%d, Socket:%ld!", nRet, hClientSocket);
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

inline BOOL CTcpServer::IsConnected(SOCKET hClientSocket)
{
	m_lock.Lock();
	SocketSessionIterator itClientSocket;
	itClientSocket = m_mapSocketSession.find(hClientSocket);
	if (itClientSocket == m_mapSocketSession.end())
	{
		m_lock.UnLock();
		return FALSE;
	}
	m_lock.UnLock();
	return TRUE;
}

inline BOOL CTcpServer::SetAttachInfo(SOCKET hClientSocket, void *pAttachInfo)
{
	m_lock.Lock();
	SocketSessionIterator itClientSocket;
	itClientSocket = m_mapSocketSession.find(hClientSocket);
	if ( itClientSocket == m_mapSocketSession.end() )
	{
		m_lock.UnLock();
		return FALSE;
	}
	PSOCKET_SESSION pSocketSession = (*itClientSocket).second;
	pSocketSession->PerHandle.pAttachInfo = pAttachInfo;
	m_lock.UnLock();
	return FALSE;
}

inline void* CTcpServer::GetAttachInfo(SOCKET hClientSocket)
{
	m_lock.Lock();
	SocketSessionIterator itClientSocket;
	itClientSocket = m_mapSocketSession.find(hClientSocket);
	if ( itClientSocket == m_mapSocketSession.end() )
	{
		m_lock.UnLock();
		return NULL;
	}
	PSOCKET_SESSION pSocketSession = (*itClientSocket).second;
	m_lock.UnLock();
	return pSocketSession->PerHandle.pAttachInfo;
}

}