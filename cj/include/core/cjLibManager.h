
#ifndef _CJLIBMANAGER_H_
#define _CJLIBMANAGER_H_

#define CJ_MAX_LIB					64

class CJCORE_API cjLibManager
{
	friend class cjObj;
public:
	static void Init();
	static void Shutdown();
	static void Tick();

	typedef int (*InitFn)(void);
	typedef void (*ShutdownFn)(void);

	static void AddLib(const char* libName, InitFn initFn, ShutdownFn shutdownFn, const char* dep);
	static void RemoveLib(const char* libName);

	static void ProcessAccumLib();

	static void AddStaticCls(void* /*ptr*/) {}

private:
	static cjBool ComputeProcessOrder();

	static void FinalizeLib();

	struct LibInfo
	{
		char                            m_name[CJ_MAXPATH];
		char                            m_dep[CJ_MAXPATH] ; // library depends string which is white space seprated library list

		InitFn                          m_initFn;
		ShutdownFn						m_shutdownFn;

		unsigned int					m_index;
	};
	
	static LibInfo						ms_lib[CJ_MAX_LIB];
	static unsigned int					ms_numLib;

	static unsigned int					ms_numInitializedLib;	
	static unsigned int					ms_procOrder[CJ_MAX_LIB];
	
	static cjBool						ms_init;

private:
	static void* Malloc(size_t siz, const  wchar_t* tag);
	static void* Malloc(size_t siz, const  char* tag);
	static void Free(void* mem);
};

#endif
