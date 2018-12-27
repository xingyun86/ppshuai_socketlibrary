//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H
#pragma once
#include <stdlib.h>
#include <memory.h>

namespace SLEXT
{

//the owner option is used when the CByteBuffer object can't 
//keep a copy of the data available
class SOCKETLIBRARY_API CByteBuffer
{
public:
	typedef char value_type;
	typedef char* pointer;
	typedef const char* const_pointer;
	typedef char* iterator;
	typedef const char* const_iterator;
	typedef std::size_t size_type;

private:
	CByteBuffer(const CByteBuffer& oByteBuffer);
	CByteBuffer &operator= (const CByteBuffer &);

public:
	CByteBuffer()
		: m_szData(0)
		, m_nSize(0)
		, m_bOwner(true)
		, m_nCapacity(0)
	{
	};
	CByteBuffer(size_t nCapacity)
		: m_szData(0)
		, m_nSize(0)
		, m_bOwner(true)
		, m_nCapacity(nCapacity)
	{
		m_szData	= (char*)malloc(nCapacity);
		m_nCapacity = nCapacity;
	};
	CByteBuffer(const char *szData, size_t nSize, bool bOwner = true)
	{
		m_nSize	 = nSize;
		m_bOwner = bOwner;
		m_nCapacity = nSize;
		if (bOwner)
		{
			m_szData = (char*)malloc(nSize);
			memcpy(m_szData, szData, nSize);
		}
		else
		{
			m_szData = (char*)szData;
		}
	};
	virtual ~CByteBuffer()
	{
		clear();
	};

	bool owner() const
	{
		return m_bOwner;
	}
	void owner(bool bOwner)
	{
		m_bOwner = bOwner;
	}

	iterator data()
	{
		return m_szData;
	};
	const_iterator data() const
	{
		return m_szData;
	};
	void data(const char *szData, size_t nSize, bool bOwner = true)
	{
		if (nSize <= 0)
			return;
		if (szData == m_szData)
			return;

		if (bOwner)
		{
			if (m_bOwner)
			{
				if (m_nCapacity < nSize)
				{
					if (m_nCapacity <= 0)
					{
						m_szData = (char*)malloc(nSize);
					}
					else
					{
						m_szData = (char*)realloc(m_szData, nSize);
					}
					m_nCapacity  = nSize;
				}
			}
			else
			{
				m_szData    = (char*)malloc(nSize);
				m_nCapacity = nSize;
			}
			memcpy(m_szData, szData, nSize);
		}
		else
		{
			if (m_bOwner && (m_szData != 0))
				free(m_szData);
			m_szData    = (char*)szData;
			m_nCapacity = nSize;
		}
		m_nSize	 = nSize;
		m_bOwner = bOwner;
	};
	size_type capacity() const
	{
		return m_nCapacity;
	};
	size_type size() const
	{
		return m_nSize;
	};
	size_type max_size() const
	{// estimate maximum array size
		//size_type nCount = (size_type)(-1) / sizeof(char);
		//return (0<nCount ? nCount:1);
		return (size_type)(-1);
	};

	void clear()
	{
		if ( (m_bOwner) && (m_szData!=0) )
		{
			free(m_szData);
		}
		m_szData	= 0;
		m_nSize		= 0;
		m_nCapacity = 0;
		m_bOwner	= true;
	};
	bool empty() const
	{
		return m_nSize == 0;
	};
	iterator begin()
	{
	    return m_szData;
	};
	const_iterator begin() const
	{
	    return m_szData;
	};
	iterator end()
	{
	    return m_szData+m_nSize;
	};
	const_iterator end() const
	{
	    return m_szData+m_nSize;
	};

	//nMultiple: 缓冲区扩大倍数
	bool append(const char *szData, size_t nSize, unsigned int nMultiple = 100)
	{
		if ((!m_bOwner) || (nSize<=0))
			return false;
		if (max_size()-m_nSize < nSize)
			return false;

		if (m_nCapacity >= (m_nSize+nSize))
		{
			memcpy(m_szData+m_nSize, szData, nSize);
		}
		else
		{
			size_type nNewCapacity = m_nCapacity + max(m_nCapacity*nMultiple/100, nSize);
			char *pNew =(char*)malloc(nNewCapacity);
			memcpy(pNew, m_szData, m_nSize);
			memcpy(pNew+m_nSize, szData, nSize);
			if (m_szData != 0)
				free(m_szData);
			m_szData	= pNew;
			m_nCapacity = nNewCapacity;
		}
		m_nSize += nSize;
		return true;
	};
	bool append(const char *szData)
	{
		return append(szData,strlen(szData));
	};
	bool append(char c)
	{
		return append((const char*)&c,sizeof(char));
	};
	bool append(int n)
	{
		return append((const char*)&n,sizeof(int));
	};
	bool append(short n)
	{
		return append((const char*)&n,sizeof(short));
	};
	bool append(float n)
	{
		return append((const char*)&n,sizeof(float));
	};
	bool append(double n)
	{
		return append((const char*)&n,sizeof(double));
	};
	void resize(size_t nSize)
	{//设置当前占用Buffer大小
		if (m_nCapacity >= nSize)
			m_nSize = nSize;
		else
			m_nSize = m_nCapacity;
	}
	bool reserve(size_t nCapacity)
	{
		if (m_bOwner)
		{
			if (m_nCapacity > 0)
				m_szData = (char*)realloc(m_szData, nCapacity);
			else
				m_szData = (char*)malloc(nCapacity);
			m_nSize		= 0;
			m_nCapacity	= nCapacity;
		}
		return true;
	}

private:
	std::size_t  m_nSize;       //当前占用Buffer大小
	std::size_t  m_nCapacity;   //Buffer的容量
	bool		 m_bOwner;		//表示是否拥有所有权
	char		*m_szData;      //缓冲区
};

};

#endif