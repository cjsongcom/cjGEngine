
#ifndef _CJWINVIEWPORTWND_H_
#define _CJWINVIEWPORTWND_H_ 

#include "cjWinWnd.h"

class cjWinViewport;

class CJENGINE_API cjWinViewportWnd : public cjWinWnd
{
public:
	cjWinViewportWnd(cjWinViewport* ownerViewport=NULL);
	virtual ~cjWinViewportWnd();

public:
	const static cjChar* CLASS_NAMEA;
	const static cjNameChar* CLASS_NAME;

	const static cjWinDWORD DEFAULT_NONCHILD_WND 
		= WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
	const static cjWinDWORD DEFAULT_CHILD_WND 
		= WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;

	virtual cjBool Create(cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, 
		HWND parentWnd = NULL, cjWinDWORD style = DEFAULT_NONCHILD_WND,
		cjWinDWORD exStyle = WS_EX_APPWINDOW, HMENU menu = NULL,
		cjHINSTANCE hInst = GetDefaultHInstance(), const cjNameChar* className = CLASS_NAME);

	inline void SetOwnerViewport(cjWinViewport* viewport) 
		{ m_ownerViewport = viewport; }

protected:
	virtual cjBool OnQueryClosing();

	virtual cjBool WndProc(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, LPARAM lParam, LRESULT* rst);
	virtual void OnDestroy();
	
protected:
	cjWinViewport*					m_ownerViewport;

};


class CJENGINE_API cjWinGLViewportWnd : public cjWinViewportWnd
{
public:
	cjWinGLViewportWnd(cjWinViewport* ownerViewport=NULL);
	virtual ~cjWinGLViewportWnd();

public:
	const static cjChar* CLASS_NAMEA;
	const static cjNameChar* CLASS_NAME;

	const static cjWinDWORD DEFAULT_NONCHILD_WND 
		= WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME 
		 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; // for opengl

	const static cjWinDWORD DEFAULT_CHILD_WND 
		= WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;

	virtual cjBool Create(cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, 
		HWND parentWnd = NULL, cjWinDWORD style = DEFAULT_NONCHILD_WND,
		cjWinDWORD exStyle = WS_EX_APPWINDOW, HMENU menu = NULL,
		HINSTANCE hInst = NULL, const cjNameChar* className = CLASS_NAME);
};


#endif