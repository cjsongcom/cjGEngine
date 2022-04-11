
#ifndef _CJTOOLCLASSDESC_H_
#define _CJTOOLCLASSDESC_H_


#ifdef CJ_PLATFORM_WIN32

typedef int (*cjToolLibInitPFN)(void);
typedef int (*cjToolLibShutdownPFN)(void);
typedef const char* (*cjToolLibGetDescPFN)(void);
typedef char* (*cjToolLibGetClassDescNamePFN)(int );
typedef int* (*cjToolLibGetClassDescCountPFN)();
typedef void* (*cjToolLibCreateClassDescPFN)(int );


class cjToolClassDesc : public cjMem
{
public:
	cjToolClassDesc() {}
	virtual ~cjToolClassDesc() {}

	virtual cjBool Init() = 0;
	virtual	void Shutdown() = 0;

	virtual cjBool Tick() = 0;

	virtual int DoCmd(const char* cmd, void* arg) { return 0; }


public:
	// Utility
	static cjBool ResolveFuncFromDLL(HMODULE hModule, 
		cjToolLibInitPFN				*initPFN,
		cjToolLibShutdownPFN			*shutdownPFN,
		cjToolLibCreateClassDescPFN		*createClassDescPFN,		
		cjToolLibGetClassDescCountPFN	*getClassDescCountPFN = 0,
		cjToolLibGetClassDescNamePFN	*getClassDescNamePFN = 0,
		cjToolLibGetDescPFN				*getDescPFN = 0
		)		
	{
		if(!hModule)
			return cjFalse;

		cjBool er = cjFalse;
	
		if(initPFN) {
			*initPFN = (cjToolLibInitPFN)GetProcAddress(hModule, MAKEINTRESOURCE(1));
			if(*initPFN == 0) return cjFalse;
		}

		if(shutdownPFN) {
			*shutdownPFN = (cjToolLibShutdownPFN)GetProcAddress(hModule, MAKEINTRESOURCE(2));
			if(*shutdownPFN == 0) return cjFalse;
		}

		if(createClassDescPFN) {
			*createClassDescPFN = (cjToolLibCreateClassDescPFN)GetProcAddress(hModule, MAKEINTRESOURCE(3));
			if(*createClassDescPFN == 0) return cjFalse;
		}

		if(getClassDescCountPFN) {
			*getClassDescCountPFN = (cjToolLibGetClassDescCountPFN)GetProcAddress(hModule, MAKEINTRESOURCE(4));
			if(*getClassDescCountPFN == 0) return cjFalse;
		}

		if(getClassDescNamePFN) {
			*getClassDescNamePFN = (cjToolLibGetClassDescNamePFN)GetProcAddress(hModule, MAKEINTRESOURCE(5));
			if(*getClassDescNamePFN == 0) return cjFalse;
		}

		if(getDescPFN) {
			*getDescPFN = (cjToolLibGetDescPFN)GetProcAddress(hModule, MAKEINTRESOURCE(6));
			if(*getDescPFN == 0) return cjFalse;
		}

		return cjTrue;
	}
};


#endif


#endif
