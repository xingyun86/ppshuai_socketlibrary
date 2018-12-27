#pragma once
#include "../config.h"
#include "bytebuffer.h"
#include <malloc.h>
#include <memory.h>

namespace SLEXT
{

CByteBuffer::CByteBuffer()
	: m_szData(0)
	, m_nSize(0)
	, m_bdeepCopy(false)
{
};

CByteBuffer::CByteBuffer(const char *szData, size_t nSize, bool bdeepCopy)
{	
	m_nSize = nSize;
	m_bdeepCopy = bdeepCopy;
	if (bdeepCopy)
	{
		m_szData = (char*)malloc(nSize);
		memcpy(m_szData,szData,nSize);
	}
	else
		m_szData = (char*)szData;
};

CByteBuffer::~CByteBuffer()
{
	clear();
};

char* CByteBuffer::data()
{
	return m_szData;
};

const char* CByteBuffer::data() const
{
	return m_szData;
};

std::size_t CByteBuffer::size() const
{
	return m_nSize;
};

inline void CByteBuffer::clear()
{
	if ( (m_bdeepCopy) && (m_szData != 0) )
	{
		::free(m_szData);
		m_szData = 0;
		m_nSize = 0;
		m_bdeepCopy = false;
	};
};

void CByteBuffer::data(const char *szData, size_t nSize, bool bdeepCopy)
{
	if (szData == 0)
		return;
	if (szData == m_szData)
		return;

	clear();
	m_nSize = nSize;
	m_bdeepCopy = bdeepCopy;
	if (bdeepCopy)
	{
		m_szData = (char*)malloc(nSize);
		memcpy(m_szData,szData,nSize);
	}
	else
		m_szData = (char*)szData;
};

inline bool CByteBuffer::empty() const 
{
	return m_nSize == 0;
};

};