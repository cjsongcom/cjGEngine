
#include "cjGamePCH.h"
#include "cjGameAutoRegClasses.h"

cjImplementLib(cjGame, CJGAME_API, "cjEngine")

cjImplementLib_Init(cjGame)
{
	cjLOG("[cjGame] lib init..");
	CJGAME_AUTOREG_CLASSES

    return 1;
}

cjImplementLib_Shutdown(cjGame)
{
	cjLOG("[cjGame] lib shutdown..");
}
 
cjImplementLib_MakeModulePkgLoad(cjGame)
{
	cjObjPkgLoad* gamePkgLoad		= (cjObjPkgLoad*)_pkgLoad;
	cjFObjImpDesc* imp				= NULL;

	cjObjPkgLoad* corePkgLoad = NULL;
	cjObjPkgLoad::_GetModulePkgLoad(&corePkgLoad, cjObj::cjObjCreatePkg(NULL, *cjName(cjName_cjCore)));

	cjObjPkgLoad* enginePkgLoad = NULL;
	cjObjPkgLoad::_GetModulePkgLoad(&enginePkgLoad, cjObj::cjObjCreatePkg(NULL, *cjName(cjName_cjEngine)));
	
	cjObjPkgLoad* pkgLoad = gamePkgLoad;


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

	// [2] - cjCore, cjObjPackage, "cjGame"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName				= cjName_cjCore;
	imp->m_className				= cjName_cjObjPackage;
	imp->m_objName					= cjName_cjGame;

	imp->m_pkgIndex					= 0;
	imp->m_srcPkgLoad				= gamePkgLoad;
	imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;
	imp->m_impObj					= NULL;


	// ------------------------------------------------------------------------
	// pkg: cjCore 
	// ------------------------------------------------------------------------

	// [3] - cjCore, cjObjCls, "cjObjCls"
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

	imp->m_pkgIndex					= -1; // to cjCore pkg
	imp->m_srcPkgLoad				= corePkgLoad; // cjCore pkg load
	imp->m_srcPkgLoadExpIndex		= cjObjIndex_None;  // for inherited cjObjCls
	imp->m_impObj					= NULL;


	// ------------------------------------------------------------------------
	// pkg: cjEngine
	// ------------------------------------------------------------------------

	

	// ------------------------------------------------------------------------
	// ExportDesc for cjGame
	// ------------------------------------------------------------------------

	//cjFObjExpDesc* exp = NULL;

	//// [0] , "cjGame" , cjObjCls
	//exp = new(pkgLoad->m_exportDescAry) cjFObjExpDesc();
	//exp->m_siz = 0;
	//exp->m_offset = 0;
	//exp->m_pkgIndex = 0;    // using cjGame pkg as outer
	//exp->m_classIndex = 0;  // 0 for cjObjCls, 

	//// valid when this object is cjObjCls, setup parent cjObjCls- cjObj (base class of cjGame)
	//exp->m_superIndex = -5; // upper class cjObj in import
	//exp->m_objName = cjName_cjGame;
	//exp->m_flags = 0;
	//exp->m_expObj = NULL;
	//exp->m_hashNext = 0;
	//
	//pkgLoad->_AddIndirectExpHash(exp);
	//
	//// [1] , "cjAppGame" , cjObjCls
	//exp = new(pkgLoad->m_exportDescAry) cjFObjExpDesc();
	//exp->m_siz = 0;
	//exp->m_offset = 0;
	//exp->m_pkgIndex = 0;    // using cjGame pkg as outer
	//exp->m_classIndex = 0;  // 0 for cjObjCls, 
	//exp->m_superIndex = 1; // upper class cjGame in export
	//exp->m_objName = cjName_cjAppGame;
	//exp->m_flags = 0;
	//exp->m_expObj = NULL;
	//exp->m_hashNext = 0;

	//pkgLoad->_AddIndirectExpHash(exp);
	
}

CJOBJ_IMPL_PKG_MODULE(cjGame)


//
// cjGame
//
	
CJOBJ_IMPL_CLASS(cjGame)

cjGame::cjGame()
{
	m_gameOperType = 0;
	m_forceExiting = cjFalse;
}

cjGame::cjGame(EGAMEOPER operType)
	: m_gameOperType(operType)
{
	m_forceExiting = cjFalse;
}

void cjGame::OnDestroy()
{
	cjSuper::OnDestroy();
}
