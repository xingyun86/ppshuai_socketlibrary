#pragma once
#include "config.h"

namespace SL
{

class SOCKETLIBRARY_API CSLSession
{
public:
	CSLSession(void);
	virtual ~CSLSession(void);

	virtual int OnEvent(int nEventID);
	virtual int GetEventID();

};

};