#pragma once

#include "../../socketlibrary/sync/threadmutex.h"
#include "../../socketlibrary/sltask.h"

using namespace SL;
class CTestServerApp;
class CTestTask : public CSLTask<SYNC::CThreadMutex>
{
public:
	CTestTask(void);
	virtual ~CTestTask(void);

	//消息处理函数
	int ProcessMessage(CSLMessageQueueNode& oNode);
public:
	CTestServerApp *m_pApp;
};
