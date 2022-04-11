
#ifndef _CJDECLARELIB_H_
#define _CJDECLARELIB_H_

#define cjDeclareLib(libname, apiexport) \
	extern "C" apiexport const cjNameChar* libname##_PkgName; \
	class apiexport cjDeclareLib_##libname { \
		public: cjDeclareLib_##libname(); \
			static cjDeclareLib_##libname* Get(); \
			static int Init(); \
			static void MakeModulePkgLoad(void* ); \
			static void Shutdown(); \
			static int ms_init; };

#define cjImplementLib(libname, apiexport, dependency) \
	extern "C" apiexport cjDeclareLib_##libname* cjDeclareLib_##libname##_impl; \
	extern "C" apiexport const cjNameChar* libname##_PkgName = _CT(#libname); \
	apiexport cjDeclareLib_##libname* cjDeclareLib_##libname##_impl = cjDeclareLib_##libname::Get(); \
	static cjDeclareLib_##libname gs_cjDeclareLib_##libname##_inst; \
	int cjDeclareLib_##libname::ms_init = 0; \
	cjDeclareLib_##libname* cjDeclareLib_##libname::Get() \
	{ \
		cjObjAddModulePkgName(L## #libname, &MakeModulePkgLoad); \
	return &gs_cjDeclareLib_##libname##_inst; \
	} \
	cjDeclareLib_##libname::cjDeclareLib_##libname() { \
	static unsigned int called = 0; \
	if(!called) { \
	called = 1; \
	cjLibManager::AddLib(#libname, cjDeclareLib_##libname::Init, \
	cjDeclareLib_##libname::Shutdown, dependency); } }

#define cjImplementLib_Init(libname) int cjDeclareLib_##libname::Init()
#define cjImplementLib_Shutdown(libname) void cjDeclareLib_##libname::Shutdown()
#define cjImplementLib_MakeModulePkgLoad(libname) void cjDeclareLib_##libname::MakeModulePkgLoad(void* _pkgLoad)


#endif
