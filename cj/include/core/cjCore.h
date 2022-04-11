/*
	cjCore.h
    
    chunjong.song 

	2013. 9. 30		chunjong.song		first created
    2014. 2. 1      chunjong.song       start porting for ios
*/


#ifndef _CJCORE_H_
#define _CJCORE_H_

#ifndef CJUNICODE
#define CJUNICODE
#endif

// primary classes
#include "cjPlatform.h"
#include "cjCoreLibType.h"
#include "cjDeclareLib.h"

typedef cjFLOAT cjEngineTime_ft;
typedef cjDOUBLE cjEngineTime_dt;

#define CJ_FORCE_INLINE	inline

// slow macros
// cjSlow, cjUniSlow

typedef cjEngineTime_dt cjEngineTime_t;
#define cjET(x)   cjEngineTime_dt(x)

#define cjSlow
#define cjVerySlow

#define cjNULL			0

#ifdef CJUNICODE

typedef wchar_t						cjTChar;

#if defined(CJ_PLATFORM_WIN32)
    #define __CTXT(txt)					L##txt
    #define __CJSTR2WSTR(str)			L##str
#else
    #define __CTXT(txt)					L## txt
    #define __CJSTR2WSTR(str)			L## #str
#endif

#define _CJSTR2WSTR(str)			__CJSTR2WSTR(str)
#define __CJFILE__					_CJSTR2WSTR(__FILE__)
#define __CJFUNCTION__				_CJSTR2WSTR(__FUNCTION__)
#define __CJLINE__					__LINE__

#define CJ_ASCII_QUOTATION_MARK		L'\x22'     // "
#define CJ_ASCII_APOSTROPHE			L'\x27'     // `

#else

typedef char						cjTChar;

#define __CTXT(txt)					txt

#define __CJFILE__					__FILE__
#define __CJFUNCTION__				__FUNCTION__
#define __CJLINE__					__LINE__

#define CJ_ASCII_QUOTATION_MARK		'\x22'     // "
#define CJ_ASCII_APOSTROPHE			'\x27'     // `


#endif // end of #ifdef CJUNICODE

typedef unsigned int				cjEnumType;
typedef unsigned int				cjBITFIELD;


#define CJ_IS_LITTLE_ENDIAN			(1 == *(unsigned char *)&(const int){1})

#define CJ_NEXT_BITFIELD(b)			((b)<<1)
#define CJ_FIRST_BITFIELD			(1)

// big endian
//#define CJ_NEXT_BITFIELD(b)		((b)>>1)
//#define CJ_FIRST_BITFIELD			(0x80000000)


typedef cjINT32                     cjNameIndex;
typedef cjINT32						cjIndexType;
enum { cjIndex_None = (cjIndexType)-1 };

typedef cjUni						cjNameChar;
#define cjNameCharIsUnicode			1


#define _CM(x)						x

#if defined(CJ_PLATFORM_WIN32)
    #define _CT(x)						L##x
    #define _CU(x)						L##x
#else
    #define _CT(x)						L##x
    #define _CU(x)						L##x
#endif


#define cjARRAYCNT(ary)				(sizeof(ary) / sizeof((ary)[0]))
#define cjMAKEOFF64(hi, lo)			(((cjLONGLONG)hi << 32) | lo)

// align address
template<class T>
inline T cjAlignAddr(const T addr, cjINT32 alignSiz)
{
	return (T)( ((cjPTR_NUMERIC)addr + alignSiz-1) & ~(alignSiz-1) );
}

#define cjUNUSED_PARAM(x)
#define cjUNUSED_VALDEF(x)			{ (x) = (x);}

#define CJNULL_API 

#if defined(__GNUC__)
#define cjError(...)				(void)0
#else
#define cjError(x)					assert(0)
#endif

// ios (len=110) = /var/containers/Bundle/Application/C61C5B46-8420-4696-8B0C-32DEB0653EDE/HelloCJApp.app/cjp/mdl/TestResChar.cjp
#define cjMAX_PATH                  260

#define CJ_DEF_DIRTY_FUNCSET(x) \
private: \
	cjUINT32								m_##x##DirtyFlag; \
public: \
	void Set##x##Dirty(cjUINT32 flag) { m_##x##DirtyFlag = flag; } \
	void Clear##x##Dirty(); \
	void Remove##x##Dirty(cjUINT32 flag) { m_##x##DirtyFlag = m_##x##DirtyFlag & (~flag); }

//cjdefer - delete and change to CJFLAG_DECL_BEGIN
#define CJ_FLAGFUNC_DECLARE(type, name) \
	private: type m__##name##flag; \

#define CJ_FLAGFUNC_INIT(name, v) \
	m__##name##flag = v;

#define	CJ_FLAGFUNC_DECLARE_CATE(type, name, flagMask, flagStPos, cateName) public: \
	void Set##cateName##Val(type val) { m__##name##flag = (m__##name##flag & ~((type)flagMask << flagStPos)) | (val << flagStPos); } \
	type Get##cateName##Val() const { return (m__##name##flag & ((type)flagMask >> flagStPos)) >> flagStPos; } \
	void Set##cateName##Flag(type x) { m__##name##flag |= ((type)x & flagMask) << flagStPos; } \
	void Remove##cateName##Flag(type x) { type v=(((type)x & flagMask) << flagStPos); m__##name##flag = (m__##name##flag & ~v) | v; } \
	cjBool Is##cateName##Flag(type x) const { return ((m__##name##flag >> flagStPos) & ((type)x & flagMask)) != 0 ? cjTrue : cjFalse; }


#define CJFLAG_DECL_BEGIN \
public: \
	enum CJTFL_MASK \
	{ \

#define CJFLAG_DECL_END	\
	}; \
    void _SetFlag(CJTFL_MASK mask, cjWORD bpos, cjUINT32 val) \
		{ _cjtfl_flag = (_cjtfl_flag & ~mask) | ((cjUINT32)val << bpos); } \
    void _SetFlagValMask(CJTFL_MASK mask, cjWORD bpos, cjWORD valMask, cjUINT32 val) \
		{ _cjtfl_flag = (_cjtfl_flag & ~mask) | ((((cjUINT32)val & valMask)) << bpos); } \
	void _SetFlagAsBool(CJTFL_MASK mask, cjWORD bpos, cjBool val) { cjUINT32 _val = val ? 1 : 0; _cjtfl_flag = (_cjtfl_flag & ~mask) | (_val << bpos);} \
	cjUINT32 _GetFlag(CJTFL_MASK mask, cjWORD bpos) const { return (_cjtfl_flag & mask) >> bpos; } \
	cjBool _GetFlagAsBool(CJTFL_MASK mask, cjWORD bpos) const { return ((_cjtfl_flag & mask) >> bpos) ? cjTrue : cjFalse; } \
	cjTFLAG							_cjtfl_flag;

#define CJFLAG_IMPL_INIT	_cjtfl_flag = 0;
#define CJFLAG_GET			_cjtfl_flag
#define CJFLAG_SERIALIZE	ser << _cjtfl_flag;
#define CJFLAG_CLONE		d->_cjtfl_flag = _cjtfl_flag;

#define CJFLAG_EXTERNAL_GETMASKED_VAL(_flag, _mask)		(_flag & _mask)
#define CJFLAG_EXTERNAL_SETMASKED_VAL(_flag, _mask)		(_flag |= _mask)


#ifdef CJDLL
extern "C" cjDLL_EXPORT cjNameChar g_cjObjPkgModuleName[];
#endif

#include "cjMem.h"
#include "cjRTTI.h"
#include "cjNumeric.h"
#include "cjSTL.h"
#include "cjLog.h"

// cjObj stuff
	// forward definitions
class cjObjCls;
class cjObj;
class cjObjMem;                    
class cjName;
class cjObjPackage;

CJCORE_API cjUINT32 cjName_GenHash(const cjNameChar* name);

#include "cjTemplate.h"
#include "cjTypeInfo.h"
#include "cjTCollection.h"

#include "cjObjBase.h"

#include "cjStream.h"
#include "cjTAry.h"
#include "cjTList.h"

#include "cjString.h"
#include "cjTString.h"
#include "cjParse.h"

#include "cjTHash.h"

#include "cjName.h"

#include "cjSerialize.h"
#include "cjRef.h"
#include "cjRefObj.h"

#include "cjSerMath.h"


#include "cjObj.h"
#include "cjObjProp.h"
#include "cjObjCls.h"
#include "cjSerialize.h"
#include "cjCoreObj.h"
#include "cjObjPkgLinker.h"

#include "cjLibManager.h"

#include "cj3DMath.h"
#include "cjExtraDat.h"

#include "cjAniDef.h"
#include "cjAniKey.h"

#include "cjController.h"
#include "cjInterpolator.h"

#include "cjAniEvaluator.h"
#include "cjAniPose.h"

#include "cjAniSeq.h"
#include "cjResAni.h"

#include "cjFile.h"
#include "cjObjFactory.h"

#include "cjNode.h"
#include "cjSceneProp.h"
#include "cjSceneNode.h"

#include "cjAniSPad.h"
#include "cjAniCntrSeq.h"
#include "cjAniCntrMan.h"
#include "cjAniSPad.h"		// because declaring cjRefObjNameHash is need before start cjAniSPad implement


template<class T>
inline cjFStream& operator << (cjFStream& stream, cjRect<T>& d)
{
	stream << d.m_left;
	stream << d.m_right;
	stream << d.m_top;
	stream << d.m_bottom;

	return stream;
}



const int cjPkgPathCountMax = 10;

struct cjInitParam
{
#ifdef CJ_PLATFORM_APPLE
    void*           m_eaglView;
    void*           m_eaglAdapter;
#endif
	void*			m_mainExecutableHInstance;
	char			m_basePath[cjFilePathHint_Max][cjMAX_PATH];

	char			m_pkgPath[cjPkgPathCountMax][cjMAX_PATH];
};

// ex: pkgPath="res/map/*.cjp"
inline void cjInitParam_AddPkgPath(cjInitParam& param, const char* pkgPath)
{
	if(!pkgPath)
		return;

	for(cjINT32 i=0; i<cjPkgPathCountMax; i++)
	{
		if(param.m_pkgPath[i][0] == 0)
		{
			cjStrcpy(param.m_pkgPath[i], pkgPath);
			return;
		}
	}
}


//
// UnitTest Support
//

class CJCORE_API cjUnitTestCtx
{
public:

};

#define CJUT_DECL(CJUTLIB, CJUT_MODULE, CJUT_TEST_NAME) \
	class cjUnitTestCtx_##CJUT_TEST_NAME; \
	CJUTLIB cjUINT32 CJUT_##CJUT_MODULE##_##CJUT_TEST_NAME##(cjUnitTestCtx_##CJUT_TEST_NAME* _ctx);

#define CJUT_EXTERN_DECL(CJUTLIB, CJUT_MODULE, CJUT_TEST_NAME) \
	extern cjUINT32 CJUT_##CJUT_MODULE##_##CJUT_TEST_NAME##(void* _ctx);

#define CJUT_IMPL_BEGIN(CJUTLIB, CJUT_MODULE, CJUT_TEST_NAME) \
	CJUTLIB cjUINT32 CJUT_##CJUT_MODULE##_##CJUT_TEST_NAME##(void* __ctx) \
	{ \
		cjUnitTestCtx_##CJUT_TEST_NAME* _ctx = (cjUnitTestCtx_##CJUT_TEST_NAME*)__ctx; \
		cjUINT32 _rst = 0;
		

#define CJUT_IMPL_END(CJUTLIB, cjUTModuleName, UTTestName) \
		return _rst; \
	} 

#define CJUT_DO(CJUT_MODULE, CJUT_TEST_NAME, _ctx) CJUT_##CJUT_MODULE##_##CJUT_TEST_NAME##(_ctx);


//
// Core APIs
//

CJCORE_API cjBool cjInit(const cjInitParam& param);
CJCORE_API void cjShutdown();

CJCORE_API cjHMODULE cjGetDllLibHandle(const cjChar* dllPath);
CJCORE_API cjHMODULE cjGetDllLibHandle(const cjUni* dllPath);

CJCORE_API void* cjGetDllProcAddr(cjHMODULE module, const char* procName);

CJCORE_API void cjObjAddModulePkgName(const cjNameChar* pkgName, cjMakeModulePkgLoadPFN pfn);

CJCORE_API void cjResetEngineTime();
CJCORE_API void cjUpdateEngineTime();
//CJCORE_API cjEngineTime_t cjGetEngineTime();
CJCORE_API cjEngineTime_t cjGetEngineTimeSec();
//CJCORE_API cjEngineTime_t cjGetEngineTimeElapsed();
CJCORE_API cjEngineTime_t cjGetEngineTimeElapsedSec();

CJCORE_API void cjGetInitParam(cjInitParam& out);

// this function will allocate buf with file size + spare size(cjTAry feeds)
// so it can add additional data without reallocating buf
CJCORE_API cjBool cjLoadFileToByteArray(cjTAry<cjBYTE>& buf, const cjChar* path, 
	cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG readFlag=0, 
	cjBool loggingErr=cjFalse);

CJCORE_API cjBool cjLoadFileToByteArray(cjTAry<cjBYTE>& buf, const cjUni* path, 
	cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG readFlag=0, 
	cjBool loggingErr=cjFalse);

CJCORE_API const cjChar* cjGetFileExtStr(const cjChar* path);
CJCORE_API const cjUni* cjGetFileExtStr(const cjUni* path);

CJCORE_API void cjSplitPathA(const char* path, cjStringA& pathOnly, cjStringA& filenameOnly, cjBool addSlash=cjFalse);


//
// cjPackage Functions
//

/*
	pkgAndNameOnly=cjTrue	: only pkgName.objName accepted
	pkgAndNameOnly=cjFalse	: pkgName.objName, pkgName.subPkgName.ObjName .. 
*/
CJCORE_API cjBool cjPkgPath_CheckAcceptable(const cjNameChar* pkgPath, 
	cjINT32 pkgAndNameOnly=cjFalse);



//
// Global variables
// 

CJCORE_API extern cjBool					cjGIsOpenGL;
CJCORE_API extern void*						g_mainExecutableHInstance;


cjDeclareLib(cjCore, CJCORE_API)


#define cjDeclareForwardTPtrPoolAlloc(apiexport) \
    class apiexport cjDeclareForwardTPtrPoolAlloc { \
        public: \
        static void Init(); \
        static void ShutDown(); \
        static void* ms_shutdownList; \
    };

cjDeclareForwardTPtrPoolAlloc(CJCORE_API)


#endif

