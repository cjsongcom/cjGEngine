
#include "cjEnginePCH.h"

#include "engine/apple/cjEAGLAdapter.h"
#include "engine/apple/cjAppleRenRes.h"


//
// cjFAppleDefFrmRenTarget
//

cjFAppleDefFrmRenTarget::cjFAppleDefFrmRenTarget()
  : cjFRenTarget(0, 0, cjRSurf_UNKNOWN, cjRSurf_UNKNOWN)
{
    cjEAGLAdapter* eaglAdapter = GetRenDev()->GetEAGLAdapter();
    eaglAdapter->GetDefaultFrameRenderBufferSize(&m_width, &m_height);
    
    m_format = eaglAdapter->GetDefaultFrameRenBufferColorFmt();
    m_dsformat = eaglAdapter->GetDefaultFrameDepthStencilFmt();
}

cjFAppleDefFrmRenTarget::~cjFAppleDefFrmRenTarget()
{
}

cjEAGLAdapter* cjFAppleDefFrmRenTarget::GetEAGLAdapter()
{
    return GetRenDev()->GetEAGLAdapter();
}


//
// cjFAppleDefFrmRenTargetGrp
//

cjFAppleDefFrmRenTargetGrp::cjFAppleDefFrmRenTargetGrp()
{
}

cjFAppleDefFrmRenTargetGrp::~cjFAppleDefFrmRenTargetGrp()
{
}

cjEAGLAdapter* cjFAppleDefFrmRenTargetGrp::GetEAGLAdapter()
{
    return GetRenDev()->GetEAGLAdapter();
}
