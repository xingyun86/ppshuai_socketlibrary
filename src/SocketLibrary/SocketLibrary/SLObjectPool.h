#pragma once

namespace SL
{

template <typename T, class TAllocator = CSLAllocator>
class CSLObjectPool
{
public:
	static T* construct(TAllocator *pAlloc)
	{
		void *p = pAlloc->allocate(sizeof(element_type));
		T *pRet = new (p) T(); //调类的构造函数
		return pRet;
	};
	static void destroy(TAllocator *pAlloc, T *pObj)
	{
		pObj->~T(); //调类的析构函数
		pAlloc->deallocate(pObj);
	};

	CSLObjectPool()
	{
		m_pAlloc = (TAllocator*)SLAllocator::Instance();
	};
	CSLObjectPool(TAllocator *pAlloc)
		: m_pAlloc(pAlloc)
	{
	};
	~CSLObjectPool()
	{
	};

	T* construct()
	{
		return (T*)m_pAlloc->construct();
	};
	void destroy(T *pObj)
	{
		m_pAlloc->destroy((void*)pObj);
	};

private:
	TAllocator	*m_pAlloc;
};

};