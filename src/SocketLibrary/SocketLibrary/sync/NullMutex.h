//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef NULL_MUTEX_H
#define NULL_MUTEX_H

#pragma once
#include "../config.h"

namespace SYNC
{

class SOCKETLIBRARY_API CNullMutex
{
private:
    CNullMutex(const CNullMutex &);
    void operator= (const CNullMutex &);

public:
	CNullMutex();
	~CNullMutex();
	
	static DWORD Wait(DWORD nTimeout);
	static bool Lock(DWORD nTimeout);
	static bool Lock();
	static bool UnLock();
	static bool TryLock();
	static HANDLE GetHandle();

private:
	static bool Clear();
};

};

#endif