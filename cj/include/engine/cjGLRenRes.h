
#ifndef _CJGLRENRES_H_
#define _CJGLRENRES_H_

#include "cjGLCommon.h"
#include "cjTResCache.h"
#include "cjGLShaderConstant.h"

class cjGLRenDev;
class cjGLVertexStream;
class cjGLIndexBuffer;
class cjGLTexture;
class cjGLResCache;
class cjGLRenTarget;
class cjGLVertexShader;
class cjGLPixelShader;
class cjGLShaderProgram;


cjINT32 cjGLGetVertexCompSizDesc(const cjVertexCompSizType compSizType, GLint* compNum=NULL,
    GLenum* compVarType=NULL);

//
// cached resource in opengl render context(source data from cjFRenResSrc)
//

struct CJENGINE_API cjGLResStatData
{
	cjTFLAG					m_dummy;
};

class CJENGINE_API cjGLResCache : public cjSerializable
{
	CJSER_DECL_CLASS(cjGLResCache, cjSerializable, CJRTTI_CF_ABSTRACT, CJENGINE_API)
	CJSER_NO_CLONEABLE

	friend class cjGLRenDev;
	friend class cjTResCacheContainer<cjGLResCache>;
   
public:
    typedef cjGLResCache* (*InstanceCreatorPFN)(const cjRTTI* , cjGLRenDev*, cjFRenResSrc*);
    
    static void RegisterInstanceCreator(cjResCacheIDType type, InstanceCreatorPFN creator);
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
    
    static void StaticGLResCacheInit();
    
public:
	cjGLResCache() { m_cacheID = 0; }
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	cjResCacheID GetCacheID() { return m_cacheID; }
	cjINT32 GetCacheRev() { return m_cacheRev; }
    
	void SetCacheRev(cjINT32 rev) { m_cacheRev = rev; }
//	cjGLRenDev* GetRenDev() { return m_renDev; }
    
    void SetLastFrameUsed(cjINT32 frame) { m_lastFrameUsed = frame; }
    cjINT32 GetLastFrameUsed() { return m_lastFrameUsed; }
    
	virtual void Cache(cjFRenResSrc* renResSrc) {}

	virtual void CollectUsageStatistics(cjGLResStatData& in);

	// resource interfaces
	virtual cjGLVertexStream* GetVertexStream() { return NULL; }
	virtual cjGLIndexBuffer* GetIndexBuffer() { return NULL; }
	virtual cjGLTexture* GetTexture() { return NULL; }

protected:	
    cjFResCacheLinkInfo* GetResCacheLinkInfo() { return &m_cli; }
    
protected:
    cjGLRenDev*								m_renDev;
    
private:
    cjFResCacheLinkInfo                     m_cli;

	cjResCacheID							m_cacheID;
	cjINT32									m_cacheRev;

	cjINT32									m_lastFrameUsed;
    
    static InstanceCreatorPFN               ms_instanceCreator[cjRSCID_Num];
};


class CJENGINE_API cjGLTexture : public cjGLResCache
{
	CJSER_DECL_CLASS(cjGLTexture, cjGLResCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);

    typedef cjGLResCache ResCacheType;

	virtual cjGLTexture* GetTexture() { return this; }
	virtual void CollectUsageStatistics(cjGLResStatData& in) {}

	cjINT32 GetWidth() { return m_width; }
	cjINT32 GetHeight() { return m_height; }

public:
	cjGLTexture() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);
    virtual void Cache(cjFRenResSrc* renResSrc);
   
public:
	GLuint									m_textureID;
	cjBool									m_isCubeMap;

    // cached stuff
	cjINT32									m_width; 
	cjINT32									m_height;
    cjINT32                                 m_firstMip;
	cjINT32									m_numMip;
    cjRSurfaceFmt							m_format;
};


//
// cjGLVertexStreamAttribLayout
//

class CJENGINE_API cjGLVertexStreamAttribLayout : public cjGLResCache
{
	CJSER_DECL_CLASS(cjGLVertexStreamAttribLayout, cjGLResCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
    virtual void Cache(cjFRenResSrc* renResSrc); // cjFVertexStreamAttribLayout*

public:
    typedef cjGLResCache ResCacheType;

	cjGLVertexStreamAttribLayout() {}
    void Init(cjGLRenDev* renDev, cjResCacheID cacheID);
    
    GLuint GetVAO() { return m_vao; }
    const cjFVertexDecl* GetVtxDecl() { return &m_vtxDecl; }
    
    static cjBool SetupVertexArrayForShaderProgram(cjGLRenDev* renDev, GLuint vao,
        const cjGLShaderProgram* prog, const cjFVertexDecl* decl);
//    static void SetupVertexAttributeOnVAO(cjGLRenDev* renDev, const cjFVertexDecl* decl);
    
protected:
    void ResetVAO();
    
private:
    GLuint                                  m_vao;      // vertex array object handle
    cjFVertexDecl                           m_vtxDecl;
};


//
// VertexStream
//
class CJENGINE_API cjGLVertexStream : public cjGLResCache
{
	CJSER_DECL_CLASS(cjGLVertexStream, cjGLResCache, CJRTTI_CF_ABSTRACT, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);

	virtual void Cache(cjFRenResSrc* renResSrc) { } // = 0; // cjFVertexStreamAttribLayout*
	virtual void CollectUsageStatistics(cjGLResStatData& in) { }

public:
	enum { INITIAL_DYN_VTXBUFSIZ = 65536 };

	virtual cjGLVertexStream* GetVertexStream() { return this; }
    virtual void* GetEAGLVertexStream() { return NULL; }

	cjGLVertexStream() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	virtual void* GetVertexData() { return 0; } // = 0;
	virtual void Reallocate(cjINT32 newSiz) {} // = 0;

	// returns index of the first vertex of appended in this stream.
	virtual cjINT32 AddVertices(cjFVertexStream* vertices) { return -1; } // = 0;
    
    virtual void ResetStream();
    
    GLuint GetVtxBufHandle(cjINT32 index=0) { return m_vtxBufHandle; }
    GLuint GetNumVertexBuffer() { return m_numVertexBuffer; }

	// for glBufferData
	static GLenum ResolveStreamUsageFlag(const cjFVertexStream* vtxStream); // GL_STATIC_DRAW ..

	// for glMapBufferData
	static GLenum ResolveStreamAccessFlag(const cjFVertexStream* vtxStream); // GL_WRITE_ONLY ..

protected:
	cjINT32									m_cachedSiz;
	cjINT32									m_tail;
    
    cjINT32                                 m_numVertexBuffer;
    GLuint                                  m_vtxBufHandle;     // vertexBuffer
};


#if defined (CJ_PLATFORM_WIN32)

class CJENGINE_API cjWGLVertexStream : public cjGLVertexStream
{
	CJSER_DECL_CLASS(cjWGLVertexStream, cjGLVertexStream, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
    virtual void Cache(cjFRenResSrc* renResSrc);

public:
	cjWGLVertexStream() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	virtual void* GetVertexData();
	virtual void Reallocate(cjINT32 newSiz);

	virtual cjINT32 AddVertices(cjFVertexStream* vertices);
};


#endif

//	// nvidia implementation
//class CJENGINE_API cjGLVertexStreamNV : public cjGLVertexStream
//{
//	CJSER_NO_CLONEABLE
//public:
//	virtual void CollectUsageStatistics();
//
//		// cjGLVertexStream
//	virtual void Cache(cjFVertexStream* src);
//	virtual void* GetVertexData();
//	virtual void Reallocate(cjINT32 newSiz);
//	virtual cjINT32 AddVertices(cjFVertexStream* vertices);
//
//public:
//	cjGLVertexStreamNV(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynVB,
//		cjUINT32 sizDyn=cjGLVertexStream::INITIAL_DYN_VTXBUFSIZ)
//		: cjGLVertexStream(renDev, cacheID, isDynVB)
//	{
//		if(isDynVB)
//			Reallocate(sizDyn);
//	}
//
//private:
//	cjBYTE*									m_vertexData;
//};


//
// cjGLVertexDeclService
//

//class CJENGINE_API cjGLVertexDeclService
//{
//public:
//    static void PerformVertexAttribFromVertexDecl(cjGLRenDev* renDev, const cjFVertexDecl* decl);
//};


//
// indexBuffer
//

class CJENGINE_API cjGLIndexBuffer : public cjGLResCache
{
	CJSER_DECL_CLASS(cjGLIndexBuffer, cjGLResCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
	virtual void Cache(cjFRenResSrc* renResSrc) {} // = 0;
 
public:
	enum { INITIAL_DYN_IDXBUFSIZ = 32768 };

public:
	virtual cjGLIndexBuffer* GetIndexBuffer() { return this; }
	virtual class cjGLIndexBufferVBO* GetIndexBufferVBO() { return NULL; }
	virtual class cjGLIndexBufferNonVBO* GetIndexBufferNonVBO() { return NULL; }

	virtual cjINT32 GetByteSiz() { return m_cachedByteSiz; }
	virtual cjINT32 GetIndexCount() { return m_indexCount; }

	virtual cjBool BindIndexBufferToGL(cjGLRenDev* renDev) { return cjFalse; } // = 0;
	virtual cjBool UnBindIndexBufferToGL(cjGLRenDev* renDev) { return cjFalse; }

	cjGLIndexBuffer() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynIB=cjFalse);

protected:
	cjINT32									m_cachedByteSiz;
	cjINT32									m_indexCount;
	cjBool									m_isDynIB;
};


// not using glveretxbufferobject
class CJENGINE_API cjGLIndexBufferNonVBO : public cjGLIndexBuffer
{
	CJSER_DECL_CLASS(cjGLIndexBufferNonVBO, cjGLIndexBuffer, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
	virtual void Cache(cjFRenResSrc* renResSrc);
	virtual class cjGLIndexBufferNonVBO* GetIndexBufferNonVBO() { return this; }

	virtual cjINT32 GetByteSiz() { return m_cachedByteSiz; }
	virtual cjINT32 GetIndexCount() { return  m_indexCount; }

	virtual cjBool BindIndexBufferToGL(cjGLRenDev* renDev) { cjAssert(0 && "notImpl"); return cjFalse; }

public:
	cjGLIndexBufferNonVBO() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynIB=cjFalse);

	virtual void* GetIndexDat() { return &m_indexDat(0); }
	virtual void Reallocate(cjINT32 newSiz);
	virtual cjINT32 AddIndices(cjFIndexBuffer* indexBuf);

protected:
	cjTAry<cjBYTE>						m_indexDat;
};


// using vbo
class CJENGINE_API cjGLIndexBufferVBO : public cjGLIndexBuffer
{
	CJSER_DECL_CLASS(cjGLIndexBufferVBO, cjGLIndexBuffer, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
	// static index buffer only, as static drawing
	virtual void Cache(cjFRenResSrc* renResSrc); 

	virtual class cjGLIndexBufferVBO* GetIndexBufferVBO() { return this; }
	
	virtual cjBool BindIndexBufferToGL(cjGLRenDev* renDev);
	virtual cjBool UnBindIndexBufferToGL(cjGLRenDev* renDev);

public:
	cjGLIndexBufferVBO() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjBool isDynIB=cjFalse);

	virtual GLuint GetHandle() { return m_handle; }
		
	virtual void Reallocate(cjINT32 newSiz); // dynamic index buffer only 
	virtual cjINT32 AddIndices(cjFIndexBuffer* indexBuf); // dynamic index buffer only

protected:
    GLuint                                  m_handle;
};


//class CJENGINE_API cjGLIndexBufferARB : public cjGLIndexBuffer
//{
//public:
//    typedef cjGLResCache ResCacheType;
//
//	virtual void* GetIndexData() { return &m_indexData(0); }
//	virtual void Cache(cjIndexBuffer* src);
//	virtual void Reallocate(cjINT32 newSiz);
//	virtual cjINT32 AddIndices(cjIndexBuffer* indices);
//
//public:
//	cjGLIndexBufferARB(cjGLRenDev* renDev, cjResCacheID cacheId, cjBool isDynIB)
//		: cjGLIndexBuffer(renDev, cacheId, isDynIB)
//	{
//		if(isDynIB)
//			Reallocate(cjGLIndexBuffer::INITIAL_DYN_IDXBUFSIZ);
//	}
//
//protected:
//	cjTAry<cjBYTE>						m_indexData;
//};


//
// Shaders
//

class CJENGINE_API cjGLShaderCache : public cjGLResCache
{
	CJSER_DECL_CLASS(cjGLShaderCache, cjGLResCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

	friend class cjGLRenDev;
	friend class cjTResCacheContainer<cjGLShaderCache>;
    
public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
	virtual void Cache(cjFRenResSrc* renResSrc) {} // cjIndexBuffer*

public:
	cjGLShaderCache() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	virtual cjGLVertexShader* GetVertexShader() { return NULL; }
	virtual cjGLPixelShader* GetPixelShader() { return NULL; }
	virtual cjGLShaderProgram* GetShaderProgram() { return NULL; }
    
};


class CJENGINE_API cjGLShaderProgram : public cjGLShaderCache
{
	CJSER_DECL_CLASS(cjGLShaderProgram, cjGLShaderCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
	virtual void Cache(cjFRenResSrc* renResSrc);
    virtual cjGLShaderProgram* GetShaderProgram() { return this; }

public:
	enum EFLAG
	{
		EFL_FAIL								= 0xfff00000,

		EFL_FAIL_CREATE_PROGOBJ					= 0x10000000,
		EFL_FAIL_LINK							= 0x20000000,

		EFL_FAIL_SRC_VERTEX_SHADER				= 0x01000000,
		EFL_FAIL_SRC_PIXEL_SHADER				= 0x02000000,
		EFL_FAIL_CACHE_VERTEX_SHADER			= 0x04000000,
		EFL_FAIL_CACHE_PIXEL_SHADER				= 0x08000000
	};

	cjGLShaderProgram() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	virtual cjUINT32 GetNumStreams() { return 0; }

	cjBool IsErr() 
	{ 
		if(m_shaderFlag & EFL_FAIL)
			return cjTrue;

		return cjFalse;
	}

	GLhandle GetHandle() { return m_handle; }
    const cjFVertexDecl* GetVertexDecl() { return &m_vertexDecl; }
    
    const cjGLVertexAttribInfo* GetVertexAttribInfo() const { return m_vertexAttribInfo; }
    cjGLShaderUniformValSet* GetUniformValSet() { return m_uniformValSet; }
    
    void Reset();

private:
	cjTFLAG									m_shaderFlag;
	cjINT32									m_lastFrameUsed;
	
	GLhandle								m_handle;
    cjFVertexDecl                           m_vertexDecl;
    
    cjGLVertexAttribInfo*                   m_vertexAttribInfo;
    cjGLShaderUniformValSet*                m_uniformValSet;
};


class CJENGINE_API cjGLCompileShader : public cjGLShaderCache
{
	CJSER_DECL_CLASS(cjGLCompileShader, cjGLShaderCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
	virtual void Cache(cjFRenResSrc* renResSrc) {}

public:
    enum { MAX_GLSL_MACRO_LEN = 1024 };

	enum EFLAG
	{
		EFL_FAIL					= 0xff000000,

		EFL_FAIL_CREATE_SHADEROBJ	= 0x10000000,

		EFL_FAIL_COMPILE_BADSRC		= 0x01000000,
		EFL_FAIL_COMPILE_COMPILING	= 0x02000000,
		EFL_FAIL_COMPILE			= EFL_FAIL_COMPILE_BADSRC | EFL_FAIL_COMPILE_COMPILING,
		
		EFL_PRECOMPILED				= 0x00000100,
		EFL_COMPILE_NEED			= 0x00000200,
		EFL_COMPILE_OK				= 0x00000400,
	};

	cjGLCompileShader() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID, cjRenShaderType shaderType);

	cjBool IsNeedToCompile() 
	{ 
		if(m_shaderFlag & EFL_FAIL_COMPILE)
			return cjFalse;

		return m_shaderFlag & EFL_COMPILE_NEED ? cjTrue : cjFalse; 
	}

	void SetShaderFlag(cjTFLAG flag, cjBool replace=cjFalse) { m_shaderFlag |= flag; }
	void UnSetShaderFlag(cjTFLAG flag) { m_shaderFlag = m_shaderFlag & (~flag); }
	void ResetShaderFlag() { m_shaderFlag = 0; }

	cjBool Compile(GLhandle shaderHandle, const GLbyte* shaderSrc, GLint shaderSrcLen,
		cjBool force=cjFalse);

	GLhandle GetShaderHandle() { return m_handle; }
    
    static void AddCompileMacro(const char* macroStr);

protected:
	void ReleaseShaderHandle();
	void DoCache(cjFShader* shader);
    
protected:
	cjTFLAG										m_shaderFlag;
	cjRenShaderType								m_shaderType;
	GLhandle									m_handle;
    
    static char                                 ms_preShaderMacroDefine[MAX_GLSL_MACRO_LEN];
    static cjINT32                              ms_preShaderMacroDefineLen;
};

class CJENGINE_API cjGLVertexShader : public cjGLCompileShader
{
	CJSER_DECL_CLASS(cjGLVertexShader, cjGLCompileShader, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
   virtual void Cache(cjFRenResSrc* renResSrc); // cjFVertexShader*

public:
    typedef cjGLShaderCache ResCacheType;
	virtual cjGLVertexShader* GetVertexShader() { return this; }

public:
	cjGLVertexShader() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);
};

class CJENGINE_API cjGLPixelShader : public cjGLCompileShader
{
	CJSER_DECL_CLASS(cjGLPixelShader, cjGLCompileShader, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

public:
   virtual void Cache(cjFRenResSrc* renResSrc); // cjFPixelShader*

public:
	cjGLPixelShader() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

    typedef cjGLShaderCache ResCacheType;
	virtual cjGLPixelShader* GetPixelShader() { return this; }
};


class CJENGINE_API cjGLRenTargetGrp : public cjGLResCache
{
	CJSER_DECL_CLASS(cjGLRenTargetGrp, cjGLResCache, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

	friend class cjGLRenDev;
    
public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
	void Cache(cjFRenResSrc* renResSrc); // cjFRenTargetGrp*

public:
    typedef cjGLResCache ResCacheType;

	cjGLRenTargetGrp() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	static cjINT32 GetMaxColorSurface() { return cjFRenTargetGrp::MAX_MULTI_RENDER_TARGETS; }

protected:
	virtual void SetRenderTarget(cjINT32 index, cjGLRenTarget* rtg);
	virtual cjBool BeginRenTargetGrp();
	virtual void EndRenTargetGrp();

	virtual void ReleaseFrameBufferObj();

protected:
	GLhandle							m_fboHandle; // glFrameBufferObj Handle
	cjGLRenTarget*						m_renTarget[cjFRenTargetGrp::MAX_MULTI_RENDER_TARGETS];
};


class CJENGINE_API cjGLRenTarget : public cjGLTexture
{
	CJSER_DECL_CLASS(cjGLRenTarget, cjGLTexture, 0, CJENGINE_API)
	CJSER_NO_CLONEABLE

	friend class cjGLRenTargetGrp;
    
public:
    static cjGLResCache* CreateInstance(const cjRTTI* glResRTTI, cjGLRenDev* renDev, cjFRenResSrc* src);
 	virtual void Cache(cjFRenResSrc* renResSrc); // cjFRenTarget* renTarget

public:
    typedef cjGLShaderCache ResCacheType;

	cjGLRenTarget() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

	virtual GLhandle GetColorSurface() { return m_handleColor; }
	virtual GLenum GetColorSurfaceType() { return m_colorSurType; }
	
	virtual GLhandle GetDepthStencil() { return m_handleDepthStencil; }
    virtual GLenum GetDepthStencilGLFmt() { return m_depthStencilGLFmt; }

protected:
	virtual void ReleaseColorSurface();
	virtual void ReleaseDepthStencil();

protected:
	GLhandle							m_handleColor;
	GLenum								m_colorSurType; // GL_TEXTURE_2D

	GLhandle							m_handleDepthStencil;
    GLenum                              m_depthStencilGLFmt;
};


//
// Apple extension
//

#if defined(CJ_PLATFORM_APPLE)
#include "engine/apple/cjEAGLRenRes.h"
#endif


#endif
