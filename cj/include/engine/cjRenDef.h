
#ifndef _CJRENDEF_H_
#define _CJRENDEF_H_


const cjFLOAT cjDiv255	= 1.0f / 255.0f;

#define cjCOLOR_ARGB(a,r,g,b) \
    ((cjUINT32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))


enum cjRFBSwapEffect
{
	cjRFB_SWAPEFF_DEFAULT,
	cjRFB_SWAPEFF_DISCARD,
	cjRFB_SWAPEFF_FLIP,
	cjRFB_SWAPEFF_COPY,
	cjRFB_SWAPEFF_MAX
};

enum cjRPresentInterval
{
	cjRPRESENT_INTERVAL_IMMEDIATE,
	cjRPRESENT_INTERVAL_ONE,
	cjRPRESENT_INTERVAL_MAX
};

enum cjRSurfaceFmt
{
	cjRSurf_UNKNOWN		= 0,

	cjRSurf_RGB8		= 0x01,
	cjRSurf_RGBA8		= 0x02,

	cjRSurf_RGB16		= 0x05,
	cjRSurf_RGBA16		= 0x06,

	cjRSurf_RGB565		= 0x09,

    cjRSurf_SRGB8       = 0x11,
    cjRSurf_SRGBA8      = 0x12,
    cjRSurf_SRGB16      = 0x15,
    cjRSurf_SRGBA16     = 0x16,

    // apple power-vr graphics - pvrtc
    // IMG_texture_compression_pvrtc
    /* compression of PVRTC is very CPU intensive,
       it is not appropriate to carry out compression on the target
       platform. Therefore this extension only supports the loading of
       compressed texture data. */
    cjRSurf_PVRTC_RGB4  = 0x71,
    cjRSurf_PVRTC_RGB2  = 0x72,
    cjRSurf_PVRTC_ARGB4 = 0x73,
    cjRSurf_PVRTC_ARGB2 = 0x74,

        // pc, s3tc(*.dds)
	cjRSurf_DXT1		= 0x81,
	cjRSurf_DXT3		= 0x82,
	cjRSurf_DXT5		= 0x83,

	cjRSurf_D16			= 0x91, // 16bit depth
	cjRSurf_D24X8		= 0x92, // 24bit depth, no stencil
	cjRSurf_D24S8		= 0x93, // 24bit depth, 8bit stencil
	cjRSurf_D32			= 0x94, // 32bit depth
    

	cjRSurf_MAX			= 0xff
};

inline cjINT32 cjGetBytesPerPixel(cjRSurfaceFmt fmt, cjINT32 numPixel)
{
	switch(fmt)
	{
	case cjRSurf_DXT1:        return numPixel / 2;
	case cjRSurf_RGBA8:       return numPixel * 4;
	case cjRSurf_DXT3:	case cjRSurf_DXT5:   return numPixel;
	case cjRSurf_RGB16:       return numPixel * 2;
	case cjRSurf_RGB8:        return numPixel * 3; // 3bytes per pixel
    default : break;
	};

	return 0;
}

enum cjTexClampMode
{
	cjTexClamp_Wrap		= 0x00,
	cjTexClamp_Clamp	= 0x01,
	cjTexClamp_Mirror   = 0x02,
	cjTexClamp_Max
};

enum cjTexFilterMode
{
	cjTexFilter_Nearest,
	cjTexFilter_Bilerp,
	cjTexFilter_TriLerp,
	cjTexFilter_Nearest_MipNearest,
	cjTexFilter_Nearest_MipLerp,
	cjTexFilter_BiLerp_MipNearest,
	cjTexFilter_AnisoTropic,
	cjTexFilter_Max,
};

enum cjTexEnvMapType
{
    cjTexEnv_WorldSpace    =0,
    cjTexEnv_CameraSpace   =1,
    cjTexEnv_MAX           =2,
};

enum cjTexCoordCount
{
    cjTexCoordNone          =0,
    cjTexCoord1D            =1,
    cjTexCoord2D            =2,
    cjTexCoord3D            =3,
    cjTexCoord4D            =4,
    cjTexCoord_MAX
};

enum cjTexCoordSrc
{
    cjTexCoordSrc0          =0, // uv stream 0
    cjTexCoordSrc1,
    cjTexCoordSrc2,
    cjTexCoordSrc3,
    cjTexCoordSrc4,
    cjTexCoordSrc5,
    cjTexCoordSrc6,
    cjTexCoordSrc7,
    cjTexCoordSrcPassThru,
    cjTexCoordSrcWorld,
    cjTexCoordSrcCamera,
    cjTexCoordSrcCameraReflect,
    cjTexCoordSrcEnvMapWorld,
    cjTexCoordSrcEnvMapCamera,
    cjTexCoordSrcSphereMap,
    cjTexCoordSrcSameToPrevious,
    cjTexCoordSrc_MAX
};


enum cjStencilOper
{
	cjSO_Keep			= 1,
	cjSO_Zero			= 2,
	cjSO_Replace		= 3,
	cjSO_IncrementSat	= 4,
	cjSO_DecrementSat	= 5,
	cjSO_Invert			= 6,
	cjSO_Increment		= 7,
	cjSO_Decrement		= 8
};

enum cjPxlCmpFunc
{
	cjPCF_Never			= 1,
	cjPCF_Less			= 2,
	cjPCF_Equal			= 3,
	cjPCF_LessEqual		= 4,
	cjPCF_Greater		= 5,
	cjPCF_NotEqual		= 6,
	cjPCF_GreaterEqual	= 7,
	cjPCF_Always		= 8
};

enum cjFogState
{
	cjFS_Store			= 0,
	cjFS_Restore		= 1,
	cjFS_Ignore			= 2
};

enum cjCullMode
{
	cjCM_CW				= 0,
	cjCM_CCW			= 1,	
	cjCM_None			= 2
};

enum cjFillMode
{
	cjFM_Solid			= 0,	// gouraud
	cjFM_Wireframe		= 1,	
};

enum cjPrimitiveType
{
	cjPT_TriangleList,
	cjPT_TriangleStrip,
	cjPT_TriangleFan,
	cjPT_PointList,
	cjPT_LineList
};

enum cjRenShaderType
{
	cjShader_Unknown = 0,
	cjShader_VertexShader,
	cjShader_PixelShader
//	cjShader_FixedVertexShader,
//	cjShader_FixedPixelShader,
//	cjShader_IntegratedShader
};

enum cjLODCate
{
	cjLODCate_None           = 0,
	cjLODCate_World          = 1,   // default 4
	cjLODCate_PlayerSkin     = 2,
	cjLODCate_Terrain        = 3,   // default 4
	cjLODCate_UI             = 4,   // default 4
	cjLODCate_RenderMap      = 5,   // default 4
	cjLODCate_LightMap       = 6,
	cjLODCate_Max            = 7,
};

enum cjRenFrameBufferBlending
{
    cjRFB_Overwrite          = 0,
    cjRFB_AlphaBlend         = 1,
    cjRFB_Modulate           = 2,
    cjRFB_MAX
};


/////////////////////////////////////////////////////////
// Vertex Component
/////////////////////////////////////////////////////////
/*
GL_BYTE          
GL_UNSIGNED_BYTE 
GL_SHORT         
GL_UNSIGNED_SHORT
GL_INT           
GL_UNSIGNED_INT  
GL_FLOAT         
GL_FIXED
*/

enum cjVertexCompSizType
{
    cjVtxCompSiz_Unknown,
	cjVtxCompSiz_FLOAT,
	cjVtxCompSiz_FLOAT2,
	cjVtxCompSiz_FLOAT3,
	cjVtxCompSiz_FLOAT4,	
	
    cjVtxCompSiz_UINT32,
    
    cjVtxCompSiz_BYTE,
    cjVtxCompSiz_BYTE2,
    cjVtxCompSiz_BYTE3,
    cjVtxCompSiz_BYTE4,
    
    cjVtxCompSiz_SHORT,
    cjVtxCompSiz_SHORT2,
    cjVtxCompSiz_SHORT3,
    cjVtxCompSiz_SHORT4,
	
	// ...
	cjVtxCompSiz_ENDCOMP = 31,
	
	cjVtxCompSiz_FLAG_MASK_BIT = 5,
	cjVtxCompSiz_FLAG_MASK = 31,

	// max 255 kind
};

CJENGINE_API cjINT32 cjGetSizFromVertexCompSizType(cjVertexCompSizType sizType);

enum cjVertexCompType
{
	cjVtxComp_Position  = 0,

	cjVtxComp_BL4Weight,
	cjVtxComp_BL4Index,

	cjVtxComp_Diffuse,
	cjVtxComp_Specular,

	cjVtxComp_Normal,
	cjVtxComp_BiNormal,
	cjVtxComp_Tangent,

	cjVtxComp_TexCoord0,
	cjVtxComp_TexCoord1,
	cjVtxComp_TexCoord2,
	cjVtxComp_TexCoord3,
	cjVtxComp_TexCoord4,
	cjVtxComp_TexCoord5,
	cjVtxComp_TexCoord6,
	cjVtxComp_TexCoord7,

    cjVtxComp_MAX, // MAX 32 kind
    
    cjVtxComp_Unknown = -1,
};

enum cjVertexCompFlag
{
	cjVtxCompFlag_Position      = 0x00000001,

	cjVtxCompFlag_BL4Weight     = 0x00000010,
	cjVtxCompFlag_BL4Index      = 0x00000020,

	cjVtxCompFlag_Diffuse       = 0x00000100,
	cjVtxCompFlag_Specular      = 0x00000200,

	cjVtxCompFlag_Normal        = 0x00001000,
	cjVtxCompFlag_BiNormal      = 0x00002000,
	cjVtxCompFlag_Tangent       = 0x00004000,

	cjVtxCompFlag_TexCoord0     = 0x00010000,
	cjVtxCompFlag_TexCoord1     = 0x00020000,
	cjVtxCompFlag_TexCoord2     = 0x00040000,
	cjVtxCompFlag_TexCoord3     = 0x00080000,
	cjVtxCompFlag_TexCoord4     = 0x00100000,
	cjVtxCompFlag_TexCoord5     = 0x00200000,
	cjVtxCompFlag_TexCoord6     = 0x00400000,
	cjVtxCompFlag_TexCoord7     = 0x00800000,

	cjVtxCompFlag_TexCoord = 
		  cjVtxCompFlag_TexCoord0 | cjVtxCompFlag_TexCoord1 | cjVtxCompFlag_TexCoord2 | 
		  cjVtxCompFlag_TexCoord3 | cjVtxCompFlag_TexCoord4 | cjVtxCompFlag_TexCoord5 | 
		  cjVtxCompFlag_TexCoord6 | cjVtxCompFlag_TexCoord7,

	cjVtxCompFlag_TexCoordOnly_Shift = 16,
};

CJENGINE_API cjVertexCompFlag cjConvertToVertexCompFlag(cjVertexCompType type);


enum cjVtxStreamAttr
{
	// See Using-VBO.pdf

	////////////////////////////////////////////////////////////////
	// usage
	////////////////////////////////////////////////////////////////

	/*
       XXX_READ : Means we must have an easy access to read the data: 
	              AGP or system memory would be fine.

       XXX_COPY : Means we are about to do some _READ_ and _DRAW_ operations.

       XXX_DRAW : Means the buffer will be used for sending data to the GPU. 
                  We may want to use the video here (for STATIC|STREAM)
   */

	// Stream
	// Assumed to be a 1-to-1 update-to-draw. Can be thought of as data
	// that is updated about once each time it¡¯s drawn. STREAM is like
	cjStreamAttr_Usage_StreamDraw			= 0x00000001,
	cjStreamAttr_Usage_StreamRead			= 0x00000002,
	cjStreamAttr_Usage_StreamCopy			= 0x00000004,

	// Static 
	// Assumed to be a 1-to-n update-to-draw. Means the data is
	// specified once (during initialization).
	cjStreamAttr_Usage_StaticDraw			= 0x00000010,
	cjStreamAttr_Usage_StaticRead			= 0x00000020,
	cjStreamAttr_Usage_StaticCopy			= 0x00000040,

	// Dynamic 
	// Assumed to be a n-to-n update-to-draw. Generally, it means data 
	// that is updated frequently, but is drawn multiple times per update. 
	// For example, this is any dynamic data that is updated every few frames or so.
	cjStreamAttr_Usage_DynamicDraw			= 0x00000100,
	cjStreamAttr_Usage_DynamicRead			= 0x00000200,
	cjStreamAttr_Usage_DynamicCopy			= 0x00000400,


	////////////////////////////////////////////////////////////////
	// access flag
	////////////////////////////////////////////////////////////////

	cjStreamAttr_Access_ReadOnly			= 0x00001000,   // GL_READ_ONLY
	cjStreamAttr_Access_WriteOnly			= 0x00002000,   // GL_WRITE_ONLY
	cjStreamAttr_Access_ReadWrite           = 0x00004000,   // GL_READ_WRITE
};


struct CJENGINE_API cjVertexComponent : public cjMem
{
	CJ_DECL_SERIALIZE(cjVertexComponent)

	static cjBool Cmp(const cjVertexComponent& lhs, const cjVertexComponent& rhs)
	{
		if( lhs.m_streamIndex != rhs.m_streamIndex ||
			lhs.m_type != rhs.m_type ||
			lhs.m_sizType != rhs.m_sizType ||
			lhs.m_stride != rhs.m_stride ||
			lhs.m_compOffset != rhs.m_compOffset )
		{
			return cjFalse;
		}

		return cjTrue;
	}

	cjBYTE						m_streamIndex;	// stream index
	cjBYTE						m_type;			// cjVertexCompType
    cjBYTE						m_sizType;		// cjVertexCompSizType
	cjBYTE						m_pad0;
    cjINT32                     m_stride;
    cjUINT32                    m_compOffset;  // in vertex	
};


class CJENGINE_API cjFVertexDecl : public cjMem
{
    CJTYPEINFO_SUPPORT_DECL(cjFVertexDecl)
	CJ_DECL_SERIALIZE(cjFVertexDecl)

public:
	const static cjTFLAG		COMP_FLAG_TEXUVS;
	const static cjTFLAG		STREAM_FLAG_MASK = 7; // 3bit

	class Hash
	{
	public:
		cjBool operator==(const cjFVertexDecl::Hash& rhs) const
		{
			if(m_compFlag != rhs.m_compFlag ||
				m_streamFlag != rhs.m_streamFlag ||
				m_sizFlag[0] != rhs.m_sizFlag[0] ||
				m_sizFlag[1] != rhs.m_sizFlag[1] )
			{
				return cjFalse;
			}

			return cjTrue;
		}

		cjBool operator!=(const cjFVertexDecl::Hash& rhs) const { return !operator==(rhs); }

		void Reset()
		{
			m_compFlag = 0;
			m_streamFlag = 0;
			m_sizFlag[0] = 0;
			m_sizFlag[1] = 0;
		}

		cjTFLAG						m_compFlag;
		cjTFLAG						m_streamFlag;
		cjTFLAG						m_sizFlag[2];
	};

	struct CompDesc 
	{
		cjVertexCompType		m_compType;
		cjVertexCompSizType		m_compSizType;
	};

    cjFVertexDecl();
    cjFVertexDecl(const cjFVertexDecl& rhs);
    ~cjFVertexDecl();

    cjFVertexDecl& operator=(const cjFVertexDecl& rhs);

    cjBool operator==(const cjFVertexDecl& rhs) const
	{
		if(this == &rhs)
			return cjTrue;

		if(m_hash == rhs.m_hash && m_stride == rhs.m_stride)
			return _VtxCompCmp(rhs);
    
		return cjFalse;
	}

	cjBool operator!=(const cjFVertexDecl& rhs) const { return !operator==(rhs); }

	cjBool CmpHash(const cjFVertexDecl& rhs) const { return m_hash == rhs.m_hash; }
	const Hash& GetHash() const { return m_hash; }
    
    cjINT32 GetStride() const { return m_stride; }
    cjINT32 GetNumComp() const { return m_numComp; }
    
    const cjVertexComponent* GetComp(cjINT32 index) const { return &m_comp[index]; }
    const cjVertexComponent* GetCompByShaderVtxAttrType(cjEShaderVtxAttr shaderVtxAttr) const; /* cjEShaderVtxAttr */
    
    cjBool HasComp(cjVertexCompType compType) const { return m_hash.m_compFlag & cjConvertToVertexCompFlag(compType) ? cjTrue : cjFalse; }
    cjINT32 GetUVCompCount() const;

    // MakeFromDecl(otherSrcDecl, 2, cjVtxComp_Position, cjVtxComp_Normal, ...)
    cjBool MakeFromOtherDecl(const cjFVertexDecl& srcDecl, cjINT32 numComp, ...);
    
	/*
		Make(2, 
			streamIndex, cjVtxComp_Position, cjVtxCompSiz_FLOAT3,
			streamIndex, comptype, compsize, 
			...);
	*/
    cjBool MakeDecl(cjINT32 numComp, ...);

	cjBool MakeFromVtxCompStreamAry(class cjVtxCompStream** ary, cjINT32 numAry=cjVtxComp_MAX);
	
	//
	// QueryMtlDesc Supply
	//
	static void SetCompStream(cjTFLAG& streamFlag, cjINT32 compIndex, cjINT32 streamIndex);
	static cjINT32 GetCompStreamIndex(const cjTFLAG& streamFlag, cjINT32 compIndex);

	static void SetCompSizType(cjTFLAG sizFlag[], cjINT32 compIndex, cjVertexCompSizType sizType);
	static cjVertexCompSizType GetCompSizType(const cjTFLAG sizFlag[], cjINT32 compIndex);

protected:

    void Reset();

	cjBool _VtxCompCmp(const cjFVertexDecl& rhs) const
	{
		cjAssert(m_numComp == rhs.m_numComp);

		for(cjINT32 i=0; i<m_numComp; i++)
		{
			if(!cjVertexComponent::Cmp(m_comp[i], rhs.m_comp[i]))
				return cjFalse;
		}

		return cjTrue;
	}

protected:
	Hash							m_hash;
	
	cjINT32							m_numComp;
    cjVertexComponent*				m_comp;

	cjINT32							m_numTexUV;
    cjINT32							m_stride;
};

CJTYPEINFO_SUPPORT_IMPL(cjFVertexDecl)


struct CJENGINE_API cjFMipmapBase : public cjMem
{
public:
    cjFMipmapBase()	{}

	cjFMipmapBase(cjBYTE potBitU, cjBYTE potBitV)
	{
        m_potSizU = 1 << potBitU;  // POT Siz = power of two (2^n)
        m_potSizV = 1 << potBitV;
        
        m_potBitU = potBitU;
        m_potBitV = potBitV;

		m_refPos = NULL;
	}
  
	cjINT32                         m_potSizU, m_potSizV;   // POT Siz
	cjBYTE                          m_potBitU, m_potBitV;   // POT Bit

	cjBYTE*                         m_refPos;				// valid only when in lock
};


struct CJENGINE_API cjFMipmap : public cjFMipmapBase
{
	CJTYPEINFO_SUPPORT_DECL(cjFMipmap)

public:
    cjFMipmap() {}
	cjFMipmap(cjBYTE potBitU, cjBYTE potBitV)
        : cjFMipmapBase(potBitU, potBitV), m_data(m_potSizU * m_potSizV)	{}    
	cjFMipmap(cjBYTE potBitU, cjBYTE potBitV, cjINT32 siz)
        : cjFMipmapBase(potBitU, potBitV), m_data(siz) {}
    
	void Clear()
	{
		cjMemZero(&m_data(0), m_data.Count());
	}

	friend cjFStream& operator<< (cjFStream& stream, cjFMipmap& mip)
	{		
		stream << mip.m_potSizU;	stream << mip.m_potSizV;
		stream << mip.m_potBitU;	stream << mip.m_potBitV;

		stream << mip.m_data;

		return stream;
	}
   
    cjTAry<cjBYTE>                m_data;
};

CJTYPEINFO_SUPPORT_IMPL(cjFMipmap)


#if defined(CJ_PLATFORM_APPLE)
#include "engine/apple/cjEAGLDef.h"
#endif


#endif

