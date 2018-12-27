#include "SLBlockTcpServer.h"
#include "SocketAPI.h"
#include "SocketLib.h"

namespace SL
{

CSLBlockTcpServer::CSLBlockTcpServer(void)
	: m_nRecvTimeOut(10000)
	, m_nSendTimeOut(10000)
	, m_nRecvBufferSize(1024)
	, m_nSendBufferSize(1024)
	, m_hListenSocket(NULL)
{
}

CSLBlockTcpServer::~CSLBlockTcpServer(void)
{
	Close();
}

void CSLBlockTcpServer::SetInterface(CAppLog *pAppLog, CSLBlockTcpEvent *pBlockEvent, CSLBlockTcpRunner *pBlockRunner)
{
	m_pAppLog = pAppLog;
	m_pBlockEvent = pBlockEvent;
	m_pBlockRunner = pBlockRunner;
}

void CSLBlockTcpServer::SetTimeOut(int nRecvTimeOut, int nSendTimeOut)
{
	m_nRecvTimeOut = nRecvTimeOut;
	m_nSendTimeOut = nSendTimeOut;
}

void CSLBlockTcpServer::SetBufferSize(int nRecvBufferSize, int nSendBufferSize)
{
	m_nRecvBufferSize = nRecvBufferSize;
	m_nSendBufferSize = nSendBufferSize;
}

bool CSLBlockTcpServer::Open(const char *szLocalIP, unsigned short nPort, int nBacklog)
{
	Close();

	//创建侦听Socket
	m_hListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == m_hListenSocket)
	{
#ifdef SHOW_PRINTFINFO
		printf("Create listen socket failure!\r\n");
#else
	#ifdef SOCKETLIB_WRITE_LOG
		m_pAppLog->WriteLog("Create listen socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return false;
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
		printf("CSLBlockTcpServer bind socket failure!\r\n");
#else
	#ifdef SOCKETLIB_WRITE_LOG
		m_pAppLog->WriteLog("CSLBlockTcpServer bind socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return false;
	}

	//监听端口上的连接请求
	if (listen(m_hListenSocket,nBacklog) == SOCKET_ERROR)
	{
#ifdef SHOW_PRINTFINFO
		printf("Listening socket failure!\r\n");
#else
	#ifdef SOCKETLIB_WRITE_LOG
		m_pAppLog->WriteLog("Listening socket failure!", __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
		return false;
	}

	//启动监听线程
	m_threadListen.Start(CSLBlockTcpServer::ListenProc, this, false);

#ifdef SHOW_PRINTFINFO
	printf("CSLBlockTcpServer::Open success, socket:%ld port:%d!\r\n", m_hListenSocket, nPort);
#else
#ifdef SOCKETLIB_WRITE_LOG
	TCHAR szLog[MAX_LOGBUFFER_LENGTH]={0};
	_stprintf(szLog, TEXT("CSLBlockTcpServer::Open success, socket:%ld port:%d!"), m_hListenSocket, nPort);
	m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif

	return true;
}

bool CSLBlockTcpServer::Close()
{
	if (NULL == m_hListenSocket)
		return false;

	//关闭监听句柄
	shutdown(m_hListenSocket,0);
	closesocket(m_hListenSocket);
	m_hListenSocket = NULL;
	m_threadListen.Stop(200);
	m_threadListen.Kill();
	return true;
}

unsigned int WINAPI CSLBlockTcpServer::ListenProc(LPVOID lParam)
{
	CSLBlockTcpServer *pSrv = (CSLBlockTcpServer*)lParam;
	SOCKET      hClientSocket;
	SOCKADDR_IN siClientSocketAddr;
	int         nClientSocketLen;

	char        szIPAddr[MAX_IPADDR_LENGTH];
	USHORT      nPort = 0;
#ifdef SOCKETLIB_WRITE_LOG
	char        szLog[MAX_LOGBUFFER_LENGTH];
#endif

	nClientSocketLen = sizeof(siClientSocketAddr);
	while (1)
	{		
		if (pSrv->m_threadListen.IsStop())
		{//退出线程
#ifdef SHOW_PRINTFINFO
			printf("ListenProc thread exit!\r\n");
#else
	#ifdef SOCKETLIB_WRITE_LOG
			pSrv->m_pAppLog->WriteLog("ListenProc thread exit!",__FILE__,__LINE__,CAppLog::LOG_LEVEL_7);
	#endif
#endif
			break;
		}

		//置0
		memset(szIPAddr, 0, sizeof(szIPAddr));

		//接收客户的请求
		hClientSocket = accept(pSrv->m_hListenSocket,(SOCKADDR*)&siClientSocketAddr,&nClientSocketLen);
		if (INVALID_SOCKET == hClientSocket)
		{
#ifdef SHOW_PRINTFINFO
			printf("accept invalid socket!\r\n");
#else
	#ifdef SOCKETLIB_WRITE_LOG
			pSrv->m_pAppLog->WriteLog("accept invalid socket!",__FILE__,__LINE__,CAppLog::LOG_LEVEL_7);
	#endif
#endif
			continue;
		}

		//取得客户端IP信息
		strncpy(szIPAddr,inet_ntoa(siClientSocketAddr.sin_addr),MAX_IPADDR_LENGTH);
		nPort = ntohs(siClientSocketAddr.sin_port);

		//调BlockTcpEvent::OnAccept
		if (!pSrv->m_pBlockEvent->OnAccept(hClientSocket))
		{
			closesocket(hClientSocket);
#ifdef SHOW_PRINTFINFO
			printf("ListenSocket:%ld, clientip:%s, port:%d, acceptconnect error!\r\n", 
				pSrv->m_hListenSocket, szIPAddr, nPort);
#else
	#ifdef SOCKETLIB_WRITE_LOG
			sprintf(szLog,"ListenSocket:%ld, clientip:%s, port:%d, acceptconnect error!", 
				pSrv->m_hListenSocket, szIPAddr, nPort);
			pSrv->m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
	#endif
#endif
			continue;
		}

		//设置Socket配置项
		CSocketAPI::SetTimeOut(hClientSocket, pSrv->m_nRecvTimeOut, pSrv->m_nSendTimeOut);
		CSocketAPI::SetBufferSize(hClientSocket, pSrv->m_nRecvBufferSize, pSrv->m_nSendBufferSize);

		//推入队列
		if (!pSrv->m_pBlockRunner->Push(hClientSocket))
		{
			closesocket(hClientSocket);
#ifdef SHOW_PRINTFINFO
			printf("Socket: %d, clientip:%s, port: %d, refuse; current online total:%ld \r\n", 
				hClientSocket, szIPAddr, nPort, pSrv->m_pBlockRunner->GetQueueSize());
#else
#ifdef SOCKETLIB_WRITE_LOG
			sprintf(szLog,"ListenSocket:%ld, clientip:%s, port: %d, refuse; current online total:%ld",
				pSrv->m_hListenSocket, szIPAddr, nPort, pSrv->m_pBlockRunner->GetQueueSize());
			pSrv->m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_7);
#endif
#endif
			continue;
		}

#ifdef SHOW_PRINTFINFO
		printf("ListenSocket:%ld, clientip:%s, port:%d, connected; total:%ld\r\n",
			pSrv->m_hListenSocket, szIPAddr, nPort, pSrv->m_pBlockRunner->GetQueueSize());
#else
	#ifdef SOCKETLIB_WRITE_LOG
		sprintf(szLog,"ListenSocket:%ld, clientip:%s, port:%d, connected; total:%ld",
			pSrv->m_hListenSocket, szIPAddr, nPort, pSrv->m_pBlockRunner->GetQueueSize());
		pSrv->m_pAppLog->WriteLog(szLog, __FILE__, __LINE__, CAppLog::LOG_LEVEL_5);
	#endif
#endif

	}

	return 0;
}

}