//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

// ---------------------------------------------------------------------
// class CFixedArray - see boost::array
// 固定长度数组
#ifndef FIXED_ARRAY_H
#define FIXED_ARRAY_H

#pragma once
#include "../config.h"

namespace SLEXT
{

template <class T, std::size_t N>
class CFixedArray
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
public:
	CFixedArray()
		: m_nUserSize(0)
	{
	};
	CFixedArray(size_type nSize)
		: m_nUserSize(nSize)
	{
	};

	size_type capacity() const
	{ 
		return N; 
	};
	size_type max_size() const
	{ 
		return N;
	};
	size_type size() const
	{ 
		return m_nUserSize; 
	};
	bool empty() const
	{ 
		return (m_nUserSize == 0); 
	};
	void clear()	
	{ 
		m_nUserSize = 0;
	};
	void resize(size_type n)
	{
		if (n <= capacity())
			m_nUserSize = n;
	};
	void rangecheck(size_type i) const 
	{
        if (i >= size())
            throw std::range_error("CFixedArray<>: index out of range");
	};

	// 建议：除非必要,不要使用这三函数
    // direct access to data (read-only)
    // use array as C array (direct read/write access to data)
    const T* data() const 
	{ 
		return m_data; 
	};
    T* data() 
	{ 
		return m_data; 
	};
    T* c_array() 
	{ 
		return m_data;
	};

    iterator begin()
	{ 
		return m_data; 
	};
    const_iterator begin() const	
	{ 
		return m_data; 
	};
    iterator end()
	{ 
		return m_data+m_nUserSize; 
	};
    const_iterator end() const	
	{ 
		return m_data+m_size; 
	};
	reference front()
	{ 
		return (*begin());
	};
	const_reference front() const	
	{ 
		return (*begin()); 
	};
	reference back()
	{ 
		return (*(end() - 1)); 
	};
	const_reference back() const	
	{ 
		return (*(end() - 1)); 
	};

    // operator[]
    reference operator[](size_type i) 
    { 
		rangecheck(i);
		return m_data[i];
    };        
    const_reference operator[](size_type i) const 
    {     
		rangecheck(i);
		return m_data[i];
	};
	reference at(size_type i)
	{
		SL_ASSERT( i < m_nUserSize && "out of range" );
		return m_data[i];
	};
	const_reference at(size_type i) const
	{
		SL_ASSERT( i < m_nUserSize && "out of range");
		return m_data[i];
	};

	void push_back(const_reference val)
	{
		if (m_nUserSize < N)
			m_data[m_nUserSize++] = val;
	};
	void pop_back()
	{
		if (m_nUserSize)
			--m_nUserSize;
	};

private:
	size_t	m_nUserSize;
	T		m_data[N];
};

};

#endif