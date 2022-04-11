
#ifndef _CJGAME_H_
#define _CJGAME_H_

#include "cjGameLibType.h"

// prerequest
#include "core/cjCore.h"
#include "engine/cjEngine.h"


CJOBJ_DECL_ABSTRACT_BEGIN(CJGAME_API, cjGame, cjObj, 0, cjGame)

public:
	enum EGAMEOPER
	{
		EGAMEOPER_GAME			= 0x1,
		EGAMEOPER_TOOL			= 0x2,
	};
	
	enum ERUN
	{
		ERUN_OK					=  0,
		ERUN_EXIT				= -1,
		ERUN_EXIT_ERR			= -2,
	};

	cjGame(); // for internal constructor
	cjGame(EGAMEOPER operType);

	virtual cjBool Init() = 0;
    
    virtual ERUN Tick() = 0;
    virtual void Draw() = 0;

	inline cjTFLAG GetGameOperType() { return m_gameOperType; }
	virtual void* GetGameParam() = 0;
	virtual cjEngine* GetEngine() = 0;

	virtual void PumpMessage() = 0;

	virtual void SetForceExiting(cjBool exiting) { m_forceExiting = exiting; }
	inline cjBool IsForceExiting() { return m_forceExiting; }

protected:
	virtual void OnDestroy() = 0;

protected:
	cjTFLAG						m_gameOperType;
	cjBool						m_forceExiting;

CJOBJ_DECL_ABSTRACT_END(cjGame)


cjDeclareLib(cjGame, CJGAME_API)
static cjDeclareLib_cjGame cjGameLibDeclare;


#endif
