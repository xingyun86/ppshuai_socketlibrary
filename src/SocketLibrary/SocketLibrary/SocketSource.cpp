//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include ".\socketsource.h"
#include "SocketLib.h"
namespace SL
{

CSocketSource::CSocketSource(void)
	: m_pAppLog(NULL)
	, m_pSocketEvent(NULL)
	, m_pSocketRunner(NULL)
	, m_nRecvBufferSize(RECV_BUFFER_SIZE)
{
	
}

CSocketSource::~CSocketSource(void)
{
}

}