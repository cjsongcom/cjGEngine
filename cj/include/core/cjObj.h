
#ifndef _CJOBJ_H_
#define _CJOBJ_H_


#include "cjObjBase.h"

class cjObjPkgLoad;
class cjObjPkgLinker;
class cjObjPackage;
class cjObjCls;
class cjPropStruct;
class cjProperty;

//
// class name whose prefix is start with 'cjF' is associated cjObj.
// so do not inherit cjMem class (memory allocate/deallocate)
//

/*
align: https://msdn.microsoft.com/en-us/library/83ythb65(v=vs.80).aspx
- Unless overridden with __declspec(align(#)), the alignment of a scalar structure member is the minimum of its size and the current packing.
- Unless overridden with __declspec(align(#)), the alignment of a structure is the maximum of the individual alignments of its member(s).
- A structure member is placed at an offset from the beginning of its parent structure which is the smallest multiple of its alignment greater than or equal to the offset of the end of the previous member.
- The size of a structure is the smallest multiple of its alignment larger greater than or equal the offset of the end of its last member.
*/

#if defined(CJ_PLATFORM_WIN32)
__declspec(align(4)) 
#endif
class CJCORE_API cjObj // do not specify any memory allocator class !!!
{
	friend class cjLibManager;
	friend class cjObjMem;
    friend class cjFObjIterator;

	CJOBJ_DECL_BASE(cjObj, cjObj, 0 , cjCore)

	static void _cjObjInternalConstructor(void* mem)
		{ new((cjObjEInternalConstructorTag*)mem)cjObj(); }

public:
	typedef cjTAry<cjObj*> cjObjAry;
	
	cjObj();
	cjObj(const cjObj& rhs); // disabled
	cjObj& operator=(const cjObj& rhs);

	// if this is called from cjObjCls, (Native cjObjCls)
	// parameter 'cjObjCls* cls' is always 'cjObjCls::cjObjGetStaticCls()' !!
	cjObj(cjObjENativeConstructTag , cjObjCls* cls, const cjNameChar* name, 
		cjTFLAG flag, void* prm, const cjNameChar* pkgName);

	cjObj(cjObjEStaticConstructTag, const cjNameChar* name, cjTFLAG flag, 
		void* prm, const cjNameChar* pkgName);

	virtual ~cjObj();		 // 1. called directly using 'inst->~fnObj()'
							 // 2. by delete operator

	// only for delete operator
	void operator delete (void* obj, size_t siz) 
	{ 
		if(!obj) return;
		siz=siz; 
		_cjObjFree(obj);	
	}

	// called from StaticInit(), cjObjCls::Register
	// CJOBJ_DECL_STATIC_REG
	void _cjObjOnStaticRegister() {}


	const cjNameChar* GetNameStr() const { return *_m_name; }
	void SetNameStr(const cjNameChar* name) { _m_name = name; }

	cjName& GetName() { return _m_name; }
	const cjName& GetName() const { return _m_name; }
	
	cjObj* GetOuter() const { return _m_outer; }
	void SetOuter(cjObj* outer) { _m_outer = outer; }
    
	// if this is created from cjObjCls, GetCls() always returns 'cjObjCls::GetStaticCls()' 
	const cjObjCls* GetCls() const { return _m_cls; }
    cjObjCls* GetCls() { return _m_cls; }
    void SetCls(cjObjCls* cls) { _m_cls = cls; }

	cjBool cjObjIsValid() const;

	inline cjBool GetObjDestroyed() 
		{ return cjObjGetFlag() & cjOF_Destroyed ? cjTrue : cjFalse; }

	cjTFLAG cjObjGetFlag() const 
	{ 
		return _m_objFlag; 
	}

	void cjObjSetFlag(cjTFLAG newFlag) 
	{ 
		_m_objFlag |= newFlag; 

		// fulfill below list 
		// 1. must have name] or
		// 2. no-named and no importable obj(not cjObjPackage, cjObjCls..)
		cjAssert(_m_name != cjName_None || !(_m_objFlag & cjOF_Importable) );
	}

	void cjObjClearFlag(cjTFLAG newFlag) 
	{ 
		_m_objFlag &= ~newFlag; 
		cjAssert(_m_name != cjName_None || !(_m_objFlag & cjOF_Importable) );
	}

	cjTFLAG cjObjGetStFlag() const { return _m_objStFlag; }
	cjTFLAG cjObjGetStFlag() { return _m_objStFlag; }

	//
	// object load/create

		// no calls constructor, just recycle and allocate
		// do not call directly, must be called in operator new or _cjObjNew.
	static cjObj* __cjObjAlloc(cjObjCls* cls, cjObj* outer=(cjObj*)cjObjGetTempPkg(),
		cjName name=cjName_None, cjTFLAG objFlag=0, void* prm=0,
		cjObj* defaultValObj=0, void* memToReuse=0, cjObj* propSuperObj=0);
	
		// create new cjObj instance
		// 1. calls __cjObjAlloc, 2. calls cls->_cjObjInternalConstructor
	static cjObj* _cjObjNew(cjObjCls* cls, cjObj* outer=(cjObj*)cjObjGetTempPkg(), 
		cjName name=cjName_None, cjTFLAG objFlag=0, void* prm=0,
		cjObj* defaultValObj=0, cjObj* propSuperObj=0);
	
		// load cjObjCls with given base cjObjCls(generally cjObjCls_cjObjCls)
		// check result cjObjCls is in baseCls
	static cjObjCls* _cjObjLoadCls(cjObjCls* baseCls, const cjNameChar* clsName, 
		cjObj* outer, cjTFLAG loadFlags=0);
	
		// lookup package and make sure target package object is loaded
		// if cjOPLF_NoLoadingFromFile for loadFlags is in, skip loading package
		// it is same to just calling cjObjFind
	static cjObj* _cjObjLoadPkgObj(cjObjCls* objCls, cjObj* outer, const cjNameChar* name, 
		const cjNameChar* fileName, cjTFLAG loadFlags);
	
		// find
	static cjObj* cjObjFind(cjObjCls* inCls, cjObj* pkg, const cjNameChar* inName, 
		cjBool exactClass=cjFalse);

		// etc
	cjBool cjObjIsCls() { return _m_objStFlag & cjOSF_cjObjCls ? cjTrue : cjFalse; }
	cjBool cjObjIsInOuterChain(cjObj* inOuter);

		// caution! - calling cjObjHasKindOfCls on cjObjCls just return 'cjObjCls'
		// normally, it is meaningless
    cjBool cjObjHasKindOfCls(cjObjCls* cls/*cjObjCls*/) const;
		
    void cjObjParseParam(const cjNameChar* param);
	
	// cjObj system entry
	static cjBool StaticInit();
	static void StaticTick();
	static void StaticShutdown();

		// register object
	static cjBool cjObjGetObjInitialized() { return _ms_cjObjInitialized; }
	virtual void _cjObjRegister(); 	// called from 1. before call StaticInit / 2. after call StaticInit

	static void _cjObjProcessAutoRegister();
	void _cjObjConditonalRegister();
    void cjObjConditionalPostLoad();
    
    static void cjObjInitProperties(cjBYTE* writeTo, cjINT32 propertySiz, cjObjCls* clsDefault,
        cjBYTE* defaultSrcData, cjINT32 defaultSrcSiz, cjObj* dstObj, cjObj* superObj);
    static void cjObjExitProperties(cjBYTE* dst, cjObjCls* cls);
    virtual void cjObjPostLoad(); // serialize default value on property
    virtual void cjObjPostEditChange();
    
    void _cjObjImportClsDefaultDat(cjObjCls* cls, cjBool setOuter=cjFalse);
	static cjName cjObjGenerateUniqueName(cjObj* outer, cjObjCls* cls);

		// package
	static cjBool cjObjIsNonFilePkg(cjObj* pkg) { return pkg->_m_objStFlag & cjOSF_NonFilePkg ? cjTrue : cjFalse; }
	static cjBool cjObjIsModulePkg(cjObj* pkg) { return pkg->cjObjGetStFlag() & cjOSF_ModulePkg ? cjTrue : cjFalse; }

			// cjObjClsName PkgName.ObjName
	const cjNameChar* cjObjGetClassNameAndPathName(cjNameChar* buf, cjINT32 bufSiz=cjMAX_PATH) const;
			// PkgName.ObjName
	const cjNameChar* cjObjGetPathName(cjNameChar* buf, cjINT32 bufSiz=cjMAX_PATH, 
		cjObj* stopOuter=NULL, cjNameChar* str=NULL) const; // returns buf
	
	static cjObjPackage* cjObjCreatePkg(cjObj* outer, const cjNameChar* pkgName, cjBool delPkgFile=cjFalse);
	// not to exported, for created object instantly
	static cjObjPackage* cjObjGetTempPkg() { return _ms_objTempPkg; } 
	static cjBool cjObjResolveNameAndPkg(cjObj*& pkg, const cjNameChar*& name, 
		cjBool createPkg=cjFalse);
	static cjBool cjObjGetPkgLoad(cjObjPkgLoad** rstPkgLoad, cjObj* pkg,
		const cjNameChar* lookupPkgFilePath, cjTFLAG loadFlags);

	// if predefinedPkgFilePaths is null, used cjObjGetPreLookupPkgFilePathAry()
	static cjBool cjObjResolvePkgFilePathByName(cjNameChar* outPkgName, 
		cjINT32 outPkgNameBufSiz, const cjNameChar* pkgName, 
		const cjNameCharPathAry* predefinedPkgFilePaths=NULL);

		// Package Load 
	cjObjPkgLoad* cjObjGetPkgLoad() { return _m_pkgload; }
	cjObjPkgLoad* cjObjGetPkgLoad() const { return _m_pkgload; }
	void cjObjSetPkgLoad(cjObjPkgLoad* pkgLoad, cjObjIndex index);
	
	cjObjIndex cjObjGetPkgLoadExpIndex() { return _m_pkgLoadExpIndex; }
	cjObjIndex cjObjGetPkgLoadExpIndex() const { return _m_pkgLoadExpIndex; }

	static cjINT32& cjObjPkgLinkerImpCount() { return _ms_objPkgLinkerImpCount; }
	static cjObjPkgLoad* cjObjPkgLinkerGetByIndex(cjINT32 i) { return _ms_objPkgLoads(i); }
	static cjTAry<cjObjPkgLoad*>& cjObjGetPkgLoads() { return _ms_objPkgLoads; }
	static cjINT32 cjObjPkgLoadGetCount() { return _ms_objPkgLoads.Num(); }	
	static void cjObjPkgLoadRemove(cjObjPkgLoad* linker);

	static void cjObjPkgLoadReset(cjObj* pkg=NULL, cjBool removeLinkerPkgLoad=cjFalse, 
		cjBool removeDeferLoads=cjTrue);
	static const cjNameCharPathAry& cjObjGetPreLookupPkgFilePathAry()
		{ return _ms_cjObjPreLookupPkgFilePaths; }
	static cjBool cjObjSavePackage(cjObj* pkg, cjObj* topObj, cjTFLAG flags, const cjNameChar* filePath);
		
		// serialize/load/save
	virtual void Serialize(cjFStream& stream);
	static void cjObjBeginLoadPkgObj();
	static void cjObjEndLoadPkgObj();
	static cjTAry<cjObj*>& cjObjGetLoadingAry() { return _ms_objOnLoading; }

		// index in _ms_objects
	cjObjIndex cjObjGetIndex() { return _m_index; }
	cjObjIndex cjObjGetIndex() const { return _m_index; }

	static cjObj* cjObjGetByIndex(cjObjIndex index);
	
	cjObjAry& cjObjGetAllObjAry() { return _ms_objects; }
	const cjObjAry& cjObjGetAllObjAry() const { return _ms_objects; }

	void cjObjAddToRoot();
	void cjObjRemoveFromRoot();
			
	// for garbage collection
	static void CollectGarbage(cjTFLAG /*keepFlags*/) {}
	static void SerializeRootSet(cjFStream& stream, cjTFLAG keepFlags, cjTFLAG requiredFlags); 

	virtual cjBool cjObjIsPendingDelete() { return cjFalse; }


protected:
	// ensure to call destroy at destructor of lastest derived class
	cjBool cjObjConditionalDestroy(); // (non-virtual) , to do destroying once
	virtual void OnDestroy();

	void cjObjSetStFlag(cjTFLAG newFlag) { _m_objStFlag |= newFlag; }
	void cjObjClearStFlag(cjTFLAG newFlag) { _m_objStFlag &= ~newFlag; }

private:
	static void* _cjObjMalloc(size_t siz, const cjNameChar* tag=NULL);
	static void _cjObjFree(void* mem);

	// management of global cjObj hash table									
		// add to global object table and register to hash table
	void							_cjObjAdd(cjObjIndex index); 

		// reg/unregister obj to hash table
	void							_cjObjHash();
	void							_cjObjUnHash(cjObjIndex outerIndex);

	static cjUINT32					_cjObjCalcHash(cjName& name)
	{
		return name.GetIndex() & (cjARRAYCNT(_ms_objHash)-1);
	}
	
private:
	// base property
	cjObjIndex							_m_index;		// array index in cjObj::_ms_objects
	cjObj*								_m_nextHash;
	cjTFLAG								_m_objStFlag;   // static flag (read-only)
	cjObjPkgLoad*						_m_pkgload;
	cjIndexType							_m_pkgLoadExpIndex;	// array index in exportDesc map of linker
														
	cjObj*								_m_outer;
	void*								_m_prm;
	cjTFLAG								_m_objFlag;
	cjName								_m_name;

	// if instance is created from cjObjCls, _m_cls is always 'cjObjCls::cjObjGetStaticCls()'
	cjObjCls*							_m_cls; 

	cjObj*								_m_nextObjAutoRegister;
	cjObj*								_m_nextMarkDestroy;

	static cjObj*						_ms_lastMarkDestroy;
	static cjBool						_ms_cjObjInitialized;   // cjObj::StaticInit();

	static cjTAry<cjObj*>				_ms_objects;			// all objects
	static cjTAry<cjINT32>				_ms_objAvail;
	static cjObj*						_ms_objHash[cjObj_MAX_HASH];

	static cjTAry<cjObj*>				_ms_objRoot;
										// prevents the object and all child from being deleted 
										// during garbage collection
		// linkers
	static cjTAry<cjObjPkgLoad*>		_ms_objPkgLoads;
	static cjINT32						_ms_objPkgLinkerImpCount;

		// loading
	static cjINT32						_ms_objBeginLoadCount;
	static cjTAry<cjObj*>				_ms_objOnLoading;

	static cjUINT32						_ms_objAutoRegisterCount;
	static cjTAry<cjObj*>				_ms_objAutoRegisters;
	static cjObj*						_ms_objNextAutoRegister;

	static cjBool						_ms_cjObjNoRegistering;

	static cjNameCharPathAry			_ms_cjObjPreLookupPkgFilePaths;
public:
	static cjObjPackage*				_ms_objTempPkg;


#if defined(CJ_PLATFORM_WIN32)
};
#else
} __attribute__((packed, aligned(4)));
#endif


//
// cjObjPackage
//

#define CJPKG_TEMP	((cjObj*)-1)
#define CJPKG_MAX_MODULE 255
#define CJPKG_DEFAULT_EXT				L"cjp"

typedef void (*cjMakeModulePkgLoadPFN)(void*);

enum cjPackageFlag
{
	// cjObjPkgLoad::ValidateImport
	cjPkgFlag_MissingImportLinks		= 0x80000000,   // Loaded from linker with broken import links.
};


CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjObjPackage, cjObj, 0, cjCore)

	friend class cjObjPkgLoad;

public:
	cjObjPackage();

	static cjNameChar* StripPkgNameOnlyFromPath(cjNameChar* outPkgNameOnly,
		const cjNameChar* inPkgPath, cjINT32 outSiz=cjMAX_PATH);

	static void AddModulePkgName(const cjNameChar* pkgName, cjMakeModulePkgLoadPFN pfn);
	static cjINT32 CheckModulePkgName(const cjNameChar* pkgName);
	
public:
	cjTFLAG								m_pkgFlags; // cjPackageFlag
	cjBool								m_dirty;

private:
	cjBool _MakeModulePkgLoad(cjObjPkgLoad* pkgLoad);

private:
	static cjNameChar					_ms_modulePkgNames[CJPKG_MAX_MODULE][cjNameSiz];
	static cjMakeModulePkgLoadPFN		_ms_modulePkgMakePFN[CJPKG_MAX_MODULE];

protected:
	virtual void OnDestroy();

CJOBJ_DECL_CLASS_END(cjObjPackage)


//
// cjObj Instance Management (find,create)
//

template<class T> 
T* cjObjFindObject(cjObj* pkg, const cjNameChar* name, cjBool exactClass=0)
{
	return (T*)cjObj::cjObjFind(T::cjObjGetStaticCls(), pkg, name, exactClass);
}

template<class T> T* cjObjCast(cjObj* src)
{
	return src && src->cjObjHasKindOfCls(T::cjObjGetStaticCls()) ? (T*)src : NULL;
}


//
// cjObj Create/Load
//

	// create new cjObj Instance
template<class T>
T* cjObjNew(cjObjCls* cls, cjObj* outer=CJPKG_TEMP, cjName name=cjName::NullName, cjTFLAG flag=0, void* prm=0)
{
//cj_mac_only	cjAssert(cls->IsChildOf(T::cjObjGetStaticCls()));

	if(outer == CJPKG_TEMP)
		outer = cjObj::cjObjGetTempPkg();

	return (T*)cjObj::_cjObjNew(cls, outer, name, flag, prm);
}

	// cjObj Load
template<class T>
T* cjObjLoadPkgObj(cjObj* outer, const cjNameChar* objName, const cjNameChar* filename=NULL, cjTFLAG loadFlag=0)
{
	return (T*)cjObj::_cjObjLoadPkgObj(T::cjObjGetStaticCls(), outer, objName, filename, loadFlag);
}

	// cjObjCls Load
template<class T>
cjObjCls* cjObjLoadCls(const cjNameChar* name, cjObj* outer=NULL, cjTFLAG loadFlag=0)
{
	return cjObj::_cjObjLoadCls(T::cjObjGetStaticCls(), name, outer, loadFlag);
}


//
// cjElement
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJCORE_API, cjElement, cjObj, 0, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
    virtual void _cjObjRegister();	
	virtual void cjObjPostLoad() { cjSuper::cjObjPostLoad(); }

public:
	cjElement(cjElement* superElement=cjNULL);
	cjElement(cjObjENativeConstructTag, cjObjCls* cls, const cjNameChar* name, const cjNameChar* pkgName, cjTFLAG flag, cjElement* superElement, void* prm=cjNULL);
	cjElement(cjObjEStaticConstructTag tag, const cjNameChar* name, const cjNameChar* pkgName, cjTFLAG flag, void* prm=cjNULL);
		
	virtual cjProperty* GetAsProperty() { return cjNULL; }

	virtual cjObjCls* GetOwnerCls();
	virtual void AddNativeProperty(cjProperty* /*prop*/) { cjAssert(!"ERR! - this function should be not called.."); }
	virtual cjINT32 GetPropertySiz() const { return 0; }
	virtual cjBool MergeBools() { return cjTrue; }

//protected:
	cjElement*						m_superElement;		// in cjObjcls : upper cjObjCls, 'parent cjClsObj of other cjClsObj'
	cjElement*						m_nextElement;		// upper cjObjCls, property 
	
CJOBJ_DECL_ABSTRACT_END(cjElement)


//
// cjPropStruct
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJCORE_API, cjPropStruct, cjElement, 0, cjCore)

	friend class cjObj;
	CJOBJ_DECL_SERIALIZE

public:
    //virtual void _cjObjRegister() { cjSuper::_cjObjRegister(); }
	//virtual void cjObjPostLoad() { cjSuper::cjObjPostLoad(); }

		// cjElement
	virtual void AddNativeProperty(cjProperty* prop);
    virtual cjINT32 GetPropertySiz() { return m_propertySiz; }
	
protected:
    // virtual void OnDestroy() { cjSuper::OnDestroy(); }
    
public:
    cjPropStruct(cjPropStruct* superStruct=cjNULL);
	cjPropStruct(cjObjENativeConstructTag tag, cjINT32 propSiz, const cjNameChar* name, const cjNameChar* pkgName, cjTFLAG flag, cjPropStruct* superPropStruct, void* prm=cjNULL);
	cjPropStruct(cjObjEStaticConstructTag tag, cjINT32 propSiz, const cjNameChar* name, const cjNameChar* pkgName, cjTFLAG flag, void* prm=cjNULL);

//	virtual cjPropStruct* GetInheritanceSuper() { return GetSuperStruct(); }
	cjBool IsChildOf(cjPropStruct* dst) const;
	cjPropStruct* GetSuperStruct() const { return (cjPropStruct*)m_superElement; }

	virtual void LinkProperty(cjFStream& stream, cjBool props);
	virtual void SerializeModifiedDefaultProperty(cjFStream& stream, cjBYTE* data, cjObjCls* defaultsCls);
	virtual void SerializePropertyInRange(cjFStream& stream, cjBYTE* data, cjINT32 maxReadBytes);
	virtual void CleanupDestroyed(cjBYTE* dat);
	    
public:
	cjElement*						m_childElement; // property element linked-list

    cjProperty*                     m_props; // linked list for all property is in this
		// sub collection of property in m_props that is needed calling constructor
    cjProperty*                     m_refProps;
	cjProperty*                     m_constructorProps; // cjPropF_NeedConstructor
	cjProperty*                     m_configProps; // cjPropF_NeedConstructor
	
    cjINT32                         m_propertySiz;

CJOBJ_DECL_ABSTRACT_END(cjPropStruct)


//
// cjObj iterator
//

class CJCORE_API cjFObjIterator
{
public:
	cjFObjIterator(cjObjCls* iterBaseClass = cjObj::cjObjGetStaticCls())
        : m_iterBaseClass(iterBaseClass), m_iterIndex(-1)
	{
		++(*this); // operator++
	}
    
	void operator++()
	{
        while(	( ++m_iterIndex < cjObj::_ms_objects.Num() ) && 
				( !cjObj::_ms_objects(m_iterIndex) || !cjObj::_ms_objects(m_iterIndex)->cjObjHasKindOfCls(m_iterBaseClass))
        );
    }
    
	cjObj* operator*()
	{
		return cjObj::_ms_objects(m_iterIndex);
	}
    
	cjObj* operator->()
	{
		return cjObj::_ms_objects(m_iterIndex);
	}
    
	operator cjBool()
	{
		return m_iterIndex < cjObj::_ms_objects.Num();
	}
    
protected:
	cjObjCls*                           m_iterBaseClass;
	cjAryOff                            m_iterIndex;
};


template<class T>
class cjTObjIterator : public cjFObjIterator
{
public:
	cjTObjIterator() : cjFObjIterator(T::cjObjGetStaticCls())
	{
    }
    
	T* operator* ()
	{
		return (T*)cjFObjIterator::operator*();
	}
    
	T* operator-> ()
	{
		return (T*)cjFObjIterator::operator->();
	}
};


//cjopti
cjBool cjElement_TestClsFlag(cjElement* element, cjObjClassStaticFlag clsStaticFlag);


template<class T, cjObjClassStaticFlag TClsFlag>
class cjTObjElementClsFlagIter
{
public:
	cjTObjElementClsFlagIter(cjPropStruct* propStruct) 
		: m_propStruct(propStruct), 
			m_element(propStruct ? propStruct->m_childElement : cjNULL)
	{
		NextElement();
	}

	cjPropStruct* GetPropStruct() { return m_propStruct; }

	operator cjBool() { return m_element != cjNULL; }

	T* operator*() { return (T*)m_element; }
	T* operator->() { return (T*)m_element; }

	void operator++()
	{
		cjAssert(m_element);
		m_element = m_element->m_nextElement;

		NextElement();
	}
	
protected:

	void NextElement()
	{
		while(m_propStruct)
		{
			while(m_element)
			{
                if(cjElement_TestClsFlag(m_element, TClsFlag))
                    return;

				m_element = m_element->m_nextElement;
			}

			m_propStruct = m_propStruct->GetSuperStruct();

			if(m_propStruct)
				m_element = m_propStruct->m_childElement;
		}
	}

	cjPropStruct*					m_propStruct;
	cjElement*						m_element;
};



template<class T>
class cjObjRef : cjMem
{
public:
	typedef T Type;

	cjObjRef() { m_dst = cjNULL; }
	cjObjRef(T* s) { m_dst = s; }

	T* operator =(T* rhs) { m_dst = rhs; return m_dst; }

	operator T*() const { return m_dst; }
	operator T*() { return m_dst; }

	T* operator *() { return m_dst; }	
	const T* operator *() const { return m_dst; }
	
	T* operator ->() { return m_dst; }
	const T* operator ->() const { return m_dst; }

	T*						m_dst;
};


#endif

