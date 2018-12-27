//**********************************************************************
//
// 
// All rights reserved.
//
// This copy of Socketlibrary is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SL_COMMON_DEF_H
#define SL_COMMON_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "slbasetype.h"

#pragma pack(push)
#pragma pack(4)

namespace SL
{

#define MSG_MAXPARAMS	 8		 //报文头中参数的最大个数
/********************************
*    定义数据结构
********************************/ 
//报文头的结构
typedef struct tag_SL_MSGHEAD
{
	uint32			nSize;						//总长度 = 协议头 ＋ 协议正文(不定长)
	uint32			nCommand;					//命令
	int32			nParams[MSG_MAXPARAMS];		//参数
}SL_MSGHEAD;
typedef struct tag_SL_MSG
{
	SL_MSGHEAD		msghead;
	char			msgcontent[1];
}SL_MSG;
typedef struct tag_SL_MSGHEAD_8
{
	uint32			nSize;						
	uint32			nCommand;					
}SL_MSGHEAD_8;
typedef struct tag_SL_MSGHEAD_12
{
	uint32			nSize;						
	uint32			nCommand;
	int32			nParams[1];
}SL_MSGHEAD_12;
typedef struct tag_SL_MSGHEAD_16
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[2];
}SL_MSGHEAD_16;
typedef struct tag_SL_MSGHEAD_20
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[3];
}SL_MSGHEAD_20;
typedef struct tag_SL_MSGHEAD_24
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[4];
}SL_MSGHEAD_24;
typedef struct tag_SL_MSGHEAD_28
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[5];
}SL_MSGHEAD_28;
typedef struct tag_SL_MSGHEAD_32
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[6];
}SL_MSGHEAD_32;
typedef struct tag_SL_MSGHEAD_36
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[7];
}SL_MSGHEAD_36;

//报文头结构的一种变体
typedef struct tag_SL_MSGHEAD_EX
{
	uint32			nSize;						
	uint32			nCommand;					
	int32			nParams[6];
	int64			nAttachID;
}SL_MSGHEAD_EX;
typedef struct tag_SL_MSG_EX
{
	SL_MSGHEAD_EX	msghead;
	char			msgcontent[1];
}SL_MSG_EX;

//报文头结构的小型变体
typedef struct tag_SL_MSGHEAD_SMALL
{
	uint16			nSize;						
	uint16			nCommand;
}SL_MSGHEAD_SMALL;
typedef struct tag_SL_MSGSMALL
{
	SL_MSGHEAD_SMALL msghead;
	char			 msgcontent[1];
}SL_MSG_SMALL;

enum SL_MSG_COMMAND_FLAG
{
	SL_MSG_COMMAND_FLAG_COMPRESS  = 1,  //压缩
	SL_MSG_COMMAND_FLAG_ENCRYPT   = 2   //加密
};

};

#pragma pack(pop)
#endif
