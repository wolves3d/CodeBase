#include "CodeBase/CodeBase.h"
#include "LogFacility.h"


const char * Va(const char * szFmt, ...)
{
	#define MAX_STR_BUFFER 4096
	static char szBuffer[MAX_STR_BUFFER];

	va_list vArgs;
	va_start(vArgs, szFmt);
	vsprintf(szBuffer, szFmt, vArgs);
	va_end(vArgs);

	return szBuffer;
}


CLog *CLog::m_instance = NULL;


CLog::CLog()
{
	m_logFile = NULL;
	SetParams();
}


CLog * CLog::Instance()
{
	if (NULL == m_instance)
	{
		m_instance = new CLog();
	}

	return m_instance;
}


void CLog::SetParams(ELogLevel logLevel, bool haltOnError, ILogDelegates * loggerDelegates)
{
	m_logLevel = logLevel;
	m_haltOnError = haltOnError;
	m_delegates = loggerDelegates;
}


void CLog::LogFile(const char *szFileName)
{
	if (NULL != m_logFile)
	{
		LOG_ERROR("Log file already open!");
	}

	m_logFile = fopen(szFileName, "wt");
}


void CLog::Log(ELogLevel logLevel, const char *logTag, const char * szMessage, const char *szFile, size_t line)
{	
	const std::string messageCopy = szMessage; // TimeLog will overwrite Va buffer, so we need a copy

	if (m_logLevel <= logLevel)
	{
		if (NULL != m_logFile)
		{
			fprintf(m_logFile, "%s ", CTextTime::NowLog().c_str());

			switch (logLevel)
			{
				case LOG_LEVEL_VERBOSE: fprintf(m_logFile, "VERBOSE "); break;
				case LOG_LEVEL_INFO: fprintf(m_logFile, "INFO "); break;
				case LOG_LEVEL_ERROR: fprintf(m_logFile, "ERROR "); break;
			}

			fprintf(m_logFile, "%s %s\n", logTag, messageCopy.c_str());
			fflush(m_logFile);
		}

		switch (logLevel)
		{
			case LOG_LEVEL_VERBOSE: printf("VERBOSE "); break;
			case LOG_LEVEL_INFO: printf("INFO "); break;
			case LOG_LEVEL_ERROR: printf("ERROR "); break;
		}

		printf("%s %s\n", logTag, messageCopy.c_str());
	}

	if (LOG_LEVEL_ERROR == logLevel)
	{
		if (m_haltOnError)
		{
			AssertMsg(szFile, line, messageCopy.c_str());
			//__debugbreak(); //__asm { int 3 }
		}
	}

	if (NULL != m_delegates)
	{
		m_delegates->OnMessage(logLevel, messageCopy.c_str());
	}
}