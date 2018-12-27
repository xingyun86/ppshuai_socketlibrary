#pragma once

#include <stdlib.h>         
#include <malloc.h>
#include <windows.h>
#include "sync/nullmutex.h"
#include "sync/guard.h"

namespace SL
{
//默认的内存块大小，经验值
#define DEFAULT_MEMORY_BLOCK_SIZE	8192

class CSLBasicNewAlloc
{
public:
	static void* allocate(size_t n)
	{ 
		return new char[n];
	};
	static void deallocate(void *p)
	{ 
		delete []p;
	};
	static void* reallocate(void *p, size_t n)
	{
		return 0;
	};
};
class CSLBasicMallocAlloc
{
public:
	static void* allocate(size_t n)
	{ 
		return malloc(n);
	};
	static void deallocate(void *p)
	{ 
		free(p);
	};
	static void* reallocate(void *p, size_t n)
	{
		return realloc(p, n);
	};
};
class CSLBasicHeapAlloc
{
public:
	static void* allocate(size_t n)
	{ 
		return ::HeapAlloc(m_hProcessHeap, 0, n);
	};
	static void deallocate(void *p)
	{ 
		::HeapFree(m_hProcessHeap, 0, p);
	};
	static void* reallocate(void *p, size_t n)
	{
		return ::HeapReAlloc(m_hProcessHeap, 0, p, n);
	};
	static HANDLE m_hProcessHeap;
};
HANDLE CSLBasicHeapAlloc::m_hProcessHeap = ::GetProcessHeap();

#ifdef SL_BASIC_NEWALLOC
	#define SLBASICALLOC CSLBasicNewAlloc
#elif STLP_BASIC_MALLOC
	#define SLBASICALLOC CSLBasicMallocAlloc
#else
	#define SLBASICALLOC CSLBasicHeapAlloc
#endif

template <class TMutex = SYNC::CNullMutex, class TBasicAlloc = SLBASICALLOC>
class CSLBlockAlloc
{
private:
	struct Block 
	{
		Block *next;
	};
	Block *m_lstFree;

	size_t m_nFreeLimit;
	size_t m_nFreeSize;
	size_t m_nBlockSize;

	TMutex		m_mtx;
	TBasicAlloc m_BasicAlloc;
private:
	CSLBlockAlloc(const CSLBlockAlloc&);
	void operator=(const CSLBlockAlloc&);

public:
	CSLBlockAlloc(size_t nFreeLimit=1024, size_t nBlockSize = DEFAULT_MEMORY_BLOCK_SIZE)
		: m_lstFree(NULL)
		, m_nBlockSize(nBlockSize)
		, m_nFreeSize(0)
		, m_nFreeLimit(nFreeLimit / nBlockSize + 1)
	{
	}
	~CSLBlockAlloc()
	{
		clear();
	}

public:
	void* allocate(size_t n)
	{
		SYNC::CGuard<TMutex> guard(m_mtx);
		if (n > (size_t)m_nBlockSize)
			return m_BasicAlloc.allocate(n);
		else
		{
			if (m_lstFree)
			{
				Block *blk  = m_lstFree;
				m_lstFree	= blk->next;
				--m_nFreeSize;
				return blk;
			}
			return m_BasicAlloc.allocate(m_nBlockSize);
		}
	}
	void deallocate(void* p)
	{
		SYNC::CGuard<TMutex> guard(m_mtx);
		if (m_nFreeSize >= m_nFreeLimit) 
		{
			m_BasicAlloc.deallocate(p);
		}
		else 
		{
			Block *blk = (Block*)p;
			blk->next  = m_lstFree;
			m_lstFree  = blk;
			++m_nFreeSize;
		}
	}
	void clear()
	{
		SYNC::CGuard<TMutex> guard(m_mtx);
		while (m_lstFree)
		{
			Block* blk = m_lstFree;
			m_lstFree  = blk->next;
			m_BasicAlloc.deallocate(blk);
		}
		m_nFreeSize = 0;
	}
};

template <class TAlloc>
class CSLProxyAlloc
{
public:
	CSLProxyAlloc(TAlloc &oAlloc) : m_pAlloc(&oAlloc){};

public:
	void* allocate(size_t n)
	{ 
		return m_pAlloc->allocate(n); 
	}
	void deallocate(void *p)
	{ 
		m_pAlloc->deallocate(p); 
	}

private:
	CSLProxyAlloc();
	TAlloc *m_pAlloc;

};

template <typename T>
class CSLAllocator
{
public:
	typedef T			value_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;

public:
	virtual void clear()
	{
	};
	virtual void* allocate(size_type n)
	{ 
		return SLBASICALLOC::allocate(n);
	};
	virtual void deallocate(void *p)
	{ 
		SLBASICALLOC::deallocate(p);
	};
	virtual pointer construct()
	{
		void *p = SLBASICALLOC::allocate(sizeof(T));
		pointer pRet = new (p) value_type();
		return pRet;
	};
	virtual void construct(void *p)
	{
		new (p) value_type();
	};
	virtual void destroy(void *p)
	{
		((pointer)p)->~T();
		deallocate(p);
	};
	static CSLAllocator* Instance()
	{
		return &m_Alloc;
	};
private:
	static CSLAllocator<char> m_Alloc;
};

template <typename TMemPool, class TMutex>
class CSLAllocator_Adapter
{
public:
	CSLAllocator_Adapter()
	{
	}
	void clear()
	{
		m_mtx.Lock();
		m_oMemPool.clear();
		m_mtx.UnLock();
	}
	void* allocate(size_t n)
	{ 
		m_mtx.Lock();		
		void *p = m_oMemPool.allocate(n);
		m_mtx.UnLock();
		return p;
	};
	void deallocate(void *p)
	{ 
		m_mtx.Lock();
		m_oMemPool.deallocate(p);
		m_mtx.UnLock();
	};
	void* construct()
	{
		m_mtx.Lock();
		void *p = m_oMemPool.construct();
		m_mtx.UnLock();
		return p;
	};
	void construct(void *p)
	{
		m_mtx.Lock();
		m_oMemPool.construct(p);
		m_mtx.UnLock();
	};
	void destroy(void *p)
	{
		m_mtx.Lock();
		m_oMemPool.destroy(p);
		m_mtx.UnLock();
	};
	TMemPool* get_allocator()
	{
		return &m_oMemPool;
	};
private:
	TMemPool    m_oMemPool;
	TMutex		m_mtx;
};

};