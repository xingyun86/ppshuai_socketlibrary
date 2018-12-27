// TestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestClient.h"
#include "TestClientApp.h"

#include "../../socketlibrary/sync/threadmutex.h"
#include "../../socketlibrary/slext/fixedobjectpool.h"
#include "../../socketlibrary/slext/bytebuffer.h"
#include "../../socketlibrary/SLScopeMemoryPool.h"
#include "../../socketlibrary/SLNormalMemoryPool.h"
#include "../../socketlibrary/SLAlloc.h"
#include "../../socketlibrary/SLObjectpool.h"

#include <string>
#include <crtdbg.h>
#include <hash_map>

inline void EnableMemLeakCheck()
{
   _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

//#ifdef _DEBUG
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif

using namespace std;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	CTestClientApp oApp;
	oApp.RegisterService(argc, argv);
	return 0;
}
