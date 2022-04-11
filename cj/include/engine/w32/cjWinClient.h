
#ifndef _CJWINCLIENT_H_
#define _CJWINCLIENT_H_

#include "cjClient.h"


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjWinClient, cjClient, 0, cjEngine)

public:
	cjWinClient();

	virtual cjBool Init(cjEngine* engine);
	virtual void Tick();
    virtual void Draw();
	virtual cjBool Exec(const cjNameChar* cmd);
	virtual void FlushCache();

	virtual cjViewport* CreateViewport(cjTFLAG vpFlag, const cjNameChar* name,
		cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, void* prm=NULL);

	virtual void OnDestroyViewport(cjViewport* viewport);

public:
	cjBool					m_inMenuLoop;

protected:
	virtual void OnDestroy();

	cjBool InitInputDevice();
	void FinalizeInputDevice();

private:
	cjBool					m_mouseEnable;
	cjBool					m_joystickEnable;

CJOBJ_DECL_CLASS_END(cjWinClient)


#endif
