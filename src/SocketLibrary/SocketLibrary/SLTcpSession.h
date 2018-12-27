//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlibrary is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SLTCPSESSION_H
#define SLTCPSESSION_H
#pragma once
#include "socketlib.h"

namespace SL
{

/*	CTcpSession
	功能：组包/封包的处理
	消息的规格:消息开始处1、2、4个字节表示消息大小
		message = messagehead + messagebody
		messagehead = size + command + option
	注意:
		缓冲区大小一定要大于等于最大完整消息包长度
*/
class CSocketSource;
class SOCKETLIBRARY_API CSLTcpSession
{
public:
	CSLTcpSession(void);
	virtual ~CSLTcpSession(void);

	virtual bool Init(PSOCKET_SESSION pSocketSession, SOCKET hSocket, uint32 nBufferSize = 1024, uint8 nMsgSizeByte = 4);
	virtual bool Uninit();
	virtual bool Reset();
	virtual bool Clear();

	// 处理消息
	virtual int ProcessMessage(const char *szMsg, int nLen);
	// 发送数据
	virtual int Send(const char *szData, int nLen);
	// 接收数据
	virtual int OnRecv(const char *szData, int nLen);

	//事件发生函数：如长时间没收到KeepAlive消息
	virtual int OnEvent(int nEventID);

	//取得事件ID
	//<0:表示关闭, 0:没有发生事件, >0：正常事件
	virtual int GetEventID();


private:
	// 取得消息大小
	uint32 GetMsgSize(const char *szData, int nLen);
	// 分配Buffer
	bool MakeBuffer();
	// 拆包
	int SplitPacket(const char *szData, int nLen);

protected:
	CSocketSource	*m_pSocketSource;
	SOCKET			 m_hSocket;

	//当收到消息中Size>m_nBufferSize时有效，由上层负责拆包
	//表示还需要收到消息长度
	uint32  m_nNeedMsgSize;
	uint32  m_nLastMsgLeft;			//最后收到消息的长度
	uint8   m_nMsgSizeByte;			//消息格式中Size占用的字节数(1,2,4,8)

	uint32  m_nBufferSize;			//缓冲区大小
	char   *m_szBuffer;

};

};

#endif