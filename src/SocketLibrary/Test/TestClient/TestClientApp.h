#pragma once
#include "../../socketlibrary/appbase.h"
#include "../../socketlibrary/applog.h"
#include "../../socketlibrary/tcprunner.h"
#include "../../socketlibrary/tcpserver.h"
#include "../../socketlibrary/tcpclient.h"
#include "../../socketlibrary/udpsource.h"
#include "../../socketlibrary/udprunner.h"
#include "../../socketlibrary/slcommondef.h"
#include "TestSocketEvent.h"

using namespace SL;
class CTestClientApp : public CAppBase
{
public:
	CTestClientApp(void);
	~CTestClientApp(void);

	virtual void DoReadyWork(); //运行前的准备工作(如设置NT服务名等)
	virtual BOOL DoRun();
	virtual BOOL DoStop();

private:
	CAppLog				m_appLog;
	CTcpRunner			m_tcpRunner;
	CTcpClient			m_tcpHttp;     //访问web网站
	CTcpClient			m_tcpTest;     //普通服务
	CTestSocketEvent	m_testSocketEvent;

	CUdpRunner			m_udpRunner;
	CUdpSource			m_udpClient;

};
