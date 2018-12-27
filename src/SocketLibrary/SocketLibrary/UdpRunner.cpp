//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include ".\udprunner.h"
#include "SocketLib.h"
#include "SocketSource.h"
#include "SocketEvent.h"
namespace SL
{

CUdpRunner::CUdpRunner(void)
{
}

CUdpRunner::~CUdpRunner(void)
{
	Close();
}

BOOL CUdpRunner::Close()
{
	if (NULL == m_hCompletionPort)
		return FALSE;

	//退出工作线程
	for (DWORD i=0; i<m_dwWorkThreadNum; i++)
	{
		//发出退出消息
		::PostQueuedCompletionStatus(m_hCompletionPort,-1,-1,NULL);
	}
	CloseHandle(m_hCompletionPort);
	m_hCompletionPort = NULL;
	m_threadGroup.Stop(WAIT_TIMEOUT);
	m_threadGroup.Kill();

	return TRUE;
}

BOOL CUdpRunner::Open(unsigned long nConcurrencyThreadNum, unsigned long nWorkThreadNum)
{
	Close();

	if (nWorkThreadNum > 0)
		m_dwWorkThreadNum = nWorkThreadNum;

	if ( nConcurrencyThreadNum <= nWorkThreadNum )
		m_dwConcurrencyThreadNum = nConcurrencyThreadNum;
	else
		m_dwConcurrencyThreadNum = 0;

	//创建完成端口句柄
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,m_dwConcurrencyThreadNum);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
#ifdef SHOW_PRINTFINFO
		printf("CUdpRunner::Open Create iocompletionPort failure!\r\n");
#endif
		return FALSE;
	}

    //启动Socket工作线程组
	m_threadGroup.Start(CUdpRunner::Run, this, m_dwWorkThreadNum);

	return TRUE;
}

unsigned __stdcall CUdpRunner::Run(LPVOID lParam)
{
	CUdpRunner *pRun		= (CUdpRunner*)lParam;
	HANDLE hCompletionPort	= pRun->m_hCompletionPort;

	int		nRet;
	DWORD	nByteTransferred;
	BOOL	bSuccess;
	PPER_HANDLE_DATA			pPerHandle;
	PPER_IO_OPERATION_DATA		pPerIoData;

	while (1)
	{
		pPerHandle = NULL;
		pPerIoData = NULL;
		nByteTransferred = -1;
		bSuccess = ::GetQueuedCompletionStatus(hCompletionPort,
											&nByteTransferred,
											(LPDWORD)&pPerHandle,
											(LPOVERLAPPED*)&pPerIoData,
											INFINITE);
		//退出信号到达，退出线程
		if (-1 == nByteTransferred)
		{
			return 1;
		}		
		if ( (NULL==pPerIoData) || (NULL==pPerHandle) || (0==nByteTransferred) )
			continue;

		if (pPerIoData->OperType == RECV_POSTED)
		{//接收数据

			//消息处理
			PPER_IO_OPERATION_UDP_RECV pPerIoRecv = (PPER_IO_OPERATION_UDP_RECV)pPerIoData;
			nRet = pPerHandle->pSocketSource->m_pSocketEvent->OnRecv(pPerHandle->pSocketSource, pPerIoRecv->szBuffer, nByteTransferred, 
					(sockaddr*)(&pPerIoRecv->addr), pPerIoRecv->addrlen);
			pPerHandle->pSocketSource->PostRecv(pPerIoRecv);

		}
		else if (pPerIoData->OperType == SEND_POSTED)
		{//数据发送完成
			pPerHandle->pSocketSource->PostSend(pPerIoData);
			continue;
		}
	}

	ExitThread(0);
	return 0;
}

}