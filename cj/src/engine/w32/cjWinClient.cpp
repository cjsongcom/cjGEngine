
#include "cjEnginePCH.h"

// win stuff
#include "ObjBase.h"

//#include "cjWinDInput.h"
#include "cjWinViewport.h"
#include "cjWinClient.h"


CJOBJ_IMPL_CLASS(cjWinClient)


cjWinClient::cjWinClient()
{
	m_inMenuLoop = cjFalse;
}

cjBool cjWinClient::Init(cjEngine* engine)
{
	cjSuper::Init(engine);

	CoInitialize(NULL);
	InitInputDevice();

	cjLOG("cjWinClient initialized");

	return cjTrue;
}

void cjWinClient::Tick()
{
	if(cjViewport::GetPrimaryViewport())
		cjViewport::GetPrimaryViewport()->ReadInput();
 
//	for(cjINT32 i=0; i<m_viewports.GetSiz(); i++)
//		m_viewports(i)->Paint();
}

void cjWinClient::Draw()
{
	for(cjINT32 i=0; i<(cjINT32)m_viewports.Num(); i++)
		m_viewports(i)->Paint();
}

cjBool cjWinClient::Exec(const cjNameChar* cmd)
{
	return cjTrue;
}

void cjWinClient::FlushCache()
{

}

cjViewport* cjWinClient::CreateViewport(cjTFLAG vpFlag, const cjNameChar* name,
	cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, void* prm)
{
	cjViewport* viewport = cjViewport::CreateInstance(vpFlag, this);
	viewport->SetClient(this);

	if(!viewport->Init(name, left, top, width, height, prm))
	{
		cjLOG(_CT("[%s] Failed to create viewport.."), __CJFUNCTION__);

		delete viewport;
		return NULL;
	}

	m_viewports.Push(viewport);

	return viewport;
}

void cjWinClient::OnDestroyViewport(cjViewport* viewport)
{
	if(!viewport)
		return;

	if(viewport->GetClient() != this)
	{
		cjLOG(_CT("[FAIL][%s] viewport to destroy has different outer client.."), __CJFUNCTION__);
		return;
	}

	m_viewports.RemoveItem(viewport);
}

void cjWinClient::OnDestroy()
{
	FinalizeInputDevice();
		
	for(cjAryOff i=0; i<m_viewports.Num(); i++)
		delete m_viewports(i);
	
	cjSuper::OnDestroy();
}

cjBool cjWinClient::InitInputDevice()
{
	//HRESULT hRst;

	//hRst = ::DirectInput8Create((HINSTANCE)cjSysGetHInstance(), DIRECTINPUT_VERSION,
	//	IID_IDirectInput8, (void**)&cjWinViewport::ms_directInput8, NULL);

	//if(FAILED(hRst))
	//{
	//	cjLOG(_CT("[ERR!][%s] failed to create DirectInput device.."), __CJFUNCTION__);
	//	return cjFalse;
	//}

	////
	//// mouse
	////
	//LPDIRECTINPUT8 di = (LPDIRECTINPUT8)cjWinViewport::ms_directInput8;

	//hRst = di->CreateDevice(GUID_SysMouse, (LPDIRECTINPUTDEVICE8*)&cjWinViewport::ms_indevMouse, NULL);
	//cjBool mouseCreation = cjTrue;

	//if(FAILED(hRst))
	//{
	//	cjLOG(_CT("[WARN!][%s] failed to create mouse.."), __CJFUNCTION__);
	//	mouseCreation = cjFalse;		
	//}
	//else
	//{
	//	LPDIRECTINPUTDEVICE8 mouse = (LPDIRECTINPUTDEVICE8)cjWinViewport::ms_indevMouse;
	//	
	//	hRst = mouse->SetDataFormat(&c_dfDIMouse2);
	//	if(FAILED(hRst))
	//	{
	//		cjLOG(_CT("[ERR!][%s] failed to set mouse data format"), __CJFUNCTION__);
	//		mouseCreation = cjFalse;
	//	}

	//	// mouse data buffer / axis
	//	DIPROPDWORD diMouseProp;
	//	diMouseProp.diph.dwSize       = sizeof(DIPROPDWORD);
	//	diMouseProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	//	diMouseProp.diph.dwObj        = 0;
	//	diMouseProp.diph.dwHow        = DIPH_DEVICE;
	//	diMouseProp.dwData            = 1024;	// buffer size

	//	if(mouseCreation)
	//	{
	//		if(FAILED(hRst=mouse->SetProperty(DIPROP_BUFFERSIZE, &diMouseProp.diph)))
	//		{
	//			cjLOG(_CT("[ERR!][%s] failed to set mouse buffer size"), __CJFUNCTION__);
	//			mouseCreation = cjFalse;
	//		}
	//	}

	//	if(mouseCreation)
	//	{
	//		diMouseProp.dwData = DIPROPAXISMODE_REL;
	//		if(FAILED(hRst=mouse->SetProperty(DIPROP_AXISMODE, &diMouseProp.diph)))
	//		{
	//			cjLOG(_CT("[ERR!][%s] failed to set mouse axis mode"), __CJFUNCTION__);
	//			mouseCreation = cjFalse;
	//		}
	//	}
	//}

	//if(mouseCreation)
	//{
	//	m_mouseEnable = cjTrue;
	//	cjLOG(_CT("[INFO!][%s] mouse attached.. \n"), __CJFUNCTION__);
	//}
	//else
	//{
	//	if(cjWinViewport::ms_indevMouse)
	//	{
	//		((LPDIRECTINPUTDEVICE8)cjWinViewport::ms_indevMouse)->Release();
	//		cjWinViewport::ms_indevMouse = NULL;
	//	}

	//	m_mouseEnable = cjFalse;
	//}

	return cjTrue;
}

void cjWinClient::FinalizeInputDevice()
{
	//if(cjWinViewport::ms_indevMouse)
	//{
	//	((LPDIRECTINPUTDEVICE8)cjWinViewport::ms_indevMouse)->Release();
	//	cjWinViewport::ms_indevMouse = NULL;
	//}
	//
	////if(cjWinViewport::ms_indevJoystick)
	////{
	////	((LPDIRECTINPUTDEVICE8)cjWinViewport::ms_indevJoystick)->Release();
	////	cjWinViewport::ms_indevJoystick = NULL;
	////}

	//if(cjWinViewport::ms_directInput8)
	//{
	//	((LPDIRECTINPUT8)cjWinViewport::ms_directInput8)->Release();
	//	cjWinViewport::ms_directInput8 = NULL;
	//}
}
