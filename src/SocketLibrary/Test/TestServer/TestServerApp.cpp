#include "StdAfx.h"
#include ".\testserverapp.h"
#include <string>

// CAccountSrvApp 构造
#ifdef _DEBUG
CTestServerApp::CTestServerApp()
	: CAppBase(TEXT("TestServer"), TEXT("Socketlib test server"))
#else
CTestServerApp::CTestServerApp()
	: CAppBase(TEXT("TestServer"), TEXT("Socketlib test server"))
#endif
{
#ifdef _DEBUG
	SetupConsole();
#endif
	DoReadyWork();
};

CTestServerApp::~CTestServerApp()
{
	DoStop();
}

//运行前的准备工作(如设置NT服务名等)
void CTestServerApp::DoReadyWork()
{
	return;
}

BOOL CTestServerApp::DoRun()
{
	m_testSocketEvent.m_pApp = this;
	m_tcpRunner.Open(0,4);
	m_appLog.OpenLog("C:\\log","cli",CAppLog::LOG_LEVEL_5,3);

	//启动TcpServer
	m_tcpServer.SetConfig(100000,500,30);
	m_tcpServer.SetInterface(&m_appLog,&m_testSocketEvent,&m_tcpRunner);
	m_tcpServer.SetBufferSize(1024,1024);
	m_tcpServer.Open("127.0.0.1",3050,25);

	m_udpRunner.Open(0,4);
	m_udpServer.SetInterface(&m_appLog,&m_testSocketEvent,&m_udpRunner);
	m_udpServer.SetBufferSize(1024,1024);
	m_udpServer.Open(3050);

	m_taskTest.m_pApp = this;
	m_taskTest.Activate(1,1000);
	return TRUE;
}

BOOL CTestServerApp::DoStop()
{
	m_tcpRunner.Close();
	m_tcpServer.Close();
	m_appLog.WriteLog("app end!",__FILE__,__LINE__,CAppLog::LOG_LEVEL_LAST);
	m_appLog.CloseLog();
	return TRUE;
}

