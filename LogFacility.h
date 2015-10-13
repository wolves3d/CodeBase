#ifndef __LogFacility_h_included__
#define __LogFacility_h_included__

#include <stdio.h>
#include <stdarg.h>


// -----------------------------------------------------------------------------


const char * Va(const char * szFmt, ...);

#include <time.h>
#include <string>

class CTextTime
{
public:

	static std::string Now()
	{
		time_t t; tm *tk;
		time(&t);
		tk = localtime(&t);

		std::string result = Va("%04d_%02d_%02d_[%02d_%02d]", 1900 + tk->tm_year, 1 + tk->tm_mon, tk->tm_mday, tk->tm_hour, tk->tm_min);
		return result;
	}

	static std::string NowLog()
	{
		time_t t; tm *tk;
		time(&t);
		tk = localtime(&t);

		std::string result = Va("%04d_%02d_%02d_%02d:%02d:%02d",
			1900 + tk->tm_year, 1 + tk->tm_mon, tk->tm_mday, tk->tm_hour, tk->tm_min, tk->tm_sec);
		return result;
	}
};


// -----------------------------------------------------------------------------


enum ELogLevel
{
	LOG_LEVEL_VERBOSE = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_ERROR,
};


struct ILogDelegates
{
	virtual void OnMessage(ELogLevel logLevel, const char *messageString) = 0;
};


class CLog
{
public:

	static CLog * Instance();
	CLog();

	void LogFile(const char *szFileName);
	void SetParams(ELogLevel logLevel = LOG_LEVEL_INFO, bool haltOnError = false, ILogDelegates * loggerDelegates = NULL);

	// FIXME: multithread unsafe
	void Log(ELogLevel logLevel, const char *logTag, const char * szMessage, const char *szFile, size_t line);


private:
	static CLog *m_instance;
	ILogDelegates *m_delegates;
	FILE *m_logFile;

	bool m_haltOnError;
	ELogLevel m_logLevel;
};


#define LOG_INFO(__formatString, ...) CLog::Instance()->Log(LOG_LEVEL_INFO, "", Va((__formatString), ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_INFO_TAG(__tag, __formatString, ...) CLog::Instance()->Log(LOG_LEVEL_INFO, (__tag), Va((__formatString), ##__VA_ARGS__), __FILE__, __LINE__)

#define LOG_VERBOSE(__formatString, ...) CLog::Instance()->Log(LOG_LEVEL_VERBOSE, "", Va((__formatString), ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_VERBOSE_TAG(__tag, __formatString, ...) CLog::Instance()->Log(LOG_LEVEL_VERBOSE, (__tag), Va((__formatString), ##__VA_ARGS__), __FILE__, __LINE__)

#define LOG_ERROR(__formatString, ...) CLog::Instance()->Log(LOG_LEVEL_ERROR, "", Va((__formatString), ##__VA_ARGS__), __FILE__, __LINE__)
#define LOG_ERROR_TAG(__tag, __formatString, ...) CLog::Instance()->Log(LOG_LEVEL_ERROR, (__tag), Va((__formatString), ##__VA_ARGS__), __FILE__, __LINE__)

#define DEBUG_ASSERT(X) do { if (false == (X)) LOG_ERROR("Assertion failed! (%s) %s:%d", #X, __FILE__, __LINE__); } while(false)

#endif // #ifndef __LogFacility_h_included__