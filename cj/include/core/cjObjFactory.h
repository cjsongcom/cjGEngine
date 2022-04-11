
#ifndef _CJOBJFACTORY_H_
#define _CJOBJFACTORY_H_


CJOBJ_DECL_ABSTRACT_BEGIN(CJCORE_API, cjObjFactory, cjObj, 0, cjCore)

public:
    enum EFACTORY_FLAG
    {
        EFACTF_CREATE_NEWINST           = 0x00000001,

        EFACTF_IMPORT_FROM_TEXT         = 0x00000010,
        EFACTF_IMPORT_FROM_BIN          = 0x00000020,
        
        EFACTF_IMPORT = EFACTF_IMPORT_FROM_TEXT | EFACTF_IMPORT_FROM_BIN,        
    };

    cjObjFactory();

    virtual cjBool IsSupportCls(cjObjCls* /*cls*/) const { return NULL; }
   
    static cjObj* StaticImportObject(cjObjCls* cls, cjObj* outer, const cjNameChar* name,
        const cjUni* filepath, cjFilePathHint filepathHint, const cjNameChar* paramText=NULL, 
		cjObjFactory* factory=NULL);

        // operations (import/export)
	virtual cjObj* ImportFromBinary(cjObj* /*cls*/, cjObj* /*outer*/, cjName /*name*/, 
		cjTFLAG /*flags*/, cjObj* /*ctx*/, const cjUni* /*type*/, const cjBYTE* /*src*/, 
		const cjBYTE* /*srcEnd*/) { return NULL; }
    
	virtual cjObj* CreateNew(cjObjCls* /*cls*/, cjObj* /*outer*/, cjName /*name*/, 
		cjTFLAG /*flags*/, cjObj* /*ctx*/) { return NULL; }
    
    cjTFLAG GetFactoryFlag() { return m_factoryFlag; }
    
protected:
    virtual void OnDestroy();
    
protected:
    cjTFLAG                         m_factoryFlag;
    cjObjCls*                       m_supportCls;

CJOBJ_DECL_ABSTRACT_END(cjObjFactory)


template<class T>
inline T* cjObjImport(cjObj* outer, const cjNameChar* name, const cjChar* filepath, 
	cjFilePathHint filepathHint=cjFilePathHint_Default, const cjNameChar* paramText=NULL,
	cjObjFactory* impFactory=NULL)
{
	cjChar2Uni c2u(filepath);

	return (T*)cjObjFactory::StaticImportObject(T::cjObjGetStaticCls(),
        outer, name, *c2u, filepathHint, paramText, impFactory);
}

template<class T>
inline T* cjObjImport(cjObj* outer, const cjNameChar* name, const cjUni* filepath,
	 cjFilePathHint filepathHint=cjFilePathHint_Default, const cjNameChar* paramText=NULL,
	 cjObjFactory* impFactory=NULL)
{
	return (T*)cjObjFactory::StaticImportObject(T::cjObjGetStaticCls(),
        outer, name, filepath, filepathHint, paramText, impFactory);
}


#endif
