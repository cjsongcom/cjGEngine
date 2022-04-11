
#include "cjCorePCH.h"
#include "cjWinWnd.h"

cjUINT32 cjWinWnd::ms_numWindow = 0;

cjWinWnd::cjWinWnd(cjWinWnd* parent, cjHINSTANCE hInstance)
	: m_hwnd(NULL), m_parent(parent), m_hInstance(hInstance), m_icon(0), m_iconsm(0), 
	m_isDestroying(cjFalse), m_sendDestroyWnd(cjFalse), m_isUniWnd(cjFalse)
{
	++ms_numWindow;	
}

cjWinWnd::~cjWinWnd()
{
	ConditionalDestroy();
	--ms_numWindow;
}

void cjWinWnd::MoveWindow(cjINT32 left, cjINT32 top, cjINT32 w, cjINT32 h, cjBool repaint)
{
	::MoveWindow(m_hwnd, left, top, w, h, repaint);
}

void cjWinWnd::Show(cjBool show)
{
	::ShowWindow(m_hwnd, show);
}

cjBool cjWinWnd::RegisterWindowClass(const cjChar* /*className*/, cjTFLAG /*style*/, 
	cjHINSTANCE hinstance, cjINT32 /*icon*/, cjINT32 /*iconsm*/)
{
	cjAssert(hinstance);
	cjAssert(!"NotImplemented!");
	return cjFalse;
}

cjBool cjWinWnd::RegisterWindowClass(const cjUni* className, cjTFLAG style,
	cjHINSTANCE hinstance, cjINT32 icon, cjINT32 iconsm)
{
	cjAssert(hinstance);

/*
  http://www.gamedev.net/topic/610133-does-win32-hate-unicode-lpwindowname/

  Does Win32 hate Unicode lpWindowName ?
  You probably have your project set to not use Unicode, which is causing you problems. >> DefWindowProc, 
  for instance, is not a function; it is a macro that expands to DefWindowProcA unless UNICODE is #defined, 
  in which case it expands to DefWindowProcW. Mixing A and W functions is a surefire way to get bizarre problems 
  exactly like what you describe. Make sure UNICODE is defined in your project, preferably by setting the appropriate 
  character set options for your project. Where you can find this depends on the version of Visual Studio you are using. 
*/

	WNDCLASSEXW clsw = {0};
	clsw.cbSize			= sizeof(clsw);
	clsw.style			= style;
	clsw.lpfnWndProc	= (WNDPROC)_WndProcW;
	clsw.hInstance		= (HINSTANCE)hinstance;
	clsw.hIcon			= icon ? (HICON)LoadIconW((HINSTANCE)hinstance, MAKEINTRESOURCEW(icon)) : NULL;
	clsw.lpszClassName	= className;
	clsw.hIconSm		= iconsm ? (HICON)LoadIconW((HINSTANCE)hinstance, MAKEINTRESOURCEW(iconsm)) : NULL;

	if(!RegisterClassExW(&clsw))
	{
		cjLOG("[%s] failed [%08x]", __FUNCTION__, ::GetLastError());
		return cjFalse;
	}

	return cjTrue;
}

cjHINSTANCE cjWinWnd::GetDefaultHInstance()
{
	return (cjHINSTANCE)g_mainExecutableHInstance;
}

cjBool cjWinWnd::CreateWndA(cjTFLAG exStyle, const cjChar* clsName, const cjChar* wndName, 
	cjTFLAG style, cjINT32 x, cjINT32 y, cjINT32 w, cjINT32 h, HWND wndParent, HMENU menu,
	cjHINSTANCE hInstance)
{		
	cjAssert(!m_hwnd);
	cjAssert(hInstance);

	if(!RegisterWindowClass(clsName, CS_DBLCLKS, hInstance))
		return cjFalse;

	m_isUniWnd = cjFalse;
	HWND wnd = CreateWindowExA(exStyle, clsName, wndName, style, x, y, w, h,
		wndParent, menu, (HINSTANCE)hInstance, (void*)this);

	if(!wnd)
	{
		cjAssert(0);
		return cjFalse;
	}

	m_hwnd = wnd;

	return cjTrue;
}

cjBool cjWinWnd::CreateWndW(cjTFLAG exStyle, const cjUni* clsName, const cjUni* wndName, 
	cjTFLAG style, cjINT32 x, cjINT32 y, cjINT32 w, cjINT32 h, HWND wndParent, HMENU menu,
	cjHINSTANCE hInstance)
{	
	cjAssert(!m_hwnd);
	cjAssert(hInstance);

	if(!RegisterWindowClass(clsName, CS_DBLCLKS, hInstance))
		return cjFalse;

	m_isUniWnd = cjTrue;
	HWND wnd = CreateWindowExW(exStyle, clsName, wndName, style, x, y, w, h,
		wndParent, menu, (HINSTANCE)hInstance, (void*)this);

	if(!wnd)
	{
		m_isUniWnd = cjFalse;
		cjAssert(0);
		return cjFalse;
	}

	m_hwnd = wnd;

	return cjTrue;
}

LRESULT CALLBACK cjWinWnd::_WndProcW(HWND hWnd, cjUINT32 msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_CREATE)
	{
		cjWinWnd* prm = (cjWinWnd*)(((CREATESTRUCTW*)lParam)->lpCreateParams);

		::SetWindowLongPtr(hWnd, 
#ifdef CJ_P64
			GWLP_USERDATA, 
#else
			GWL_USERDATA, 
#endif
			(LONG_PTR)prm);
	}

	cjWinWnd* glue = (cjWinWnd*)::GetWindowLongPtr(hWnd, 
#ifdef CJ_P64
		GWLP_USERDATA 
#else
		GWL_USERDATA
#endif
	);

	if(glue)
	{
		LRESULT rst = 0;

		if(msg == WM_DESTROY)
		{
			glue->m_sendDestroyWnd = cjTrue;
			if(!glue->m_isDestroying)
			{
				// generated WM_DESTROY from clicking close button
				glue->ConditionalDestroy();
				delete glue;
			}
		}
		else if(msg == WM_CLOSE)
		{
			if(!glue->OnQueryClosing())
				return 0; // ignore

			// By default, the DefWindowProc function calls 
			// the DestroyWindow function to destroy the window.
		}
		else
		{
			if(glue->WndProc(hWnd, msg, wParam, lParam, &rst))
				return rst;
		}
	}

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

cjBool cjWinWnd::WndProc(HWND , cjUINT32 , WPARAM , LPARAM , LRESULT* /*rst*/)
{
	return cjFalse;
}

cjBool cjWinWnd::IsValid()
{
	if(!m_hwnd || !IsWindow(m_hwnd))
		return cjFalse;

	return cjTrue;
}

void cjWinWnd::GetClientSiz(cjINT32& width, cjINT32& height)
{
	RECT rectClient;
	::GetClientRect(m_hwnd, &rectClient);

	width = rectClient.right;
	height = rectClient.bottom;
}

void cjWinWnd::GetPos(cjINT32& left, cjINT32& top)
{
	RECT rcRect;
	GetWindowRect(m_hwnd, &rcRect);

	left = rcRect.left;
	top = rcRect.top;
}

void cjWinWnd::ConditionalDestroy()
{
	if(!m_isDestroying)
	{
		m_isDestroying = cjTrue;
		OnDestroy();		
	}
}

void cjWinWnd::OnDestroy()
{
	// prevent calling from destroyWindow or WM_DESTROY as twice
	if(m_hwnd)
	{
		::SetWindowLongPtr(m_hwnd,
#ifdef CJ_P64
			GWLP_USERDATA, 
#else
			GWL_USERDATA, 
#endif
		NULL);

		if(!m_sendDestroyWnd)
			::DestroyWindow(m_hwnd);

		m_hwnd = NULL;
	}		
}


//
// cjWinDialog
//

cjWinDailog::cjWinDailog(cjHWND parentWnd, cjWORD winResID, cjHINSTANCE hInstance)
{
	m_parentWnd = parentWnd;
	m_dlgResID = winResID;
	m_moduleInstance = hInstance;

	if(!hInstance)
		m_moduleInstance = (cjHINSTANCE)GetModuleHandle(NULL);


	m_hwnd = NULL;
	m_isDestroying = cjFalse;
	m_sendDestroyWnd = cjFalse;
}

cjWinDailog::~cjWinDailog()
{
	ConditionalDestroy();
}

void cjWinDailog::ConditionalDestroy()
{
	if(!m_isDestroying)
	{
		m_isDestroying = cjTrue;
		OnDestroy();		
	}
}

INT_PTR cjWinDailog::DoModal()
{
	INT_PTR rst = ::DialogBoxParamA((HINSTANCE)m_moduleInstance, MAKEINTRESOURCEA(m_dlgResID),
		(HWND)m_parentWnd, (DLGPROC)&_DlgProc, (LPARAM)this);	

	return rst;
}

INT_PTR CALLBACK cjWinDailog::_DlgProc(cjHWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_INITDIALOG)
	{
		::SetWindowLongPtr((HWND)hwnd, 
#ifdef CJ_P64
			DWLP_USER,
#else
			DWL_USER,
#endif
			(LONG_PTR)lParam);

		return ((cjWinDailog*)lParam)->InitDialog(hwnd) ? TRUE : FALSE;
	}

	cjWinDailog* glue = (cjWinDailog*)GetWindowLongPtr((HWND)hwnd, 
#ifdef CJ_P64
		DWLP_USER 
#else
		DWL_USER
#endif
	);

	if(glue)
	{
		switch(msg)
		{
		case WM_DESTROY:
			{
				glue->m_sendDestroyWnd = cjTrue;
				if(!glue->m_isDestroying)
				{
					// generated WM_DESTROY from clicking close button
					//glue->OnDestroyingByWM_DESTROY();
					glue->ConditionalDestroy();
					// delete glue;
				}
			}
			break;

		case WM_CLOSE:
			{
				if(!glue->OnQueryClosing())
					return TRUE; // ignore
			}
			break;

		case WM_COMMAND:
			{
				if(glue->OnCommand(hwnd, wParam, lParam))
					return TRUE;
			}
			break;

		default:
			{
				if(glue->DlgProc(hwnd, msg, wParam, lParam))
					return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

void cjWinDailog::OnDestroy()
{
	// prevent calling from destroyWindow or WM_DESTROY as twice
	if(m_hwnd)
	{
		::SetWindowLongPtr((HWND)m_hwnd,
#ifdef CJ_P64
			DWLP_USER, 
#else
			DWL_USER, 
#endif
			NULL);

		if(!m_sendDestroyWnd)
		{
			m_hwnd = NULL;
			::EndDialog((HWND)m_hwnd, 0);
		}
		else
			m_hwnd = NULL;
	}	
}

cjBool cjWinDailog::InitDialog(cjHWND hwnd)
{
	m_hwnd = hwnd;
	return cjTrue; // set dialog keyboard focus by system
}

void cjWinDailog::MoveWindow(cjINT32 left, cjINT32 top, cjINT32 w, cjINT32 h, cjBool repaint)
{
	::MoveWindow((HWND)m_hwnd, left, top, w, h, repaint);
}

void cjWinDailog::Show(cjBool show)
{
	::ShowWindow((HWND)m_hwnd, show);
}

void cjWinDailog::GetClientSiz(cjINT32& width, cjINT32& height)
{
	RECT rectClient;
	::GetClientRect((HWND)m_hwnd, &rectClient);

	width = rectClient.right;
	height = rectClient.bottom;
}

void cjWinDailog::GetPos(cjINT32& left, cjINT32& top)
{
	RECT rcRect;
	GetWindowRect((HWND)m_hwnd, &rcRect);

	left = rcRect.left;
	top = rcRect.top;
}