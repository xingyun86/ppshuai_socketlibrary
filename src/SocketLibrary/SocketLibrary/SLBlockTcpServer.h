#pragma once

#include "applog.h"
#include "sync/Thread.h"
#include "slblocktcprunner.h"
#include "slblocktcpevent.h"

namespace SL
{

class SOCKETLIB_API CSLBlockTcpServer
{
public:
	CSLBlockTcpServer(void);
	virtual ~CSLBlockTcpServer(void);

	void SetInterface(CAppLog *pAppLog, CSLBlockTcpEvent *pBlockEvent, CSLBlockTcpRunner *pBlockRunner);
	void SetBufferSize(int nRecvBufferSize, int nSendBufferSize);
	void SetTimeOut(int nRecvTimeOut, int nSendTimeOut);

	bool Open(const char *szLocalIP, unsigned short nPort, int nBacklog = 10);
	bool Close();

private:
	static unsigned int WINAPI ListenProc(LPVOID lParam);
private:
	CAppLog				*m_pAppLog;
	CSLBlockTcpEvent	*m_pBlockEvent;
	CSLBlockTcpRunner	*m_pBlockRunner;

	//listen
	SYNC::CThread		m_threadListen;
	SOCKET				m_hListenSocket;

	//常用Socket配置项(缓冲区，超时)
	int					m_nRecvTimeOut;		//时间单位为ms
	int					m_nSendTimeOut;		//时间单位为ms
	int					m_nRecvBufferSize;
	int					m_nSendBufferSize;
};

};