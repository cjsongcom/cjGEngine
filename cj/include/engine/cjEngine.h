
#ifndef _CJENGINE_H_
#define _CJENGINE_H_

#include "cjEngineLibType.h"

// prerequests
#include "core/cjCore.h"
#include "core/cj3DMath.h"

// engine stuff
class cjEngine;
class cjViewport;
class cjRenDev;

#include "cjClient.h"
#include "cjViewport.h"
#include "cjCamera.h"
#include "cjKey.h"

	// render stuff
#include "cjRenRes.h"

#include "cjShaderDef.h"
#include "cjShader.h"
#include "cjShaderRes.h"
#include "cjShaderManager.h"

#include "cjRenDev.h"
#include "cjMaterial.h"
#include "cjTexture.h"

#include "cjEngineExtDat.h"

	// physics
#include "physics/cjPhy.h"

    // geometry
#include "cjGeometry.h"
#include "cjMesh.h"

#include "cjResChr.h"

    // obj import factory
#include "cjObjFactoryForEngine.h"

	// scene stuff
#include "engine/scene/cjSceneStuff.h"

#include "cjShaderMaterial.h"



CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjEngine, cjObj, cjClsFL_Transient, cjEngine)

public:
	enum ETICKRST
	{
		ETICKRST_EXIT = 1,
		ETICKRST_OK = 0,

		ETICKRST_FATAL_EXIT_BAD_TIME = -1
	};

	cjEngine();

	virtual cjBool Init();
	virtual ETICKRST Tick(cjFLOAT elapsedTime) = 0;
	virtual void Draw(cjViewport* viewport) = 0;

	inline cjRenDevParam* GetRenDevParam() { return m_renDevParam;}

	virtual cjBool CreateRenDev(cjRenDevParam* renDevPrm) = 0;
	virtual cjClient* GetClient() = 0;

	virtual cjBool IsRendererCreated() = 0;
	virtual cjRenDev* GetRenDev() = 0;

	static cjEngine* GetEngine() { return ms_engine; }

	static cjString ConvertDataPath(const cjChar* dataPath);

	virtual cjBool OnCharKey(cjViewport* viewport, cjInputKey key) { return cjFalse; }
	virtual cjBool OnMouseWheel(cjViewport* viewport, cjUINT32 wParam, cjSWORD zDelta) { return cjFalse; }

	// a key (FNIK_MouseX, FNIK_MouseY, FNIK_MouseW) has meaning below for 'delta' parameter
	// FNIK_MouseX, FNIK_MouseY : mousemovement delta from (DirectInput), mouseX,Y position * mouseXYMultiplier
	// FNIK_MouseW : amount of wheeled
	virtual cjBool GenerateInputEvent(cjViewport* viewport, cjINT32 key, 
		cjInputAction action, cjFLOAT delta=0.0f) { delta = delta; return cjFalse; }
	
	virtual void SetExit(cjBool forceExit) { m_exit = forceExit; }
	inline cjBool GetExit() { return m_exit; }

protected:
	virtual void OnDestroy();

protected:
	cjRenDevParam*					m_renDevParam;
	cjBool							m_exit;
    
    static cjEngine*                ms_engine;

CJOBJ_DECL_ABSTRACT_END(cjEngine)


cjFLOAT CJENGINE_API cjGetLevelTimeSecond();

cjDeclareLib(cjEngine, CJENGINE_API)
static cjDeclareLib_cjEngine cjEngineLibDeclare;


#endif

