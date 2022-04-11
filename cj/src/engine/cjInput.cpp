
#include "cjEnginePCH.h"
#include "cjInput.h"

#include "cjViewport.h"


cjInput::cjInput()
{
	m_viewport = NULL;

	cjMemSet(m_isKeyDown, 0, sizeof(m_isKeyDown));

	m_execInputAction = CJIA_None;
	m_execInputDelta = 0.0f;

	m_keyBindStr[CJIK_W] = _CU("keyForward");
	m_keyBindStr[CJIK_S] = _CU("keyBackward");
	m_keyBindStr[CJIK_A] = _CU("keyLeft");
	m_keyBindStr[CJIK_D] = _CU("keyRight");

	m_keyBindStr[CJIK_MouseX] = _CU("mouseMov Axis=x Multiplier=1.0");
	m_keyBindStr[CJIK_MouseY] = _CU("mouseMov Axis=y Multiplier=1.0");
	m_keyBindStr[CJIK_MouseW] = _CU("mouseWheel");
}

cjInput::~cjInput()
{

}

void cjInput::Init(cjViewport* viewport)
{
	m_viewport = viewport;
	ResetInput();
}

cjBool cjInput::Exec(const cjNameChar* cmd)
{
	cjNameChar tmp[MAX_BINDKEY_STR_LEN];

	//cjWinViewport* fwv = (cjWinViewport*)m_viewport;


	//if(fnParseCommand(&cmd, _CT("mouseMov")))
	//{
	//	fnTChar axisName[2];

	//	if(m_viewport->IsMouseCapture())
	//	{
	//		// Mouse moving
	//		if(GetInputActionForExec() == FNIA_Axis)
	//		{
	//			if(fnParse(cmd, _CT("Axis="), axisName, 2))
	//			{
	//				INT isX = 0;

	//				if(axisName[0] == _CT('x'))
	//					isX = 1;

	//				cjFLOAT pitchDelta = 0.0f;
	//				cjFLOAT yawDelta = 0.0f;

	//				if(isX)
	//					yawDelta = GetInputDeltaForExec() * 0.01;
	//				else
	//					pitchDelta = GetInputDeltaForExec() * 0.01;

	//				fwv->DoCameraRotateDelta(pitchDelta, -yawDelta, 0);
	//			}
	//		}
	//	}
	//}
	//else if(fnParseCommand(&cmd, _CT("mouseWheel")))
	//{
	//	if(m_viewport->IsMouseCapture())
	//	{
	//		cjFLOAT wheel = GetInputDeltaForExec();			
	//		fwv->DoCameraMoveDelta(wheel, 0, 0);
	//	}
	//}
	//else if(fnParseCommand(&cmd, _CT("keyForward")))
	//{
	//	fwv->DoCameraMoveDelta(0.1f);
	//}
	//else if(fnParseCommand(&cmd, _CT("keyBackward")))
	//{
	//	fwv->DoCameraMoveDelta(-0.1f);
	//}
	//else if(fnParseCommand(&cmd, _CT("keyLeft")))
	//{
	//	fwv->DoCameraMoveDelta(0.0f,  0.1f);
	//}
	//else if(fnParseCommand(&cmd, _CT("keyRight")))
	//{
	//	fwv->DoCameraMoveDelta(0.0f, -0.1f);
	//}

	return cjFalse;
}

void cjInput::ResetInput()
{
	cjMemSet(m_isKeyDown, 0, sizeof(m_isKeyDown));

	cjBool doReset = cjTrue;
	m_viewport->UpdateInputData(doReset);
}

void cjInput::DispatchInput(cjFLOAT elapsed)
{
	//if(cjGIsRunning)
	//{
	//	if(elapsed != -1.f)
	//	{
	//		for(INT i=0; i<CJIK_MAX; i++)
	//		{				
	//			if(m_isKeyDown[i]) // pressed key only
	//				ProcessInputEvent((cjInputKey)i, CJIA_Hold, elapsed);
	//		}
	//	}
	//}
}

cjBool cjInput::UpdateInputEventState(cjInputKey key, cjInputAction action, cjFLOAT /*delta*/)
{
	switch(action)
	{
	case CJIA_Press:
		{
			if(m_isKeyDown[key]) 
				return cjFalse; // already pressed

			m_isKeyDown[key] = 1;
		}
		break;

	case CJIA_Release:
		{
			if(!m_isKeyDown[key])
				return cjFalse; // already released

			m_isKeyDown[key] = 0;
		}
		break;

	default:
		break;
	}

	return cjTrue;
}

cjBool cjInput::ProcessInputEvent(cjInputKey key, cjInputAction action, cjFLOAT delta)
{
	//if(m_keyBindStr[key].Len())
	//{
	//	SetInputParamForExec(action, delta);
	//	ExecInputCommands(*m_keyBindStr[key]);
	//	SetInputParamForExec(CJIA_None);

	//	return cjTrue;
	//}

	return cjFalse;
}

cjBool cjInput::ExecInputCommands(const cjNameChar* cmd)
{
	if(!cmd)
		return cjFalse;

	//// parse key binded string and exec
	//cjNameChar curLine[MAX_BINDKEY_STR_LEN];

	//while(cjParseLine(&cmd, curLine, cjARRAY_CNT(curLine)))
	//{
	//	const cjNameChar* toParse = curLine;
	//	
	//	if(m_execInputAction== CJIA_Press 
	//		|| (m_execInputAction == CJIA_Release && cjParseCommand(&toParse, _CT("OnRelease"))) )
	//	{
	//		m_viewport->Exec(toParse);
	//	}
	//	else
	//	{
	//		Exec(toParse);
	//	}
	//}

	return cjFalse;	
}
