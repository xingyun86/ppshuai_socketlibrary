#include "StdAfx.h"
#include ".\testclientapp.h"
#include <string>
#include "../../socketlibrary/slcommondef.h"
#include "../../socketlibrary/slext/ByteBuffer.h"

// CAccountSrvApp 构造
#ifdef _DEBUG
CTestClientApp::CTestClientApp()
	: CAppBase(TEXT("TestClient"), TEXT("Socketlib test client"))
#else
CTestClientApp::CTestClientApp()
	: CAppBase(TEXT("TestClient"), TEXT("Socketlib test client"))
#endif
{
#ifdef _DEBUG
	SetupConsole();
#endif
	DoReadyWork();
};

CTestClientApp::~CTestClientApp()
{
	DoStop();
}

//运行前的准备工作(如设置NT服务名等)
void CTestClientApp::DoReadyWork()
{
	return;
}

BOOL CTestClientApp::DoRun()
{
	std::string str;

	m_appLog.OpenLog("C:\\log","cli",CAppLog::LOG_LEVEL_5,3);

	m_tcpRunner.Open(0,4);
	m_tcpTest.SetInterface(&m_appLog,&m_testSocketEvent,&m_tcpRunner);
	m_tcpTest.SetBufferSize(1024,1024);
	m_tcpTest.Open("127.0.0.1", 3050, 0);

	m_udpRunner.Open(0,2);
	m_udpClient.SetInterface(&m_appLog,&m_testSocketEvent,&m_udpRunner);
	m_udpClient.SetBufferSize(1024,1024);
	m_udpClient.Open("127.0.0.1",3050,2050);
	
	char szSend[]={"Welcome you user socketlib"};
	m_udpClient.Send(szSend,20,"127.0.0.1",3050,1);

	str = "Welcome you use socketlib";
	m_tcpTest.Send(str.c_str(), str.size(),0);
	Sleep(10);
	m_tcpTest.Send(str.c_str(), str.size(),2);
	Sleep(10);
	m_tcpTest.Send(str.c_str(), str.size(),4);
	m_tcpTest.Send(str.c_str(), str.size(),3);
	m_tcpTest.Send(str.c_str(), str.size(),5);
	m_tcpTest.Send(str.c_str(), str.size(),6);
	m_tcpTest.Send(str.c_str(), str.size(),7);
	Sleep(10);

	////以下代码测试连接断开后，自动重连服务器
	//m_tcpTest.Close();
	//for (int i=0;i<1000;i++)
	//{
	//	m_tcpTest.Send(str.c_str(), str.size());
	//	m_tcpTest.Send(str.c_str(), str.size());
	//	m_tcpTest.Send(str.c_str(), str.size());
	//	m_tcpTest.Send(str.c_str(), str.size());
	//}

	////测试http协议
	//m_tcpHttp.SetInterface(&m_appLog,&m_testSocketEvent,&m_tcpRunner);
	//m_tcpHttp.SetBufferSize(1024,32);
	//m_tcpHttp.Open("www.google.com",80,0);
	//str = "GET / HTTP/1.1\r\nHost:www.google.com\r\nAccept:*/*\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\r\nConnection:Keep-Alive\r\n\r\n";
	//m_tcpHttp.Send(str.c_str(), str.size());

	//Sleep(100);
	return TRUE;
}

BOOL CTestClientApp::DoStop()
{
	m_tcpRunner.Close();
	m_tcpHttp.Close();
	m_appLog.WriteLog("app end!",__FILE__,__LINE__,CAppLog::LOG_LEVEL_LAST);
	m_appLog.CloseLog();
	return TRUE;
}

