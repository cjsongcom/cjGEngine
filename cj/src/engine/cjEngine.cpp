
#include "cjEnginePCH.h"
#include "cjEngineAutoRegClasses.h"

#include "cjGLRenRes.h"


cjImplementLib(cjEngine, CJENGINE_API, "cjCore")


cjImplementLib_Init(cjEngine)
{
	cjLOG("[cjEngine] lib init..");
	CJENGINE_AUTOREG_CLASSES

	cjGLResCache::StaticGLResCacheInit();

	cjEngineExtDat_StaticInit();
    
    return 1;
}

cjImplementLib_Shutdown(cjEngine)
{
	cjLOG("[cjEngine] lib shutdown..");
}

cjImplementLib_MakeModulePkgLoad(cjEngine)
{
	cjObjPkgLoad* enginePkgLoad		= (cjObjPkgLoad*)_pkgLoad;

	cjFObjImpDesc* imp				= NULL;	
	
	cjObjPkgLoad* corePkgLoad = NULL;
	cjObjPkgLoad::_GetModulePkgLoad(&corePkgLoad, cjObj::cjObjCreatePkg(NULL, *cjName(cjName_cjCore)));


	cjObjPkgLoad* pkgLoad= enginePkgLoad;

	// ------------------------------------------------------------------------
	// import : package define
	// ------------------------------------------------------------------------

	//       ClsPkg    ClsName     ObjName
	// [0] - cjCore, cjObjPackage, "cjCore"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName				= cjName_cjCore;
	imp->m_className				= cjName_cjObjPackage;
	imp->m_objName					= cjName_cjCore;

	imp->m_pkgIndex					= 0;
	imp->m_srcPkgLoad				= corePkgLoad; // cjCore pkg load
	imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // cjObjPackage has no expIndex
	imp->m_impObj					= NULL;

	// [1] - cjCore, cjObjPackage, "cjEngine"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName				= cjName_cjCore;
	imp->m_className				= cjName_cjObjPackage;
	imp->m_objName					= cjName_cjEngine;

	imp->m_pkgIndex					= 0;
	imp->m_srcPkgLoad				= enginePkgLoad;
	imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;
	imp->m_impObj					= NULL;


	// ------------------------------------------------------------------------
	// import : class, cjCore 
	// ------------------------------------------------------------------------

	// [2] - cjCore, cjObjCls, "cjObjCls"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName				= cjName_cjCore;
	imp->m_className				= cjName_cjObjCls;
	imp->m_objName					= cjName_cjObjCls;

	imp->m_pkgIndex					= -1;
	imp->m_srcPkgLoad				= corePkgLoad; // cjCore pkg load
	imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	imp->m_impObj					= NULL;
	
	// [4] - cjCore, cjObjCls, "cjObj"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName				= cjName_cjCore;
	imp->m_className				= cjName_cjObjCls;
	imp->m_objName					= cjName_cjObj;

	imp->m_pkgIndex					= -1;
	imp->m_srcPkgLoad				= corePkgLoad; // cjCore pkg load
	imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	imp->m_impObj					= NULL;


	// ------------------------------------------------------------------------
	// import : class, cjEngine 
	// ------------------------------------------------------------------------

	//// [5] - cjCore, cjObjCls, "cjViewport"
	//imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	//imp->m_classPkgName				= cjName_cjCore;
	//imp->m_className				= cjName_cjObjCls;
	//imp->m_objName					= cjName_cjViewport;

	//imp->m_pkgIndex					= -2;
	//imp->m_srcPkgLoad				= enginePkgLoad;
	//imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	//imp->m_impObj					= NULL;

	//// [6] - cjCore, cjObjCls, "cjClient"
	//imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	//imp->m_classPkgName				= cjName_cjCore;
	//imp->m_className				= cjName_cjObjCls;
	//imp->m_objName					= cjName_cjClient;

	//imp->m_pkgIndex					= -2;
	//imp->m_srcPkgLoad				= enginePkgLoad;
	//imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	//imp->m_impObj					= NULL;
	//
	//// [7] - cjCore, cjObjCls, "cjRenDev"
	//imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	//imp->m_classPkgName				= cjName_cjCore;
	//imp->m_className				= cjName_cjObjCls;
	//imp->m_objName					= cjName_cjRenDev;

	//imp->m_pkgIndex					= -2;
	//imp->m_srcPkgLoad				= enginePkgLoad;
	//imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	//imp->m_impObj					= NULL;
	//
	//// [8] - cjCore, cjObjCls, "cjGeometry"
	//imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	//imp->m_classPkgName				= cjName_cjCore;
	//imp->m_className				= cjName_cjObjCls;
	//imp->m_objName					= cjName_cjGeometry;

	//imp->m_pkgIndex					= -2;
	//imp->m_srcPkgLoad				= enginePkgLoad;
	//imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	//imp->m_impObj					= NULL;

	//// [9] - cjCore, cjObjCls, "cjMesh"
	//imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	//imp->m_classPkgName				= cjName_cjCore;
	//imp->m_className				= cjName_cjObjCls;
	//imp->m_objName					= cjName_cjMesh;

	//imp->m_pkgIndex					= -2;
	//imp->m_srcPkgLoad				= enginePkgLoad;
	//imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	//imp->m_impObj					= NULL;


	//////////////////////////////////////////////////////////////////////////////////
	// export : cjEngine
	//////////////////////////////////////////////////////////////////////////////////

	//cjFObjExpDesc* exp = NULL;

	//// [0] "cjEngine" , cjObjCls
	//exp = new(pkgLoad->m_exportDescAry) cjFObjExpDesc();
	//exp->m_siz = 0;	
	//exp->m_offset = 0;	
	//exp->m_pkgIndex = 0; // using cjEngine pkg as outer
	//exp->m_classIndex = 0;  // 0 for cjObjCls, 
	//// valid when this object is cjObjCls, setup parent cjObjCls-cjObj (base class of cjGame)
	//exp->m_superIndex = -5; // upper class cjObj in import
	//exp->m_objName = cjName_cjEngine;
	//exp->m_flags = 0;
	//exp->m_expObj = NULL;
	//exp->m_hashNext = 0;
	//pkgLoad->_AddIndirectExpHash(exp);


	//// [1] "cjAppEngine", cjObjCls
	//exp = new(pkgLoad->m_exportDescAry) cjFObjExpDesc();
	//
	//exp->m_superIndex = 1; // upper class cjEngine in export
	//exp->m_objName = cjName_cjAppEngine;
	//exp->m_siz = 0;	exp->m_offset = 0;	exp->m_pkgIndex = 0;  exp->m_classIndex = 0;
	//exp->m_flags = 0; exp->m_expObj = NULL;	exp->m_hashNext = 0;
	//pkgLoad->_AddIndirectExpHash(exp);
	//

	//// [2] "cjClient", cjObjCls
	//exp = new(pkgLoad->m_exportDescAry) cjFObjExpDesc();

	//exp->m_superIndex = -5; // upper class cjObj in import
	//exp->m_objName = cjName_cjAppClient;
	//exp->m_siz = 0;	exp->m_offset = 0;	exp->m_pkgIndex = 0;  exp->m_classIndex = 0;
	//exp->m_flags = 0; exp->m_expObj = NULL;	exp->m_hashNext = 0;
	//pkgLoad->_AddIndirectExpHash(exp);

	//
	//// [3] "cjAppClient", cjObjCls
	//exp = new(pkgLoad->m_exportDescAry) cjFObjExpDesc();

	//exp->m_superIndex = 3; // upper class cjClient in export
	//exp->m_objName = cjName_cjAppClient;
	//exp->m_siz = 0;	exp->m_offset = 0;	exp->m_pkgIndex = 0;  exp->m_classIndex = 0;
	//exp->m_flags = 0; exp->m_expObj = NULL;	exp->m_hashNext = 0;
	//pkgLoad->_AddIndirectExpHash(exp);


		
}


CJOBJ_IMPL_PKG_MODULE(cjEngine)


//
// cjEngine
//

CJOBJ_IMPL_CLASS(cjEngine)

cjEngine* cjEngine::ms_engine = NULL;

cjEngine::cjEngine()
{
    ms_engine = this;

	m_renDevParam = NULL;
	m_exit = cjFalse;
}

cjBool cjEngine::Init()
{
	cjObjAddToRoot();

	return cjTrue;
}

void cjEngine::OnDestroy()
{
    ms_engine = NULL;

	cjObjRemoveFromRoot();
    cjSuper::OnDestroy();
}
