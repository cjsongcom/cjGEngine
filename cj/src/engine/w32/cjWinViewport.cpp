
#include "cjEnginePCH.h"

//#include "cjWinClient.h"
//#include "cjWinDInput.h"
#include "cjWinViewportWnd.h"
#include "cjInput.h"

#include "cjWinViewport.h"
#include "cjCamera.h"


CJOBJ_IMPL_CLASS(cjWinViewport)


void* cjWinViewport::ms_directInput8	= NULL;
void* cjWinViewport::ms_indevMouse		= NULL;
//void* cjWinViewport::ms_indevJoystick	= NULL;


cjWinViewport::cjWinViewport()	: m_wnd(NULL)
{
	m_imeEnable = cjFalse;

	POINT mousePos;
	GetCursorPos(&mousePos);

	m_windowMouseX = mousePos.x;
	m_windowMouseY = mousePos.y;
 
	m_byPassWndProcCnt = 0;
	m_mouseCapturing = cjFalse;

	m_focused = cjFalse;

	//cjtest
	//m_camera = new cjCamera;
	//m_camera->SetCamera(cjVec3(-300,0,0), cjVec3(0,0,0));
	//m_camera->SetFov(CJ_PI/4.0f, 1.3f);
}

cjBool cjWinViewport::Init(const cjNameChar* title, cjINT32 left, cjINT32 top, 
	cjINT32 width, cjINT32 height, void* prm)
{
	if(!cjSuper::Init(title, left, top, width, height, prm))
		return cjFalse;

//	SetName(title);
	if(!m_wnd)
	{
		if(m_useOpenGL)
			m_wnd = new cjWinGLViewportWnd(this);
		else
			m_wnd = new cjWinViewportWnd(this);
	}

	if(!m_wnd->Create(left, top, width, height, NULL, cjWinViewportWnd::DEFAULT_NONCHILD_WND, WS_EX_APPWINDOW, NULL))
//		::GetModuleHandleA("libengineD.dll")))
	{
		delete m_wnd;
		m_wnd = NULL;

		return cjFalse;
	}

	if(m_viewportFlag & EVF_PrimaryView)
		SetPrimaryViewport(this);

	return cjTrue;
}

cjBool cjWinViewport::Init(void* givenWnd, const cjNameChar* title, void* prm)
{	
	// cj ugly
	cjWinViewportWnd* wnd = (cjWinViewportWnd*)givenWnd;

	if(!wnd || m_wnd)
		return cjFalse;

	cjINT32 width, height;
	cjINT32 left, top;

	wnd->GetClientSiz(width, height);
	wnd->GetPos(left, top);

	if(!cjSuper::Init(title, left, top, width, height, prm))
		return cjFalse;

//	SetName(title);

	m_wnd = wnd;

	if(m_viewportFlag & EVF_PrimaryView)
		SetPrimaryViewport(this);

	return cjTrue;
}

void cjWinViewport::Tick()
{
}

void cjWinViewport::UpdateInputData(cjBool reset, cjFLOAT elapsed)
{
//	HRESULT hr;

	// IME
	//if(m_imeEnable)
	//{
	//	HWND imeWnd = (HWND)GetWnd();

	//	HIMC hImc = ImmGetContext(imeWnd);
	//	if(!hImc)
	//	{
	//		hImc = ImmCreateContext();

	//		if(hImc)
	//			ImmAssociateContext(imeWnd, hImc); 
	//		else
	//			m_imeEnable = cjFalse;
	//	}
	//	else
	//		ImmReleaseContext(imeWnd, hImc);
	//}

//	if(reset)
//	{
//		// reset keyboard state
//		BYTE keyStates[CJIK_MAX];
//		cjMemSet(keyStates, 0, sizeof(keyStates));
//
//		keyStates[CJIK_NumLock]		= (BYTE)GetKeyState(CJIK_NumLock);
//		keyStates[CJIK_CapsLock]	= (BYTE)GetKeyState(CJIK_CapsLock);
//		keyStates[CJIK_ScrollLock]	= (BYTE)GetKeyState(CJIK_ScrollLock);
//        keyStates[CJIK_Ctrl]		= (BYTE)GetKeyState(CJIK_Ctrl);
//        keyStates[CJIK_Shift]		= (BYTE)GetKeyState(CJIK_Shift);
//        keyStates[CJIK_Alt]			= (BYTE)GetKeyState(CJIK_Alt);
//
//		SetKeyboardState(keyStates);
//	}
//
//
//	BYTE updatedKey[CJIK_MAX] = {0};
//
//	//
//	// DirectInput mouse
//	//
//
//	cjFLOAT mouseXMultiplier = 1.0f;
//	cjFLOAT mouseYMultiplier = 1.0f;
//
//	DWORD remainElements = 1;
//	while(remainElements && ms_indevMouse)
//	{
//		LPDIRECTINPUTDEVICE8 devMouse = (LPDIRECTINPUTDEVICE8)ms_indevMouse;
//		DIDEVICEOBJECTDATA devEvent;
//		
//
//		if(FAILED(hr=devMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
//			&devEvent, &remainElements, 0)))
//		{
//			// immediately exit
//			if(m_focused)
//				AcquireInputDevice(1); // mouse
//
//			remainElements = 0;
//		}
//		else if(remainElements)
//		{
//			switch(devEvent.dwOfs)
//			{								
//			case DIMOFS_X: 
//				GenerateInputEvent(CJIK_MouseX, CJIA_Axis, +mouseXMultiplier * ((cjINT32)devEvent.dwData) ); break;
//
//			case DIMOFS_Y: 
//				GenerateInputEvent(CJIK_MouseY, CJIA_Axis, -mouseYMultiplier * ((cjINT32)devEvent.dwData) ); break;
//
//			case DIMOFS_Z: // MouseWheel -caution! devEvent.dwData can be abnormal value
//
//				//cj
//#ifdef CJDEBUG
//					if(devEvent.dwData == 0xffffffff)
//					{
//						__asm int 3;
//					}
//#endif
//
//				GenerateInputEvent(CJIK_MouseW, CJIA_Axis, (cjINT32)devEvent.dwData);
//
//				if(((cjINT32)devEvent.dwData) < 0)
//				{
//					GenerateInputEvent( CJIK_MouseWheelDown, CJIA_Press );
//					GenerateInputEvent( CJIK_MouseWheelDown, CJIA_Release );
//				}
//				else if(((cjINT32)devEvent.dwData) > 0)
//				{
//					GenerateInputEvent( CJIK_MouseWheelUp, CJIA_Press );
//					GenerateInputEvent( CJIK_MouseWheelUp, CJIA_Release );
//				}
//				break;
//
//
//			// left
//            case DIMOFS_BUTTON0:
//				if(devEvent.dwData & 0x80) 
//					GenerateInputEvent( CJIK_LeftMouse, CJIA_Press );
//				else
//					GenerateInputEvent( CJIK_LeftMouse, CJIA_Release );
//				break;
//
//			// right
//			case DIMOFS_BUTTON1:
//				if(devEvent.dwData & 0x80) 
//					GenerateInputEvent( CJIK_RightMouse, CJIA_Press );
//				else
//					GenerateInputEvent( CJIK_RightMouse, CJIA_Release );
//				break;
//
//			// middle
//			case DIMOFS_BUTTON2:
//				if(devEvent.dwData & 0x80) 
//					GenerateInputEvent( CJIK_MiddleMouse, CJIA_Press );
//				else
//					GenerateInputEvent( CJIK_MiddleMouse, CJIA_Release );
//				break;
//			default:
//                break;        
//			}
//		}
//	}
//
//	updatedKey[CJIK_LeftMouse]		= 1;
//	updatedKey[CJIK_RightMouse]		= 1;
//	updatedKey[CJIK_MiddleMouse]	= 1;
//
//
//	//
//	// Keyboard.
//	//
//	if(reset) // reset only on focused 
//		reset = GetFocus() == GetWnd();
//
//	for(cjINT32 i=0; i<CJIK_MAX; i++)
//	{
//		if(!updatedKey[i])
//		{
//			if(!m_input->IsKeyDown(i))
//			{
// 				if(reset && (GetKeyState(i) & 0x8000))
//					GenerateInputEvent(i, CJIA_Press);
//			}
//			else
//			{
//				if(!(GetKeyState(i) & 0x8000))
//					GenerateInputEvent(i, CJIA_Release);
//			}
//		}
//	}
}

cjBool cjWinViewport::GenerateInputEvent(cjINT32 key, cjInputAction action, 
	cjFLOAT delta)
{
	return GetClient()->GetEngine()->GenerateInputEvent(this, (cjInputKey)key,
		action, delta);
}

void cjWinViewport::UpdateMousePosition()
{
	POINT pt;

	// foViewport
	::GetCursorPos( &pt );
	m_mouseScreenPos = cjPoint<cjINT32>(pt.x, pt.y);

	::ScreenToClient((HWND)GetWnd(), &pt);
	m_mouseClientPos = cjPoint<cjINT32>(pt.x, pt.y);

	// cjWinViewport
	m_windowMouseX = pt.x; m_windowMouseY = pt.y;
}

void cjWinViewport::SetMouseCapture(cjBool doCapture, cjBool clipMouseArea, 
	cjBool focusedOnly)
{
	if(doCapture) clipMouseArea = cjTrue;

	// if not focused, return
	if(clipMouseArea && focusedOnly && GetFocus() != (HWND)GetWnd())
		return;

	HWND targetWnd = (HWND)GetWnd();

	RECT clientRectOnScreen;
	::GetClientRect(targetWnd, &clientRectOnScreen);
	MapWindowPoints(targetWnd, NULL, (POINT*)&clientRectOnScreen, 2);

	if(doCapture)
	{
		HWND foregroundWnd = ::GetForegroundWindow(); // get topmost
		cjBool doAttach = (foregroundWnd == targetWnd);

		if(doAttach)
		{
			AttachThreadInput(GetWindowThreadProcessId(foregroundWnd, NULL), 
				GetCurrentThreadId(), true); // attach thread input
		}

		// chage viewport to foreground window
		SetForegroundWindow(targetWnd);
		SetActiveWindow(targetWnd);
			
		if(doAttach)
		{
			AttachThreadInput(GetWindowThreadProcessId(foregroundWnd, NULL), 
				GetCurrentThreadId(), false); // detach thread input
		}

		// center to mouse
		SetCursorPos( (clientRectOnScreen.left+clientRectOnScreen.right)/2,
			(clientRectOnScreen.top + clientRectOnScreen.bottom)/2 );
		

		SetCapture(targetWnd);
		while(ShowCursor(FALSE)>=0);
	}
	else
	{
		// stop capturing
		if(!IsFullScreen())	SetCapture(NULL);

		while(ShowCursor(TRUE)<0);
	}

	// set workable mouse area
	ClipCursor(clipMouseArea ? &clientRectOnScreen : NULL);

	m_mouseCapturing = doCapture;
}

void cjWinViewport::Paint()
{
	GetClient()->GetEngine()->Draw(this);
}

cjBool cjWinViewport::RenderLock()
{
	cjBool rst = cjTrue;

	if(!m_wnd || !m_wnd->IsValid())
		return cjFalse;	
	
	rst = cjSuper::RenderLock();

	if(!rst)
		return cjFalse;

	return cjTrue;
}

void cjWinViewport::RenderUnLock()
{
	cjSuper::RenderUnLock();
}

void cjWinViewport::Present()
{
	cjSuper::Present();
}

cjBool cjWinViewport::ViewportWndProc(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, LPARAM lParam, LRESULT* rst)
{
	if(m_byPassWndProcCnt)
		return cjFalse;

	if(rst) *rst = 0;

	switch(uiMsg)
	{
	case WM_CREATE:
		{
			SetFocus(hWnd);
			return cjFalse;
		}

//	case WM_DESTROY: // use onWndClose() instead
//		{
//		}

	case WM_PAINT:
		{
		//	m_dirtyViewport = cjTrue;
			ValidateRect(hWnd, NULL);

			return cjFalse;
		}


	//
	// key down/up, char
	//
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			cjInputKey key = (cjInputKey)wParam;

			// alt(menu up)-enter
			if(key==CJIK_Enter && (GetKeyState(VK_MENU)&0x8000))
			{
				ToggleFullScreen();
			}
			else if( (key>=0) && (key<CJIK_MAX) && GenerateInputEvent(key, CJIA_Press) )
				Paint();

			return cjFalse;
		}

	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			cjInputKey key = (cjInputKey)wParam;
			if((key>=0) && (key<CJIK_MAX) && GenerateInputEvent(key, CJIA_Release))
				Paint();

			if (m_mouseCapturing == m_mouseShow)
				SetMouseCapture(!m_mouseShow, 0, 1); // no clip mouse moving

			return cjFalse;
		}

	case WM_SYSCHAR:
	case WM_CHAR:
		{
			cjInputKey key = (cjInputKey)wParam;

						// char key only
			if(key == CJIK_Enter || !GetClient()->GetEngine()->OnCharKey(this, key))
			{
				if(uiMsg == WM_SYSCHAR)
					return cjFalse;
			}

			return cjFalse;
		}

	case WM_ERASEBKGND:
		{
			return cjTrue; // skip DefWindowProc
		}

	case WM_ACTIVATE:
		{
			if(wParam == WA_INACTIVE) 
				SetDrag(0);

			return cjFalse;
		}


	//
	// mouse
	//
	case WM_MOUSEACTIVATE:
		{
			// Sent when the cursor is in an inactive window and 
			// the user presses a mouse button. 
			// The parent window receives this message only if 
			// the child window passes it to the DefWindowProc function.
			if( !m_mouseCapturing && !m_mouseShow )
			{
				POINT pt; ::GetCursorPos( &pt );
				::ScreenToClient((HWND)GetWnd(), &pt);

				if(pt.y < 0) // pt is in client area
					m_mouseDragging = cjTrue;
			}

			(*rst) = MA_ACTIVATE;
			
			return cjTrue;
		}

	case WM_LBUTTONDBLCLK:
		{
			return cjFalse;
		}

	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		{			
			return cjFalse;
		}

	case WM_LBUTTONUP:	case WM_RBUTTONUP:	case WM_MBUTTONUP:
		{
			if(m_mouseDragging) 
				m_mouseDragging = 0;

			if(m_mouseCapturing == m_mouseShow)
				SetMouseCapture(!m_mouseShow, cjFalse, cjTrue);

			//cjdefer
			//if(GetClient()->m_inMenuLoop)
			//	return cjFalse;

			POINT clientAreaTopLeftOnScreen = {0,0};	
			::ClientToScreen((HWND)GetWnd(), &clientAreaTopLeftOnScreen);

			cjInputKey key = CJIK_None;

			switch(uiMsg)
			{
			case WM_LBUTTONUP: key = CJIK_LeftMouse; break;
			case WM_RBUTTONUP: key = CJIK_RightMouse; break;
			case WM_MBUTTONUP: key = CJIK_MiddleMouse; break;
			default: break;
			}

			GenerateInputEvent(key, CJIA_Release);
			
			// cancel dragging, if no mouse button is pressed
			if(	!m_input->IsKeyDown(CJIK_LeftMouse) 
				&& !m_input->IsKeyDown(CJIK_MiddleMouse)
				&& !m_input->IsKeyDown(CJIK_RightMouse))
			{
                SetDrag(0);
			}

			return cjFalse;
		}

		case WM_MOUSEWHEEL:
		{		
			// short zDelta = HIWORD(wParam);

			// this will be handled UpdateInputData
			//GenerateInputEvent(CJIK_MouseW, CJIA_Axis, zDelta);

			//if(zDelta < 0)
			//{
			//	GenerateInputEvent(CJIK_MouseWheelDown, CJIA_Press);
			//	GenerateInputEvent(CJIK_MouseWheelDown, CJIA_Release);
			//}
			//else if( zDelta > 0 )
			//{
			//	GenerateInputEvent(CJIK_MouseWheelUp, CJIA_Press);
			//	GenerateInputEvent(CJIK_MouseWheelUp, CJIA_Release);
			//}

			//GetClient()->GetEngine()->OnMouseWheel(this, wParam, zDelta);
			
			return cjFalse;
		}

		case WM_MOUSEMOVE:
		{
			UpdateMousePosition();
			return cjFalse;
		}


		//
		// Menu Loop
		//

		//case WM_ENTERMENULOOP:
		//{
		//	GetWinClient()->m_inMenuLoop = 1;
		//	SetDrag( 0 );

		//	UnAcquireInputDevice(1); // mouse

		//	return cjFalse;
		//}

		//case WM_EXITMENULOOP:
		//{
		//	GetWinClient()->m_inMenuLoop = 0;
		//	AcquireInputDevice(1); // mouse

		//	return cjFalse;
		//}

		case WM_CANCELMODE:  // Cancel mouse dragging, scrollbar input
		{
			SetDrag( 0 );

			return cjFalse;
		}


		//
		// Sizing
		//
		case WM_SIZE:
		{
			//cjINT32 newXSiz = LOWORD(lParam);
			//cjINT32 newYSiz = HIWORD(lParam);

			//cjBool minimized = (wParam == SIZE_MINIMIZED); 
			//cjBool restored  = (wParam == SIZE_RESTORED);

			//if(wParam == SIZE_MAXIMIZED)
			//{
			//	// do capture, clip mouse workable area, focused
			//	SetMouseCapture(cjTrue, cjTrue, cjTrue);
			//}

			//if(minimized)
			//	SetMouseCapture(0,0,0); // stop capture

			//if(IsFullScreen())
			//{
			//	if(restored)
			//	{
			//		m_byPassWndProcCnt++; // skip reentrant to this func
			//		GetWnd()->MoveWindow(m_savedWindowRect, cjTrue);
			//		m_byPassWndProcCnt--;
			//	}

			//	return cjFalse;
			//}
			//else
			//{
			//	// windowed mode
			//	if(m_renDev)
			//		m_renDev->SetResolution(this, newXSiz, newYSiz, IsFullScreen());

			//	return cjFalse;
   //     	}
			return cjFalse;
		}


		//
		// set/kill focus
		//
		case WM_SETFOCUS:
		{
			OnFocusEvent(1);

			if((m_mouseCapturing == m_mouseShow) && !m_mouseDragging)
				SetMouseCapture(!m_mouseShow, 0, 1);

			if(m_input)
				m_input->ResetInput();			

			// Acquire devices.
			AcquireInputDevice(1); // mouse

			

			// Disable IME (input method editor) for Japanese Windows
			// ImmAssociateContext((HWND)GetWnd(), NULL );
 
			return cjFalse;
		}

		case WM_KILLFOCUS:
		{
			if(IsFullScreen())
				EndFullScreen();

			SetMouseCapture(cjFalse, cjFalse, cjFalse);
			SetDrag(cjFalse);

			if(m_input)
				m_input->ResetInput();

			UnAcquireInputDevice(1); //  mouse

			OnFocusEvent(0);

			return cjFalse;
		}


		//
		// SYSCommand
		//
		case WM_SYSCOMMAND:
		{
			DWORD nID = wParam & 0xFFF0;

			if( nID==SC_SCREENSAVE || nID==SC_MONITORPOWER )
				return 0;
			else if( nID==SC_MAXIMIZE )
			{
				// Maximize.
				ToggleFullScreen();
				return cjFalse;
			}
			else if( IsFullScreen() && (nID==SC_NEXTWINDOW || nID==SC_PREVWINDOW || nID==SC_TASKLIST || nID==SC_HOTKEY) )
			{
				return cjTrue; // skip processing wndProcedure
			}
			else
				return cjFalse;
		}

	} // end of switch

	return cjFalse;
}

void* cjWinViewport::GetWnd()
{
	if(m_wnd)
		return (void*)((HWND)*m_wnd);
	else
		return NULL;
}

void cjWinViewport::OnDestroy()
{
	if(m_wnd)
	{
		delete m_wnd;
		m_wnd = NULL;
	}

	if(GetClient())
		GetClient()->OnDestroyViewport(this);

	cjSuper::OnDestroy();
}

cjBool cjWinViewport::AttachRenderDevice(cjRenDev* renDev)
{
	cjBool rst = cjSuper::AttachRenderDevice(renDev);

	if(!rst)
		return cjFalse;

	cjBool fullScreen = cjFalse;
	cjINT32 colorBPP = 32;

	cjINT32 width = GetWidth();
	cjINT32 height = GetHeight();

	if(!fullScreen)
		GetClientSiz(width, height);

	if(!renDev->SetResolution(this, width, height, fullScreen, colorBPP))
	{
		cjLOG(_CT("Failed on SetResolution.."));
		return cjFalse;
	}

	m_renDev = renDev;

	return cjTrue;
}

void cjWinViewport::DetachRenderDevice()
{
	if(!m_renDev)
		return;

	m_renDev->UnSetResolution(this);
	m_renDev = NULL;
}

void cjWinViewport::GetClientSiz(cjINT32& width, cjINT32& height)
{
	if(m_wnd)
	{
		m_wnd->GetClientSiz(width, height);
	}
	else
	{
		width = 0;
		height = 0;
	}
}

cjBool cjWinViewport::OnQueryWndClosing()
{
	return cjTrue;
}

// called from  cjWinViewport::OnDestroy or cjWinViewportWnd::OnDestroy
void cjWinViewport::OnWndClose()
{
	if(m_wnd)
	{
		SetDrag(cjFalse);
	
		if(m_renDev && m_renDev->IsFullScreen())
			EndFullScreen();

		DetachRenderDevice();
		m_wnd = NULL;
	}

	if(!(cjObjGetFlag() & cjOF_Destroyed)) // already started destroying
		delete this;
}

void cjWinViewport::EndFullScreen()
{
	if(m_renDev)
	{
//		m_renDev->SetRes(this, GetClient()->m_windowedViewportX, GetClient()->m_windowedViewportX, 0);
	}

	if(m_input)
		m_input->ResetInput();
}

void cjWinViewport::ToggleFullScreen()
{
	//if( !ToggleFullscreenMutex )
	//{
	//	ToggleFullscreenMutex = 1;

 //   	if( BlitFlags & BLIT_Fullscreen )
 //   	{
	//		EndFullscreen();

	//		if ( GIsEditor )
	//			SetMouseCapture(1,1,1);
	//		else if ( Captured == bShowWindowsMouse )
	//			SetMouseCapture(!bShowWindowsMouse,0,1);
	//	}
	//	else if( !(Actor->ShowFlags & SHOW_ChildWindow) )
	//	{
	//		debugf(TEXT("AttemptFullscreen"));
	//		TryRenderDevice( TEXT("ini:Engine.Engine.RenderDevice"), INDEX_NONE, INDEX_NONE, 1 );
	//	}

	//	ToggleFullscreenMutex = 0;
	//}
}

cjBool cjWinViewport::AcquireInputDevice(cjUINT32 type)
{
	//if(type == 1 && ms_indevMouse)
	//	return SUCCEEDED( ((LPDIRECTINPUTDEVICE8)(ms_indevMouse))->Acquire() ) ? cjTrue : cjFalse;

	return cjFalse;
}

cjBool cjWinViewport::UnAcquireInputDevice(cjUINT32 type)
{
	//if(type == 1 && ms_indevMouse)
	//	return SUCCEEDED( ((LPDIRECTINPUTDEVICE8)(ms_indevMouse))->Unacquire() ) ? cjTrue : cjFalse;

	return cjFalse;
}

void cjWinViewport::ToggleMouseShow()
{
	m_mouseShow = m_mouseShow ? cjFalse : cjTrue;

	if(GetForegroundWindow() == (HWND)GetWnd())
		while(::ShowCursor(m_mouseShow ? TRUE : FALSE));
}

cjBool cjWinViewport::Exec(const cjNameChar* cmd)
{
	if(!cmd)
		return cjFalse;

	//if(cjParseCommand(&cmd, _CT("ResetCamera")))
	//{
	//	m_camera->RotateByAxis(0,0,0, cjFalse);
	//	m_camera->SetEyePos(cjVec3(-300,0,0));
	//}
	//else if(cjParseCommand(&cmd, _CT("ResetTestCam")))
	//{
	//	m_camera->RotateByAxis(0,0,0, cjFalse);
	//	m_camera->SetEyePos(cjVec3(-300,0,0));
	//}
	
	return cjTrue;
}

//void cjWinViewport::DoCameraDist(cjFLOAT distDelta)
//{
//}

void cjWinViewport::OnFocusEvent(cjBool focused)
{
	if(focused)
		m_focused = cjTrue;
	else
		m_focused = cjFalse;
}

cjBool cjWinViewport::ResizeViewport(cjBool fullscreen, cjINT32 width, cjINT32 height)
{
	cjClient* client = GetClient();

	if(fullscreen)
	{
		width = client->GetFullscreenViewportWidth();
		height = client->GetFullscreenViewportHeight();
	}
	else
	{
		cjAlign(width, 2);
		cjAlign(height, 2);
	}

	if(fullscreen)
	{
		SetFocus((HWND)GetWnd());
		SetDrag(cjFalse);
	}

	// fullscreen to windowed
	if(!fullscreen && m_wasFullscreen && IsMouseCapture())
	{
		SetMouseCapture(cjFalse, cjFalse, cjFalse);
	}

	m_wasFullscreen = fullscreen;
	m_width = width;
	m_height = height;

	if(fullscreen)
	{	
		SetDrag(cjTrue);
		SetMouseCapture(cjTrue, cjTrue, cjFalse);
	}

	if(fullscreen)
	{
		if(width && height)
		{
			client->m_fullScreenW = width;
			client->m_fullScreenH = height;
		}
	}
	else
	{
		if(width && height)
		{
			client->m_windowedViewportW = width;
			client->m_windowedViewportH = height;
		}
	}

	return cjTrue;
}

void cjWinViewport::SetPendingFrame(cjBool pending)
{

}
