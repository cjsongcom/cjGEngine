
#include "cjEnginePCH.h"

#include "engine/apple/cjEAGLAdapter.h"
#include "engine/apple/cjAppleRenRes.h"
#include "engine/apple/cjEAGLRenRes.h"

#include "engine/cjGLRenDev.h"

//
// cjEAGLDefFrmRenTarget
//

CJSER_IMPL_CLASS(cjEAGLDefFrmRenTarget, cjGLRenTarget)

cjEAGLDefFrmRenTarget*  cjEAGLDefFrmRenTarget::ms_eaglDefFrmRenTarget = NULL;


void cjEAGLDefFrmRenTarget::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
    cjSuper::Init(renDev, cacheID);
}

cjEAGLDefFrmRenTarget::~cjEAGLDefFrmRenTarget()
{
    ms_eaglDefFrmRenTarget = NULL;
}

void cjEAGLDefFrmRenTarget::ReleaseColorSurface()
{
	if(m_handleColor)
	{
// handled by cjEAGLAdapter
//		m_renDev->glDeleteTextures(1, &m_handleColor);
		m_handleColor = NULL;
	}
}

void cjEAGLDefFrmRenTarget::ReleaseDepthStencil()
{
	if(m_handleDepthStencil)
	{
// handled by cjEAGLAdapter
//		m_renDev->glDeleteRenderbuffers(1, &m_handleDepthStencil);
		m_handleDepthStencil = NULL;
	}
}

void cjEAGLDefFrmRenTarget::Cache(cjFRenResSrc* renResSrc)
{
	if(!renResSrc || !renResSrc->GetRenTarget())
		return;
    
	if(GetCacheRev() == renResSrc->GetCacheRev())
		return;

	SetCacheRev(renResSrc->GetCacheRev());

    cjFRenTarget* renTarget = (cjFRenTarget*)renResSrc;
    cjFAppleDefFrmRenTarget* appDefFrmRT = renTarget->GetAppleDefFrmRenTarget();

    if(!appDefFrmRT)
        return;

    if(!ms_eaglDefFrmRenTarget)
    {
        ms_eaglDefFrmRenTarget = this;
    }
    else
    {
        if(ms_eaglDefFrmRenTarget != this) return;
    }
 
//  releasing color surface & depth stencil buffer doesn't need..
//  they are managed in eaglAdpater
//	ReleaseColorSurface();
//	ReleaseDepthStencil();
    m_handleColor = m_handleDepthStencil = 0;
    
//	cjGLRenDev* renDev = m_renDev;
    cjEAGLAdapter* eaglAdapter = appDefFrmRT->GetEAGLAdapter();
    eaglAdapter->GetDefaultFrameRenderBufferSize(&m_width, &m_height);
    
    if(eaglAdapter->GetDefaultFrameRenBufferColorFmt() != cjRSurf_UNKNOWN)
    {
        m_handleColor = eaglAdapter->GetDefaultFrameRenderBufferColor();
        m_colorSurType = GL_RENDERBUFFER;
    }
    
    if(eaglAdapter->GetDefaultFrameDepthStencilFmt() != cjRSurf_UNKNOWN)
    {
        m_handleDepthStencil = eaglAdapter->GetDefaultFrameRenderBufferDepth();
        m_depthStencilGLFmt = eaglAdapter->GetDefaultFrameDepthStencilGLFmt();
    }
    
}


//
// cjEAGLDefFrmRenTargetGrp
//

CJSER_IMPL_CLASS(cjEAGLDefFrmRenTargetGrp, cjGLRenTargetGrp)

void cjEAGLDefFrmRenTargetGrp::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
    cjSuper::Init(renDev, cacheID);
}

cjEAGLDefFrmRenTargetGrp::~cjEAGLDefFrmRenTargetGrp()
{
}

void cjEAGLDefFrmRenTargetGrp::Cache(cjFRenResSrc* renResSrc)
{
	if(!renResSrc)
		return;

	if(GetCacheRev() == renResSrc->GetCacheRev())
		return;

	SetCacheRev(renResSrc->GetCacheRev());
    
    cjFRenTargetGrp* renTargetGrp = (cjFRenTargetGrp*)renResSrc;
    cjFAppleDefFrmRenTargetGrp* appDefFrmRTGrp = renTargetGrp->GetAppleDefFrmRenTargetGrp();
    
    if(!appDefFrmRTGrp)
       return;

	cjGLRenDev* renDev = m_renDev;
        cjEAGLAdapter* eaglAdapter = renDev->GetEAGLAdapter();
    
        m_fboHandle = eaglAdapter->GetDefaultFrameBuffer();

	for(cjINT32 i=0; i<cjFRenTargetGrp::MAX_MULTI_RENDER_TARGETS; i++)
	{
		cjFRenTarget* rt = renTargetGrp->GetRenTarget(i);
		cjGLRenTarget* rtCache = NULL;

		if(rt)
		{
			rtCache = (cjGLRenTarget*)renDev->FindResCache(rt->GetCacheID());
            
            if(!rtCache)
			{
                if(i==0)
                {
                    rtCache = cjEAGLDefFrmRenTarget::NewInst();
                    ((cjEAGLDefFrmRenTarget*)rtCache)->Init(renDev, rt->GetCacheID());
                }
                else
                {
                    rtCache = cjGLRenTarget::NewInst();
                    rtCache->Init(renDev, rt->GetCacheID());
                }
			}

			if(rt->GetCacheRev() != rtCache->GetCacheRev())
				rtCache->Cache(rt);
		}

		m_renTarget[i] = rtCache;
	}
}

void cjEAGLDefFrmRenTargetGrp::ReleaseFrameBufferObj()
{

}


//
//
//

CJSER_IMPL_CLASS(cjEAGLVertexStream, cjGLVertexStream)

void cjEAGLVertexStream::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
    cjSuper::Init(renDev, cacheID);
}

cjEAGLVertexStream::~cjEAGLVertexStream()
{
//   	if(m_handle)
//	{
//        m_renDev->glBindVertexArray(0);
//		m_renDev->glDeleteVertexArrays(1, &m_handle);
//        
//        m_handle = 0;
//	}
}

void cjEAGLVertexStream::Cache(cjFRenResSrc* renResSrc)
{
    if(renResSrc->GetCacheRev() == GetCacheRev())
        return;

    SetCacheRev(renResSrc->GetCacheRev());

    cjGLRenDev* renDev = m_renDev;
    cjFVertexStream* srcVtxStream = (cjFVertexStream *)renResSrc;
    
    //const cjFVertexDecl* vtxDecl = srcVtxStream->GetVertexDecl();
    
    ResetStream();
    
    m_numVertexBuffer = 1;
    renDev->glGenBuffers(m_numVertexBuffer, &m_vtxBufHandle);
    renDev->glBindBuffer(GL_ARRAY_BUFFER, m_vtxBufHandle);
    
    // upload vertex stream data to gpu
    renDev->glBufferData(GL_ARRAY_BUFFER, srcVtxStream->GetByteSiz(), 0, ResolveStreamUsageFlag(srcVtxStream));
    void* buf = renDev->glMapBuffer(GL_ARRAY_BUFFER, ResolveStreamAccessFlag(srcVtxStream));
    
    cjAssert(buf);
    srcVtxStream->CopyCompStreamIntoSingleStream((cjBYTE*)buf);
    
    renDev->glUnmapBuffer(GL_ARRAY_BUFFER);
    renDev->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void* cjEAGLVertexStream::GetVertexData()
{
    return NULL;
}

void cjEAGLVertexStream::Reallocate(cjINT32 newSiz)
{
}
