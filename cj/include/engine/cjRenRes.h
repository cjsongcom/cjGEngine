
#ifndef _CJRENRESSRC_H_
#define _CJRENRESSRC_H_

#include "cjShaderConstant.h"

#include "cjResCacheID.h"
#include "cjRenDef.h"

class cjTexture;

class cjFTexture;
class cjFCubeMap;
class cjFBaseTexture;
class cjFRenTarget;
class cjFAuxRenTarget;
class cjFRenDev;
class cjFRenTarget;
class cjFRenColorSurface;
class cjFRenDepthStencil;
struct cjFVertexComponent;
class cjFVertexStream;
class cjMeshModfier;
class cjMeshSkinModfier;


struct cjFResCacheLinkInfo
{
	void*									m_container; // from renDev
	void*                                   m_nextRes;

	cjUINT32								m_hashIndex;
	void*                                   m_hashNext;
};


//
// Render Resources Sources
//

class CJENGINE_API cjFRenResSrc : public cjMem
{
public:
    virtual ~cjFRenResSrc();
    
	virtual cjResCacheIDType GetResCacheIDType() = 0;

	cjResCacheID GetCacheID() { return m_cacheID; }
	cjResCacheID GetCacheID() const { return m_cacheID; }

	virtual cjINT32 GetCacheRev() { return m_cacheRev; }
	virtual cjINT32 GetCacheRev() const { return m_cacheRev; }

	void IncCacheRev() { m_cacheRev++; }
    
    static cjRenDev* GetRenDev();
    
    // accessor
    virtual class cjFRenTargetGrp* GetRenTargetGrp() { return NULL; }
    virtual class cjFRenTarget* GetRenTarget() { return NULL; }

#if defined(CJ_PLATFORM_APPLE)
    virtual class cjFAppleDefFrmRenTargetGrp* GetAppleDefFrmRenTargetGrp() { return NULL; }
    virtual class cjFAppleDefFrmRenTarget* GetAppleDefFrmRenTarget() { return NULL; }
#endif
   
    virtual class cjFBaseTexture* GetBaseTexture() { return NULL; }
    virtual class cjFTexture* GetTexture() { return NULL; }
    
    virtual class cjFShader* GetShader() { return NULL; }
    virtual class cjFShaderProgram* GetShaderProgram() { return NULL; }
    
    virtual class cjFVertexStreamAttribLayout* GetVertexStreamAttribLayout() { return NULL; }
    virtual class cjFVertexStream* GetVertexStream() { return NULL; }
#if defined(CJ_PLATFORM_APPLE)
    virtual class cjFEAGLVertexStream* GetEAGLVertexStream() { return NULL; }
#endif
	
protected:
	cjFRenResSrc()
	{
		m_cacheRev = 1;
	}


protected:
	cjResCacheID					m_cacheID;
	cjINT32							m_cacheRev;
};


//
// Index buffer
//


class CJENGINE_API cjFIndexBuffer : public cjFRenResSrc
{
public:
	// serialize
	CJENGINE_API friend cjFStream& SerializeStream(cjFStream& stream, cjFIndexBuffer** d);
	virtual cjFStream& operator << (cjFStream& stream);	

public:
	cjFIndexBuffer() 
	{ 	
		m_cacheID = cjResMakeCacheID(cjRSCID_Shader);
		m_indexInterval = 3;
	}

    virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_IndexStream; }

	cjINT32 GetStride() { return sizeof(cjWORD); }
	cjINT32 GetByteSiz() { return m_indices.Num() * sizeof(cjWORD); }
	cjINT32 GetNumIndex() { return m_indices.Num(); } 
	cjINT32 GetNumPrimitive() { return m_indices.IsEmpty() ? 0 : m_indices.Num() / m_indexInterval; }
	void SetIndexInterval(cjINT32 interval=3) { m_indexInterval = interval; }

	void* GetBuf() { return &m_indices(0); }	
	
	cjWORD GetIndex(cjAryOff index) 
	{ 
		return m_indices(index); 
	}

	virtual void SetIndexDat(cjINT32 indexCount, void* indexBuf=NULL, cjINT32 stride=sizeof(cjWORD));

	// ret: copied size in byte
	virtual cjINT32 CopyTo(void* dst, cjINT32 bytesStPos=0, cjINT32 bytesToCopy=-1); // -1 for entire copy	
	virtual void OptimizeTriListIndexDatSequence();

private:
	cjTAry<cjWORD>								m_indices;
	cjINT32										m_indexInterval;
};

CJENGINE_API cjFStream& SerializeStream(cjFStream& stream, cjFIndexBuffer** d);


//
// texture
//

class CJENGINE_API cjFBaseTexture : public cjFRenResSrc
{
public:
	virtual cjFBaseTexture* GetBaseTexture() { return this; }
	virtual cjFTexture* GetTexture() { return NULL; }
    virtual cjFCubeMap* GetCubeMap() { return NULL; }
	virtual cjFRenTarget* GetRenTarget() { return NULL; }
	virtual cjFRenTarget* GetRenDSTarget() { return NULL; }

	virtual cjINT32 GetWidth() = 0;
	virtual cjINT32 GetHeight() = 0;
	virtual cjINT32 GetFirstMip() = 0;
	virtual cjINT32 GetNumMip() = 0;

	virtual cjRSurfaceFmt GetFormat() = 0;
	virtual cjRSurfaceFmt GetDSFormat() { return cjRSurf_UNKNOWN; }
    
	virtual cjTexClampMode GetUClamp() = 0;
	virtual cjTexClampMode GetVClamp() = 0;
};


class CJENGINE_API cjFTexture : public cjFBaseTexture
{
public:
	virtual cjFTexture* GetTexture() { return this; }

public:
	virtual ~cjFTexture() {}

	virtual void* GetRawTextureData(cjINT32 mipIndex) = 0;
	virtual void UnloadRawTextureData(cjINT32 mipIndex) = 0;
    virtual void GetTextureData(cjINT32 mipIndex, void* dst, cjINT32 dstStride,
        cjRSurfaceFmt dstFmt) = 0;
    
	virtual cjTexture* GetOwnerCJTexture() = 0;
};


// implementation for cjFStaticTexture is in cjTexture.cpp
class cjFStaticTexture : public cjFTexture
{
public:
    // cjFRenResSrc
    virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_Texture; }
	virtual cjINT32 GetCacheRev();
    
    // cjFBaseTexture
    virtual cjINT32 GetWidth();     // of cjTexture
	virtual cjINT32 GetHeight();
	virtual cjINT32 GetFirstMip();
	virtual cjINT32 GetNumMip();
    
    virtual cjRSurfaceFmt GetFormat();
	virtual cjTexClampMode GetUClamp();
	virtual cjTexClampMode GetVClamp();
    
    // cjFTexture
	virtual void* GetRawTextureData(cjINT32 mipIndex);
	virtual void UnloadRawTextureData(cjINT32 mipIndex);
    virtual void GetTextureData(cjINT32 mipIndex, void* dst, cjINT32 dstStride,
        cjRSurfaceFmt dstFmt);
    
	virtual cjTexture* GetOwnerCJTexture() { return m_ownerTex; }
    
public:
    cjFStaticTexture(cjTexture* ownerTex);
    virtual ~cjFStaticTexture();
    
private:
    cjFStaticTexture() {}

private:
	cjTexture*                          m_ownerTex;
};


//
// RenderTarget
//

class CJENGINE_API cjFRenTarget : public cjFBaseTexture
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_RenderTarget; }
    virtual cjFRenTarget* GetRenTarget() { return this; }

public:
	virtual cjINT32 GetWidth() { return m_width; }
	virtual cjINT32 GetHeight() { return m_height; }
   	virtual cjINT32 GetFirstMip() { return 0; }
	virtual cjINT32 GetNumMip() { return 1; }
    
	virtual cjRSurfaceFmt GetFormat() { return m_format; }
	virtual cjTexClampMode GetUClamp() { return cjTexClamp_Clamp; }
	virtual cjTexClampMode GetVClamp() { return cjTexClamp_Clamp; }

public:
	cjFRenTarget(cjINT32 width, cjINT32 height, cjRSurfaceFmt fmt, cjRSurfaceFmt=cjRSurf_UNKNOWN);
	virtual ~cjFRenTarget();

	virtual cjRSurfaceFmt GetDSFormat(){ return m_dsformat; } // depth-stencil format
	cjBool HasDepthStencil() { return m_dsformat != cjRSurf_UNKNOWN ? cjTrue : cjFalse; }

protected:
	cjFRenTarget() {}
	
protected:
	cjINT32								m_width;
	cjINT32								m_height;

	cjRSurfaceFmt						m_format;	// color surface
	cjRSurfaceFmt						m_dsformat;	// depth-stencil
};


//
// Basic shader stuff definition
//

enum 
{ 
	cjShaderStreamsMax		= 16,
	cjVertexComponentMax	= 8
};

enum EPixelShader
{
	PS_None					= 0,
	PS_Terrain3Layer		= 1,
	PS_Terrain4Layer		= 2,
};


//
// cjFShader
//

class CJENGINE_API cjFShader : public cjFRenResSrc
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_Shader; }
    virtual cjFShader* GetShader() { return this; }

public:
	enum EFLAG
	{
		EFLAG_VERTEX_SHADER		= 0x00000001,
		EFLAG_PIXEL_SHADER		= 0x00000002,
		
		EFLAG_SRCFROM_FILE		= 0x00000100,
		EFLAG_SRCFROM_MEM		= 0x00000200,		

		EFLAG_PRECOMPILED		= 0x00001000,

		EFLAG_NULL_SHADER		= 0x80000000
	};

//	enum ESHADER_FROM_FILE_TAG { ESHADER_FROM_FILE };
//	enum ESHADER_FROM_MEM_TAG  { ESHADER_FROM_MEM };
	enum ESHADER_FROM_STR_TAG  { ESHADER_FROM_STR };

	virtual ~cjFShader();

	virtual cjBool IsPreCompiled() { return m_flag & EFLAG_PRECOMPILED ? cjTrue : cjFalse; }

	virtual const cjChar* GetName() { return *m_name; }
	virtual const cjChar* GetShaderPath() { return NULL; }

	virtual cjRenShaderType GetShaderType()
	{ 
		if(m_flag & EFLAG_VERTEX_SHADER) return cjShader_VertexShader;
		else if(m_flag & EFLAG_PIXEL_SHADER) return cjShader_PixelShader;

		return cjShader_Unknown;
	}
    
    virtual class cjFVertexShader* GetVertexShader() { return NULL; }
    virtual class cjFPixelShader* GetPixelShader() { return NULL; }
	
	const cjBYTE* GetShaderSrc() { return m_shader.IsEmpty() ? NULL : &m_shader(0); }
	cjINT32 GetShaderLen() { return m_shader.Num(); }

protected:
	cjFShader() {}
	cjFShader(cjTFLAG flag, const char* name, cjINT32 ver);

	void GenerateFromMem(const cjBYTE* src, cjINT32 len);

private:
	cjTFLAG							m_flag;
	cjINT32							m_comptiableVer;

	cjStringA						m_name;
	cjTAry<cjBYTE>					m_shader;
};


class CJENGINE_API cjFVertexShader : public cjFShader
{
public:
	// cjFShader
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_Shader; }
    virtual cjFVertexShader* GetVertexShader() { return this; }

public:
	//explicit cjFVertexShader(cjGLRenDev* renDev, ERESFROM_FILE_TAG, const char* path, 
	//	const char* name, cjINT32 ver);

	// vtxDecl: cjNULL -> generate cjFVertexDecl from given shaderText
	explicit cjFVertexShader(ESHADER_FROM_STR_TAG, const char* shaderText,
		const char* name, cjINT32 ver=-1);
};

class CJENGINE_API cjFPixelShader : public cjFShader
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_Shader; }
    virtual cjFPixelShader* GetPixelShader() { return this; }

public:
	explicit cjFPixelShader(ESHADER_FROM_STR_TAG, const char* shaderText,
		const char* name, cjINT32 ver=-1);
};


class CJENGINE_API cjFShaderProgram : public cjFRenResSrc
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_Shader; }
    virtual cjFShaderProgram* GetShaderProgram() { return this; }

public:
	cjFShaderProgram(const char* name);
	cjFShaderProgram(cjFVertexShader* vertexShader, cjFPixelShader* pixelShader, 
		cjBool delOnDestroy=cjTrue);

	virtual ~cjFShaderProgram();
    
	cjFVertexShader* GetVertexShader() { return m_vtxShader; }
	cjFPixelShader* GetPixelShader() { return m_pxlShader; }

	cjFShader* GetShader(cjRenShaderType shaderType);
	void SetShader(cjRenShaderType shaderType, cjFShader* shader, cjBool delOnDestroy=cjTrue);

private:
	cjStringA						m_name;

	cjFVertexShader*				m_vtxShader;
	cjBool							m_delVtxShader;

	cjFPixelShader*					m_pxlShader;
	cjBool							m_delPxlShader;

};


//
// cjFRenderTargetGroup
//

class CJENGINE_API cjFRenTargetGrp : public cjFRenResSrc
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_RenderTarget; }
    virtual cjFRenTargetGrp* GetRenTargetGrp() { return this; }

public:
	cjFRenTargetGrp();
	virtual ~cjFRenTargetGrp();

#if defined(CJ_PLATFORM_WIN32)
	enum { MAX_MULTI_RENDER_TARGETS = 4 };
    
#elif defined(CJ_PLATFORM_APPLE)
    enum { MAX_MULTI_RENDER_TARGETS = 1 };
#else
    
#endif
	cjINT32 GetWidth(cjINT32 idxRenTarget);
	cjINT32 GetHeight(cjINT32 idxRenTarget);
    
	inline cjFRenTarget* GetRenTarget(cjINT32 index) { return m_renTarget[index]; }
	void AttachRenTarget(cjINT32 index, cjFRenTarget* rt);

protected:
	cjFRenTarget*						m_renTarget[MAX_MULTI_RENDER_TARGETS];
};


//
// cjVtxStreamComp
//

class CJENGINE_API cjVtxCompStream : public cjMem
{
public:
	cjFStream& operator >> (cjFStream& stream);

	void SetCompInfo(cjVertexCompType type, cjVertexCompSizType sizType, cjBYTE streamIndex=0);

	cjBYTE GetStreamIndex() const { return m_streamIndex; }
	cjVertexCompType GetCompType() const { return (cjVertexCompType)m_type; }
	cjVertexCompSizType GetCompSizType() const { return (cjVertexCompSizType)m_sizType; }
	cjINT32 GetElementSiz() const { return m_elementSiz; }

	cjBYTE* Lock();
	void UnLock();

	template<class T>
	T* Begin() { return (T*)m_data.Begin(); }

	template<class T>
	T* End() { return (T*)m_data.End(); }

	void SetVertexCount(cjINT32 count);
	cjINT32 GetVertexCount() const { return m_vertexCount; }

	void Copy(cjBYTE* dst, cjINT32 preOffset, cjINT32 stride);

protected:
	cjBYTE									m_streamIndex;
	cjBYTE 									m_type; // cjVertexCompType
	cjBYTE 									m_sizType; // cjVertexCompSizType
	cjBYTE									m_pad0;
	cjINT32									m_elementSiz;

	cjINT32									m_vertexCount;
	cjTAry<cjBYTE>							m_data;
};



//
// MeshModifier
//

class CJENGINE_API cjMeshModfier : public cjMem 
{
public:
	virtual cjMeshSkinModfier* GetSkinModifier() { return 0; }

};


class CJENGINE_API cjMeshSkinModfier : public cjMeshModfier
{
public:
	virtual cjMeshSkinModfier* GetSkinModifier() { return this; }

	
	cjINT32									m_skinBoneCount;
	cjTransform*							m_skinBoneTMAry;
};


//
// cjFVertexStream
//

CJENGINE_API cjFStream& SerializeStream(cjFStream& stream, cjFVertexStream** d);


class CJENGINE_API cjFVertexStream : public cjFRenResSrc
{
public:
	enum { EClsStaticVertexStream = 1 };
	enum { MAX_STREAMS = 4 };

	typedef cjFVertexStream* StreamAry[MAX_STREAMS];
		
		// serialize
	virtual cjBYTE GetInstanceClsID() = 0; // 1:cjFStaticVertexStream
	static cjFVertexStream* CreateInstance(cjBYTE clsID);
	CJENGINE_API friend cjFStream& SerializeStream(cjFStream& stream, cjFVertexStream** d);
	virtual cjFStream& operator << (cjFStream& stream) = 0;

    virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_VertexStream; }
    virtual cjFVertexStream* GetVertexStream() { return this; }

public:
	class CJENGINE_API Key
	{
	public:
		typedef Key HashKey;

		class CJENGINE_API HashKeyCmp
		{ public:
			cjBool operator()(const HashKey& lhs, const HashKey& rhs) const { return lhs == rhs; }
		};

		class CJENGINE_API KeyHasher
		{
		public:
			cjTHasherValType operator()(const HashKey& key) const
			{
				cjTHasherPtr<cjFVertexStream*> ptrHasher;
								
				cjUINT32 hash = key.m_streamCount;

				for(cjINT32 i=0; i<key.m_streamCount; i++)
					hash += ptrHasher(key.m_streams[i]);
			
				return hash;
			}
		};

		cjBool operator ==(const Key& rhs) const
		{
			if(m_streamCount == rhs.m_streamCount)
			{
				for(cjINT32 i=0; i<m_streamCount; i++)
				{
					if(m_streams[i] != rhs.m_streams[i])
						return cjFalse;
				}

				return cjTrue;
			}
			return cjFalse;
		}

		Key(const StreamAry streams, cjINT32 streamCount)
		{
			m_streamCount = streamCount;
			cjMemCpy(m_streams, streams, sizeof(StreamAry));
		}

		Key() {}
	
	public:
		cjFVertexStream::StreamAry	m_streams;
		cjINT32						m_streamCount;
	};
	
public:
    virtual ~cjFVertexStream();

    virtual cjINT32 GetStride() const = 0;
    virtual cjINT32 GetVertexCount() const = 0;
	virtual void SetVertexCount(cjINT32 count) = 0;
    virtual cjINT32 GetByteSiz() const = 0;

	virtual cjPrimitiveType GetPrimitiveType() const { return cjPT_TriangleList; }
	virtual void SetPrimitiveType(cjPrimitiveType) {}

    virtual const cjFVertexDecl* GetVertexDecl() const = 0;
	virtual void SetAttrib(cjUINT32 attrib) = 0;
	virtual cjUINT32 GetAttrib() const = 0;

	virtual cjVtxCompStream* CreateVtxCompStream(cjVertexCompType compType, 
		cjVertexCompSizType compSizType, cjBool updateVtxDecl=cjFalse) = 0;
	virtual cjVtxCompStream* GetVtxCompStream(cjVertexCompType compType) = 0;

	virtual void CopyCompStreamIntoSingleStream(cjBYTE* dst) const = 0;

	virtual void Validate() {}
	
//  virtual cjINT32 DumpVertexComponents(cjVertexComponent* comp=NULL) const = 0;
// 	virtual void DumpVertices(void* buf) = 0;
//  virtual void* GetVertexBuffer(cjINT32 startVertexIndex=0) = 0;
    
protected:
    cjFVertexStream();
};


class CJENGINE_API cjFStaticVertexStream : public cjFVertexStream
{
public:
	virtual cjBYTE GetInstanceClsID() { return 1; }
	virtual cjFStream& operator << (cjFStream& stream);

public:
	virtual cjINT32 GetStride() const { return m_vtxDecl.GetStride(); }
    virtual cjINT32 GetVertexCount() const { return m_numVertex; }
	virtual void SetVertexCount(cjINT32 count);
	virtual cjINT32 GetByteSiz() const;

    virtual const cjFVertexDecl* GetVertexDecl() const { return &m_vtxDecl; }
	virtual void SetAttrib(cjUINT32 attrib);
	virtual cjUINT32 GetAttrib() const { return m_attr; }
	virtual cjVtxCompStream* CreateVtxCompStream(cjVertexCompType compType, 
		cjVertexCompSizType compSizType, cjBool updateVtxDecl=cjFalse);
	virtual cjVtxCompStream* GetVtxCompStream(cjVertexCompType compType)
		{ return m_compStream[compType]; }

	virtual void CopyCompStreamIntoSingleStream(cjBYTE* dst) const;

public:
	enum ESTREAM_DIRTY
	{
		ESD_NEED_UPDATE_VERTEX_DECL			= 0x00000001,
		ESD_NEED_CACHE_REV					= 0x10000000,
	};
	CJ_DEF_DIRTY_FUNCSET(Stream)

public:
	cjFStaticVertexStream();
	cjFStaticVertexStream(const cjINT32 vtxCount, const cjFVertexDecl* decl = NULL, 
		const void* rawVertexSrc = NULL, cjUINT32 streamAttr = cjStreamAttr_Usage_StaticDraw 
			| cjStreamAttr_Access_WriteOnly) cjSlow; 

    virtual ~cjFStaticVertexStream();

	virtual void Validate() { ClearStreamDirty();  }

	virtual cjPrimitiveType GetPrimitiveType() const { return m_primitiveType; }
	virtual void SetPrimitiveType(cjPrimitiveType type) { m_primitiveType = type; }

 	//virtual void DumpVertices(void* buf)
  //      { cjMemCpy(buf, &m_data(0), m_data.Num()); }
  //  
  //  virtual void* GetVertexBuffer(cjINT32 startVertexIndex=0)
  //      { return &m_data(startVertexIndex * m_vtxDecl.GetStride()); }

private:
	void Init(cjBool isSerialize, cjINT32 vtxCount, const cjFVertexDecl* decl, 
			const void* rawVertexSrc, cjUINT32 streamAttr);
	void Clean();

	void UpdateVertexDecl();
	
private:
    cjFVertexDecl                           m_vtxDecl;
	cjUINT32								m_attr;
	cjPrimitiveType							m_primitiveType;
	cjINT32                                 m_numVertex;

	cjVtxCompStream*						m_compStream[cjVtxComp_MAX]; // cjVtxComp_MAX, maximum 32	
};

//
//struct CJENGINE_API cjFShaderStreamDecl : public cjMem
//{
//public:
//	cjBool operator==(cjFShaderStreamDecl& rhs) const
//	{
//		if(m_numStream != rhs.m_numStream)
//			return cjFalse;
//
//		for(cjINT32 i=0; i<m_numStream; i++)
//		{
//			if(!(m_decl[i] == rhs.m_decl[i]))
//				return cjFalse;
//		}
//
//		return cjTrue;
//	}
//
//	cjBool operator!=(cjFShaderStreamDecl& rhs) const
//	{
//		return operator==(rhs) ? cjFalse : cjTrue;
//	}
//
//	cjFVertexDecl                        m_decl[cjShaderStreamsMax];
//	cjINT32								m_numStream;
//};


//
// cjFVertexStreamAttribLayout
//

class CJENGINE_API cjFVertexStreamAttribLayout : public cjFRenResSrc
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_VertexStreamAttrib; }
    virtual cjFVertexStreamAttribLayout* GetVertexStreamAttribLayout() { return this; }

public:
    cjFVertexStreamAttribLayout(const cjFVertexStream::StreamAry streams, cjINT32 streamCount);
    virtual ~cjFVertexStreamAttribLayout();
    
    cjINT32 GetNumStream() { return m_streamCount; }
	cjFVertexStream* GetStream(cjINT32 index) { return const_cast<cjFVertexStream*>(m_streams[index]); }
	
	cjBool CmpStream(const cjFVertexStream::StreamAry streams, cjINT32 streamCount) const
	{
		if(m_streams != streams || streamCount != m_streamCount)
			return cjFalse;

		return cjTrue;
	}

private:
    cjFVertexStream::StreamAry		m_streams;      // ref
    cjINT32							m_streamCount;
};



//
// apple extension
//

#if defined(CJ_PLATFORM_APPLE)
#include "engine/apple/cjAppleRenRes.h"
#endif


#endif

