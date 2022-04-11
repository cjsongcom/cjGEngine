
#include "cjEnginePCH.h"

CJOBJ_IMPL_CLASS(cjClient)

cjClient::cjClient()
{
    m_cjtestFile = NULL;
}

void cjClient::OnDestroy()
{
    if(m_cjtestFile)
    {
        delete m_cjtestFile;
        m_cjtestFile = NULL;
    }

	cjSuper::OnDestroy();
}

cjBool cjClient::Init(cjEngine* engine)
{
	m_engine = engine;

	m_fullScreenW = m_windowedViewportW = 1024;
	m_fullScreenH = m_windowedViewportH = 768;

	m_noLight = 0;
    m_fullScreen = 0;

	m_gamma = 2.2f;
	m_hudScale = 1.0f;
	m_currentViewport = NULL;

//    cjTexture* tex = cjObjImport<cjTexture>(NULL, _CU("testsprite"), "spr/test_alpha_nocomp.tga");
//    
//    if(tex)
//    {
//        tex->CreateMip(1);
//    }
//
//    
//    m_cjtestFile = cjFile::Create("ini/spriteinfo.jsn");
//    if(m_cjtestFile->IsValid())
//    {
//        cjStreamOffset siz = m_cjtestFile->GetSiz();
//        
//        cjTAry<cjBYTE> buf(siz);
//        if(m_cjtestFile->Read(&buf(0), siz))
//        {
//            cjLOG("success to read..");
//        }
//        
//        
//        
//
//        if(m_cjtestFile)
//        {
//            delete m_cjtestFile;
//            m_cjtestFile = NULL;
//        }
//    }

	return cjTrue;
}

void cjClient::FlushCache()
{
	for(cjAryOff i=0; i<m_viewports.Num(); i++)
	{
		cjViewport* vp = m_viewports(i);

//		if(vp->GetRenDev())
//			vp->GetRenDev()->FlushCache(vp);
	}
}
