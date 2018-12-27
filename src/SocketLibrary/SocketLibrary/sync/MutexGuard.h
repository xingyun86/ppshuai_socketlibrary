#pragma once

namespace SYNC
{

template <class TMutex>
class CMutexGuard
{
private:
	//= Prevent assignment and initialization.
	// Disallow copying and assignment.
    CMutexGuard(const CMutexGuard &);
    void operator= (const CMutexGuard &);

public:
	CMutexGuard(TMutex & mtx)
	 : m_mtx(mtx)
	{
		m_mtx.Lock();
	};

	CMutexGuard(TMutex & mtx, DWORD nTimeOut)
	 : m_mtx(mtx)
	{
		m_mtx.Lock(nTimeOut);
	};

	~CMutexGuard(void)
	{
		m_mtx.UnLock();
	};

private:
	TMutex &m_mtx;

};

};