#include "StdAfx.h"
#include ".\testtask.h"
#include ".\testserverapp.h"

CTestTask::CTestTask(void)
{
}

CTestTask::~CTestTask(void)
{
}

int CTestTask::ProcessMessage(CSLMessageQueueNode& oNode)
{
	printf("tcprecv Socket:%ld msg:(%s) len:%d\r\n", oNode.attachID, oNode.msg.data(), oNode.msg.size());

	//将收到的数据直接发回客户
	m_pApp->m_tcpServer.Send(oNode.attachID, oNode.msg.data(), oNode.msg.size());
	printf("Socket:%ld sendmsg:(%s) len:%d\r\n", oNode.attachID, oNode.msg.data(), oNode.msg.size());

	return 1;
}