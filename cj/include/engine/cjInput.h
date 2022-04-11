
#ifndef _CJINPUT_H_
#define _CJINPUT_H_

#include "cjKey.h"


class cjInput : public cjMem
{
public:
	cjInput();
	virtual ~cjInput();

	virtual void Init(cjViewport* viewport);
	virtual cjBool Exec(const cjNameChar* cmd);

	virtual void ResetInput();
	virtual void DispatchInput(cjFLOAT elapsed);

	virtual cjBool UpdateInputEventState(cjInputKey key, cjInputAction action, cjFLOAT delta=0.0f);
	inline cjBYTE IsKeyDown(int key)  { return m_isKeyDown[cjClamp<cjINT32>(key, 0, CJIK_MAX)]; }

	virtual cjBool ProcessInputEvent(cjInputKey key, cjInputAction action, cjFLOAT delta=0.0f);

	void SetInputParamForExec(cjInputAction action, cjFLOAT delta=0.f)
		{ m_execInputAction = action; m_execInputDelta = delta; }

	cjInputAction GetInputActionForExec() { return m_execInputAction; }
	cjFLOAT GetInputDeltaForExec() { return m_execInputDelta; }

	static const cjUINT32 MAX_BINDKEY_STR_LEN = 256;

protected:
	virtual cjBool ExecInputCommands(const cjNameChar* cmd);
		
protected:
	cjViewport*						m_viewport;

	cjBYTE							m_isKeyDown[CJIK_MAX];
	cjNameString					m_keyBindStr[CJIK_MAX];

	cjInputAction					m_execInputAction;
	cjFLOAT							m_execInputDelta;
};

#endif

