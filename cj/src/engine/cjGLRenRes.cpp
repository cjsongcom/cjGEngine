
#include "cjEnginePCH.h"

#include "cjGLRenDev.h"
#include "cjGLRenRes.h"


cjINT32 cjGLGetVertexCompSizDesc(const cjVertexCompSizType compSizType, GLint* compNum, GLenum* compVarType)
{
    GLint _compNum = 0;
    GLenum _compVarType = GL_FLOAT;
    cjINT32 retSiz = 0;

    switch(compSizType)
    {
    case cjVtxCompSiz_FLOAT     : _compNum = 1; retSiz = sizeof(cjFLOAT); break;
	case cjVtxCompSiz_FLOAT2    : _compNum = 2; retSiz = sizeof(cjFLOAT)*2; break;
	case cjVtxCompSiz_FLOAT3    : _compNum = 3; retSiz = sizeof(cjFLOAT)*3; break;
    case cjVtxCompSiz_FLOAT4    : _compNum = 4; retSiz = sizeof(cjFLOAT)*4; break;
    
	case cjVtxCompSiz_UINT32    : _compNum = 1; _compVarType = GL_UNSIGNED_INT; retSiz = sizeof(cjTFLAG); break;
    
    case cjVtxCompSiz_BYTE      : _compNum = 1; _compVarType = GL_BYTE; retSiz = sizeof(cjBYTE); break;
    case cjVtxCompSiz_BYTE2     : _compNum = 2; _compVarType = GL_BYTE; retSiz = sizeof(cjBYTE)*2; break;
    case cjVtxCompSiz_BYTE3     : _compNum = 3; _compVarType = GL_BYTE; retSiz = sizeof(cjBYTE)*3; break;
    case cjVtxCompSiz_BYTE4     : _compNum = 4; _compVarType = GL_BYTE; retSiz = sizeof(cjBYTE)*4; break;
    
    case cjVtxCompSiz_SHORT     : _compNum = 1; _compVarType = GL_SHORT; retSiz = sizeof(cjSWORD); break;
    case cjVtxCompSiz_SHORT2    : _compNum = 2; _compVarType = GL_SHORT; retSiz = sizeof(cjSWORD)*2; break;
    case cjVtxCompSiz_SHORT3    : _compNum = 3; _compVarType = GL_SHORT; retSiz = sizeof(cjSWORD)*3; break;
    case cjVtxCompSiz_SHORT4    : _compNum = 4; _compVarType = GL_SHORT; retSiz = sizeof(cjSWORD)*4; break;
        
    case cjVtxCompSiz_Unknown:
    default:
        break;
    }
    
    if(compNum)
        *compNum = _compNum;
    
    if(compVarType)
        *compVarType = _compVarType;
    
    return retSiz;
}


//
// cjGLResCache
//

CJSER_IMPL_CLASS(cjGLResCache, cjSerializable)

cjGLResCache::InstanceCreatorPFN cjGLResCache::ms_instanceCreator[cjRSCID_Num] = { cjNULL, };

void cjGLResCache::StaticGLResCacheInit()
{
    cjMemZero(ms_instanceCreator, sizeof(ms_instanceCreator));
    
    RegisterInstanceCreator(cjRSCID_RenderTarget,           &cjGLRenTarget::CreateInstance);
    RegisterInstanceCreator(cjRSCID_Texture,                &cjGLTexture::CreateInstance);
    RegisterInstanceCreator(cjRSCID_Shader,                 &cjGLShaderCache::CreateInstance);
    RegisterInstanceCreator(cjRSCID_VertexStreamAttrib,     &cjGLVertexStreamAttribLayout::CreateInstance);
    RegisterInstanceCreator(cjRSCID_VertexStream,           &cjGLVertexStream::CreateInstance);
	RegisterInstanceCreator(cjRSCID_IndexStream,			&cjGLIndexBuffer::CreateInstance);
}

void cjGLResCache::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	m_renDev = renDev;
	m_cacheID = cacheID;
	m_cacheRev = 0;
    
    cjMemZero(&m_cli, sizeof(cjFResCacheLinkInfo));
    
	// Warn! - AddResCache() must be called in constructor here
	m_renDev->AddResCache(this);
}

cjGLResCache::~cjGLResCache()
{
	if(m_renDev)
	{
		m_renDev->RemoveResCache(this);
		m_renDev = cjNULL;
	}
}

void cjGLResCache::CollectUsageStatistics(cjGLResStatData& in)
{

}

void cjGLResCache::RegisterInstanceCreator(cjResCacheIDType type, InstanceCreatorPFN creator)
{
    ms_instanceCreator[type] = creator;
}

cjGLResCache* cjGLResCache::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
    cjGLResCache* resCacheGL = ms_instanceCreator[src->GetResCacheIDType()](glResRTTI, renDev, src);
    
    cjAssert(resCacheGL);
    
    return resCacheGL;
}




//
// cjGLTexture
//

CJSER_IMPL_CLASS(cjGLTexture, cjGLResCache)

void cjGLTexture::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{	
	cjGLResCache::Init(renDev, cacheID);

	m_textureID = 0;
	m_isCubeMap = cjFalse;

	m_width = 0;
	m_height = 0;
	m_numMip = 1;
	m_firstMip = 0;
	m_format = cjRSurf_UNKNOWN;	
}

cjGLTexture::~cjGLTexture()
{
}

void cjGLTexture::Cache(cjFRenResSrc* renResSrc)
{
    if(GetCacheRev() == renResSrc->GetCacheRev())
		return;
    
    SetCacheRev(renResSrc->GetCacheRev());

    cjFBaseTexture* src = renResSrc->GetBaseTexture();

    // skip cjRenderTarget
    if(src->GetRenTarget())
    {
        cjError("ERR! - bad paramter 'cjRenderTarget'..");
        return;
    }

	cjINT32	firstMip = src->GetFirstMip();
	cjINT32	width	 = src->GetWidth();
	cjINT32	height	 = src->GetHeight();
	cjINT32	numMip   = src->GetNumMip();
    cjRSurfaceFmt srcFormat = src->GetFormat();
    
    // is hardware compressable format (s3tc, pvrtc)
    cjBool isHWCompressSurfaceFmt = m_renDev->IsHWCompressSurfaceFmt(srcFormat);
	cjBool forceCompress = isHWCompressSurfaceFmt;

	//cjINT32	potUSiz = width >> firstMip;
	//cjINT32	potVSiz = height >> firstMip;

	cjFCubeMap* cubemap	= src->GetCubeMap();
    cjFTexture*	texture	= src->GetTexture();

	if(texture)
	{
        // check previous cache state
		if(!m_textureID ||
            m_width != (width >> firstMip) || m_height != (height >> firstMip) ||
            m_firstMip != firstMip || m_numMip != numMip ||
            m_format != srcFormat )
		{
			if(m_textureID)
				m_renDev->glDeleteTextures(1, &m_textureID);
            
			m_renDev->glGenTextures(1, &m_textureID);

			m_renDev->glBindTexture(GL_TEXTURE_2D, m_textureID);
			m_renDev->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                m_renDev->m_useTrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
            
			m_renDev->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
			if(m_renDev->m_supportAnisotropicFilter)
            {
				m_renDev->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    (GLfloat)m_renDev->m_levelAnisotropicFilter);
            }
		}

		m_renDev->glBindTexture(GL_TEXTURE_2D, m_textureID);
        
    // opengl doesn't handle non aligned format (4byte-alignment)
    // so GL_RGBA or GB_BGRA is only used. see below
        
/*
    You create storage for a Texture and upload pixels to it with glTexImage2D??
    (or similar functions, as appropriate to the type of texture). 
    If your program crashes during the upload, or diagonal lines appear in the resulting image, 
    this is because the alignment of each horizontal line of your pixel array is not multiple of 4. 
    This typically happens to users loading an image that is of the RGB or BGR format (for example, 24 BPP images), 
    depending on the source of your image data.
    Example, your image width = 401 and height = 500. The height is irrelevant; 
    what matters is the width. If we do the math, 401 pixels x 3 bytes = 1203, which is not divisible by 4. 
    Some image file formats may inherently align each row to 4 bytes, but some do not. 
    For those that don't, each row will start exactly 1203 bytes from the start of the last. 
    OpenGL's row alignment can be changed to fit the row alignment for your image data. 
    This is done by calling glPixelStorei(GL_UNPACK_ALIGNMENT, #)?? where # is the alignment you want. 
    The default alignment is 4.
    
    And if you are interested, most GPUs like chunks of 4 bytes.

    In other words, GL_RGBA??or GL_BGRA??is preferred when each component is a byte. 
    GL_RGB??and GL_BGR??is considered bizarre since most GPUs, 
    
    most CPUs and any other kind of chip don't handle 24 bits.
    This means, the driver converts your GL_RGB??or GL_BGR??to what the GPU prefers, which typically is BGRA.
 
    Similarly, if you read a buffer with glReadPixels?? you might get similar problems. 
    There is a GL_PACK_ALIGNMENT??just like the GL_UNPACK_ALIGNMENT?? 
    The default alignment is again 4 which means each horizontal line must be a multiple of 4 in size. If you read the buffer
    with a format such as GL_BGRA??or GL_RGBA??you won't have any problems since the line will always be a multiple of 4. 
    If you read it in a format such as GL_BGR??or GL_RGB??then you risk running into this problem.
*/
        
        cjINT32 totalMipLevel = 0;
        
        for(cjINT32 mipLevel=firstMip; mipLevel<numMip; mipLevel++)
        {
            GLenum texGLFmt = 0;
            
            switch(srcFormat)
            {
            case cjRSurf_RGBA8: texGLFmt = GL_BGRA;	break;
			// cjdefer 
            case cjRSurf_RGB8: cjAssert(!"ERR! - only accepted 4byte-alignmented format, as ARGB8 "); break;
            
    #if defined(CJ_PLATFORM_WIN32)
            case cjRSurf_DXT1:  texGLFmt = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
            case cjRSurf_DXT3:  texGLFmt = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
            case cjRSurf_DXT5:  texGLFmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
    #endif
          
    #if defined(CJ_PLATFORM_APPLE)
            case cjRSurf_PVRTC_RGB4: texGLFmt = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; break;
            case cjRSurf_PVRTC_RGB2: texGLFmt = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG; break;
            case cjRSurf_PVRTC_ARGB4: texGLFmt = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; break;
            case cjRSurf_PVRTC_ARGB2: texGLFmt = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG; break;
    #endif
                
            default: texGLFmt = GL_BGRA; break;
            }

            // cancel compressing if hardware compressing is not supported.
            if(!m_renDev->IsSupportHWCompressSurfaceFmt())
            	texGLFmt = GL_BGRA; // opengl is only support 4byte alignment format

            cjINT32 mipWidth = width >> mipLevel;
            cjINT32	mipHeight = height >> mipLevel;

            // trying to generate compressed texture, if supported
            if(isHWCompressSurfaceFmt && m_renDev->IsSupportHWCompressSurfaceFmt())
            {
                if(mipWidth<4 || mipHeight<4) // skip compress under size 2
                    break;

                cjBool tempData = cjFalse;
                cjBYTE* data = (cjBYTE*)texture->GetRawTextureData(mipLevel);
                    
                if(!data)
                {
                    // fill black to know corrupted texture
                    data = new cjBYTE[mipWidth * mipHeight];
                    cjMemZero(data, mipWidth * mipHeight);
                    tempData = cjTrue;
                }
                    
                // 3rd prm, internal fmt = GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA.
                // 7th prm, GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA
                m_renDev->glCompressedTexImage2D(GL_TEXTURE_2D, mipLevel - firstMip,
                    texGLFmt, mipWidth, mipHeight, 0,
                    cjGetBytesPerPixel(srcFormat, mipWidth * mipHeight), data);

                if(tempData)
                    delete [] data;

                texture->UnloadRawTextureData(mipLevel);
            }
            else
            {
                // generate uncompressed texture
                if(mipWidth < 2 || mipHeight < 2)
                {
                    cjAssert(!"ERR! - not acceptable texture size..");
                    break;
                }

                cjBool tempData = cjFalse;
                cjBYTE* data = cjNULL;
                
                if(!isHWCompressSurfaceFmt)
                    data = (cjBYTE*)texture->GetRawTextureData(mipLevel);
                
                /*
                    data layout
                    
                   (0,0)         (sizW, 0)
                   BGRA BGRA BGRA BGRA
                   BGRA BGRA BGRA BGRA
                            ...
                */
                

                if(!data)
                {
                    if(isHWCompressSurfaceFmt)
                    {
                        texture->GetOwnerCJTexture()->ConvertHWCompressSurface(mipLevel, 1, 0, &data);
                    }
                    else
                    {
                        data = new cjBYTE[mipWidth * mipHeight * 4];
                        texture->GetTextureData(mipLevel, data, mipWidth*4, srcFormat);
                    }
                    tempData = cjTrue;
                }

                cjAssert(texGLFmt == GL_BGRA);
                
                m_renDev->glTexImage2D(GL_TEXTURE_2D, mipLevel-firstMip,
#if defined(CJ_PLATFORM_APPLE)
                    GL_RGBA, // internal fmt
#else
                    forceCompress ? GL_COMPRESSED_RGBA : GL_RGBA,
#endif
                    mipWidth, mipHeight, 0,
                    texGLFmt,
                    GL_UNSIGNED_BYTE, // component size
                    data);
                
                m_renDev->GetLastGLErr();
                
                if(tempData)
                    delete [] data;
                    
                texture->UnloadRawTextureData(mipLevel);
            }

            totalMipLevel++;
            
        } // end of for

		m_renDev->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, cjMax(0, totalMipLevel-1));
        
	}	
	else if(cubemap)
	{
//        // handle cubemap
//		if( !TextureID || CachedWidth != (Width >> FirstMip) || CachedHeight != (Height >> FirstMip) || CachedFirstMip != FirstMip || CachedNumMips != NumMips || CachedFormat != ActualFormat )
//		{
//			if( TextureID )
//				RenDev->glDeleteTextures( 1, &TextureID );	
//			RenDev->glGenTextures( 1, &TextureID );
//
//			RenDev->glBindTexture( GL_TEXTURE_CUBE_MAP, TextureID );
//
//			RenDev->glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, RenDev->UseTrilinear ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST );
//			RenDev->glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//		}
//
//		RenDev->glBindTexture( GL_TEXTURE_CUBE_MAP, TextureID );
//
//		INT MaxLevel = 0;
//		for( INT MipLevel=FirstMip; MipLevel<NumMips; MipLevel++ )
//		{
//			GLenum Format;
//			switch( ActualFormat )
//			{
//			case TEXF_RGBA8:
//				Format = GL_BGRA_EXT;
//				break;
//			case TEXF_DXT1:
//				Format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
//				break;
//			case TEXF_DXT3:
//				Format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
//				break;
//			case TEXF_DXT5:
//				Format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
//				break;
//			default:
//				Format = GL_BGRA_EXT;
//				break;
//			}
//
//			// Fall back to decompressing if S3TC is not supported.
//			if( !RenDev->SUPPORTS_GL_EXT_texture_compression_s3tc )
//				Format = GL_BGRA;
//
//			INT MipWidth	= Width >> MipLevel,
//				MipHeight	= Height >> MipLevel;
//
//			for( INT FaceIndex=0; FaceIndex<6; FaceIndex ++ )
//			{
//				FTexture* Texture = Cubemap->GetFace( FaceIndex );
//
//				if(!Texture)
//					continue;
//
//				if( IsDXTC(ActualFormat) && RenDev->SUPPORTS_GL_EXT_texture_compression_s3tc )
//				{	
//					if( MipWidth < 4 || MipHeight < 4 )
//						break;
//
//					check(Texture->GetRawTextureData( MipLevel ));
//					RenDev->glCompressedTexImage2D(
//						GL_TEXTURE_CUBE_MAP_POSITIVE_X + FaceIndex, 
//						MipLevel - FirstMip,
//						Format,
//						MipWidth,
//						MipHeight,
//						0,
//						GetBytesPerPixel(ActualFormat, MipWidth * MipHeight ),
//						Texture->GetRawTextureData( MipLevel )
//					);
//					
//					Texture->UnloadRawTextureData( MipLevel );
//				}
//				else
//				{	
//					UBOOL TemporaryData	= 0;
//					BYTE* Data			= cjNULL;
//
//					if( !IsDXTC( ActualFormat ) )
//						Data = (BYTE*) Texture->GetRawTextureData( MipLevel );
//
//					if( !Data || ActualFormat != SourceFormat )
//					{	
//						if( IsDXTC( ActualFormat ) )
//						{
//							Texture->GetUTexture()->ConvertDXT( MipLevel, 1, 0, &Data );				
//						}
//						else
//						{
//							Data = new BYTE[MipWidth * MipHeight * 4];
//							Texture->GetTextureData( MipLevel, Data, MipWidth * 4, ActualFormat, 0 );
//						}
//						TemporaryData = 1;
//					}
//
//					RenDev->glTexImage2D(
//						GL_TEXTURE_CUBE_MAP_POSITIVE_X + FaceIndex,
//						MipLevel - FirstMip,
//						ForceCompression ? GL_COMPRESSED_RGBA_ARB : GL_RGBA,
//						MipWidth,
//						MipHeight,
//						0,
//						Format,
//						GL_UNSIGNED_BYTE,
//						Data
//					);
//				
//					if( TemporaryData )
//						delete [] Data;
//					
//					Texture->UnloadRawTextureData( MipLevel );
//				}
//
//				if( FaceIndex == 0 )
//					MaxLevel++;
//
//			}
//		}
//
//		RenDev->glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, Max(0,MaxLevel-1) );
//
//		IsCubemap = 1;
	}
    else
    {
        cjError("ERR! - bad cjRenResSrc..");
        return;
    }

//	SetCacheRev(src->GetCacheRev());
	m_format = srcFormat;
	m_width	= width >> firstMip;
	m_height = height >> firstMip;
	m_firstMip = firstMip;
	m_numMip = numMip;
}

cjGLResCache* cjGLTexture::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
	cjAssert(!"not impl");
    return cjNULL;
}


//
// cjGLVertexStreamAttribLayout
//

CJSER_IMPL_CLASS(cjGLVertexStreamAttribLayout, cjGLResCache)

void cjGLVertexStreamAttribLayout::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLResCache::Init(renDev, cacheID);
    m_vao = 0;
}

cjGLVertexStreamAttribLayout::~cjGLVertexStreamAttribLayout()
{
    ResetVAO();
}

void cjGLVertexStreamAttribLayout::ResetVAO()
{
    if(m_vao)
    {
        m_renDev->glDeleteVertexArrays(1, &m_vao);
        m_vao = cjNULL;
    }
}

void cjGLVertexStreamAttribLayout::Cache(cjFRenResSrc* renResSrc)
{
    if(GetCacheRev() == renResSrc->GetCacheRev())
		return;
	
	SetCacheRev(renResSrc->GetCacheRev());
    ResetVAO();
    
    cjGLRenDev* renDev = m_renDev;
    cjFVertexStreamAttribLayout* srcVAL = (cjFVertexStreamAttribLayout*)renResSrc;
    
    renDev->glGenVertexArrays(1, &m_vao);

	CJGL_BIND_ASSERT_VAO(renDev)

    renDev->glBindVertexArray(m_vao);

    cjINT32 numStream = srcVAL->GetNumStream();
    
    for(cjINT32 i=0; i<numStream; i++)
    {
        cjFVertexStream* curVtxStream = srcVAL->GetStream(i);
        cjGLVertexStream* curVtxStreamGLCache = cjTResolveGLRenRes<cjGLVertexStream>(renDev, curVtxStream);
        
        if(i==0)
            m_vtxDecl = *curVtxStream->GetVertexDecl();

        if(!curVtxStreamGLCache)
            continue;
        
		// attach vtx stream handle to active vertex array object
        renDev->glBindBuffer(GL_ARRAY_BUFFER, curVtxStreamGLCache->GetVtxBufHandle(0));
		
//		SetupVertexAttributeOnVAO(renDev, curVtxStream->GetVertexDecl());
    }
    
    renDev->glBindVertexArray(0);

	// cjstemp
	// off
	// #X 0 renDev->glBindBuffer(GL_ARRAY_BUFFER, 0); 

}

//cjopti
cjBool cjGLVertexStreamAttribLayout::SetupVertexArrayForShaderProgram(cjGLRenDev* renDev, GLuint vao,
    const cjGLShaderProgram* prog, const cjFVertexDecl* decl)
{
    const cjGLVertexAttribInfo* vainfo = prog->GetVertexAttribInfo();
    cjINT32 vtxStride = decl->GetStride();

    for(cjINT32 i=0; i<vainfo->GetNumAttrib(); i++)
    {
        cjEShaderVtxAttr shaderVtxAttr;
        GLint            shaderVtxAttrLoc;
        
        vainfo->GetInfo(i, &shaderVtxAttr, &shaderVtxAttrLoc);
        const cjVertexComponent* comp = decl->GetCompByShaderVtxAttrType(shaderVtxAttr);
        
        if(!comp)
            return cjFalse;
        
        GLint compNum = 1;  // x=1, xy=2, xyz=3, xyzw=4
        GLenum compVarType = GL_FLOAT;
        
        cjGLGetVertexCompSizDesc((const cjVertexCompSizType)comp->m_sizType, &compNum, &compVarType);

		//cjtodo
		// need checking component size with vainfo

        GLboolean doNormalize = GL_FALSE;
		/*
			If set to GL_TRUE, normalized indicates that values stored in an integer format are 
			to be mapped to the range [-1,1] (for signed values) or [0,1] (for unsigned values) 
			when they are accessed and converted to floating point.
		*/

        cjUINT32 compOffset = comp->m_compOffset;
 
        renDev->glEnableVertexAttribArray(shaderVtxAttrLoc);
        renDev->glVertexAttribPointer((GLuint)shaderVtxAttrLoc,
                compNum, compVarType, doNormalize, vtxStride, (void*)compOffset);
    }
	    
    return cjTrue;
}

//void cjGLVertexStreamAttribLayout::SetupVertexAttributeOnVAO(cjGLRenDev* renDev, const cjFVertexDecl* decl)
//{
//    cjINT32 vtxStride = decl->GetStride();
//
//    for(cjINT32 i=0; i<decl->GetNumComp(); i++)
//    {
//        const cjVertexComponent& comp = decl->GetComp(i);
//        
//        GLint compNum = 1;  // x=1, xy=2, xyz=3, xyzw=4
//        GLenum compVarType = GL_FLOAT;
//        
//        cjGLGetVertexCompSizDesc(comp.m_sizType, &compNum, &compVarType);
//        GLboolean doNormalize = GL_FALSE;
//        GLsizei stride = comp.m_stride;
//        cjUINT32 compOffset = comp.m_compOffset;
//        
//        cjVertexCompType compType = comp.m_type;
//        renDev->glEnableVertexAttribArray(compType);
//        renDev->glVertexAttribPointer((GLuint)compType, compNum, compVarType, doNormalize, vtxStride, (void*)compOffset);
//    }
//}

cjGLResCache* cjGLVertexStreamAttribLayout::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
	cjGLVertexStreamAttribLayout* inst = cjGLVertexStreamAttribLayout::NewInst();
	inst->Init(renDev, src->GetCacheID());
    return inst;
}


//
// VertexStream
//

CJSER_IMPL_CLASS(cjGLVertexStream, cjGLResCache)

void cjGLVertexStream::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLResCache::Init(renDev, cacheID);
    m_cachedSiz = 0;
    m_tail = 0;
    
    m_numVertexBuffer = 0;
    m_vtxBufHandle = 0;
}

cjGLVertexStream::~cjGLVertexStream()
{
    ResetStream();
}

void cjGLVertexStream::ResetStream()
{
    if(m_vtxBufHandle)
        m_renDev->glDeleteBuffers(m_numVertexBuffer, &m_vtxBufHandle);
    
    m_numVertexBuffer = 0;
    m_vtxBufHandle = 0;
}

cjGLResCache* cjGLVertexStream::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
    cjFVertexStream* vs = src->GetVertexStream();
    
    if(!vs)
        return 0;

#if defined(CJ_PLATFORM_APPLE)
	cjEAGLVertexStream* inst = new cjEAGLVertexStream;
	inst->Init(renDev, src->GetCacheID());
	return inst;
#else
	cjWGLVertexStream* inst = cjWGLVertexStream::NewInst();
	inst->Init(renDev, src->GetCacheID());
	return inst;
#endif
}



GLenum cjGLVertexStream::ResolveStreamUsageFlag(const cjFVertexStream* vtxStream)
{
	cjUINT32 attrib = vtxStream->GetAttrib();

#if defined(CJ_PLATFORM_APPLE)

	if(attrib & 0xf)
	{
		if(attrib & cjStreamAttr_Usage_StreamDraw) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_StreamDraw);
		if(attrib & cjStreamAttr_Usage_StreamRead) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_StreamRead);
		if(attrib & cjStreamAttr_Usage_StreamCopy) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_StreamCopy);
	}
	else if(attrib & 0xf0)
	{
		if(attrib & cjStreamAttr_Usage_StaticDraw) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_StaticDraw);
		if(attrib & cjStreamAttr_Usage_StaticRead) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_StaticRead);
		if(attrib & cjStreamAttr_Usage_StaticCopy) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_StaticCopy);
	}
	else if(attrib & 0xf00)
	{
		if(attrib & cjStreamAttr_Usage_DynamicDraw) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_DynamicDraw);
		if(attrib & cjStreamAttr_Usage_DynamicRead) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_DynamicRead);
		if(attrib & cjStreamAttr_Usage_DynamicCopy) return cjConvertEAGLStreamAttr(cjStreamAttr_Usage_DynamicCopy);
	}
    
    return 0;

#else

	if(attrib & 0xf)
	{
		if(attrib & cjStreamAttr_Usage_StreamDraw) return GL_STREAM_DRAW;
		if(attrib & cjStreamAttr_Usage_StreamRead) return GL_STREAM_READ;
		if(attrib & cjStreamAttr_Usage_StreamCopy) return GL_STREAM_COPY;
	}
	else if(attrib & 0xf0)
	{
		if(attrib & cjStreamAttr_Usage_StaticDraw) return GL_STATIC_DRAW;
		if(attrib & cjStreamAttr_Usage_StaticRead) return GL_STATIC_READ;
		if(attrib & cjStreamAttr_Usage_StaticCopy) return GL_STATIC_COPY;
	}
	else if(attrib & 0xf00)
	{
		if(attrib & cjStreamAttr_Usage_DynamicDraw) return GL_DYNAMIC_DRAW;
		if(attrib & cjStreamAttr_Usage_DynamicRead) return GL_DYNAMIC_READ;
		if(attrib & cjStreamAttr_Usage_DynamicCopy) return GL_DYNAMIC_COPY;
	}

	return 0;
#endif

}

GLenum cjGLVertexStream::ResolveStreamAccessFlag(const cjFVertexStream* vtxStream)
{
	cjUINT32 attrib = vtxStream->GetAttrib();

#if defined(CJ_PLATFORM_APPLE)
	if(attrib & cjStreamAttr_Access_ReadOnly) return cjConvertEAGLStreamAttr(cjStreamAttr_Access_ReadOnly);
	if(attrib & cjStreamAttr_Access_WriteOnly) return cjConvertEAGLStreamAttr(cjStreamAttr_Access_WriteOnly);
    if(attrib & cjStreamAttr_Access_ReadWrite) return cjConvertEAGLStreamAttr(cjStreamAttr_Access_ReadWrite);
    
    return 0;
    
#else
	if(attrib & cjStreamAttr_Access_ReadOnly) return GL_READ_ONLY;
	if(attrib & cjStreamAttr_Access_WriteOnly) return GL_WRITE_ONLY;
    if(attrib & cjStreamAttr_Access_ReadWrite) return GL_READ_WRITE;

	return 0;
#endif
}


#if defined (CJ_PLATFORM_WIN32)

CJSER_IMPL_CLASS(cjWGLVertexStream, cjGLVertexStream)

void cjWGLVertexStream::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLVertexStream::Init(renDev, cacheID);
}

cjWGLVertexStream::~cjWGLVertexStream()
{
}

void cjWGLVertexStream::Cache(cjFRenResSrc* renResSrc)
{
	cjFVertexStream* srcVtxStream = (cjFVertexStream *)renResSrc;
	
    if(renResSrc->GetCacheRev() == GetCacheRev())
        return;

    cjGLRenDev* renDev = m_renDev;
    SetCacheRev(renResSrc->GetCacheRev());

//    const cjFVertexDecl* vtxDecl = srcVtxStream->GetVertexDecl();
    
    ResetStream();
    
    m_numVertexBuffer = 1;

    renDev->glGenBuffers(m_numVertexBuffer, &m_vtxBufHandle);
    renDev->glBindBuffer(GL_ARRAY_BUFFER, m_vtxBufHandle);
    
    // upload vertex stream data to gpu
/*
GL_STATIC_DRAW should be used for vertex data that is specified once and never changed. 
               Your application should create these vertex buffers during initialization 
			    and use them repeatedly until your application shuts down.

GL_DYNAMIC_DRAW should be used when the buffer is expected to change after it is created. 
                Your application should still allocate these buffers during initialization 
				and periodically update them by mapping the buffer.

GL_STREAM_DRAW is used when your application needs to create transient geometry that is 
               rendered and then discarded. This is most useful when your application 
			   must dynamically change vertex data every frame in a way that cannot be performed 
			   in a vertex shader. To use a stream vertex buffer, your application initially fills 
			   the buffer using glBufferData, then alternates between drawing using the buffer and modifying the buffer.
*/
	cjINT32 vtxStreamByteSiz = srcVtxStream->GetByteSiz();
	GLenum streamUsageFlag = ResolveStreamUsageFlag(srcVtxStream);

    renDev->glBufferData(GL_ARRAY_BUFFER, vtxStreamByteSiz, cjNULL, streamUsageFlag);
	void* buf = renDev->glMapBuffer(GL_ARRAY_BUFFER, ResolveStreamAccessFlag(srcVtxStream));

	cjAssert(buf);
	srcVtxStream->CopyCompStreamIntoSingleStream((cjBYTE*)buf);
	renDev->glUnmapBuffer(GL_ARRAY_BUFFER);
    renDev->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void* cjWGLVertexStream::GetVertexData()
{
	return cjNULL;
}

void cjWGLVertexStream::Reallocate(cjINT32 newSiz)
{
}

cjINT32 cjWGLVertexStream::AddVertices(cjFVertexStream* vertices)
{
	return 0;
}

#endif // end of defined (CJ_PLATFORM_WIN32)


//
// cjGLIndexBuffer
//

CJSER_IMPL_CLASS(cjGLIndexBuffer, cjGLResCache)

void cjGLIndexBuffer::Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynIB)
{
	cjGLResCache::Init(renDev, cacheID);

	m_cachedByteSiz = 0;
	m_indexCount = 0;
	m_isDynIB = isDynIB;
}

cjGLResCache* cjGLIndexBuffer::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
#if defined(CJ_PLATFORM_APPLE)
	cjGLIndexBufferVBO* inst = cjGLIndexBufferVBO::NewInst();
	inst->Init(renDev, src->GetCacheID());
	return inst;
#else
	cjGLIndexBufferVBO* inst = cjGLIndexBufferVBO::NewInst();
	inst->Init(renDev, src->GetCacheID());
	return inst;
#endif
}

cjGLIndexBuffer::~cjGLIndexBuffer()
{
}


//
// cjGLIndexBufferNonVBO
//

CJSER_IMPL_CLASS(cjGLIndexBufferNonVBO, cjGLIndexBuffer)

void cjGLIndexBufferNonVBO::Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynIB)
{
	cjGLIndexBuffer::Init(renDev, cacheID, isDynIB);
}

cjGLIndexBufferNonVBO::~cjGLIndexBufferNonVBO()
{
}

void cjGLIndexBufferNonVBO::Cache(cjFRenResSrc* renResSrc)
{
    if(renResSrc->GetCacheRev() == GetCacheRev())
        return;

    SetCacheRev(renResSrc->GetCacheRev());

//    cjGLRenDev* renDev = m_renDev;
    cjFIndexBuffer* srcIndexBuf = (cjFIndexBuffer*)renResSrc;

	cjINT32	srcSiz = srcIndexBuf->GetByteSiz();

	if(m_cachedByteSiz != srcSiz)
	{
		m_indexDat.RemoveAll(srcSiz); // adjust buf
		m_indexDat.PushEmpty(srcSiz);
	}

	// copy index data from source
	srcIndexBuf->CopyTo(&m_indexDat(0));

	m_cachedByteSiz = srcSiz;
	m_indexCount = srcIndexBuf->GetNumIndex();
}

void cjGLIndexBufferNonVBO::Reallocate(cjINT32 newSiz)
{
	if(!m_isDynIB)
		return;

	m_indexDat.RemoveAll(newSiz); // adjust buf siz
	m_indexDat.PushEmpty(newSiz); // add blink

	m_cachedByteSiz	= newSiz;
	m_indexCount = 0;
}

cjINT32 cjGLIndexBufferNonVBO::AddIndices(cjFIndexBuffer* indexBuf)
{
	cjINT32 siz = indexBuf->GetByteSiz();
	cjINT32 stride = indexBuf->GetStride();

	if(siz > m_cachedByteSiz)
		Reallocate(siz);

	if(m_indexCount + siz > m_cachedByteSiz)
		m_indexCount = 0;

	cjINT32 curStPos = m_indexCount;	
	indexBuf->CopyTo(&m_indexDat(curStPos)); // copy index data from source indexBuf

	m_indexCount = curStPos + siz;

	return curStPos / stride; // return current indexbuffer start pos
}
    

//
// cjGLIndexBufferVBO
//

CJSER_IMPL_CLASS(cjGLIndexBufferVBO, cjGLIndexBuffer)

void cjGLIndexBufferVBO::Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynIB)
{
	cjGLIndexBuffer::Init(renDev, cacheID, isDynIB);
	m_handle = 0;
}

cjGLIndexBufferVBO::~cjGLIndexBufferVBO()
{
	if(m_handle)
		m_renDev->glDeleteBuffers(1, &m_handle);

	m_handle = 0;
}

void cjGLIndexBufferVBO::Cache(cjFRenResSrc* renResSrc)
{
	cjAssert(!m_isDynIB);

	if(renResSrc->GetCacheRev() == GetCacheRev())
		return;

	SetCacheRev(renResSrc->GetCacheRev());
	
	cjFIndexBuffer* srcIndexBuf = (cjFIndexBuffer*)renResSrc;
	cjINT32	siz = srcIndexBuf->GetByteSiz();

	if(!m_handle || m_cachedByteSiz != siz)
	{
		if(!m_handle)
			m_renDev->glGenBuffers(1, &m_handle);

		m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		m_renDev->glBufferData(GL_ELEMENT_ARRAY_BUFFER, siz, cjNULL, GL_STATIC_DRAW );
	}
	else
		m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);

	void* lockedBuf = m_renDev->glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, cjGL_WRITE_ONLY);
	cjAssert(lockedBuf);

	srcIndexBuf->CopyTo(lockedBuf);
	m_renDev->glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	m_cachedByteSiz = siz;
	m_indexCount = srcIndexBuf->GetNumIndex();

	// cjstemp
	// off m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

cjBool cjGLIndexBufferVBO::BindIndexBufferToGL(cjGLRenDev* renDev)
{
	if(!m_handle)
		return cjFalse;

	renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);

	return cjTrue;
}

cjBool cjGLIndexBufferVBO::UnBindIndexBufferToGL(cjGLRenDev* renDev)
{
	// cjstemp
	// off renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return cjTrue;
}


void cjGLIndexBufferVBO::Reallocate(cjINT32 newSiz)
{
	cjAssert(m_isDynIB);

	if(!m_handle)
		m_renDev->glGenBuffers(1, &m_handle);

	m_cachedByteSiz	= newSiz;
	m_indexCount = 0;
}

cjINT32 cjGLIndexBufferVBO::AddIndices(cjFIndexBuffer* indexBuf)
{
	// dynamic index buffer only!!
	cjAssert(m_isDynIB);
	cjINT32	siz = indexBuf->GetByteSiz();

	m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
	m_renDev->glBufferData(GL_ELEMENT_ARRAY_BUFFER, siz, cjNULL, GL_DYNAMIC_DRAW);
	
	// lock writing only buffer
	void* lockedBuf = m_renDev->glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, cjGL_WRITE_ONLY);
	cjAssert(lockedBuf);

	indexBuf->CopyTo(lockedBuf); // into agp
	m_renDev->glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// cjstemp
	// on m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return 0; // return start pos for index
}


//
// cjGLVertexDeclService
//
//
//void cjGLVertexDeclService::PerformVertexAttribFromVertexDecl(cjGLRenDev* renDev, const cjFVertexDecl* decl)
//{
//    cjBYTE* bufOffset = 0;
//
//    for(cjINT32 i=0; i<decl->GetNumComp(); i++)
//    {
//        const cjVertexComponent& comp = decl->GetComp(i);
//        
//        GLint compNum = 1;  // x=1, xy=2, xyz=3, xyzw=4
//        GLenum compVarType = GL_FLOAT;
//        
//        cjGLGetVertexCompSizDesc(comp.m_siz, &compNum, &compVarType);
//        GLboolean doNormalize = GL_FALSE;
//        GLsizei stride = comp.m_siz;
//        
//        cjVertexCompType compType = comp.m_type;
//        renDev->glEnableVertexAttribArray(compType);
//        renDev->glVertexAttribPointer((GLuint)compType, compNum, compVarType, doNormalize, stride, (void*)bufOffset);
//        
//        bufOffset += stride;
//    }
//}


//
// cjGLShaderCache
//

CJSER_IMPL_CLASS(cjGLShaderCache, cjGLResCache)

void cjGLShaderCache::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLResCache::Init(renDev, cacheID);
}

cjGLShaderCache::~cjGLShaderCache()
{
}

cjGLResCache* cjGLShaderCache::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
    cjFShaderProgram* sp = src->GetShaderProgram();
    
    if(sp)
	{
		cjGLShaderProgram* spInst = cjGLShaderProgram::NewInst();
		spInst->Init(renDev, src->GetCacheID());
		return spInst;
	}
    
    return cjGLCompileShader::CreateInstance(glResRTTI, renDev, src);
}


//
// cjGLShaderProgram
//

CJSER_IMPL_CLASS(cjGLShaderProgram, cjGLShaderCache)

void cjGLShaderProgram::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLShaderCache::Init(renDev, cacheID);

	m_shaderFlag = 0;
	m_lastFrameUsed = 0;

	m_handle = cjNULL;
    
    m_vertexAttribInfo = cjNULL;
    m_uniformValSet = cjNULL;
}

cjGLShaderProgram::~cjGLShaderProgram()
{
    Reset();
}

void cjGLShaderProgram::Reset()
{
    if(m_vertexAttribInfo)
    {
        delete m_vertexAttribInfo;
        m_vertexAttribInfo = cjNULL;
    }

    if(m_uniformValSet)
    {
        delete m_uniformValSet;
        m_uniformValSet = cjNULL;
    }
    
 	if(m_handle)
	{
		m_renDev->glDeleteProgram(m_handle);
		m_handle = cjNULL;
	}

	m_shaderFlag = 0;
}

void cjGLShaderProgram::Cache(cjFRenResSrc* renResSrc)
{
	cjAssert(GetCacheID() == renResSrc->GetCacheID());
	if(GetCacheRev() == renResSrc->GetCacheRev())
		return;
	
	SetCacheRev(renResSrc->GetCacheRev());

    cjFShaderProgram* srcSP = (cjFShaderProgram*)renResSrc;

    Reset();
    
    ///////////////////////////////////////////////////////////////////////////

	cjGLRenDev* renDev = m_renDev;
	cjFVertexShader* vtxShader = srcSP->GetVertexShader();
	cjFPixelShader* pxlShader = srcSP->GetPixelShader();

	if(!vtxShader || !pxlShader)
	{
		m_shaderFlag |= EFL_FAIL_SRC_VERTEX_SHADER | EFL_FAIL_SRC_PIXEL_SHADER;
		cjLOG("[GL] Warn! - caching for cjGLShaderProgram is failed..[CACHE SRC FAIL]");
		return;
	}

	cjGLVertexShader* vtxShaderCache = (cjGLVertexShader*)renDev->FindResCache(vtxShader->GetCacheID());
	if(!vtxShaderCache)
	{
		vtxShaderCache = cjGLVertexShader::NewInst();
		vtxShaderCache->Init(m_renDev, vtxShader->GetCacheID());
	}
	vtxShaderCache->Cache(vtxShader);

	cjGLPixelShader* pxlShaderCache = (cjGLPixelShader*)renDev->FindResCache(pxlShader->GetCacheID());
	if(!pxlShaderCache)
	{
		pxlShaderCache = cjGLPixelShader::NewInst();
		pxlShaderCache->Init(m_renDev, pxlShader->GetCacheID());
	}
	pxlShaderCache->Cache(pxlShader);

//	if(!vtxShaderCache->IsCached() || !pxlShaderCache->IsCached())
// {
//		m_shaderFlag |= EFL_FAIL_CACHE_VERTEX_SHADER | EFL_FAIL_CACHE_PIXEL_SHADER;
//		cjLOG("[GL] Warn! - caching for cjGLShaderProgram is failed..[CACHE FAIL]");
//		return;
//	}
 
	m_handle = renDev->glCreateProgram();

	renDev->glAttachShader(m_handle, vtxShaderCache->GetShaderHandle());
	renDev->glAttachShader(m_handle, pxlShaderCache->GetShaderHandle());

	// this must be performed before linking shader or call again glLinkProgram
	// renDev->glBindAttribLocation(m_handle, EVTXAttrPosition, "a_position");
	// renDev->glBindAttribLocation(m_handle, EVTXAttrNormal, "a_normal");

	/*
		in shader
		layout(location = attribute index) in vec3 position;
		layout(location = 2) in vec3 values[4];
		>> This will allocate the attribute indices 2, 3, 4, and 5. (value[4])

		layout(location = 0) out vec4 color;
		layout(location = 1) out vec2 texCoord;
		layout(location = 2) out vec3 normal;
		
		layout(binding = 3) uniform sampler2D mainTexture;
		layout(binding = 1, std140) uniform MainBlock
		{
			vec3 data;
		};

		renDev->glGenVertexArrays(1, &g_vertexArray);

		//renDev->glBindVertexArray(g_vertexArray);
    
		renDev->glGenBuffers(1, &g_vertexBuffer);
		renDev->glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
		renDev->glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexData), gCubeVertexData, GL_STATIC_DRAW);
    
		renDev->glEnableVertexAttribArray(EVTXAttrPosition);
		renDev->glVertexAttribPointer(EVTXAttrPosition, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
		renDev->glEnableVertexAttribArray(EVTXAttrNormal);
		renDev->glVertexAttribPointer(EVTXAttrNormal, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
    
		//renDev->glBindVertexArray(0);

	*/


	// link
	renDev->glLinkProgram(m_handle);

    // find active attribute locations. this must be performed after linking shader
    m_vertexAttribInfo = new cjGLVertexAttribInfo(renDev, m_handle);

	if(!renDev->CheckGLErrStatus(m_handle, GL_LINK_STATUS, __CJFUNCTION__, __CJLINE__))
	{
		m_shaderFlag |= EFL_FAIL_LINK;
		cjLOG("[GL] Warn! - caching for cjGLShaderProgram is failed..[FAIL_LINK]");
	}
	
/*
	1.Compile the vertex shader source into a shader object.
	2.Compile the fragment shader source into a shader object.
	3.Create a program ID with glCreateProgram.
	4.Attach the vertex and shader objects to our program with glAttachShader. 
	5.If we are targeting 2.1, here is where we would bind any attribute locations manually. 
	  For example, we would bind the Position attribute to index 0. 
	  For this we use glBindAttribLocation. If we are targeting newer versions of GLSL, we can skip this step.
	6.We then link the program with glLinkProgram.
	7.If the program succeeded in compiling, we can now detach and delete the vertex 
	  and fragment shader objects as they are no longer needed 
	  -- using glDetachShader and glDeleteShader, respectively. 

	  These are only flags for OpenGL, and the objects will be deleted 
	  when they are no longer associated with any rendering states.
*/

	//detach shaders which are no longer needed
	renDev->glDetachShader(m_handle, vtxShaderCache->GetShaderHandle());
	renDev->glDetachShader(m_handle, pxlShaderCache->GetShaderHandle());
    
    // get uniform location & description
    m_uniformValSet = new cjGLShaderUniformValSet;
    m_uniformValSet->Generate(renDev, m_handle);
}


//
// cjGLCompileShader
//

CJSER_IMPL_CLASS(cjGLCompileShader, cjGLShaderCache)

char cjGLCompileShader::ms_preShaderMacroDefine[cjGLCompileShader::MAX_GLSL_MACRO_LEN] = { "  \n" };
cjINT32 cjGLCompileShader::ms_preShaderMacroDefineLen = (cjINT32)strlen(" \n");

void cjGLCompileShader::Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjRenShaderType shaderType)
{
	cjGLShaderCache::Init(renDev, cacheID);

	m_shaderFlag = 0;
	m_shaderType = shaderType;
	m_handle = cjNULL;
}

cjGLCompileShader::~cjGLCompileShader()
{
	ReleaseShaderHandle();
}

void cjGLCompileShader::ReleaseShaderHandle()
{
	if(m_handle)
	{
		m_renDev->glDeleteShader(m_handle);
		m_handle = cjNULL;
	}
}

cjBool cjGLCompileShader::Compile(GLhandle shaderHandle, const GLbyte* shaderSrc,
	GLint shaderSrcLen, cjBool force)
{
	if(!shaderHandle)
	{
		SetShaderFlag(EFL_FAIL_COMPILE);
		return cjFalse;
	}

	if(!(m_shaderFlag & EFL_COMPILE_NEED))
		return cjFalse;

	if(force)
		UnSetShaderFlag(EFL_FAIL_COMPILE);

	if(m_shaderFlag & EFL_FAIL_COMPILE)
		return cjFalse;

	cjBool rst = cjTrue;
	cjGLRenDev* renDev = m_renDev;

	// upload shader string
	// if shaderSrcLen is null then it assums shaderSrc is ends with null terminator
//    const char* shaderSrcAry[2] = { ms_preShaderMacroDefine,
//        (const char*)shaderSrc };

//    const char* shaderSrcAry[2] = { ms_preShaderMacroDefine, (const char*)shaderSrc };
//    GLint sizShaderSrc = ms_preShaderMacroDefineLen + shaderSrcLen;
    
//	renDev->glShaderSource(
//        shaderHandle,
//        2,
//        (const GLchar**)shaderSrcAry,
//        &sizShaderSrc);
    
	renDev->glShaderSource(
        shaderHandle,
        1,
        (const GLchar**)&shaderSrc,
        &shaderSrcLen);
    
	
	if(cjGLRenDev::GetLastGLErr(&rst))
		SetShaderFlag(EFL_FAIL_COMPILE_BADSRC);

	cjBool compiled = cjFalse;

	if(rst)
	{ 
		// compile shader
		renDev->glCompileShader(shaderHandle);

		if(renDev->CheckGLErrStatus(shaderHandle, GL_COMPILE_STATUS, __CJFUNCTION__, __CJLINE__))
			compiled = cjTrue;
	}
		
	if(!compiled)
	{
		SetShaderFlag(EFL_FAIL_COMPILE);
		return cjFalse;
	}

	UnSetShaderFlag(EFL_COMPILE_NEED);
	SetShaderFlag(EFL_COMPILE_OK);	

	return cjTrue;		
}

void cjGLCompileShader::DoCache(cjFShader* src)
{
	cjAssert(src->GetCacheID() == GetCacheID());

	if(src->GetCacheRev() == GetCacheRev())
		return;

	SetCacheRev(src->GetCacheRev());

	ResetShaderFlag();
	ReleaseShaderHandle();

	cjGLRenDev* renDev = m_renDev;

	cjTFLAG flag = 0;

	if(src->IsPreCompiled())
		flag |= EFL_PRECOMPILED;
	else
		flag |= EFL_COMPILE_NEED;

	SetShaderFlag(flag);

	m_handle = renDev->glCreateShader(m_shaderType == cjShader_VertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

	if(!m_handle)
	{
		SetShaderFlag(EFL_FAIL_CREATE_SHADEROBJ);
		return;
	}

	if(!Compile(m_handle, (const GLbyte*)src->GetShaderSrc(), src->GetShaderLen()))
	{
		ReleaseShaderHandle();
		return;
	}
}

cjGLResCache* cjGLCompileShader::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
    cjFShader* sh = src->GetShader();
    
    if(!sh)
        return 0;
    
    if(sh->GetVertexShader())
	{
		cjGLVertexShader* inst = cjGLVertexShader::NewInst();
		inst->Init(renDev, src->GetCacheID());
	}
    else if(sh->GetPixelShader())
	{
		cjGLPixelShader* inst = cjGLPixelShader::NewInst();
		inst->Init(renDev, src->GetCacheID());
	}
    
    return 0;
}

void cjGLCompileShader::AddCompileMacro(const char* macroStr)
{

}


// 
// cjGLVertexShader
//

CJSER_IMPL_CLASS(cjGLVertexShader, cjGLCompileShader)

cjGLVertexShader::~cjGLVertexShader()
{
}

void cjGLVertexShader::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLCompileShader::Init(renDev, cacheID, cjShader_VertexShader);
}

void cjGLVertexShader::Cache(cjFRenResSrc* renResSrc)
{
	cjAssert(renResSrc->GetCacheID() == GetCacheID());
	DoCache((cjFShader*)renResSrc);
}

//
// cjGLPixelShader
//

CJSER_IMPL_CLASS(cjGLPixelShader, cjGLCompileShader)

cjGLPixelShader::~cjGLPixelShader()
{
}

void cjGLPixelShader::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLCompileShader::Init(renDev, cacheID, cjShader_PixelShader);
}

void cjGLPixelShader::Cache(cjFRenResSrc* renResSrc)
{
	cjAssert(renResSrc->GetCacheID() == GetCacheID());
	DoCache((cjFShader*)renResSrc);
}


//
////
////	cjDynamicVtxStream
////
//
//cjDynamicVtxStream::cjDynamicVtxStream()
//{
//	m_cacheId = cjMakeCacheID(CID_RenderVertices);
//	m_rev = 1;
//}
//
//void cjDynamicVtxStream::Init(cjBYTE** rstBuf, cjINT32 numVtx, cjINT32 stride,
//	cjTFLAG componentFlag)
//{
//	m_siz = numVtx * stride;
//	m_stride = stride;
//	m_componentFlag = componentFlag;
//
//    if(m_siz > m_verticesAsByte.GetSiz()) // extend
//		m_verticesAsByte.Insert(m_siz - m_verticesAsByte.GetSiz());
//
//	*rstBuf = &m_verticesAsByte(0);
//	m_rev++;
//}
//
//cjINT32 cjDynamicVtxStream::GetComponents(cjVertexComponent* rstComponents)
//{
//    cjINT32 numComps = 0;
//
//    if(m_componentFlag & VF_Position )
//    {
//        rstComponents[numComps] = cjVertexComponent(CT_Float3,FVF_Position);
//        numComps++;
//    }
//    if(m_componentFlag & VF_Normal )
//    {
//        rstComponents[numComps] = cjVertexComponent(CT_Float3,FVF_Normal);
//        numComps++;
//    }
//    if(m_componentFlag & VF_Diffuse )
//    {
//        rstComponents[numComps] = cjVertexComponent(CT_Color,FVF_Diffuse);
//        numComps++;
//    }
//    if(m_componentFlag & VF_Specular )
//    {
//        rstComponents[numComps] = cjVertexComponent(CT_Color,FVF_Specular);
//        numComps++;
//    }
//    if(m_componentFlag & VF_Tex1 )
//    {
//        rstComponents[numComps] = cjVertexComponent(CT_Float2,FVF_TexCoord0);
//        numComps++;
//    }
//    if(m_componentFlag & VF_Tex2 )
//    {
//        rstComponents[numComps] = cjVertexComponent(CT_Float2,FVF_TexCoord1);
//        numComps++;
//    }
//    return numComps;
//}
//
//void cjDynamicVtxStream::GetStreamData(void* dest)
//{
//	cjMemCpy(dest, &m_verticesAsByte(0), m_siz);
//}
//
//void cjDynamicVtxStream::GetRawStreamData(void** dest, cjINT32 firstVertex)
//{
//	*dest = cjNULL;
//}
//


//
// cjGLRenTargetGrp
//

CJSER_IMPL_CLASS(cjGLRenTargetGrp, cjGLResCache)

void cjGLRenTargetGrp::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLResCache::Init(renDev, cacheID);

	m_fboHandle = 0;
	cjMemSet(m_renTarget, 0, sizeof(m_renTarget));
}

cjGLRenTargetGrp::~cjGLRenTargetGrp()
{
    ReleaseFrameBufferObj();
}

void cjGLRenTargetGrp::Cache(cjFRenResSrc* renResSrc)
{
	if(!renResSrc)
		return;

	if(GetCacheRev() == renResSrc->GetCacheRev())
		return;

	SetCacheRev(renResSrc->GetCacheRev());

	cjGLRenDev* renDev = m_renDev;
    cjFRenTargetGrp* renTargetGrp = (cjFRenTargetGrp*)renResSrc;

	if(!m_fboHandle)
	{
		cjLOG("[GL] using glGenFramebuffers..");
		renDev->glGenFramebuffers(1, &m_fboHandle);
		renDev->GetLastGLErr();

		// check framebuffer status
		GLenum status = renDev->glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if((status != GL_FRAMEBUFFER_COMPLETE) || renDev->GetLastGLErr())
		{
			cjLOG("[GL] Error in framebuffer object operation.. %s", 
				cjGLRenDev::GetGLFrameBufferStatusText(status));
			return;
		}
	}

	for(cjINT32 i=0; i<cjFRenTargetGrp::MAX_MULTI_RENDER_TARGETS; i++)
	{
		cjFRenTarget* rt = renTargetGrp->GetRenTarget(i);
		cjGLRenTarget* rtCache = cjNULL;

		if(rt)
		{
			rtCache = (cjGLRenTarget*)renDev->FindResCache(rt->GetCacheID());

			if(!rtCache)
			{
				rtCache = cjGLRenTarget::NewInst();
				rtCache->Init(renDev, rt->GetCacheID());
				//((cjGLRenTarget*)rtCache)->Init(renDev, rt->GetCacheID());
			}

			if(rt->GetCacheRev() != rtCache->GetCacheRev())
				rtCache->Cache(rt);
		}

		m_renTarget[i] = rtCache;
	}

}

void cjGLRenTargetGrp::SetRenderTarget(cjINT32 index, cjGLRenTarget* rtg)
{
	m_renTarget[index] = rtg;
}

cjBool cjGLRenTargetGrp::BeginRenTargetGrp()
{
	if(!m_fboHandle)
		return cjFalse;

	cjGLRenDev* renDev = m_renDev;

//	cjGLRenTargetGrp* prevRenTargetGrp = renDev->FindResCache( );
//	if(renDev->GetCurGLRenTargetGrp() == this)
//	{
//		cjAssert("Err! - This renderTargetGrp is already actived..");
//		return cjFalse;
//	}

	if(!m_renTarget[0])
	{
		cjLOG("Err! - first render target is invalid..");
		return cjFalse;
	}

 	renDev->glBindFramebuffer(GL_FRAMEBUFFER, m_fboHandle);
    
    GLuint defaultColorSurHandle = 0;

	for(cjINT32 i=0; i<cjFRenTargetGrp::MAX_MULTI_RENDER_TARGETS; i++)
	{
		cjGLRenTarget* cur = m_renTarget[i];
        
		GLhandle colorSurHandle = cur ? cur->GetColorSurface() : 0;
		GLenum colorSurType = cur ? cur->GetColorSurfaceType() : GL_RENDERBUFFER; // GL_RENDERBUFFER or GL_TEXTURE2D

		// attach color render target to framebuffer
        if(colorSurType == GL_RENDERBUFFER)
        {
            renDev->glBindRenderbuffer(GL_RENDERBUFFER, colorSurHandle);
            renDev->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_RENDERBUFFER, colorSurHandle);
        }
        else
        {
            renDev->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, colorSurType, colorSurHandle, 0);
        }
        
		if(i == 0)
		{
            defaultColorSurHandle = cur->GetColorSurface();
			GLhandle dsHandle = cur->GetDepthStencil();

            if(dsHandle)
            {
                // attach depth-stencil to framebuffer
                renDev->glBindRenderbuffer(GL_RENDERBUFFER, dsHandle);
                renDev->glRenderbufferStorage(GL_RENDERBUFFER, cur->GetDepthStencilGLFmt(),
                    cur->GetWidth(), cur->GetHeight());
//                renDev->glRenderbufferStorage(GL_RENDERBUFFER, 33190, 640, 1136);

                renDev->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dsHandle);
            }
		}

       	renDev->CheckGLFrameBufferStatus();
	}
    
    renDev->glBindRenderbuffer(GL_RENDERBUFFER, defaultColorSurHandle);

	GLenum status = renDev->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	return status == GL_FRAMEBUFFER_COMPLETE ? cjTrue : cjFalse;
}

void cjGLRenTargetGrp::EndRenTargetGrp()
{
	if(!m_fboHandle)
		return;

	cjGLRenDev* renDev = m_renDev;

	//if(renDev->GetCurGLRenTargetGrp() != this)
	//{
	//	cjAssert("Err! - current renderTargetGrp is not match..");
	//	return;
	//}

	renDev->glBindFramebuffer(GL_FRAMEBUFFER, m_fboHandle);

	// reset color surface
 	for(cjINT32 i=0; i<cjFRenTargetGrp::MAX_MULTI_RENDER_TARGETS; i++)
 		renDev->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, 0, 0);

    // reset depth buffer
 	renDev->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);

 	renDev->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cjGLRenTargetGrp::ReleaseFrameBufferObj()
{
	if(m_fboHandle)
	{
		m_renDev->glDeleteFramebuffers(1, &m_fboHandle);
		m_fboHandle = 0;
	}
}

cjGLResCache* cjGLRenTargetGrp::CreateInstance(const cjRTTI* glResRTTI, 
	cjGLRenDev* renDev, cjFRenResSrc* src)
{
    if(!src->GetRenTargetGrp())
        return 0;

	cjGLRenTargetGrp* grpInst = cjGLRenTargetGrp::NewInst();
	grpInst->Init(renDev, src->GetCacheID());
    
    return grpInst;
}


//
// cjGLRenTarget
//

CJSER_IMPL_CLASS(cjGLRenTarget, cjGLTexture)

void cjGLRenTarget::Init(cjGLRenDev* renDev, cjResCacheID cacheID)
{
	cjGLTexture::Init(renDev, cacheID);

	m_handleColor = 0;
	m_colorSurType = GL_RENDERBUFFER;
	m_handleDepthStencil = 0;
    m_depthStencilGLFmt = 0;
}

cjGLRenTarget::~cjGLRenTarget()
{
	ReleaseColorSurface();
	ReleaseDepthStencil();
}

void cjGLRenTarget::ReleaseColorSurface()
{
	if(m_handleColor)
	{
		m_renDev->glDeleteTextures(1, &m_handleColor);
		m_handleColor = cjNULL;
	}
}

void cjGLRenTarget::ReleaseDepthStencil()
{
	if(m_handleDepthStencil)
	{
		m_renDev->glDeleteRenderbuffers(1, &m_handleDepthStencil);
		m_handleDepthStencil = cjNULL;
	}
}

void cjGLRenTarget::Cache(cjFRenResSrc* renResSrc)
{
	if(!renResSrc || !renResSrc->GetRenTarget())
		return;

	if(GetCacheRev() == renResSrc->GetCacheRev())
		return;

	SetCacheRev(renResSrc->GetCacheRev());

	ReleaseColorSurface();
	ReleaseDepthStencil();

	cjGLRenDev* renDev = m_renDev;
    cjFRenTarget* renTarget = (cjFRenTarget*)renResSrc;
    
	GLint width = renTarget->GetWidth();
	GLint height = renTarget->GetHeight();
    
    m_width = width;
    m_height = height;
	m_depthStencilGLFmt = 0;
    
	cjGLRenDevParam* renDevPrm = m_renDev->GetGLRenDevParam();

	if(renTarget->GetFormat() != cjRSurf_UNKNOWN)
	{
		GLenum colorFmt = GL_RGBA;
		GLenum colorCompSiz = GL_UNSIGNED_BYTE; // 8bit color
		GLint mipMap = 0;
		GLint level = 0;

		m_colorSurType = GL_TEXTURE_2D;

		renDev->glGenTextures(1, &m_handleColor);
		m_renDev->GetLastGLErr();

		renDev->glBindTexture(m_colorSurType, m_handleColor);
		m_renDev->GetLastGLErr();

		renDev->glTexParameteri(m_colorSurType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_renDev->GetLastGLErr();

		renDev->glTexParameteri(m_colorSurType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		m_renDev->GetLastGLErr();

		renDev->glTexImage2D(m_colorSurType, level, GL_RGBA, width, height, 0, colorFmt, colorCompSiz, cjNULL);
		m_renDev->GetLastGLErr();

		renDev->glTexParameteri(m_colorSurType, GL_TEXTURE_MAX_LEVEL, mipMap);
		m_renDev->GetLastGLErr();
		
		if(renDev->ms_caps_cjGL_EXT_texture_filter_anisotropic)
			renDev->glTexParameterf(m_colorSurType, GL_TEXTURE_MAX_ANISOTROPY_EXT,  renDevPrm->GetGLAnisoLevel());

		m_renDev->GetLastGLErr();
	}
	
	if(renTarget->HasDepthStencil())
	{
		GLenum glDepthFmt = 0;
		cjRSurfaceFmt dsFormat = renTarget->GetDSFormat();

		if(dsFormat == cjRSurf_D16)
			glDepthFmt = GL_DEPTH_COMPONENT16;
		else if(dsFormat == cjRSurf_D24X8)
			glDepthFmt = GL_DEPTH_COMPONENT24;
		else if(dsFormat == cjRSurf_D32)
			glDepthFmt = GL_DEPTH_COMPONENT32;

		m_depthStencilGLFmt = glDepthFmt;

		if(glDepthFmt != 0)
		{
			renDev->glGenRenderbuffers(1, &m_handleDepthStencil); //depth stencil, renderable texture
			renDev->glBindRenderbuffer(GL_RENDERBUFFER, m_handleDepthStencil);
			renDev->glRenderbufferStorage(GL_RENDERBUFFER, glDepthFmt, width, height); // alloc
			renDev->glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	if(m_renDev->GetLastGLErr())
	{
		cjLOG(_CT("[GL] Failed to create render target!"));
		ReleaseColorSurface();
		ReleaseDepthStencil();
	}
    
}

cjGLResCache* cjGLRenTarget::CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src)
{
    if(!src->GetRenTarget())
        return 0;

	cjGLRenTarget* rtInst = cjGLRenTarget::NewInst();
	rtInst->Init(renDev, src->GetCacheID());
    
    return rtInst;
}


//
////
//// cjGLRenColorSurface
////
//
//cjGLRenColorSurface::cjGLRenColorSurface(cjGLRenDev* renDev, cjResCacheID cacheID)
//	: cjGLRenTarget(renDev, cacheID)
//{  
//}
//
//cjGLRenColorSurface::~cjGLRenColorSurface()
//{
//	ReleaseSurface();
//}
//
//void cjGLRenColorSurface::ReleaseSurface()
//{
//	if(GetRenDev() && m_handle)
//	{
//		GetRenDev()->glDeleteTextures(1, &m_handle);
//		m_handle = cjNULL;
//	}
//}
//
//void cjGLRenColorSurface::Cache(cjFRenTarget* renTarget)
//{
//	if(!renTarget)
//		return;
//
//	if(GetCacheRev() == renTarget->GetCacheRev())
//		return;
//	SetCacheRev(renTarget->GetCacheRev());
//
//	ReleaseSurface();
//
//	if(renTarget->GetType() != cjFRenTarget::ERT_COLOR);
//		return;
//
//	cjAssert(m_handle);
//	cjGLRenDev* renDev = GetRenDev();
//
//	GLint width = renTarget->GetWidth();
//	GLint height = renTarget->GetHeight();
//
//	renDev->glGenTextures(1, &m_handle);
//	renDev->glBindTexture(  GL_TEXTURE_2D, m_handle);
//	renDev->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	renDev->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	renDev->glTexImage2D(   GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cjNULL);
//	renDev->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
//	
//	if(renDev->ms_caps_cjGL_EXT_texture_filter_anisotropic)
//		renDev->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, renDev->GetRenDevParam()->GetTexAnisoLevel());
//
//	renDev->glBindTexture(GL_TEXTURE_2D, 0);
//}
//
//
////
//// cjGLRenDepthStencil
////
//
//cjGLRenDepthStencil::cjGLRenDepthStencil(cjGLRenDev* renDev, cjResCacheID cacheID)
//	: cjGLRenTarget(renDev, cacheID)
//{
//}
//
//cjGLRenDepthStencil::~cjGLRenDepthStencil()
//{
//}
//
//void cjGLRenDepthStencil::Cache(cjFRenTarget* renTarget)
//{
//	if(!renTarget)
//		return;
//
//	if(GetCacheRev() == renTarget->GetCacheRev())
//		return;
//
//	SetCacheRev(renTarget->GetCacheRev());
//
//	ReleaseRT();
//
//	if(renTarget->GetType() != cjFRenTarget::ERT_DEPTHSTENCIL);
//		return;
//
//
//	RenDev->glGenRenderbuffersEXT(1, &DepthStencilSurface);
//
//	if(colorSurface)
//	{
//		
//	}
//	else if(depthStencil)
//	{
//		if(renTarget->GetFormat() == cjRSurf_D16)
//			glDepthFmt = GL_DEPTH_COMPONENT16;
//		else if(renTarget->GetFormat() == cjRSurf_D24X8)
//			glDepthFmt = GL_DEPTH_COMPONENT24;
//		else if(renTarget->GetFormat() == cjRSurf_D32)
//			glDepthFmt = GL_DEPTH_COMPONENT32;
//	}
//
//		
//	if(glDepthFmt)
//	{
//
//		if(colorSurface)
//			renDev->glDeleteFramebuffersEXT(1, & RenderTargetSurface);
//
//		if(depthStencil)
//			renDev->glDeleteRenderbuffersEXT(1, &depthStencilSurface);
//
//	}	
//
//}
