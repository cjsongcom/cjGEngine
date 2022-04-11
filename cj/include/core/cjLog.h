
#ifndef _CJLOG_H_
#define _CJLOG_H_


#include "cjMutex.h"

//
// wide character => cjLOG(_CU("%ls"));
//

/*
	cjLOGS

	level   0=info
			1=err
			2=warn

	
	setcion 11, Level 1 : cjobj, prop
					  2 : proeprty warning





*/

#define CJLOG_INFO			0
#define CJLOG_ERR			1
#define CJLOG_WARN			2


#ifdef _CJ_ENABLE_LOGGING_

#if defined(CJ_PLATFORM_WIN32)
	#define cjLOGS cjLogger
    #define cjLOG cjLogger
	#define cjLOGE cjLogger
	#define cjLOGW cjLogger
    #define cjTrace cjLogger

#elif defined(CJ_PLATFORM_APPLE)
	#define cjLOGS cjLogger
    #define cjLOG cjLogger
	#define cjLOGE cjLogger
	#define cjLOGW cjLogger
	#define cjTrace cjLogger

#else
    #define cjLOG cjLogger
	#define cjLOGE cjLogger
	#define cjLOGW cjLogger
    #define cjTrace cjLogger
#endif


#else

	// not using log
#ifdef __GNUC__
        // GNUC support Variadic Macros
    #define cjLOG(...) (void) 0
	#define cjLOGE cjLogger
	#define cjLOGW (void) 0
	#define cjDBGSTR(...) (void) 0
    #define cjTrace(...) (void) 0

#else
	#define cjLOG __noop
	#define cjLOGE cjLogger
	#define cjLOGW __noop	
	#define cjDBGSTR __noop
	#define cjTrace __noop
#endif

#endif


class CJCORE_API cjLogger
{
public:
	enum EFLAG
	{	
		EFL_LOG_DATE					= 0x00000001,

		EFL_LOG_FILE					= 0x00000010,
		EFL_LOG_FUNC_LINE				= 0x00000020,
		
		EFL_LOG_LOCATION				= 0x000000f0,

		EFL_LOG_START_TRUNCATE			= 0x00000100,
	};

	explicit cjLogger(cjUINT32 section, cjUINT32 level, const cjChar* fmt, ...);
	explicit cjLogger(cjUINT32 section, cjUINT32 level, const cjUni* fmt, ...);

	explicit cjLogger(cjINT32 msgType, const cjChar* fmt, ...);
	explicit cjLogger(cjINT32 msgType, const cjUni* fmt, ...);

	explicit cjLogger(const cjChar* fmt, ...);
	explicit cjLogger(const cjUni* fmt, ...);

	explicit cjLogger(const char* file, const char* func, int line, const cjChar* fmt, ...);
	explicit cjLogger(const char* file, const char* func, int line, const cjUni* fmt, ...);
	
	static void SetLogFilePath(const char* path, const char* name);

	static void SetFlag(cjTFLAG flag) { ms_flag |= flag; }

	static void _StaticInit();
	static void _StaticShutdown();

	static void CloseLog();
	static void FlushLog();
	static void GetTimeStr(char* buf, cjINT32 siz=cjMAX_PATH);
	
	enum { MAX_LOG_LEN = 1024 };

protected:
	cjLogger() {}

	void FormatLog(const cjChar* fmt, va_list arg);
	void FormatLog(const cjUni* fmt, va_list arg);

	void DoLogA();
	void DoLogW();

	static cjTFLAG					ms_flag;
	static cjChar					ms_logPos[MAX_LOG_LEN];
	static cjUni					ms_logBuf[MAX_LOG_LEN];
	cjINT32							m_msgType;

	static cjChar					ms_logFilePath[cjMAX_PATH];
	static FILE*					ms_logFP;

	static cjMutex					ms_lock;
};

// serialize log
#define cjSLOG(fmt, ...)  cjLogger(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);
#define cjMLOG(fmt, ...)  cjLogger(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);

#endif
