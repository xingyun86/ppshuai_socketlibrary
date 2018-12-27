//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

// AppServer.cpp : Defines the entry point for the DLL application.
//
#include "AppBase.h"
#include "SocketAPI.h"
namespace SL
{

CAppBase::CAppBase(LPCTSTR pszServiceName, LPCTSTR pszDisplayName)
	: CNTService(pszServiceName,pszDisplayName)
	, m_oEventStop(true,false)
{
	DoReadyWork();
}

CAppBase::~CAppBase()
{

}

void CAppBase::Stop()
{
	ReportStatus(SERVICE_STOP_PENDING, 11000);
	m_oEventStop.Signal();
	return;
};

void CAppBase::Run(DWORD dwArgc, LPTSTR *ppszArgv)
{
	ReportStatus(SERVICE_START_PENDING);
	ReportStatus(SERVICE_RUNNING);
	CSocketAPI::LoadSocketLib(SL_WINSOCK_VERSION);

	//main loop
	if (!DoRun())
		m_oEventStop.Signal();
	
	m_oEventStop.Lock();
	DoStop();
	CSocketAPI::UnloadSocketLib();

	return;
}

//运行前的准备工作(如设置NT服务名等)
void CAppBase::DoReadyWork()
{
	return;
}

BOOL CAppBase::DoRun()
{
	return FALSE;
}

BOOL CAppBase::DoStop()
{
	return TRUE;
}

}
