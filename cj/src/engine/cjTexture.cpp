
#include "cjEnginePCH.h"
#include "cjTexture.h"


//
// cjTexture
//

static void SerializeMips(cjTexture* tex, cjFStream& stream, cjTAry<cjFMipmap>& mips)
{
	CJ_UNREF_VALUE(tex);

	if(stream.IsObjOnly())
		return;

	if(stream.IsLoading())
	{
		cjINT32 loadMipCount;

		stream << cjFStreamCmprI32(loadMipCount);
		mips.RemoveAll(loadMipCount);

		//		cjINT32 lod = tex->GetDefaultLOD();
		for(cjINT32 i=0; i<loadMipCount; i++)
			stream << *(::new(mips) cjFMipmap);
	}
	else if(stream.IsSaving())
	{	
		// Save
		cjINT32 mipCount = mips.Count();

		stream << cjFStreamCmprI32(mipCount);

		for(cjINT32 i=0; i<mips.Count(); i++)
			stream << mips(i);
	}
}


CJOBJ_IMPL_CLASS(cjTexture)
CJOBJ_IMPL_SERIALIZE_BEGIN(cjTexture)
	SerializeMips(this, stream, m_mip);
CJOBJ_IMPL_SERIALIZE_END(cjTexture)


cjTexture::cjTexture()
{
    m_hasAlphaChannel = cjFalse;
    m_renderTwoSide = cjFalse;

    m_mipCreateErr = cjFalse;
    
    m_procedural = cjFalse;
    m_lastTextureSurfaceUpdatedTime = cjET(0);
    
    m_doDirtyEveryUpdate = cjFalse;
    m_dirtyTexture = cjFalse;
    
    m_texAniNext = NULL;
    m_texAniCur = NULL;
    
    m_texAniPeroid = cjET(0);
    m_texAniFrameElapsed = cjET(0);
    m_texLastUpdateTime = cjET(0);
    m_texAccumTime = cjET(0);
    
    m_texInterface = NULL;
    m_maskTexture = NULL;
    m_alphaTexture = NULL;
    
    m_twoSided = cjFalse;
    m_maskTexture = NULL;
    m_alphaTexture = NULL;
    
    m_texInterface = new cjFStaticTexture(this);
}

void cjTexture::OnDestroy()
{
    CJ_SAFE_DELETE(m_texInterface);
    cjSuper::OnDestroy();
}

cjBitmapMaterial* cjTexture::Get(cjEngineTime_t curTime, cjViewport* viewport)
{
//    Update( curTime );
//    return m_texAniCur ? m_texAniCur : this;
    return this;
}

void cjTexture::CreateMip(cjINT32 mipLevel, cjBool doAutoGenMip)
{
    if(mipLevel == 1)
        return;
    
    cjINT32 endMipLevel = 0;
    
    cjINT32 w = GetMaterialUSiz();
    cjINT32 h = GetMaterialVSiz();
    cjRSurfaceFmt fmt = GetFormat();
    
    cjBYTE cpw = cjGetNearestPOTBit(w);
    cjBYTE cph = cjGetNearestPOTBit(h);

    while(cpw && cph)
    {
       cpw--; cph--;

       if(!cpw || !cph)
          break;
            
        endMipLevel++;
    }
    
    if(mipLevel != 0 && endMipLevel > mipLevel)
        endMipLevel = mipLevel;

    // Preseve mip level 0 !!
	if(m_mip.Num() > 1)
		m_mip.Remove(1, m_mip.Num()-1);
    
    // Allocating space for mip level data 1~N
	if(endMipLevel)
	{
        cjBYTE potBitU = cjGetNearestPOTBit(w);
        cjBYTE potBitV = cjGetNearestPOTBit(h);

        for(cjINT32 i=1; i<endMipLevel; i++, potBitU--, potBitV--)
        {
			switch(fmt)
			{
			case cjRSurf_RGBA8:
				::new(m_mip) cjFMipmap(potBitU, potBitV, (1 << potBitU) * (1 << potBitV) * 4);
				break;
                
//			case cjRSurf_XRGB8:
//				::new(m_mip) cjFMipmap(potBitU, potBitV, (1 << potBitU) * (1 << potBitV) * 3);
//				break;
                
            default:
                cjAssert(_CT("ERR! - cjTexture::CreateMip.. (1)"));
                break;
			}
        }

	} // end of if(endMipLevel)
}


/*
	called after

	1. cjObjNew<cjTexture>(..)
	2. _ImportTextureFromFile(..)
*/
cjBool cjTexture::Init(cjINT32 sizU, cjINT32 sizV, cjRSurfaceFmt fmt)
{
    m_sizU = sizU;
    m_sizV = sizV;
    m_format = fmt;
    
    m_potbitU = cjGetNearestPOTBit(sizU);
    m_potbitV = cjGetNearestPOTBit(sizV);

    // create mips
    m_mipCreateErr = cjFalse;
    m_mip.RemoveAll();
    
    m_minLOD = m_maxLOD = m_curLOD = 0;
    
    // create space for mip level 0
    switch(m_format)
    {
    case cjRSurf_RGBA8:
		::new(m_mip) cjFMipmap(m_potbitU, m_potbitV, m_sizU * m_sizV * 4); // ARGB8 8(Byte)*4
        break;
        
    case cjRSurf_RGB8:
        // most GPUs like chunks of 4 bytes
        // In other words, GL_RGBA??or GL_BGRA??is preferred when each component is a byte.
        // most CPUs and any other kind of chip don't handle 24 bits
        // This means, the driver converts your GL_RGB??or GL_BGR??to what the GPU prefers,
        // which typically is BGRA
        
        // force RGB8 -> RGBA8
        ::new(m_mip) cjFMipmap(m_potbitU, m_potbitV, m_sizU * m_sizV * 4);
        break;
        
    default:
        m_mipCreateErr = cjTrue;
        return cjFalse;
    }
    
    if(!m_mipCreateErr)
        m_mip(0).Clear();
    
    return cjTrue;
}

void cjTexture::cjObjPostLoad()
{
	cjSuper::cjObjPostLoad();

    m_curLOD = GetMaterialUSiz() > 1024 ? 1 : 0;

    // determine maxLOD
	m_maxLOD = m_mip.Num() - 1;
    
	for(cjAryOff i=0; i<m_mip.Num(); i++)
	{
        cjINT32 sizU = m_mip(i).m_potSizU;
        cjINT32 sizV = m_mip(i).m_potSizV;
        
        // [128, 64, 32, 16] x [32, 16]
		if( (sizU<64) || ((sizV<64) && (sizU<256 && sizV<256)))
		{
            m_maxLOD = i;
			break;
		}
	}
    
    m_minLOD = cjMin(m_curLOD, m_maxLOD);

	// for animation
    m_texAccumTime = 0;
	SetLastUpdateTime(cjGetEngineTimeSec());
}

void cjTexture::Tick(cjEngineTime_t elapsedTime)
{
    if(m_texAniPeroid != 0)
    {
        m_texAccumTime += elapsedTime;
        
        if(m_texAccumTime > m_texAniPeroid)
        {
            m_texAccumTime -= m_texAniPeroid;
            
            if(m_texAccumTime > m_texAniPeroid)
                m_texAccumTime = m_texAniPeroid;
            
            if(!m_texAniCur) m_texAniCur = this;
            m_texAniCur = m_texAniCur->m_texAniNext;
            if(!m_texAniCur) m_texAniCur = this;
        }
    }
}

void cjTexture::Update(cjEngineTime_t curTime)
{
	if(curTime > GetLastUpdateTime())
	{
		if(m_doDirtyEveryUpdate)
			m_dirtyTexture = 1;
        
		Tick(curTime - GetLastUpdateTime());
	}

	SetLastUpdateTime(curTime);
}

cjRSurfaceFmt cjTexture::ConvertHWCompressSurface(cjINT32 mipLevel,
    cjBool forceRGBA, cjBool use4444, cjBYTE** outBuf)
{
    return cjRSurf_UNKNOWN;
}

cjINT32 cjTexture::GetDefaultLOD()
{
    cjINT32 lod;

    lod = m_curLOD;
    lod = cjClamp(lod, (cjINT32)m_minLOD, (cjINT32)m_maxLOD);

    return lod;
}


//
// cjFStaticTexture
//

cjFStaticTexture::cjFStaticTexture(cjTexture* ownerTex)
{
    m_ownerTex = ownerTex;
    
    m_cacheID = cjResMakeCacheID(cjRSCID_Texture);
}

cjFStaticTexture::~cjFStaticTexture()
{

}

cjINT32 cjFStaticTexture::GetCacheRev()
{
	if(m_ownerTex->m_dirtyTexture)
	{
		IncCacheRev();
		m_ownerTex->m_dirtyTexture = cjFalse;
    }

	return cjFRenResSrc::GetCacheRev();
}

cjINT32 cjFStaticTexture::GetWidth()
{
	return m_ownerTex->GetMaterialUSiz();
}

cjINT32 cjFStaticTexture::GetHeight()
{
	return m_ownerTex->GetMaterialVSiz();
}

cjINT32 cjFStaticTexture::GetFirstMip()
{
	cjINT32 firstMip = m_ownerTex->GetDefaultLOD();
    
	while(((GetWidth() >> firstMip) < 64) && ((GetHeight() >> firstMip) < 64) && (firstMip > 0))
		firstMip--;

	return cjMax(firstMip, m_ownerTex->m_minLOD);
}

cjINT32 cjFStaticTexture::GetNumMip()
{
	return m_ownerTex->m_mip.Num();
}

cjRSurfaceFmt cjFStaticTexture::GetFormat()
{
	return (cjRSurfaceFmt)m_ownerTex->GetFormat();
}

cjTexClampMode cjFStaticTexture::GetUClamp()
{
	return (cjTexClampMode)m_ownerTex->GetTexClampModeU();
}

cjTexClampMode cjFStaticTexture::GetVClamp()
{
	return (cjTexClampMode)m_ownerTex->GetTexClampModeV();
}


//
// cjFTexture
//

void* cjFStaticTexture::GetRawTextureData(cjINT32 mipIndex)
{
	return &m_ownerTex->m_mip(mipIndex).m_data(0);
}

void cjFStaticTexture::UnloadRawTextureData(cjINT32 mipIndex)
{
}

void cjFStaticTexture::GetTextureData(cjINT32 mipIndex, void* dst, cjINT32 dstStride,
    cjRSurfaceFmt dstFmt)
{
//	cjFMipmap&		mipmap = m_ownerTex->m_mip(mipIndex);
//	cjRSurfaceFmt	srcFmt = (cjRSurfaceFmt)m_ownerTex->GetFormat();
//
//	if(&mipmap.m_data(0) == NULL)
//	{
//		cjLOG(_CU("Warn! - Texture is corrupted.."));
//		return;
//	}
//
//	if(dstFmt == cjRSurf_DXT1 || dstFmt == cjRSurf_DXT3 || dstFmt == cjRSurf_DXT5)
//	{
//        cjAssert(_CT("ERR! - Not implemented for cjFStaticTexture::GetTextureData.. (DXT1,3,5).."));
//	}
//	else if(srcFmt == dstFmt)
//	{
//		// No format conversion.
//		cjBYTE*	srcData = &mipmap.m_data(0);
//		cjBYTE*	dstData = (cjBYTE*)dst;
//		cjINT32	srcStride = cjGetBytesPerPixel(srcFmt, mipmap.m_potSizU);
//
//		if(srcStride == dstStride)
//			cjMemCpy(dstData, srcData, srcStride * mipmap.m_potSizV);
//		else
//		{
//			for(cjINT32 y=0; y<mipmap.m_potSizV;y++)
//			{
//				cjMemCpy(dstData, srcData, srcStride);
//
//				srcData += srcStride;
//				dstData += dstStride;
//			}
//		}
//	}
}



//
// cjTextureTM
//

CJSER_IMPL_CLASS(cjTextureTM, cjSerializable);

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjTextureTM)
	
	d->m_uvOffset		= m_uvOffset;		
	d->m_uvAxisWRot		= m_uvAxisWRot;
	d->m_uvScale		= m_uvScale;
	d->m_uvRotCenter	= m_uvRotCenter;
							
	d->m_isDirtyMatrix	= m_isDirtyMatrix;
	d->m_cachedMatrix	= m_cachedMatrix;
							
	d->m_texTMFlag		= m_texTMFlag;
	
CJSER_IMPL_CLONE_PROPCOPY_END(cjTextureTM)

CJSER_IMPL_CLONE(cjTextureTM)


cjTextureTM::cjTextureTM()
{
	MakeIdentity();
}

cjTextureTM::~cjTextureTM()
{
}

void cjTextureTM::Init(const cjVec2& uvOffset, cjFLOAT uvAxisWRot, 
	const cjVec2& uvScale, const cjVec2& uvRotCenter, cjTFLAG texFlag)
{
	m_uvOffset = uvOffset;
	m_uvAxisWRot = uvAxisWRot;
	m_uvScale = uvScale;
	m_uvRotCenter = uvRotCenter;
	m_isDirtyMatrix = cjTrue;
	m_texTMFlag = texFlag;
}

void cjTextureTM::MakeIdentity()
{
	m_uvOffset = cjVec2::ZERO;
	m_uvAxisWRot = 0.0f;
	m_uvScale = cjVec2(1.0f, 1.0f);
	m_uvRotCenter = cjVec2(0.5f, 0.5f);
	m_isDirtyMatrix = cjTrue;
	m_texTMFlag = 0;
}

bool cjTextureTM::operator==(const cjTextureTM& dst) const
{
    return m_uvAxisWRot == dst.m_uvAxisWRot &&
			m_uvOffset == dst.m_uvOffset &&
			m_uvScale == dst.m_uvScale &&
			m_texTMFlag == dst.m_texTMFlag &&
			m_uvRotCenter == dst.m_uvRotCenter;
}

bool cjTextureTM::operator!=(const cjTextureTM& dst) const
{
    return !operator==(dst);
}


void cjTextureTM::UpdateMatrix()
{
	cjFLOAT radSin, radCos;
	cjSinCosf(m_uvAxisWRot, radSin, radCos);

	if(m_texTMFlag & TTF_ScaleFirstAndRotate)
	{
		cjMatrix3 rotMat; 
		rotMat.MakeZRotation(m_uvAxisWRot);

		cjMatrix3 sclMat;
		sclMat.SetRow(0, m_uvScale.x, 0.0f, 0.0f);
		sclMat.SetRow(1, 0.0f, m_uvScale.y, 0.0f);
		sclMat.SetRow(2, 0.0f, 0.0f, 1.0f);

		cjMatrix3 uvOffMat;
		uvOffMat.SetRow(0, 1.0f, 0.0f, -m_uvOffset.x);
		uvOffMat.SetRow(1, 0.0f, 1.0f, m_uvOffset.y);
		uvOffMat.SetRow(2, 0.0f, 0.0f, 1.0f);

		cjMatrix3 rotCenter;
		rotCenter.SetRow(0, 1.0f, 0.0f, m_uvRotCenter.x);
		rotCenter.SetRow(1, 0.0f, 1.0f, m_uvRotCenter.y);
		rotCenter.SetRow(2, 0.0f, 0.0f, 1.0f);

        cjMatrix3 revRotCenter;
        revRotCenter.SetRow(0, 1.0f, 0.0f, -m_uvRotCenter.x);
        revRotCenter.SetRow(1, 0.0f, 1.0f, -m_uvRotCenter.y);
        revRotCenter.SetRow(2, 0.0f, 0.0f, 1.0f);

        m_cachedMatrix = rotCenter * sclMat * rotMat * uvOffMat * revRotCenter;
	}
	else // if(m_texTMFlag & TTF_RotateFirstAndScale)
	{
		cjMatrix3 rotMat; 
		rotMat.MakeZRotation(-m_uvAxisWRot);

		cjMatrix3 sclMat;
		sclMat.SetRow(0, m_uvScale.x, 0.0f, 0.0f);
		sclMat.SetRow(1, 0.0f, m_uvScale.y, 0.0f);
		sclMat.SetRow(2, 0.0f, 0.0f, 1.0f);

		cjMatrix3 uvOffMat;
		uvOffMat.SetRow(0, 1.0f, 0.0f, m_uvOffset.x);
		uvOffMat.SetRow(1, 0.0f, 1.0f, m_uvOffset.y);
		uvOffMat.SetRow(2, 0.0f, 0.0f, 1.0f);

		cjMatrix3 rotCenter;
		rotCenter.SetRow(0, 1.0f, 0.0f, m_uvRotCenter.x);
		rotCenter.SetRow(1, 0.0f, 1.0f, m_uvRotCenter.y);
		rotCenter.SetRow(2, 0.0f, 0.0f, 1.0f);

        cjMatrix3 revRotCenter;
        revRotCenter.SetRow(0, 1.0f, 0.0f, -m_uvRotCenter.x);
        revRotCenter.SetRow(1, 0.0f, 1.0f, -m_uvRotCenter.y);
        revRotCenter.SetRow(2, 0.0f, 0.0f, 1.0f);

        m_cachedMatrix = rotCenter * rotMat * revRotCenter * uvOffMat * sclMat;
	}

	m_isDirtyMatrix = cjFalse;
}