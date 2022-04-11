
#include "cjCorePCH.h"
#include "cjLog.h"

#include <stdio.h>

cjMutex cjLogger::ms_lock;

cjTFLAG cjLogger::ms_flag = cjLogger::EFL_LOG_DATE | cjLogger::EFL_LOG_FILE | cjLogger::EFL_LOG_FUNC_LINE;
cjChar cjLogger::ms_logPos[MAX_LOG_LEN];
cjUni cjLogger::ms_logBuf[MAX_LOG_LEN] = {0};
cjChar cjLogger::ms_logFilePath[cjMAX_PATH];
FILE* cjLogger::ms_logFP;


cjLogger::cjLogger(cjUINT32 section, cjUINT32 level, const cjChar* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	DoLogA();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(cjUINT32 section, cjUINT32 level, const cjUni* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	DoLogA();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(cjINT32 /*msgType*/, const cjChar* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	DoLogA();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(const cjChar* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;
	va_start(arg, fmt);
	FormatLog((const cjChar*)fmt, arg);
	va_end(arg);

	DoLogA();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(cjINT32 /*msgType*/, const cjUni* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	DoLogW();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(const cjUni* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	DoLogW();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(const char* file, const char* func, int line, const cjChar* fmt, ...)
{
	ms_lock.DoLock();
	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	if(ms_flag & EFL_LOG_LOCATION)
	{
		if( (ms_flag & EFL_LOG_LOCATION) == EFL_LOG_FILE )
			cjSprintf(ms_logPos, MAX_LOG_LEN, "%s(%d):", file, line);
		else if( (ms_flag & EFL_LOG_LOCATION) == (EFL_LOG_FILE | EFL_LOG_FUNC_LINE))
			cjSprintf(ms_logPos, MAX_LOG_LEN, "%s(%d):%s ", file, line, func);
		else
			ms_logPos[0] = 0;
	}

	DoLogA();

	ms_lock.DoUnLock();
}

cjLogger::cjLogger(const char* file, const char* func, int line, const cjUni* fmt, ...)
{
	ms_lock.DoLock();

	va_list arg;    
	va_start(arg, fmt);
	FormatLog(fmt, arg);
	va_end(arg);

	if(ms_flag & EFL_LOG_LOCATION)
	{
		if( (ms_flag & EFL_LOG_LOCATION) == EFL_LOG_FILE )
			cjSprintf(ms_logPos, MAX_LOG_LEN, "%s(%d):", file, line);
		else if( (ms_flag & EFL_LOG_LOCATION) == (EFL_LOG_FILE | EFL_LOG_FUNC_LINE))
			cjSprintf(ms_logPos, MAX_LOG_LEN, "%s(%d):%s ", file, line, func);
		else
			ms_logPos[0] = 0;
	}

	DoLogW();

	ms_lock.DoUnLock();
}

void cjLogger::SetLogFilePath(const char* path, const char* name)
{
	cjSprintf(ms_logFilePath, cjMAX_PATH, "%s/%s", path, name);
}

void cjLogger::_StaticInit()
{
	if(cjStrlen(ms_logFilePath) != 0)
	{
		if(ms_flag & EFL_LOG_START_TRUNCATE)
		{
			ms_logFP = fopen(ms_logFilePath, "w");

			if(ms_logFP)
				fclose(ms_logFP);

			ms_logFP = cjNULL;
		}

		ms_logFP = fopen(ms_logFilePath, "a+");
	}
}

void cjLogger::_StaticShutdown()
{
	CloseLog();
}

void cjLogger::CloseLog()
{
	if(ms_logFP)
		fclose(ms_logFP);

	ms_logFP = 0;
}

void cjLogger::GetTimeStr(char* buf, cjINT32 siz)
{
#ifdef CJ_PLATFORM_WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(buf, siz, "%02d-%02d:%02d:%02d:%02d:%03d", st.wMonth, st.wDay, st.wHour, st.wMinute, 
		st.wSecond, st.wMilliseconds);
#else
// cjtemp
//	#error "not impl"
	sprintf(buf, "not impl");
#endif
}

void cjLogger::FormatLog(const cjChar* fmt, va_list arg)
{
	cjvsnprintf((cjChar*)ms_logBuf, MAX_LOG_LEN, fmt, arg);
}

void cjLogger::FormatLog(const cjUni* fmt, va_list arg)
{
	cjvsnwprintf(ms_logBuf, MAX_LOG_LEN, fmt, arg);
}

void cjLogger::DoLogA()
{
#ifdef CJ_PLATFORM_WIN32
	OutputDebugStringA((cjChar*)ms_logBuf);
	OutputDebugStringA("\n");
#else
    __cjLog_Impl_Apple__((cjChar*)ms_logBuf);
#endif

	if(ms_logFP)
	{
		char timeStr[cjMAX_PATH];
		GetTimeStr(timeStr);

		if(ms_flag & EFL_LOG_DATE)
			fprintf(ms_logFP, "%s ", timeStr);
		
		if(ms_flag & EFL_LOG_LOCATION)
			fprintf(ms_logFP, "%s ", ms_logPos);
		
		fprintf(ms_logFP, "%s \n", (cjChar*)ms_logBuf);	
		fflush(ms_logFP);
	}
}

void cjLogger::DoLogW()
{
#ifdef CJ_PLATFORM_WIN32
	OutputDebugStringW(ms_logBuf);
	OutputDebugStringW(L"\n");
#else
    __cjLog_Impl_Apple__((wchar_t*)ms_logBuf);
#endif

	if(ms_logFP)
	{
		char timeStr[cjMAX_PATH];
		GetTimeStr(timeStr);

		cjUni2Char u2c((cjUni*)ms_logBuf);

		if(ms_flag & EFL_LOG_DATE)
			fprintf(ms_logFP, "%s ", timeStr);
		
		if(ms_flag & EFL_LOG_LOCATION)
			fprintf(ms_logFP, "%s ", ms_logPos);
		
		fprintf(ms_logFP, "%s \n", (cjChar*)(*u2c));
		fflush(ms_logFP);
	}
}


//
//#include <stdio.h>
//#include <stdarg.h>
//
//
//void PrintFError ( const char * format, ... )
//{
//    char buffer[256];
//    va_list args;
//    va_start (args, format);
//    vsnprintf (buffer,256,format, args);
//    perror (buffer);
//    va_end (args);
//}
//

/*
std::string str = "normal String";
std::wstring strW = L"wideStr";
widePrint(L"str: %S wStr: %ls",str.c_str(), strW.c_str()); //Windows - VS2008
widePrint(L"str: %s wStr: %ls",str.c_str(), strW.c_str()); //Mac - XCode 3.2.6
*/
