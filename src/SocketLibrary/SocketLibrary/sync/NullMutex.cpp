//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "nullmutex.h"

namespace SYNC
{

inline CNullMutex::CNullMutex()
{
};

inline CNullMutex::~CNullMutex()
{
};

inline DWORD CNullMutex::Wait(DWORD nTimeout)
{ 
	return 0; 
};

inline bool CNullMutex::Lock(DWORD nTimeout)
{ 
	return false; 
};

inline bool CNullMutex::Lock()
{ 
	return false;
};

inline bool CNullMutex::UnLock()
{ 
	return false;
};

inline bool CNullMutex::TryLock()
{ 
	return false;
};

inline HANDLE CNullMutex::GetHandle()
{
	return NULL;
};

inline bool CNullMutex::Clear()
{
	return true;
};

};