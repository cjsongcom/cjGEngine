
#include "cjEnginePCH.h"

#include "cjClient.h"
#include "cjRenTargetGrp.h"
#include "cjInput.h"


CJOBJ_IMPL_CLASS(cjViewport)

cjUINT32 cjViewport::ms_numViewports = 0;
cjViewport* cjViewport::ms_primaryViewport = NULL;
void* cjViewport::ms_renDevFocusWnd = NULL;


cjViewport::cjViewport() 
{
	m_viewportFlag = 0;	

	m_mouseShow = cjTrue;	
	m_mouseDragging = cjFalse;

	m_isDestroying = cjFalse;
	m_wasFullscreen = cjFalse;

	m_prm = NULL;

	m_pendingPresentFrame = cjFalse;
	m_totalRenFrameCnt = 0;
	m_beginRenderTime = 0.0;
	m_ri = NULL;
	m_lastPresentTime = 0.0;

	m_input = NULL;
	m_renDev = NULL;
	m_client = NULL;
	m_useOpenGL = cjFalse;

	m_camDefaultEye = cjVec3(0,0, 800);
	m_camDefaultLookAt = cjVec3(0,0,0);
	m_camDefaultFovY = CJ_PI/4.0f;
	m_camDefaultAspect = 1.3f;

	m_camera = new cjCamera;
	
	DoCameraReset();
	
	++ms_numViewports;
}

cjBool cjViewport::Init(const cjNameChar* , cjINT32 left, cjINT32 top, cjINT32 width,
	cjINT32 height, void* prm)
{
	if((m_viewportFlag & cjViewport::EVF_ToolViewport))
		return cjFalse;

	if(m_viewportFlag & EVF_PrimaryView)
	{
		if(GetPrimaryViewport())
		{
			cjLOG(_CT("[ERR][%s] primary viewport is already exist."));
			return cjFalse;
		}
	}

	m_left = left;
	m_top = top;
	m_width = width;
	m_height = height;

	m_prm = prm;

	if(!m_input)
	{
		m_input = new cjInput;
		m_input->Init(this);
	}

	return cjTrue;
}

cjViewport* cjViewport::CreateInstance(cjTFLAG flag, cjObj* outer)
{
	cjViewport* viewport = NULL;

//cj	viewport = new(outer) cjWinViewport();
	
	viewport->SetOuter(outer);
	viewport->SetViewportFlag(flag, 1);

	return viewport;
}

void cjViewport::Tick()
{
}

void cjViewport::ReadInput(cjFLOAT elapsedTime)
{
	if(elapsedTime != -1.0f)
		UpdateInputData(cjFalse, elapsedTime);

	if(m_input)
		m_input->DispatchInput(elapsedTime);
}

cjBool cjViewport::RenderLock()
{
	if(!m_renDev)
		return cjFalse;

//	m_beginRenderTime = cjGlobalTime;
	m_totalRenFrameCnt++;

	m_ri = m_renDev->BeginRender(this);

	return m_ri ? cjTrue : cjFalse;
}

void cjViewport::RenderUnLock()
{
	m_renDev->EndRender(m_ri);
	m_pendingPresentFrame = cjTrue;
}

void cjViewport::Present()
{
	m_pendingPresentFrame = cjFalse;
	m_renDev->Present(this);

//	m_lastPresentTime = cjGlobalTime;
}

void cjViewport::OnDestroy()
{
	if(m_camera)
		m_camera = cjNULL;

	if(!GetObjDestroyed())
		--ms_numViewports;

	if(m_input)
	{
		delete m_input;
		m_input = NULL;
	}

	cjSuper::OnDestroy();

	if(m_viewportFlag & EVF_PrimaryView)
		SetPrimaryViewport(NULL);	
}

cjBool cjViewport::AttachRenderDevice(cjRenDev* renDev)
{
	cjClient* ownClient = GetClient();

	if(renDev)
	{
		if(renDev != ownClient->GetEngine()->GetRenDev())
			return cjFalse;
	}

	return cjTrue;
}

//void cjViewport::SetOutputRenderTargetGrp(cjRenderTargetGrp* output)
//{
//	m_output = output;
//}
//
//cjRenderTargetGrp* cjViewport::GetOutputRenderTargetGrp()
//{
//	return m_output;
//}

void cjViewport::DoCameraRotate(cjFLOAT yaw, cjFLOAT pitch, cjFLOAT roll, cjBool isDelta)
{
	m_camera->Rotate(yaw, pitch, roll, isDelta);
}

void cjViewport::DoCameraMove(cjFLOAT dir, cjFLOAT right, cjFLOAT up, cjBool isDelta)
{
	m_camera->Move(dir, right, up, isDelta);
}

void cjViewport::DoCameraDist(cjFLOAT distDelta)
{
}

void cjViewport::DoCameraReset()
{
	m_camera->SetDefault();

	m_camera->SetFovY(m_camDefaultFovY, m_camDefaultAspect);
	m_camera->SetCamera(m_camDefaultEye, m_camDefaultLookAt);
	m_camera->ClearDirty();
}

const cjMatrix* cjViewport::GetViewMatrix()
{
	return m_camera->GetViewMatrix();
}
