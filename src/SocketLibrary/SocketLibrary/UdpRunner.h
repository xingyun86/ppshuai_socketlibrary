//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#pragma once
#include "SocketRunner.h"

namespace SL
{

class SOCKETLIBRARY_API CUdpRunner : public CSocketRunner
{
public:
	CUdpRunner(void);
	virtual ~CUdpRunner(void);

	BOOL Open(unsigned long nConcurrencyThreadNum = 0, unsigned long nWorkThreadNum = 4);
	BOOL Close();

private:
	static unsigned __stdcall Run(LPVOID lParam);

};

};
