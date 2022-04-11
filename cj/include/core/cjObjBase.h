
#ifndef _CJOBJBASE_H_
#define _CJOBJBASE_H_


typedef int						cjObjIndex;
enum { cjObjIndex_None = (cjObjIndex)-1 };
enum { cjObj_MAX_HASH = 4096 };

enum { cjObj_ClsPropertySizAlignByte = 4 };

enum cjObjEInternalConstructorTag	{ cjObjEInternalConstructor		};

enum cjObjENativeConstructTag		{ cjObjENativeConstruct			};
enum cjObjEStaticConstructTag		{ cjObjEStaticClassConstruct	};

enum cjObjEPropertyTag              { cjObjEProperty                };

enum cjObjClassStaticFlag
{
	cjClsFL_Abstract             = 0x00000001,
	cjClsFL_cjObjCls             = 0x00000002,
    cjClsFL_Transient            = 0x00000004,

    cjClsFL_Property             = 0x00000100,
    cjClsFL_ClsProperty          = 0x00000200,
    cjClsFL_ObjProperty          = 0x00000400,
	cjClsFL_SerObjRefProperty	 = 0x00000800,
	cjClsFL_BoolProperty		 = 0x00001000,
};

enum cjObjPkgLoadFlags
{
	cjOPLF_None							= 0x00000000,	// No flags.
	cjOPLF_AllowMissingImport			= 0x00000001,   // set to null for missing imports
	cjOPLF_NoVerify						= 0x00000002,
	cjOPLF_CheckExistingInExpDescOnly	= 0x00000004,	// do not actually load. just check
	cjOPLF_NoLoadingFromFile			= 0x00000008,	// do not load from file
};


enum cjObjStFlag	// _m_objStFlag
{
	cjOSF_cjObjCls					= 0x00000001, // cjObjCls
	cjOSF_cjStaticObj				= 0x00000002, // static allocated object, do not call delete

	cjOSF_Package					= 0x00000010,
	cjOSF_TempPkg					= 0x00000040,
	cjOSF_ModulePkg					= 0x00000080,

	// pkg is only exist as memory (module package(core, engine, game..), temp package)
	cjOSF_NonFilePkg				= cjOSF_TempPkg | cjOSF_ModulePkg,
	
	cjOSF_DestroyAtExit				= cjOSF_cjStaticObj,	// this object will be destroyed when program is terminated	
};


enum cjObjFlag		// _m_objFlag;
{
	cjOF_Transient					= 0x00000001, // do not save
	cjOF_Importable					= 0x00000002, // obj can be used for importing from other obj
//	cjOF_Private					= 0x00000004,

		//
		// loading & serialize	
		//	
	// cjOF_NeedLoad -> Add cjOF_Preloading, Remove cjOF_NeedLoad 
	// -> Serialize -> Remove cjOF_Preloading
	cjOF_NeedLoad					= 0x00000010,
	cjOF_Preloading					= 0x00000020,

	// cjObjPkgLoad::CreateExport
	// cjObjStaticConstructObject(... , RF_NeedLoad | RF_NeedPostLoad)
	// cjOF_NeedPostLoad -> in ConditionalPostLoad(), 
	//   remove cjOF_NeedPostLoad | cjOF_PostLoadInProgress -> cjObjPostLoad
	cjOF_NeedPostLoad				= 0x00000040,
	cjOF_PostLoadInProgress			= 0x00000080, // cjObjPostLoad() is in progress, prevent reentrying
		
		// import/export/serialize tag
	cjOF_TagImport					= 0x00010000,
	cjOF_TagExport					= 0x00020000,
	cjOF_TagSerialze				= 0x00040000,

	// trace, debugging, garbage collect
	cjOF_TraceRouteDestroy			= 0x01000000, // trace routing calling destroy to child
	cjOF_Unreachable				= 0x02000000,
	cjOF_TagGarbage					= 0x04000000,

		// destroying
	cjOF_MarkDestroy				= 0x10000000,	
	cjOF_Destroyed					= 0x80000000,
	
		// composite
    cjOF_Invalid					= cjOF_MarkDestroy | cjOF_Destroyed,
	cjOF_ObjLoadFlags				= cjOF_Importable
};


enum cjPropFlag
{
    cjPropF_Edit                    = 0x00000001, // property is settable in the editor.
	cjPropF_Export                  = 0x00000002, // can be exported.
	cjPropF_Transient               = 0x00000004, // property shouldn't be saved, set to zero at load time.
	cjPropF_Native					= 0x00000008,
	cjPropF_NeedConstructorLink     = 0x00000010, // needs construction/destruction.
	cjPropF_Config					= 0x00000020,

};


//
// Declare
//

#ifdef CJDLL
	// dll build

	#define CJOBJ_DECL_BASE_EX(TAPIName, TClass, TSuper, TFlag, TPkg) \
	__declspec(align(4)) class TAPIName TClass : public TSuper \
	{ \
		private: static cjObjCls	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* cjObjGetStaticCls() { return &_cjobjcls; } \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer=(cjObj*)cjObjGetTempPkg(), cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; }

	#define CJOBJ_DECL_BASE_EX2(TAPIName, TClass, TSuper, TSuper1, TFlag, TPkg) \
	__declspec(align(4)) class TAPIName TClass : public TSuper , public TSuper1 \
	{ \
		private: static cjObjCls	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* cjObjGetStaticCls() { return &_cjobjcls; } \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer=(cjObj*)cjObjGetTempPkg(), cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; }


	#define CJOBJ_DECL_BASE(TClass, TSuper, TFlag, TPkg) \
		private: static cjObjCls	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* cjObjGetStaticCls() { return &_cjobjcls; } \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer=(cjObj*)cjObjGetTempPkg(), cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; }


//////////////////////////////////////////////////////////////////////////
#else // #elif defined(CJLIB)
//////////////////////////////////////////////////////////////////////////

//
// static lib build
//

#ifdef CJ_PLATFORM_WIN32

	#define CJOBJ_DECL_BASE_EX(TAPIName, TClass, TSuper, TFlag, TPkg) \
	__declspec(align(4)) class TAPIName TClass : public TSuper \
	{ \
		private: static cjObjCls*	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* _cjobjclsNext[2]; \
		static cjObjCls* _cjObjGetStaticClass##TClass(const cjNameChar* pkgName); \
		static void _cjObjInitStaticCls##TClass(); \
		static cjObjCls* cjObjGetStaticCls() \
		{ \
			if(!_cjobjcls) \
			{ \
				_cjobjcls = _cjObjGetStaticClass##TClass(_CU(#TPkg)); \
				_cjObjInitStaticCls##TClass(); \
			} \
			return _cjobjcls; \
		} \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; } \


	#define CJOBJ_DECL_BASE_EX2(TAPIName, TClass, TSuper, TSuper1, TFlag, TPkg) \
	__declspec(align(4)) class TAPIName TClass : public TSuper , public TSuper1 \
	{ \
		private: static cjObjCls*	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* _cjobjclsNext[2]; \
		static cjObjCls* _cjObjGetStaticClass##TClass(const cjNameChar* pkgName); \
		static void _cjObjInitStaticCls##TClass(); \
		static cjObjCls* cjObjGetStaticCls() \
		{ \
			if(!_cjobjcls) \
			{ \
				_cjobjcls = _cjObjGetStaticClass##TClass(_CU(#TPkg)); \
				_cjObjInitStaticCls##TClass(); \
			} \
			return _cjobjcls; \
		} \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; } \


	// m_clsFlag in cjObjCls is TFlag
	#define CJOBJ_DECL_BASE(TClass, TSuper, TFlag, TPkg) \
		private: static cjObjCls*	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* _cjobjclsNext[2]; \
		static cjObjCls* _cjObjGetStaticClass##TClass(const cjNameChar* pkgName); \
		static void _cjObjInitStaticCls##TClass(); \
		static cjObjCls* cjObjGetStaticCls() \
		{ \
			if(!_cjobjcls) \
			{ \
				_cjobjcls = _cjObjGetStaticClass##TClass(_CU(#TPkg)); \
				_cjObjInitStaticCls##TClass(); \
			} \
			return _cjobjcls; \
		} \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; } \



#else // of -#-ifdef CJ_PLATFORM_WIN32


	// for apple, linux
	//#error "need 4byte aligned class declaration.." __declspec(align(4))
	//#error " class { } __attribute__((packed, aligned(4)));  need "

/*
	aligned (alignment)

	This attribute specifies a minimum alignment for the variable or structure field, measured in bytes. For example, the declaration:
	int x __attribute__ ((aligned (16))) = 0;
	causes the compiler to allocate the global variable x on a 16-byte boundary.

	packed
	The packed attribute specifies that a variable or structure field should have the 
	 >> smallest possible alignment <<
	one byte for a variable, and one bit for a field, unless you specify a larger value with the aligned attribute.
	Here is a structure in which the field x is packed, so that it immediately follows a
*/

	#define CJOBJ_DECL_BASE_EX(TAPIName, TClass, TSuper, TFlag, TPkg) \
	class TAPIName TClass : public TSuper \
	{ \
		private: static cjObjCls*	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* _cjobjclsNext[2]; \
		static cjObjCls* _cjObjGetStaticClass##TClass(const cjNameChar* pkgName); \
		static void _cjObjInitStaticCls##TClass(); \
		static cjObjCls* cjObjGetStaticCls() \
		{ \
			if(!_cjobjcls) \
			{ \
				_cjobjcls = _cjObjGetStaticClass##TClass(_CU(#TPkg)); \
				_cjObjInitStaticCls##TClass(); \
			} \
			return _cjobjcls; \
		} \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; } \


	#define CJOBJ_DECL_BASE_EX2(TAPIName, TClass, TSuper, TSuper1, TFlag, TPkg) \
	class TAPIName TClass : public TSuper , public TSuper1 \
	{ \
		private: static cjObjCls*	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* _cjobjclsNext[2]; \
		static cjObjCls* _cjObjGetStaticClass##TClass(const cjNameChar* pkgName); \
		static void _cjObjInitStaticCls##TClass(); \
		static cjObjCls* cjObjGetStaticCls() \
		{ \
			if(!_cjobjcls) \
			{ \
				_cjobjcls = _cjObjGetStaticClass##TClass(_CU(#TPkg)); \
				_cjObjInitStaticCls##TClass(); \
			} \
			return _cjobjcls; \
		} \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; } \


	// m_clsFlag in cjObjCls is TFlag
	#define CJOBJ_DECL_BASE(TClass, TSuper, TFlag, TPkg) \
		private: static cjObjCls*	_cjobjcls; \
		public: \
			enum { _CJOBJCLS_FLAG_ = TFlag }; \
		typedef TClass cjThisClass;\
		typedef TSuper cjSuper; \
		static cjObjCls* _cjobjclsNext[2]; \
		static cjObjCls* _cjObjGetStaticClass##TClass(const cjNameChar* pkgName); \
		static void _cjObjInitStaticCls##TClass(); \
		static cjObjCls* cjObjGetStaticCls() \
		{ \
			if(!_cjobjcls) \
			{ \
				_cjobjcls = _cjObjGetStaticClass##TClass(_CU(#TPkg)); \
				_cjObjInitStaticCls##TClass(); \
			} \
			return _cjobjcls; \
		} \
		void* operator new(size_t , cjObj* outer=(cjObj*)cjObjGetTempPkg(), cjName name=cjName_None, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, name, flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, const cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObj* outer, cjNameChar* name, cjTFLAG flag=0, void* prm=0) \
			{ return __cjObjAlloc(cjObjGetStaticCls(), outer, cjName(name), flag, prm, NULL, NULL, NULL); } \
		void* operator new(size_t , cjObjEInternalConstructorTag* mem) \
			{ return (void*)mem; } \
        

#endif

//////////////////////////////////////////////////////////////////////////
#endif // endof #elif defined(CJLIB)
//////////////////////////////////////////////////////////////////////////



#define CJOBJ_DECL_CLASS_BEGIN(TAPIName, TClass, TBase, TFlag, TPkg) \
	CJOBJ_DECL_BASE_EX(TAPIName, TClass, TBase, TFlag, TPkg) \
	static void* cjGetStaticCls(cjUINT32* flag=cjNULL) { if(flag) { *flag=0; } return (void*)cjObjGetStaticCls(); } \
	friend cjFStream& operator<<(cjFStream& stream, TClass*& c) \
		{ return stream << *(cjObj**)&c; } \
	virtual ~TClass() { cjObjConditionalDestroy(); } \
	static void _cjObjInternalConstructor(void* mem) \
		{ new( (cjObjEInternalConstructorTag*)mem )TClass(); } \

#define CJOBJ_DECL_CLASS_BEGIN_EX2(TAPIName, TClass, TBase, TBase1, TFlag, TPkg) \
	CJOBJ_DECL_BASE_EX2(TAPIName, TClass, TBase, TBase1, TFlag, TPkg) \
	static void* cjGetStaticCls(cjUINT32* flag=cjNULL) { if(flag) { *flag=0; } return (void*)cjObjGetStaticCls(); } \
	friend cjFStream& operator<<(cjFStream& stream, TClass*& c) \
		{ return stream << *(cjObj**)&c; } \
	virtual ~TClass() { cjObjConditionalDestroy(); } \
	static void _cjObjInternalConstructor(void* mem) \
		{ new( (cjObjEInternalConstructorTag*)mem )TClass(); } \


#ifdef CJ_PLATFORM_WIN32
#define CJOBJ_DECL_CLASS_END(TClass)  };
#else
#define CJOBJ_DECL_CLASS_END(TClass)  } __attribute__((packed, aligned(4)));
#endif

#define CJOBJ_DECL_ABSTRACT_BEGIN(TAPIName, TClass, TBase, TFlag, TPkg) \
	CJOBJ_DECL_BASE_EX(TAPIName, TClass, TBase, TFlag | cjClsFL_Abstract, TPkg) \
	static void* cjGetStaticCls(cjUINT32* flag=cjNULL) { if(flag) { *flag=0; } return (void*)cjObjGetStaticCls(); } \
	friend cjFStream& operator<<(cjFStream& stream, TClass*& c) \
		{ return stream << *(cjObj**)&c; } \
	virtual ~TClass() { cjObjConditionalDestroy(); }

#ifdef CJ_PLATFORM_WIN32
#define CJOBJ_DECL_ABSTRACT_END(TClass) };
#else
#define CJOBJ_DECL_ABSTRACT_END(TClass) } __attribute__((packed, aligned(4)));
#endif


//
//#define CJOBJ_DECL_CLASS_(TClass, TBase, TFlag, TPkg) \
//	CJOBJ_DECL_BASE(TClass, TBase, TFlag, TPkg) \
//	friend cjFStream& operator<<(cjFStream& stream, TClass*& c) \
//		{ return stream << *(cjObj**)&c; } \
//	virtual ~TClass() { cjObjConditionalDestroy(); } \
//	static void _cjObjInternalConstructor(void* mem) \
//		{ new( (cjObjEInternalConstructorTag*)mem )TClass(); } \
//
//#define CJOBJ_DECL_ABSTRACT_(TClass, TBase, TFlag, TPkg) \
//	public:	\
//	CJOBJ_DECL_BASE(TClass, TBase, TFlag | cjClsFL_Abstract, TPkg) \
//	virtual ~TClass() { cjObjConditionalDestroy(); }


//
// Implement
//

#ifdef CJDLL

		////////////////////////////////////////////////////////////////////////////////
		#ifdef _MSC_VER
		////////////////////////////////////////////////////////////////////////////////

			// _cjobjcls is in global area
			#define CJOBJ_IMPL_CLASS(TClass) \
				cjObjCls TClass::_cjobjcls \
				( \
				 cjObjENativeConstruct, \
				 sizeof(TClass), \
				 TClass::_CJOBJCLS_FLAG_ | cjClsFL_cjObjCls, \
				 TClass::cjSuper::cjObjGetStaticCls(), \
				 _CU(#TClass), \
				 0, \
				 cjOF_Transient | cjOF_Importable, \
				 0, \
				 g_cjObjPkgModuleName, \
				 (void (*)(void*))&TClass::_cjObjInternalConstructor, \
				 (void (cjObj::*)())&TClass::_cjObjOnStaticRegister \
				 ); \
				 \
				extern "C" cjDLL_EXPORT cjObjCls* cjObjCls_##TClass;\
				cjDLL_EXPORT cjObjCls* cjObjCls_##TClass = TClass::cjObjGetStaticCls();

			#define CJOBJ_IMPL_PKG_MODULE(pkgName) \
				extern "C" cjDLL_EXPORT cjNameChar g_cjObjPkgModuleName[] = _CU(#pkgName);
	
		////////////////////////////////////////////////////////////////////////////////
		#else
		////////////////////////////////////////////////////////////////////////////////
		
			#error "not impl" // so - gnu is not supported
			#define CJOBJ_IMPL_PKG_MODULE(pkgName) \
				extern "C" { cjNameChar g_cjObjPkgModuleName##pkg##[] = _CU(#pkgName);} \ // static library
				extern "C" { cjBYTE g_cjObjPkgModuleLoaded##pkgName; }  // for prevent linker remove unreferenced value in static link

		////////////////////////////////////////////////////////////////////////////////
		#endif // end of _MSC_VER
		////////////////////////////////////////////////////////////////////////////////


#elif defined(CJLIB)  // of #ifdef CJDLL


		/////////////////////////////////////////////////////////////////////
		// Static Library Build
		/////////////////////////////////////////////////////////////////////

		#define CJOBJ_IMPL_CLASS(TClass) \
			cjObjCls* TClass::_cjobjcls = NULL; \
			cjObjCls* TClass::_cjobjclsNext[2]; \
			cjObjCls* TClass::_cjObjGetStaticClass##TClass(const cjNameChar* pkgName) \
			{ \
				cjObjCls* newCls = ::new cjObjCls \
				( \
					cjObjEStaticClassConstruct, \
					sizeof(TClass), \
					TClass::_CJOBJCLS_FLAG_ | cjClsFL_cjObjCls, \
					_CU(#TClass), \
					0, \
					cjOF_Transient | cjOF_Importable, \
					0, \
					pkgName, \
					(void (*)(void*))&TClass::_cjObjInternalConstructor, \
					(void (cjObj::*)())&TClass::_cjObjOnStaticRegister \
				); \
				if(cjObjCls::ms_next) \
                    TClass::_cjobjclsNext[0] = (cjObjCls*)cjObjCls::ms_next; \
				cjObjCls::ms_next = &TClass::_cjobjclsNext[0]; \
				TClass::_cjobjclsNext[1] = (cjObjCls*)newCls; \
				cjObjCls::ms_count++; \
				return newCls; \
			} \
			void TClass::_cjObjInitStaticCls##TClass() \
			{ /* called from static class */ \
				if(TClass::cjSuper::cjObjGetStaticCls() != TClass::_cjobjcls) \
				{\
					TClass::_cjobjcls->m_superElement = TClass::cjSuper::cjObjGetStaticCls(); \
				}\
				else \
				{\
					TClass::_cjobjcls->m_superElement = NULL; \
				}\
				TClass::_cjobjcls->SetCls(cjObjCls::cjObjGetStaticCls()); \
				if(TClass::_cjobjcls->cjObjGetObjInitialized() \
				   && TClass::_cjobjcls->GetCls()==TClass::_cjobjcls->cjObjGetStaticCls()) \
					TClass::_cjobjcls->_cjObjRegister(); \
			}

		#ifdef _MSC_VER
			#define CJOBJ_IMPL_PKG_MODULE(pkgName) \
			extern "C" { cjBYTE __declspec(dllexport) g_cjObjPkgModuleLoaded##pkgName; }  // for prevent linker remove unreferenced value in static link
			//extern "C" { cjNameChar g_cjObjPkgModuleName##pkgName[] = _CU(#pkgName);}
		#else
			#define CJOBJ_IMPL_PKG_MODULE(pkgName) \
			cjBYTE g_cjObjPkgModuleLoaded##pkgName;
			//extern "C" { cjNameChar g_cjObjPkgModuleName##pkg##[] = _CU(#pkgName);}
		#endif


#endif


// #define CJ_USE_ADJUST_CJOBJCLS_PROPSIZ_ALIGN

//
// Serialize
//

	//
	// cjObj
	//

	// debug support
#define CJOBJ_DBG_SERIALIZE_BEGIN(_stream, _func, _line, _fname) \
		cjWORD __lastDBGSerial = 0; \
		cjWORD __curDBGSerial = 0; \
		cjStreamOffset __curDBGOffset = 0; \
		cjStreamOffset __lastDBGEndOffset = 0; \
		if(_stream.IsLoading()) { \
			_stream << __curDBGSerial; \
			__curDBGOffset = _stream.GetDebugCurContentPos(__curDBGSerial); \
			if(__curDBGOffset) __curDBGOffset -= sizeof(__curDBGSerial); \
		} \
		if(_stream.IsSaving()) { \
			_stream.ValidateStreamDBGInfo(); \
			const cjNameChar* __dbgSerClsName = GetCls()->GetName(); \
			cjNameChar __dbgSerNameBuf[cjMAX_PATH]; \
			const cjNameChar* __dbgSerName = cjObjGetPathName(__dbgSerNameBuf); \
			__lastDBGSerial = _stream.BeginDBGSerInfo(&__curDBGSerial, __dbgSerClsName, __dbgSerName, _func, _line, _fname); \
			__curDBGOffset = _stream.GetDebugCurContentPos(__curDBGSerial); \
			_stream << __curDBGSerial; \
		} \

#define CJOBJ_DBG_SERIALIZE_END(_stream, _func, _line, _fname) \
		if(_stream.IsLoading()) { \
			_stream << __lastDBGSerial; \
			__lastDBGEndOffset = _stream.GetDebugCurContentPos(__lastDBGSerial); \
			if(__lastDBGEndOffset) __lastDBGEndOffset -= sizeof(__lastDBGSerial); \
			_stream.m_dbgLastDBGSerial = __lastDBGSerial; \
			_stream.m_dbgLastBeginOffset = __curDBGOffset; \
			_stream.m_dbgLastEndOffset = __lastDBGEndOffset; \
		} \
		if(_stream.IsSaving()) { \
			_stream.EndDBGSerInfo(_func, _line, _fname, __lastDBGSerial); \
			_stream.Flush(); \
		} 


#define CJOBJ_DECL_SERIALIZE \
	public: \
			virtual void Serialize(cjFStream& stream);

#define CJOBJ_IMPL_SERIALIZE_BEGIN(CLSNAME) \
	void CLSNAME::Serialize(cjFStream& stream) \
	{ \
		CJOBJ_DBG_SERIALIZE_BEGIN(stream, __FUNCTION__, __LINE__, __FILE__) \
		cjSuper::Serialize(stream); 


#define CJOBJ_IMPL_SERIALIZE_END(CLSNAME) \
		CJOBJ_DBG_SERIALIZE_END(stream, __FUNCTION__, __LINE__, __FILE__) \
	}


#define CJOBJ_DECL_STATIC_REG \
	void _cjObjOnStaticRegister(); \

#define CJOBJ_IMPL_STATIC_REG_BEGIN(cls) \
	void cls::_cjObjOnStaticRegister() { 

#define CJOBJ_IMPL_STATIC_REG_END(cls) \
	}




	//
	// not cjObj
	//

// REV=0 for not read/write revision

#define CJ_DECL_SERIALIZE(CLSNAME)	\
	friend cjFStream& operator<<(cjFStream& stream, CLSNAME& d);

#define CJ_IMPL_SERIALIZE_BEGIN(CLSNAME) \
	cjFStream& operator<<(cjFStream& stream, CLSNAME& d) \
	{ \
		if(stream.IsObjOnly()) \
			return stream;

#define CJ_IMPL_SERIALIZE_END(CLSNAME) \
		return stream; \
	}

#define CJ_DECL_SERIALIZE_INLINE_BEGIN(CLSNAME) \
	friend cjFStream& operator<<(cjFStream& stream, CLSNAME& d) \
	{ \
		if(stream.IsObjOnly()) \
			return stream;

#define CJ_DECL_SERIALIZE_INLINE_END(CLSNAME) \
		return stream; \
	}

#define CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(CLSNAME, SUPER_CLSNAME) \
	friend cjFStream& operator<<(cjFStream& stream, CLSNAME& d) \
	{ \
		if(stream.IsObjOnly()) \
			return stream; \
		\
		stream << (SUPER_CLSNAME&)d;

#define CJ_DECL_SERIALIZE_EX_INLINE_END(CLSNAME, SUPER_CLSNAME) \
		return stream; \
	}


#endif
