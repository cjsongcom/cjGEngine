
#include "cjCorePCH.h"
#include "cjObjCls.h"


cjINT32		cjObjCls::ms_count = 0;
cjObjCls**	cjObjCls::ms_next = NULL;
cjBool		cjObjCls::ms_onShutdown = cjFalse;

CJOBJ_IMPL_CLASS(cjObjCls)
//CJRTTI_IMPL_CJOBJCLS(cjObjCls, cjRTTI_Attr_Disable_Instancing)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjObjCls)

	// reload default
	if(stream.IsLoading())
	{
		cjINT32 propSiz = GetPropertySiz();

		cjAssert(propSiz >= sizeof(cjObj));
		cjAssert(!GetSuperCls() || !(GetSuperCls()->cjObjGetFlag() & cjOF_NeedLoad));

		m_default.RemoveAll(propSiz, true);
		cjObjGetDefaultObj()->_cjObjImportClsDefaultDat(this);
		SerializeModifiedDefaultProperty(stream, &m_default(0), GetSuperCls());

		m_lastUniqueNameGenIdx = 0;
	}
	else if(stream.IsSaving())
	{
		cjAssert(m_default.Count() == GetPropertySiz());
		SerializeModifiedDefaultProperty(stream, &m_default(0), GetSuperCls());
	}
	else
	{
		cjAssert(m_default.Count() == GetPropertySiz());
		SerializePropertyInRange(stream, &m_default(0), 0 );
	}

CJOBJ_IMPL_SERIALIZE_END(cjObjCls)


cjObjCls::cjObjCls()
{
}

//cjObjCls::~cjObjCls()
//{
//#ifndef CJDLL
//	// when compiled as static library,
//	// cjObjCls is must be called after shutdown..
//	// because cjObjCls is instanced from different new.
//	if(!ms_onShutdown)
//	{
//		cjError("ERR! - cjObjCls(NativeClass) must be called after shutdown!!");
//	}
//#endif
//}

// called at time to initialize dll

cjObjCls::cjObjCls(
	cjObjENativeConstructTag, 
	cjTFLAG						siz,
	cjTFLAG						clsFlag,
	cjObjCls*					superCls,
	const cjNameChar*			name,
	const cjChar*				cfgName,	
	cjTFLAG						objFlag,
	void*						prm,
	const cjNameChar*			pkgName,
	void						(*internalConstructor)(void*),
	void						(cjObj::*classStaticConstructor)()
) 
: m_clsFlag(clsFlag),
  m_cfgName(cfgName),
  m_lastUniqueNameGenIdx(0),
  m_clsInternalConstructor(internalConstructor),
  m_pfcjObjOnStaticRegister(classStaticConstructor),
  m_pkgNameW(pkgName),
  cjPropStruct(
	cjObjENativeConstruct,
	siz,
	name,
	pkgName,
	objFlag,
	superCls != this ? superCls : cjNULL, //cjtrace
	prm
	)
{	
	cjObjSetStFlag(cjOSF_cjObjCls);
}


//
// static library version
//

cjObjCls::cjObjCls(
	cjObjEStaticConstructTag, 
	cjTFLAG							siz,
	cjTFLAG							clsFlag,
	const cjNameChar*				name,
	const cjChar*					cfgName,	
	cjTFLAG							objFlag,
	void*							prm,
	const cjNameChar*				pkgName,
	void							(*internalConstructor)(void*),
	void							(cjObj::*classStaticConstructor)()
)
: m_clsFlag(clsFlag),
  m_cfgName(cfgName),
  m_lastUniqueNameGenIdx(0),
  m_pkgNameW(pkgName),
  m_clsInternalConstructor(internalConstructor),
  m_pfcjObjOnStaticRegister(classStaticConstructor),
  cjPropStruct(
	cjObjEStaticClassConstruct, 
	siz, 
	name,
	pkgName,
	objFlag, 
	prm
	)
{
    cjObjSetStFlag(cjOSF_cjObjCls);
}

void cjObjCls::OnDestroy()
{
	cjObj::cjObjExitProperties(&m_default(0), this);
	m_default.RemoveAll();
    
	cjSuper::OnDestroy();
}

void cjObjCls::_cjObjRegister()
{
	cjSuper::_cjObjRegister();
    
//    if(GetSuperCls())
//    {
//        GetSuperCls()->_cjObjConditonalRegister();
//        cjLOG(_CU("%ls"), *(GetSuperCls()->GetName()));
//    }
    
    // init default object
    cjINT32 sizProp = GetPropertySiz();
    m_default.RemoveAll(sizProp); // remove all element and resize, numElem=0, numMax=sizProp
    m_default.PushEmpty(sizProp); // fill element

	cjObjGetDefaultObj()->_cjObjImportClsDefaultDat(this);

	// static construct
	(cjObjGetDefaultObj()->*m_pfcjObjOnStaticRegister)(); // register property on this class

	// Link the cleanup.
	cjFStream dummy;
    LinkProperty(dummy, cjFalse); // do not load property from stream(file)
}

void cjObjCls::cjObjPostLoad()
{
	cjSuper::cjObjPostLoad();

	if(GetSuperCls())
		GetSuperCls()->cjObjConditionalPostLoad();
}

void cjObjCls::LinkProperty(cjFStream& stream, cjBool props)
{
	cjSuper::LinkProperty(stream, props);
}
