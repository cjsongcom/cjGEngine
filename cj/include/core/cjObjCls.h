
#ifndef _CJOBJCLS_H_
#define _CJOBJCLS_H_


CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjObjCls, cjPropStruct, cjClsFL_cjObjCls, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
    virtual void _cjObjRegister();

	virtual void cjObjPostLoad();    
	cjPropStruct* GetInheritanceSuper() {return GetSuperCls();}

	virtual void LinkProperty(cjFStream& stream, cjBool props);
	virtual cjBool MergeBools() { return cjTrue; }

protected:
	virtual void OnDestroy();

public:
//	CJRTTI_DECL_CJOBJCLS(cjObjCls)
	cjObjCls();
	
	// from global static object initialize
	cjObjCls(cjObjENativeConstructTag, 
		cjUINT32 siz, 
		cjTFLAG clsFlag, 
		cjObjCls* superCls, 
		const cjNameChar* name, 
		const cjChar* cfgName, 		
		cjTFLAG objFlag,
		void* prm,
		const cjNameChar* pkgName, void(*internalConstructor)(void*), 
		void(cjObj::*classStaticConstructor)());

	// static library version
	cjObjCls(cjObjEStaticConstructTag, 
		cjUINT32 siz, 
		cjTFLAG clsFlag, 
		const cjNameChar* name, 
		const cjChar* cfgName, 		
		cjTFLAG objFlag,
		void* prm,
		const cjNameChar* pkgName, 
		void(*internalConstructor)(void*), 
		void(cjObj::*classStaticConstructor)());

    // Warning! - return object from calling cjObjGetDefaultObj() has vtable of cjObjCls_XXXXX, not a cjObj
    // but class instance size is same to associated cjObj (ex: cjObjCls_cjGame <-> cjGame )
    // so, you can use property of cjGame, but can't call member function of cjGame, but cjObjCls_cjGame
    cjObj* cjObjGetDefaultObj()
	{
        cjAssert(m_default.Num() == GetPropertySiz());
		return (cjObj*)&m_default(0);
	}
    
//    const cjName& GetClsName() { return m_name; }
	cjTFLAG cjObjGetClassFlag() { return m_clsFlag; }
	const cjUni* GetPkgNameW() { return m_pkgNameW; }

	/*
		this
		cjObjCls_cjAppGame

			[superPropertyCls]
		[0] cjObjCls_cjAppGame->GetSuperPropertyCls()	: cjObjCls_cjObjCls
		[1] cjObjCls_cjObjCls->GetSuperPropertyCls()	: NULL
				
			[superCls]
		[0] cjObjCls_cjAppGame->GetSuperCls()	: cjObjCls_cjGame
		[1] cjObjCls_cjGame->GetSuperCls()		: cjObjCls_cjObj
		[2] cjObjCls_cjObj->GetSuperCls()		: NULL
	*/	

	cjObjCls* GetSuperCls() const { return (cjObjCls*)m_superElement; }
    
private:
	// calling cjObjHasKindOfCls() on a class is almost falling bad result
	// use IsChildOf instead using cjObjHasKindOfCls
	cjBool cjObjHasKindOfCls(cjObjCls* cls) const { return cjObj::cjObjHasKindOfCls(cls); }
    
public:
	cjTFLAG							m_clsFlag;
	const cjChar*					m_cfgName;
	const cjUni*					m_pkgNameW;	
	cjUINT32						m_lastUniqueNameGenIdx;

	// calls cjObjXXX::_cjObjInternalConstructor 
	void							(*m_clsInternalConstructor)(void *); 

	// called from cjObj::StaticInit();
	// calls cjObjXXX::_cjObjOnStaticRegister()
	void							(cjObj::*m_pfcjObjOnStaticRegister)();

	// Realloc() of cjTAry must be not called until cjMem::_StaticInit_() is called    
	cjTAry<cjBYTE>					m_default; // property array

	
	static cjINT32					ms_count;
	static cjObjCls**				ms_next;
	static cjBool					ms_onShutdown;

CJOBJ_DECL_CLASS_END(cjObjCls)


#endif
