
#ifndef _CJWINVIEWPORT_H_
#define _CJWINVIEWPORT_H_

#include "cjViewport.h"

class cjWinViewportWnd;
class cjCamera;


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjWinViewport, cjViewport, cjClsFL_Transient, cjEngine)
	friend class cjWinClient;
	friend class cjWinViewportWnd;
	friend class cjGLRenInterface;

public:
	cjWinViewport();

	virtual cjBool Init(const cjNameChar* title, cjINT32 left, cjINT32 top, 
		cjINT32 width, cjINT32 height, void* prm=NULL);
	virtual cjBool Init(void* givenWnd, const cjNameChar* title, void* prm);

	virtual void Tick();
	virtual void Paint();

	// input
	virtual void UpdateInputData(cjBool reset, cjFLOAT elapsed=0.0f);
	cjBool GenerateInputEvent(cjINT32 key, cjInputAction action, cjFLOAT delta=0.0f);
	virtual void UpdateMousePosition();

	virtual cjBool IsMouseCapture() { return m_mouseCapturing; }
	virtual void SetMouseCapture(cjBool doCapture, cjBool clipMouseArea=cjTrue, 
		cjBool focusedOnly=cjFalse);
	virtual void ToggleMouseShow();

	virtual cjBool ResizeViewport(cjBool fullscreen, cjINT32 width, cjINT32 height);
	virtual void SetPendingFrame(cjBool pending);

	virtual cjBool Exec(const cjNameChar* cmd);

	// type 1: mouse
	cjBool AcquireInputDevice(cjUINT32 type);
	cjBool UnAcquireInputDevice(cjUINT32 type);

	// rendering
	virtual cjBool RenderLock();
	virtual void RenderUnLock();

	virtual void Present();

	virtual cjBool IsFullScreen()  { return cjFalse; }

	virtual cjHWND GetWnd();

	virtual cjBool ViewportWndProc(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, 
		LPARAM lParam, LRESULT* rst);

	virtual cjBool AttachRenderDevice(cjRenDev* renDev);
	virtual void DetachRenderDevice();

	virtual void GetClientSiz(cjINT32& width, cjINT32& height);

	virtual cjBool OnQueryWndClosing();

	void EndFullScreen();
	void ToggleFullScreen();

	inline cjWinClient* GetWinClient() { return (cjWinClient*)m_client; }

	// cjtemp test
	//void DoCameraRotateDelta(cjFLOAT pitchDelta=0.0f, cjFLOAT yawDelta=0.0f, cjFLOAT rollDelta=0.0f);
	//void DoCameraMoveDelta(cjFLOAT viewDelta=0.0f, cjFLOAT rightDelta=0.0f, cjFLOAT upDelta=0.0f);
	//void DoCameraDist(cjFLOAT distDelta);

protected:
	virtual void OnDestroy();
	virtual void OnWndClose();

	virtual void OnFocusEvent(cjBool focused);

protected:
	cjWinViewportWnd*						m_wnd;

	cjBool									m_imeEnable;

	cjINT32									m_windowMouseX;
	cjINT32									m_windowMouseY;

	cjINT32									m_byPassWndProcCnt;
	cjBool									m_mouseCapturing;

	cjBool									m_focused;
	
	// shared stuff between viewports
	// managed by fnWinClient
	static void*							ms_directInput8;	// LPDIRECTINPUT8
		// created from ms_directInput8
	static void* 							ms_indevMouse;		// LPDIRECTINPUTDEVICE8
	//static void* 							ms_indevJoystick;	// LPDIRECTINPUTDEVICE8

//	class cjRenderScreen*					m_renderScreen;

CJOBJ_DECL_CLASS_END(cjWinViewport)


#endif
