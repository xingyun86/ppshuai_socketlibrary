//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "sltcpsession.h"
#include "socketsource.h"
namespace SL
{

inline CSLTcpSession::CSLTcpSession(void)
	: m_szBuffer(NULL)
	, m_nMsgSizeByte(4)
	, m_nBufferSize(1024)
	, m_nNeedMsgSize(0)
	, m_nLastMsgLeft(0)
	, m_pSocketSource(NULL)
	, m_hSocket(INVALID_SOCKET) 
{
};

inline CSLTcpSession::~CSLTcpSession(void)
{
	Clear();
};

inline bool CSLTcpSession::Init(PSOCKET_SESSION pSocketSession, SOCKET hSocket, uint32 nBufferSize, uint8 nMsgSizeByte)
{
	Reset();

	m_pSocketSource = pSocketSession->PerHandle.pSocketSource;
	pSocketSession->PerHandle.pAttachInfo = this;

	m_hSocket		 = hSocket;
	m_nBufferSize	 = nBufferSize;
	m_nMsgSizeByte	 = nMsgSizeByte;
	return true;
};

inline bool CSLTcpSession::Uninit()
{
	return true;
};

inline bool CSLTcpSession::Reset()
{
	m_nNeedMsgSize	= 0;
	m_nLastMsgLeft  = 0;
	return true;
};

int CSLTcpSession::ProcessMessage(const char *szMsg, int nLen)
{
	return 0;
};

inline int CSLTcpSession::Send(const char *szData, int nLen)
{
	return m_pSocketSource->Send(m_hSocket, szData, nLen);
};

inline int CSLTcpSession::OnRecv(const char *szData, int nLen)
{
	SplitPacket(szData,nLen);
	return 0;
};

// 拆包函数
int CSLTcpSession::SplitPacket(const char *szData, int nLen)
{
	if (nLen <= 0)
		return 0;

	char *p = (char*)szData;
	if (m_nNeedMsgSize > 0)
	{
		if (m_nNeedMsgSize >= nLen)
		{
			ProcessMessage(p, nLen);
			m_nNeedMsgSize -= nLen;
		}
		else
		{
			ProcessMessage(p, m_nNeedMsgSize);
			m_nNeedMsgSize = 0;
			SplitPacket(p+m_nNeedMsgSize, nLen-m_nNeedMsgSize);
		}
		return 0;
	}

	int nMsgSize = 0;
	int nOffset  = 0;
	if (m_nLastMsgLeft == 0)
	{
		nMsgSize = GetMsgSize(p, nLen);
		if (nMsgSize == 0)
		{
			MakeBuffer();
			memcpy(m_szBuffer, p, nLen);
			m_nLastMsgLeft = nLen;
			return 0;
		}
		if (nMsgSize == nLen)
		{
			ProcessMessage(p, nMsgSize);
			return 0;
		}
		else if (nMsgSize < nLen)
		{
			ProcessMessage(p, nMsgSize);
			p += nMsgSize;
			SplitPacket(p, nLen-nMsgSize);
		}
		else
		{
			if (nMsgSize > m_nBufferSize)
			{
				ProcessMessage(p, nLen);
				m_nNeedMsgSize = nMsgSize - nLen;
				return 0;
			}
			else
			{
				MakeBuffer();
				memcpy(m_szBuffer, p, nLen);
				m_nLastMsgLeft = nLen;
			}
		}
	}
	else
	{
		if (m_nLastMsgLeft < m_nMsgSizeByte)
		{
			if ( (m_nLastMsgLeft+nLen) < m_nMsgSizeByte )
			{
				memcpy(m_szBuffer+m_nLastMsgLeft, p, nLen);
				m_nLastMsgLeft += nLen;
				return 0;
			}
			else
			{
				nOffset = m_nMsgSizeByte-m_nLastMsgLeft;
				memcpy(m_szBuffer+m_nLastMsgLeft, p, nOffset);
				m_nLastMsgLeft += nOffset;
				p += nOffset;
			}
		}
		nMsgSize = GetMsgSize(m_szBuffer, m_nMsgSizeByte);
		if (nMsgSize == 0)
		{
			MakeBuffer();
			memcpy(m_szBuffer+m_nLastMsgLeft, p, nLen);
			m_nLastMsgLeft += nLen;
			return 0;
		}
		if (nMsgSize <= m_nBufferSize)
		{
			if ( nMsgSize == (m_nLastMsgLeft+nLen) )
			{
				MakeBuffer();
				memcpy(m_szBuffer+m_nLastMsgLeft, p, nLen);
				m_nLastMsgLeft = 0;
				ProcessMessage(m_szBuffer, nMsgSize);
			}
			else if ( nMsgSize > (m_nLastMsgLeft+nLen) )
			{
				MakeBuffer();
				memcpy(m_szBuffer+m_nLastMsgLeft, p, nLen);
				m_nLastMsgLeft += nLen;
			}
			else
			{
				MakeBuffer();
				nOffset = nMsgSize - m_nLastMsgLeft;
				memcpy(m_szBuffer+m_nLastMsgLeft, p, nOffset);
				m_nLastMsgLeft = 0;
				ProcessMessage(m_szBuffer, nMsgSize);
				p = p + nOffset;
				SplitPacket(p, nLen-nOffset);
			}
		}
		else
		{
			ProcessMessage(m_szBuffer, m_nLastMsgLeft);
			ProcessMessage(p, nLen);
			m_nLastMsgLeft = 0;
			m_nNeedMsgSize = nMsgSize-m_nLastMsgLeft-nLen;
		}
	}
	return 0;
};

// 分配Buffer
inline bool CSLTcpSession::MakeBuffer()
{
	if (m_szBuffer == NULL)
		m_szBuffer = (char*)::malloc(m_nBufferSize);
	if (m_szBuffer != NULL)
		return true;
	return false;
};

inline bool CSLTcpSession::Clear()
{
	if (m_szBuffer != NULL)
	{
		::free(m_szBuffer);
		m_szBuffer = NULL;
	};
	return true;
};


int CSLTcpSession::OnEvent(int nEventID)
{
	return 0;
}

int CSLTcpSession::GetEventID()
{
	return 0;
}

//取得消息大小
//没有考虑跨平台的字节顺序
//一般字节顺序:BIG Endian，LITTLE Endian
inline uint32 CSLTcpSession::GetMsgSize(const char *szData, int nLen)
{
	if (nLen < m_nMsgSizeByte)
		return 0;

	uint32 nSize = 0;
	switch (m_nMsgSizeByte)
	{
	case 1:
		nSize = (uint8)*szData;
		break;
	case 2:
		{
			uint16 *p = (uint16*)szData;
			nSize = (*p);
		}
		break;
	case 4:
		{
			uint32 *p = (uint32*)szData;
			nSize = (*p);
		}
		break;
	default:
		break;
	};
	return nSize;
};

}