// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include <iostream>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// TODO: reference additional headers your program requires here
#include "../../socketlibrary/config.h"
#ifdef _DEBUG
	#pragma comment(lib,"Debug\\socketlib.lib")
#else
	#pragma comment(lib,"Release\\socketlib.lib")
#endif

