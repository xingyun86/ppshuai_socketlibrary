#pragma once
#include "config.h"
#include "slblocktcpclient.h"

namespace SL
{

class SOCKETLIB_API CSLBlockTcpEvent
{
public:
	CSLBlockTcpEvent(void);
	virtual ~CSLBlockTcpEvent(void);

	virtual bool OnAccept(SOCKET hClientSocket) { return true; };
	virtual int  OnProcess(CSLBlockTcpClient &oBlockTcpClient) { return -1; };

};

};