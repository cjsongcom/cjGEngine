
#include "cjEnginePCH.h"

#include "cjWinViewport.h"
#include "cjWinViewportWnd.h"



//
// cjWinViewportWnd
//

const cjChar* cjWinViewportWnd::CLASS_NAMEA = "cjWinViewportWndClass";
const cjNameChar* cjWinViewportWnd::CLASS_NAME = _CU("cjWinViewportWndClass");


cjWinViewportWnd::cjWinViewportWnd(cjWinViewport* ownerViewport)
	: m_ownerViewport(ownerViewport)
{	
}

cjWinViewportWnd::~cjWinViewportWnd()
{
	ConditionalDestroy();
}

cjBool cjWinViewportWnd::Create(cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, 
	HWND parentWnd, cjWinDWORD style, cjWinDWORD exStyle, HMENU menu, cjHINSTANCE hInst, 
	const cjNameChar* className)
{
	if(!m_hwnd)
	{
		#if cjNameCharIsUnicode
			cjBool rst = __super::CreateWndW(exStyle, className, 
				*m_ownerViewport->GetName(), style, left, top, width, height, 
					parentWnd, menu, hInst);
		#else
			cjBool rst = __super::CreateWndA(exStyle, className, 
				*m_ownerViewport->GetName(), style, left, top, width, height, 
					parentWnd, menu, hInst);
		#endif

		return rst;
	}
	else
	{
		// resize existing window
		SetWindowPos(m_hwnd, NULL, left, top, width, height, SWP_NOACTIVATE);
	}

	return cjTrue;
}

cjBool cjWinViewportWnd::WndProc(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, LPARAM lParam, LRESULT* rst)
{
	if(m_ownerViewport)
		return m_ownerViewport->ViewportWndProc(hWnd, uiMsg, wParam, lParam, rst);
	else
		return cjFalse;
}

cjBool cjWinViewportWnd::OnQueryClosing()
{
	if(m_ownerViewport)
		return m_ownerViewport->OnQueryWndClosing();

	return cjTrue;
}

void cjWinViewportWnd::OnDestroy()
{
	if(m_ownerViewport)
	{
		m_ownerViewport->OnWndClose();
		m_ownerViewport = NULL;
	}

	cjWinWnd::OnDestroy();
}


//
// cjWinGLViewportWnd
//

const cjChar* cjWinGLViewportWnd::CLASS_NAMEA = "cjWinGLViewportWndClass";
const cjNameChar* cjWinGLViewportWnd::CLASS_NAME = _CU("cjWinGLViewportWndClass");


cjWinGLViewportWnd::cjWinGLViewportWnd(cjWinViewport* ownerViewport)
	: cjWinViewportWnd(ownerViewport)
{
}

cjWinGLViewportWnd::~cjWinGLViewportWnd()
{
	ConditionalDestroy();
}

cjBool cjWinGLViewportWnd::Create(cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, 
	HWND parentWnd, cjWinDWORD style, cjWinDWORD exStyle, HMENU menu, HINSTANCE hInst, 
	const cjNameChar* className)
{
	return cjWinViewportWnd::Create(left, top, width, height, parentWnd, style | CS_OWNDC, 
		exStyle, menu, hInst, className);
}
