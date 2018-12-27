// class CSLStaticMemoryPool - see winx::stdex::AutoFreeAllocT
// 局部内存分配器: 应用于函数内部/某一范围内的不断重复分配内存
#pragma once
#include "SLAlloc.h"

namespace SL
{

template <typename T,
	class TAlloc = SLBASICALLOC, 
	std::size_t MEMORY_BLOCK_SIZE = DEFAULT_MEMORY_BLOCK_SIZE>
class CSLScopeMemoryPool : public CSLAllocator<T>
{
public:
	enum { HeaderSize = sizeof(void*) };
	enum { BlockSize  = MEMORY_BLOCK_SIZE - HeaderSize };

private:
	struct DestroyNode
	{
		DestroyNode	*pNext;
	};
	struct MemBlock
	{
		MemBlock  *pNext;
		char	   buffer[BlockSize];
	};

	char		*m_begin;
	char		*m_end;
	DestroyNode	*m_destroyChain;
	TAlloc		 m_Alloc;

private:
	CSLScopeMemoryPool(const CSLScopeMemoryPool&);
	const CSLScopeMemoryPool& operator= (const CSLScopeMemoryPool&);

	MemBlock* ChainHeader() const
	{
		return (MemBlock*)(m_begin - HeaderSize);
	};
public:

	CSLScopeMemoryPool()
		: m_destroyChain(NULL)
	{
		m_begin = m_end = (char*)HeaderSize;
	};
	explicit CSLScopeMemoryPool(TAlloc alloc)
		: m_Alloc(alloc)
		, m_destroyChain(NULL)
	{
		m_begin = m_end = (char*)HeaderSize;
	}
	~CSLScopeMemoryPool()
	{
		clear();
	};
	void clear()
	{
		DestroyNode	*pCurr;
		T			*p;
		while (m_destroyChain)
		{
			pCurr = m_destroyChain;
			m_destroyChain = m_destroyChain->pNext;
			p = (T*)(pCurr+1);
			p->~T();
		};
		MemBlock *pTemp;
		MemBlock *pHeader = ChainHeader();
		while (pHeader)
		{
			pTemp = pHeader->pNext;
			m_Alloc.deallocate(pHeader);
			pHeader = pTemp;
		}
		m_begin = m_end = (char*)HeaderSize;
	};

	void* allocate(size_t n = sizeof(T))
	{
		if ((size_t)(m_end - m_begin) < n)
		{
			if (n >= BlockSize)
			{
				MemBlock* pHeader	= ChainHeader();
				MemBlock* pNew		= (MemBlock*)m_Alloc.allocate(HeaderSize + n);
				if (pHeader)
				{
					pNew->pNext		= pHeader->pNext;
					pHeader->pNext	= pNew;
				}
				else
				{
					m_end = m_begin = pNew->buffer;
					pNew->pNext = NULL;
				}
				return pNew->buffer;
			}
			else
			{
				MemBlock* pNew	= (MemBlock*)m_Alloc.allocate(sizeof(MemBlock));
				pNew->pNext		= ChainHeader();
				m_begin			= pNew->buffer;
				m_end			= m_begin + BlockSize;
			}
		}
		return m_end -= n;
	};
	void deallocate(void *p)
	{
	};
	pointer construct()
	{
		DestroyNode* pNode	= (DestroyNode*)allocate(sizeof(DestroyNode)+sizeof(T));
		pNode->pNext		= m_destroyChain;
		m_destroyChain		= pNode;
		pointer pRet		= new (pNode+1) value_type();
		return pRet;
	};
	void construct(pointer p)
	{
		new (p) value_type();
	};
	void destroy(void *p)
	{
	};
};

};
