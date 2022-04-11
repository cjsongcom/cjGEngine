
#include "cjEnginePCH.h"
#include "cjObjFactoryForEngine.h"

#include "core/cjImgLib.h"


static void* cjTextureCreator_TGA(const cjBYTE* dataSt, const cjBYTE* dataEn, void* objCls, void* outer, cjName name, cjTFLAG flags);


//
// cjTextureFactory
//

CJOBJ_IMPL_CLASS(cjTextureFactory)

CJOBJ_IMPL_STATIC_REG_BEGIN(cjTextureFactory)

	cjImgTGA::cjTextureCreator = &cjTextureCreator_TGA;

    m_factoryFlag = EFACTF_IMPORT_FROM_BIN;
    m_supportCls = cjTexture::cjObjGetStaticCls();
        
CJOBJ_IMPL_STATIC_REG_END(cjTextureFactory)


cjTextureFactory::cjTextureFactory()
{
}

cjBool cjTextureFactory::IsSupport(cjObjCls* cls)
{
    if(cls != m_supportCls)
        return cjFalse;
    
    return cjTrue;
}

cjObj* cjTextureFactory::ImportFromBinary(cjObj* cls, cjObj* outer, cjName name, cjTFLAG flags, 
	cjObj* ctx, const cjUni* type, const cjBYTE* src, const cjBYTE* srcEnd)
{
    cjObj* rst = NULL;
//    cjPtrDiff dataLen = srcEnd - src;
    
    if(cjImgTGA::CheckExt(type))
    {
        rst = (cjObj*)cjImgTGA::cjTextureCreator(src, srcEnd, cls, outer, name, flags);
    }

    return rst;
}

void* cjTextureCreator_TGA(const cjBYTE* dataSt, const cjBYTE* dataEn, void* objCls, void* outer, cjName name, cjTFLAG flags)
{
    cjINT32 dataLen = (cjINT32)(dataEn - dataSt);
    
    if(dataLen < sizeof(cjImgTGAHeader))
        return NULL;

	cjImgTGAHeader* hdr = (cjImgTGAHeader*)dataSt;

    cjTexture* texture = cjObjNew<cjTexture>((cjObjCls*)objCls, (cjObj*)outer, name, flags);


    cjRSurfaceFmt fmtSurface = cjRSurf_UNKNOWN;
    
    switch(hdr->m_bitsPerPixel)
    {
    case 32 : fmtSurface = cjRSurf_RGBA8; break;
    case 24 : fmtSurface = cjRSurf_RGB8; break;
    default:  return NULL;
    }

    // cjRSurf_RGB8 will be converted cjRSurf_RGBA8 for lack of opengl
    if(!texture->Init(hdr->m_width, hdr->m_height, fmtSurface))
    {
        delete texture;
        return NULL;
    }
    
    texture->cjObjPostLoad(); // init texure object value, determine lod

	cjFMipmap& rootMip = texture->m_mip(0);
	cjBYTE* dst = (cjBYTE*)&rootMip.m_data(0);

	if(!cjImgTGA::LoadImgStream(dataSt, dataEn, dst, 0))
	{
		delete texture;
		return NULL;
	}

	return texture;
}
