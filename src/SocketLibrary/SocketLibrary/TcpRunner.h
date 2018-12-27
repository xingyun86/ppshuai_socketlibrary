/*
* Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com)
* All rights reserved.
* 
* 文件名称：TcpRunner.h
* 摘    要：TcpRunner
* 作    者：张保元 bolidezhang@gmail.com
* 完成日期：2005/07/16
*
*/

#pragma once
#include "SocketRunner.h"

namespace SL
{

class SOCKETLIBRARY_API CTcpRunner : public CSocketRunner
{
public:
	CTcpRunner(void);
	virtual ~CTcpRunner(void);

	BOOL Open(unsigned long nConcurrencyThreadNum = 0, unsigned long nWorkThreadNum = 4);
	BOOL Close();
private:
	static unsigned WINAPI Run(LPVOID lParam);

};

};
