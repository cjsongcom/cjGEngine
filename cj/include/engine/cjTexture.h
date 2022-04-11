
#ifndef _CJTEXTURE_H_
#define _CJTEXTURE_H_


class cjFBaseTexture;


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTexture, cjBitmapMaterial, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE

public:
    virtual void cjObjPostLoad();

    // cjBitmapMaterial
    virtual cjFBaseTexture* GetTexInterface() { return m_texInterface; }
    virtual cjBitmapMaterial* Get(cjEngineTime_t curTime, cjViewport* viewport);
    
public:
    cjTexture();

    // mip manage
    cjINT32 GetFirstMip();
	cjINT32 GetNumMip() { return m_mip.Num(); }
	cjFMipmapBase* GetMip(cjINT32 i) { return &m_mip(i); }
    
    // create space for mip level 1~N, but mip level 0 data is not allocated here
    
    // mipLevel 0: generate full mipmap
    // mipLevel 1: mip level 0 (actually same to skipping this function);
    // mipLevel 2~N : mip level 1~(N-1)
	void CreateMip(cjINT32 mipLevel=1, cjBool doAutoGenMip=cjTrue);
    
    cjINT32 GetDefaultLOD();
	
    // allocating space for mip level 0 is here.
	virtual cjBool Init(cjINT32 usiz, cjINT32 vsiz, cjRSurfaceFmt fmt);

	virtual void Tick(cjEngineTime_t elapsedTime); // called from Update
	virtual void Update(cjEngineTime_t curTime);
    
	cjEngineTime_t GetLastUpdateTime() { return m_texLastUpdateTime; }
	void SetLastUpdateTime(cjEngineTime_t curTime) { m_texLastUpdateTime = curTime; }

	cjBool HasAlphChannel() const { return m_hasAlphaChannel; }

//	cjBool ReplaceTextureFromFile(const cjNameChar* path, cjRSurfaceFmt fmt=cjRSurf_UNKNOWN);
	
public:
	// internal use only
    cjRSurfaceFmt ConvertHWCompressSurface(cjINT32 mipLevel, cjBool forceRGBA, cjBool use4444, cjBYTE** outBuf);
    
public:
    cjBool                                  m_hasAlphaChannel;
    cjBool                                  m_renderTwoSide;
//    cjBool                                  m_chagnedFromLastRender;

    cjBool                                  m_mipCreateErr;
	cjTAry<cjFMipmap>						m_mip;

    // precedural texture stuff
    cjBool                                  m_procedural; // generated texture by real time, not to be stored
    cjEngineTime_t                          m_lastTextureSurfaceUpdatedTime;
    
    cjBool                                  m_doDirtyEveryUpdate;
    cjBool                                  m_dirtyTexture;
    
	// texture animation stuff
	cjTexture*                              m_texAniNext;
	cjTexture*                              m_texAniCur;
    
    cjEngineTime_t                          m_texAniPeroid; // 0 for no animation
    cjEngineTime_t                          m_texAniFrameElapsed;
    
	cjEngineTime_t                          m_texLastUpdateTime;
    cjEngineTime_t                          m_texAccumTime;
    
    cjFBaseTexture*                         m_texInterface;
    
    cjTexture*                              m_maskTexture;
    cjTexture*                              m_alphaTexture;
    
    cjBool                                  m_twoSided;
    

    // Mip Index(LOD)
    
    // image size width,height
    /*
        mip 0 : resource img size   (highest)
        mip 1 : mip 0     / 2
        mip 2 : mip 1     / 2
        mip x : mip x-1   / 2       (lower)
    
                            using mip index
          img width       min  | normal  | high
        * < 1024      :    0       0        0
        * > 1024      :    1       1        0
    
    // m_curLOD(normal)    : 0 (width < 1024), 1:(width > 1024)
    // m_minLOD(bestest)   : min(m_curLOD, m_maxLOD)
    // m_maxLOD(lowerst)   : m_mip.Num()-1, cjTexture::cjObjPostLoad
    
    // m_maxLOD
    // 1. m_maxLOD = m_mip.Num()-1
    // 2.1 i=0 to mip.num()
    // 2.2   width > 64                                 : m_maxLOD=i, break;
    // 2.3   or width < 256 && height range(2 ~ 32)     : m_maxLOD=i, break;
    // 2.4 loop 2.1
    */
    
    cjINT32                                 m_curLOD, m_minLOD, m_maxLOD;

protected:
    virtual void OnDestroy();

CJOBJ_DECL_CLASS_END(cjTexture)


//
// cjTextureTM
//

class CJENGINE_API cjTextureTM : public cjSerializable
{
	CJSER_DECL_CLASS(cjTextureTM, cjSerializable, 0, CJENGINE_API)
	CJSER_DECL_CLONE(cjTextureTM)

public:
	enum TextureTMFlag
	{
		TTF_ScaleFirstAndRotate		= 0x00000001, // 3dsmax style scale first
		TTF_RotateFirstAndScale		= 0x00000002, // maya
	};

	cjTextureTM();
	void Init(const cjVec2& uvOffset, cjFLOAT uvAxisWRot=0.0f, 
		const cjVec2& uvScale=cjVec2(1.0f, 1.0f), 
		const cjVec2& uvRotCenter=cjVec2(0.5f, 0.5f), cjTFLAG texFlag=0);

	inline const cjMatrix3* GetMatrix();

	void MakeIdentity();

	cjVec2 GetUVOffset() const { return m_uvOffset; }
	inline void SetUVOffset(const cjVec2& uvOffset);
	
	cjFLOAT GetUVAxisWRot() const { return m_uvAxisWRot; }  // w axis in texture space
	inline void SetUVAxisWRot(cjFLOAT uvAxisWRot);

	cjVec2 GetUVScale() const { return m_uvScale; }
	inline void SetUVScale(const cjVec2& uvScale);

	cjTFLAG GetTextureTMFlag() const { return m_texTMFlag; }
	inline void SetTextureTMFlag(cjTFLAG flag);

	cjVec2 GetUVRotCenter() const { return m_uvRotCenter; }
	inline void SetUVRotCenter(const cjVec2& uvRotCenter);

	bool operator==(const cjTextureTM& dst) const;
	bool operator!=(const cjTextureTM& dst) const;

	static const cjTextureTM Identity;

protected:
	void UpdateMatrix();

private:
	cjVec2								m_uvOffset;
	cjFLOAT								m_uvAxisWRot;
	cjVec2								m_uvScale;
	cjVec2								m_uvRotCenter;

	cjBool								m_isDirtyMatrix;
	cjMatrix3							m_cachedMatrix;

	cjTFLAG								m_texTMFlag;
};


inline const cjMatrix3* cjTextureTM::GetMatrix()
{
	if(m_isDirtyMatrix)
		UpdateMatrix();

	return &m_cachedMatrix;
}

inline void cjTextureTM::SetUVOffset(const cjVec2& uvOffset)
{
	if(m_uvOffset != uvOffset)
	{
		m_uvOffset = uvOffset;
		m_isDirtyMatrix = cjTrue;
	}
}

inline void cjTextureTM::SetUVAxisWRot(cjFLOAT uvAxisWRot)
{
	if(m_uvAxisWRot != uvAxisWRot)
	{
		m_uvAxisWRot = uvAxisWRot;
		m_isDirtyMatrix = cjTrue;
	}
}

inline void cjTextureTM::SetUVScale(const cjVec2& uvScale) 
{ 
	if(m_uvScale != uvScale)
	{			
		m_uvScale = uvScale;
		m_isDirtyMatrix = cjTrue;
	}
}

inline void cjTextureTM::SetTextureTMFlag(cjTFLAG flag)
{
	if(m_texTMFlag != flag)
	{			
		m_texTMFlag = flag;
		m_isDirtyMatrix = cjTrue;
	}
}

inline void cjTextureTM::SetUVRotCenter(const cjVec2& uvRotCenter)
{
	if(m_uvRotCenter != uvRotCenter)
	{			
		m_uvRotCenter = uvRotCenter;
		m_isDirtyMatrix = cjTrue;
	}
}


#endif

