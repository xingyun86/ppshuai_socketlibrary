//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#pragma once
#include "../config.h"
#include "guard.h"
namespace SYNC
{
class SOCKETLIBRARY_API CThreadManager
{
public:
	CThreadManager();
	~CThreadManager();

protected:
	CThreadMutex		m_lock;

};

};