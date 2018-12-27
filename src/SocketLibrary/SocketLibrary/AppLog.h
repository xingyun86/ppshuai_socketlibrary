/*
* Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com)
* All rights reserved.
* 
* 文件名称：AppLog.h
* 摘    要：系统日志类
* 当前版本：1.0.0.1018
* 作    者：张保元 bolidezhang@gmail.com
* 完成日期：2005/10/18
*
*/

#ifndef APPLOG_H
#define APPLOG_H

#pragma once
#include <string>
#include "config.h"

namespace SL
{

class SOCKETLIBRARY_API CAppLog
{
public:

	//log级别
	enum LOG_LEVEL
	{
		LOG_LEVEL_0 = 0, //不记录log
		LOG_LEVEL_1,
		LOG_LEVEL_2,
		LOG_LEVEL_3,
		LOG_LEVEL_4,
		LOG_LEVEL_5,
		LOG_LEVEL_6,
		LOG_LEVEL_7,
		LOG_LEVEL_8,
		LOG_LEVEL_LAST  //最高级别
	};

	CAppLog(void);
	virtual ~CAppLog(void);

	//创建日志文件
	virtual bool BuildFile();
	//日志操作函数
	virtual bool OpenLog(LPCSTR pszLogPath, LPCSTR pszLogFileNamePrefix, 
		LOG_LEVEL nLogLevel = LOG_LEVEL_0, USHORT nEverydayLogFileNum = 1);
	virtual bool CloseLog();

    //记录Log(备注：以天为时间单位产生日志文件)
	virtual bool WriteLog(LPCSTR pszLog, LPCSTR pszFileName, int nLine, LOG_LEVEL nLevel, bool bLevelEqual = false);

	//记录Log(备注：以天为时间单位产生日志文件)
	virtual bool WriteLog(LPCSTR pszLog, LOG_LEVEL nLevel, bool bLevelEqual = false);

protected:
	FILE			*m_pFile;                 //文件指针
	std::string	    m_strLogPath;             //Log目录
	std::string     m_strLogFileNamePrefix;   //Log文件名(不包括文件扩展名和日期)
	std::string     m_strLogLastFileName;     //最后一次记录Log时的文件名
	LOG_LEVEL		m_nLogLevel;              //Log级别
	USHORT          m_nEverydayLogFileNum;    //每天产生Log文件数
};

};

#endif