#pragma once
#include "../../socketlibrary/appbase.h"
#include "../../socketlibrary/applog.h"
#include "../../socketlibrary/tcprunner.h"
#include "../../socketlibrary/tcpserver.h"
#include "../../socketlibrary/tcpclient.h"
#include "../../socketlibrary/udprunner.h"
#include "../../socketlibrary/udpsource.h"
#include "TestSocketEvent.h"
#include "TestTask.h"

using namespace SL;
class CTestServerApp : public CAppBase
{
public:
	CTestServerApp(void);
	~CTestServerApp(void);

	virtual void DoReadyWork(); //运行前的准备工作(如设置NT服务名等)
	virtual BOOL DoRun();
	virtual BOOL DoStop();

public:
	CAppLog				m_appLog;
	CTcpRunner			m_tcpRunner;
	CTcpServer			m_tcpServer;
	CTestSocketEvent	m_testSocketEvent;

	CUdpRunner			m_udpRunner;
	CUdpSource			m_udpServer;

	CTestTask			m_taskTest;

};
