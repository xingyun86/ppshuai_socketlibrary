// TestServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestServer.h"
#include "TestServerApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	CTestServerApp oApp;
	oApp.RegisterService(argc, argv);
	return 0;
}
