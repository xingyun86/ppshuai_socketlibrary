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
namespace SYNC
{

template <class TMutex>
class CGuard
{
private:
	//= Prevent assignment and initialization.
	// Disallow copying and assignment.
    CGuard(const CGuard &);
    void operator= (const CGuard &);
	CGuard();

public:
	CGuard(TMutex & mtx)
	 : m_mtx(mtx)
	{
		m_mtx.Lock();
	};

	CGuard(TMutex & mtx, DWORD nTimeOut)
	 : m_mtx(mtx)
	{
		m_mtx.Lock(nTimeOut);
	};

	~CGuard(void)
	{
		m_mtx.UnLock();
	};

private:
	TMutex &m_mtx;

};

};