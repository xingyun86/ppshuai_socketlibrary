#pragma once

#include "sync/threadmutex.h"
#include "sync/threadgroup.h"
#include "sync/semaphore.h"
#include "slblocktcpevent.h"
#include <list>

namespace SL
{

class SOCKETLIB_API CSLBlockTcpRunner
{
public:
	CSLBlockTcpRunner(void);
	virtual ~CSLBlockTcpRunner(void);

	bool	Open(uint16 nMaxThreadNum = 100,uint16 nDefaultThreadNum = 10,uint32 nQueueMaxSize = 200);
	bool	Close();

	bool	Push(SOCKET hSocket);
	SOCKET	Pop();
	void	SetInterface(CSLBlockTcpEvent *pBlockEvent);

	int		GetThreadNum() const;
	int		GetQueueSize() const;

private:
	static unsigned int WINAPI Run(LPVOID lParam);

private:
	SYNC::CThreadGroup	m_threadGroup;
	SYNC::CSemaphore	m_semaphore;

	SYNC::CThreadMutex	m_lock;
	std::list<SOCKET>	m_lstSocket;
	uint32				m_nQueueMaxSize;

	CSLBlockTcpEvent	*m_pBlockEvent;
};

};