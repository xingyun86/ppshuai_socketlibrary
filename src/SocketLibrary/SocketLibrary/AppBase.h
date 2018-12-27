//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef APPBASE_H
#define APPBASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "NTService.h"
#include "sync/event.h"
namespace SL
{

class SOCKETLIBRARY_API CAppBase : public CNTService
{
public:
	CAppBase(LPCTSTR pszServiceName, LPCTSTR pszDisplayName);
	virtual ~CAppBase();

	virtual void DoReadyWork(); //运行前的准备工作(如设置NT服务名等)
	virtual BOOL DoRun();
	virtual BOOL DoStop();

protected:
	virtual void Run(DWORD dwArgc, LPTSTR *ppszArgv);//Service Mainloop
	virtual void Stop();//Stop service

protected:
	SYNC::CEvent m_oEventStop; //NTService used

};

};

#endif