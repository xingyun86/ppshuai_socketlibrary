//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of Socketlib is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#include "AppLog.h"
#include <time.h>
namespace SL
{

CAppLog::CAppLog(void)
	:m_nLogLevel(LOG_LEVEL_0),
	 m_nEverydayLogFileNum(1),
	 m_pFile(NULL)
{
	m_strLogPath.clear();
	m_strLogFileNamePrefix.clear();
	m_strLogLastFileName.clear();
}

CAppLog::~CAppLog(void)
{
	m_strLogPath.clear();
	m_strLogFileNamePrefix.clear();
	m_strLogLastFileName.clear();
	CloseLog();
}

bool CAppLog::CloseLog()
{
	if (m_pFile != NULL)
	{
		if (!fclose(m_pFile))
			return false;
		m_pFile = NULL;
	};
	return true;
};

bool CAppLog::OpenLog(LPCSTR   pszLogPath, 
					  LPCSTR   pszLogFileNamePrefix, 
					  LOG_LEVEL nLogLevel,
					  USHORT    nEverydayLogFileNum)
{
	m_strLogPath = pszLogPath;
	m_strLogFileNamePrefix = pszLogFileNamePrefix;
	m_nLogLevel = nLogLevel;
	if (nEverydayLogFileNum<=0)
		m_nEverydayLogFileNum = 1;
	else if (nEverydayLogFileNum>24)
		m_nEverydayLogFileNum = 2;
	else
		m_nEverydayLogFileNum = nEverydayLogFileNum;
	CloseLog();

	return true;
};

bool CAppLog::BuildFile()
{
	// 取当前日期
	char szDate[20]={0};
	time_t	tt;
	time(&tt);
	struct tm *time1= localtime(&tt);
	strftime(szDate, 20, "%Y-%m-%d", time1);

	//计算log文件名编号
	USHORT nLogFileID = time1->tm_hour * m_nEverydayLogFileNum/24  + 1;
	char   szLogID[3] = {0};
	sprintf(szLogID,"%d",nLogFileID);

	std::string strTemp;
	if (m_pFile == NULL)
	{
		m_strLogLastFileName = m_strLogFileNamePrefix + szDate + "_" + szLogID + ".log";
		strTemp = m_strLogPath + m_strLogLastFileName;
		m_pFile = fopen(strTemp.c_str(),"a+");
	}
	else
	{
		strTemp = m_strLogFileNamePrefix + szDate + "_" + szLogID + ".log";
		if (m_strLogLastFileName != strTemp)
		{//产生新的Log文件
			CloseLog();
			m_strLogLastFileName = strTemp;
			strTemp = m_strLogPath + m_strLogLastFileName;
			m_pFile = fopen(strTemp.c_str(),"a+");
		}
	}

	if (m_pFile == NULL)
		return false;
	return true;
}

//记录Log
bool CAppLog::WriteLog(LPCSTR	  pszLog, 
					   LPCSTR    pszFileName, 
					   int        nLine, 
					   LOG_LEVEL  nLevel,
					   bool 	  bLevelEqual)
{
	if (m_nLogLevel == LOG_LEVEL_0)
		return false;
	if (bLevelEqual)
	{
		if (m_nLogLevel != nLevel)
			return false;
	}
	else
	{
		if (m_nLogLevel > nLevel)
			return false;
	}

	if ( !BuildFile() )
		return false;

	char szTime[20]={0};
	time_t	tt;
	time(&tt);
	struct tm *time1= localtime(&tt);
	strftime(szTime, 20, "%Y-%m-%d %H:%M:%S", time1);

	//记录Log
	fprintf(m_pFile,"%s; Thread ID:%ld; Source:%s,%ld \n", szTime, GetCurrentThreadId(), pszFileName, nLine);
	fprintf(m_pFile,"%s\n",pszLog);
	fflush(m_pFile);

	return true;
}

//记录Log
bool CAppLog::WriteLog(LPCSTR		pszLog, 
					   LOG_LEVEL	nLevel,
					   bool 		bLevelEqual)
{
	if (m_nLogLevel == LOG_LEVEL_0)
		return false;
	if (bLevelEqual)
	{
		if (m_nLogLevel != nLevel)
			return false;
	}
	else
	{
		if (m_nLogLevel > nLevel)
			return false;
	}
	if ( !BuildFile() )
		return false;

	char szTime[20]={0};
	time_t	tt;
	time(&tt);
	struct tm *time1= localtime(&tt);
	strftime(szTime, 20, "%Y-%m-%d %H:%M:%S", time1);

	//记录Log
	fprintf(m_pFile,"%s, %s\n", pszLog, szTime);
	fflush(m_pFile);

	return true;
}

}
