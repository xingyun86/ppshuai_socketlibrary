#pragma once
#include "SLAlloc.h"

namespace SL
{

template <typename T,
	class TAlloc = SLBASICALLOC,
	std::size_t NODE_SIZE = sizeof(T), 
	std::size_t MEMORY_BLOCK_SIZE = DEFAULT_MEMORY_BLOCK_SIZE>
class CSLNormalMemoryPool : public CSLAllocator<T>
{
public:
	enum { BLOCK_NODE_COUNT = (MEMORY_BLOCK_SIZE-sizeof(void*))/(NODE_SIZE+sizeof(void*)) };

private:
	struct FreeNode
	{
		FreeNode *pNext;
		char	  buffer[NODE_SIZE];
	};
	struct MemBlock
	{
		MemBlock *pNext;
		FreeNode  node[BLOCK_NODE_COUNT];
	};
	struct DestroyNode
	{
		DestroyNode	*pNext;
	};

	MemBlock	*m_pMemBlockHeader;
	FreeNode	*m_pFreeNodeHeader;
	DestroyNode	*m_destroyChain;
	TAlloc		 m_Alloc;

public:
	CSLNormalMemoryPool()
		: m_pMemBlockHeader(NULL)
		, m_pFreeNodeHeader(NULL)
		, m_destroyChain(NULL)
	{
	};
	explicit CSLNormalMemoryPool(TAlloc alloc)
		: m_Alloc(alloc)
		, m_pMemBlockHeader(NULL)
		, m_pFreeNodeHeader(NULL)
		, m_destroyChain(NULL)
	{
	}
	~CSLNormalMemoryPool()
	{
		clear();
	};

	void clear()
	{
		//DestroyNode	*pCurr;
		//T			*p;
		//while (m_destroyChain)
		//{
		//	pCurr = m_destroyChain;
		//	m_destroyChain = m_destroyChain->pNext;
		//	p = (T*)(pCurr+1);
		//	p->~T();
		//};
		MemBlock *pTemp;
		while (m_pMemBlockHeader)
		{
			pTemp = m_pMemBlockHeader->pNext;
			m_Alloc.deallocate(m_pMemBlockHeader);
			m_pMemBlockHeader = pTemp;
		};
	};

	void* allocate(size_t n = sizeof(T))
	{
		if (n > NODE_SIZE)
			return NULL;
		if (m_pFreeNodeHeader==NULL)
		{
			MemBlock *pNewBlock = (MemBlock*)m_Alloc.allocate(sizeof(MemBlock));
			for (size_t i = 0; i<BLOCK_NODE_COUNT-1; ++i)
				pNewBlock->node[i].pNext = &pNewBlock->node[i+1];
			pNewBlock->node[BLOCK_NODE_COUNT-1].pNext = NULL;
			m_pFreeNodeHeader = &pNewBlock->node[0];
			pNewBlock->pNext  = m_pMemBlockHeader;
			m_pMemBlockHeader = pNewBlock;
		}
		void *p = m_pFreeNodeHeader->buffer;
		m_pFreeNodeHeader = m_pFreeNodeHeader->pNext;
		return p;
	};
	void deallocate(void *p)
	{
		FreeNode *pNode		= (FreeNode*)((char*)p-sizeof(void*));
		pNode->pNext		= m_pFreeNodeHeader;
		m_pFreeNodeHeader	= pNode;
		return;
	};
	pointer construct()
	{
		void *p = allocate(sizeof(T));
		pointer pRet = new (p) value_type();
		return pRet;
	};
	void construct(void *p)
	{
		new (p) value_type();
	};
	void destroy(void *p)
	{
		((pointer)p)->~T();
		deallocate(p);
	};
};

};