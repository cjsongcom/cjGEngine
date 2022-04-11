#ifndef _CJAPPENGINE_H_
#define _CJAPPENGINE_H_

#include "cjEngine.h"


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjAppEngine, cjEngine, 0, cjEngine)

public:
	virtual cjBool GenerateInputEvent(cjViewport* viewport, cjINT32 key, 
		cjInputAction action, cjFLOAT delta=0.0f);

public:
	cjAppEngine();

	virtual cjBool Init();
	virtual ETICKRST Tick(cjFLOAT elapsedTime);
	virtual void Draw(cjViewport* viewport);

	virtual cjBool CreateRenDev(cjRenDevParam* renDevPrm);
	virtual cjBool CreateAudioDev();

	virtual cjClient* GetClient() { return m_client; }
	virtual cjBool IsRendererCreated() { return m_renDev ? cjTrue : cjFalse; }

	virtual cjRenDev* GetRenDev() { return m_renDev; }

	virtual cjBool OnCharKey(cjViewport* viewport, cjInputKey key);

	const cjNameChar* GetAxisMeshName(cjBool nameOnly = cjFalse);

protected:
	virtual void OnDestroy();

	cjMesh* BuildAxisMesh();

protected:
	static cjAppEngine*					ms_appEngine;
	cjINT32								m_init;

	cjClient*							m_client;
	cjRenDev*							m_renDev;    

public:
	//cjtemp
	cjMesh*                             m_cjTestMesh;
	cjMeshInstance*                     m_cjTestMeshInst;
	cjAnimatorRPtr						m_cjTestAnimator;

	cjMesh*								m_axisMesh;

	cjColor								m_backGroundColor;

CJOBJ_DECL_CLASS_END(cjAppEngine)


#endif
