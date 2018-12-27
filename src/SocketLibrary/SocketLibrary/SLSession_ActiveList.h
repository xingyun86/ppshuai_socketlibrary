#pragma once
#include <list>
#include "sync/thread.h"
#include "socketevent.h"

namespace SL
{

template <class TSession, class TMutex>
class CSLSession_ActiveList
{
public:
	CSLSession_ActiveList(void)
	{
		m_nScanTimesSleepTime	= 50;
		m_nScanGroupCount		= 20;
		m_nScanGroupSleepTime	= 20;
	};
	~CSLSession_ActiveList(void)
	{
		Close();
	};
	bool Close()
	{
		m_threadScan.Stop(200);
		m_threadScan.Kill();
		std::list<TSession*>::iterator itSession = m_lstSession.begin();
		for (;itSession != m_lstSession.end(); ++itSession)
			m_pSocketEvent->OnUninitSocket(*itSession);
		m_lstSession.clear();
		return true;
	};

	int Activate(CSocketEvent *pSocketEvent, uint32 nScanTimesSleepTime=50, uint32 nScanGroupCount=20, uint32 nScanGroupSleepTime=20)
	{
		m_pSocketEvent			= pSocketEvent;
		m_nScanTimesSleepTime	= nScanTimesSleepTime;
		m_nScanGroupCount		= nScanGroupCount;
		m_nScanGroupSleepTime	= nScanGroupSleepTime;
		return m_threadScan.Start(CSLSession_ActiveList<TSession,TMutex>::ScanProc, this, false);
	};
	int AppendSession(void *pSession)
	{
		m_lock.Lock();
		m_lstSession.push_back((TSession*)pSession);
		m_lock.UnLock();
		return 0;
	};

private:

	static unsigned int WINAPI ScanProc(LPVOID lParam)
	{
		int nEventID = 0;
		int nScanCount = 0;
		TSession* pSession = NULL;
		std::list<TSession*>::iterator itSession;
		CSLSession_ActiveList<TSession,TMutex> *pActiveList =(CSLSession_ActiveList<TSession,TMutex>*)lParam;
		while (1)
		{
			if (pActiveList->m_threadScan.IsStop())
				return -1;
		
			nScanCount = 1;
			itSession = pActiveList->m_lstSession.begin();
			while (itSession != pActiveList->m_lstSession.end())
			{
				pSession = *itSession;
				nEventID = pSession->GetEventID();
				if (nEventID < 0)
				{//删除Session
					pActiveList->m_lock.Lock();
					itSession = pActiveList->m_lstSession.erase(itSession);
					pActiveList->m_lock.UnLock();
					pActiveList->m_pSocketEvent->OnUninitSocket(pSession); //回收Session
				}
				else 
				{
					if (nEventID > 0)
						pSession->OnEvent(nEventID);

					++itSession;
				}

				if ( (nScanCount % pActiveList->m_nScanGroupCount)==0 ) 
				{
					nScanCount = 1;
					Sleep(pActiveList->m_nScanGroupSleepTime);
				}
				else
					nScanCount++;
			};
			Sleep(pActiveList->m_nScanTimesSleepTime);
		};
		return 0;
	};

private:
	CSocketEvent		   *m_pSocketEvent;
	std::list<TSession*>	m_lstSession;
	SYNC::CThread			m_threadScan;
	TMutex					m_lock;

	//扫描一遍后Sleep时间(ms)
	uint32					m_nScanTimesSleepTime;
	//扫描一组的项数
	uint32					m_nScanGroupCount;
	//扫描一组后Sleep时间(ms)
	uint32					m_nScanGroupSleepTime;

};

};