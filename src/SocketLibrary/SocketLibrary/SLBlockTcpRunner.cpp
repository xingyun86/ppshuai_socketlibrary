#include "SLBlockTcpRunner.h"

namespace SL
{

CSLBlockTcpRunner::CSLBlockTcpRunner(void)
	: m_nQueueMaxSize(200)
{
}

CSLBlockTcpRunner::~CSLBlockTcpRunner(void)
{
	Close();
}

void CSLBlockTcpRunner::SetInterface(CSLBlockTcpEvent *pBlockEvent)
{
	m_pBlockEvent = pBlockEvent;
}

bool CSLBlockTcpRunner::Open(uint16 nMaxThreadNum, uint16 nDefaultThreadNum,uint32 nQueueMaxSize)
{
	m_nQueueMaxSize = nQueueMaxSize;
	m_semaphore.Create(NULL, 0, nQueueMaxSize, NULL);
	return m_threadGroup.Start(CSLBlockTcpRunner::Run, this, nMaxThreadNum);
}

bool CSLBlockTcpRunner::Close()
{
	int nThreadCount = GetThreadNum();
	for (int i=0; i<nThreadCount; i++)
		m_semaphore.UnLock();
	m_semaphore.Clear();
	m_threadGroup.Stop(200);
	m_threadGroup.Kill();
	m_lstSocket.clear();
	return true;
}

inline int CSLBlockTcpRunner::GetThreadNum() const
{
	return m_threadGroup.GetThreadCount();
}

inline int CSLBlockTcpRunner::GetQueueSize() const
{
	return m_lstSocket.size();
}

bool CSLBlockTcpRunner::Push(SOCKET hSocket)
{
	bool bRet = false;
	m_lock.Lock();
	if (m_nQueueMaxSize > m_lstSocket.size())
	{
		m_lstSocket.push_back(hSocket);
		m_semaphore.UnLock();
		bRet = true;
	}
	m_lock.UnLock();	
	return bRet;
}

SOCKET CSLBlockTcpRunner::Pop()
{
	SOCKET hSocket = NULL;
	m_lock.Lock();
	if (!m_lstSocket.empty())
	{
		hSocket = *m_lstSocket.begin();
		m_lstSocket.pop_front();
	}
	m_lock.UnLock();
	return hSocket;
};

unsigned int __stdcall CSLBlockTcpRunner::Run(LPVOID lParam)
{
	CSLBlockTcpRunner *pRun = (CSLBlockTcpRunner*)lParam;
	CSLBlockTcpClient oBlockClient(NULL);
	SOCKET hSocket = NULL;

	while (1)
	{
		//退出线程
		if (pRun->m_threadGroup.IsStop())
			return -1;

		pRun->m_semaphore.Lock();
		hSocket = pRun->Pop();
		if (hSocket > 0)
		{//取到处理Socket句柄
			oBlockClient.Attach(hSocket);
			if (pRun->m_pBlockEvent->OnProcess(oBlockClient) < 0)
			{//当处理结果<0时，关闭Sokcet
				oBlockClient.Close();
			}
		}
	}
	return 0;
}

}