
#include "cjEnginePCH.h"
#include "cjRenRes.h"


//
// cjFRenResSrc
//

cjRenDev* cjFRenResSrc::GetRenDev()
{
	cjEngine* engine = cjEngine::GetEngine();
	return engine ? engine->GetRenDev() : NULL;
}

cjFRenResSrc::~cjFRenResSrc()
{
   	cjResCacheID cacheID = GetCacheID();

	if(cacheID && GetCacheRev())
	{
		cjRenDev* renDev = GetRenDev();

		if(renDev)
			renDev->DisposeResCache(cacheID);
	}
}


//
// cjFBaseTexutre
//

    // cjFStaticTexture
// implemented in cjTexture.cpp

//
// cjFVertexStreamAttribLayout
//

cjFVertexStreamAttribLayout::cjFVertexStreamAttribLayout(const cjFVertexStream::StreamAry streams,
	cjINT32 streamCount)
{
	m_cacheID = cjResMakeCacheID(cjRSCID_VertexStreamAttrib);

	cjMemCpy(m_streams, streams, sizeof(streams));
    m_streamCount = streamCount;
}

cjFVertexStreamAttribLayout::~cjFVertexStreamAttribLayout()
{

}


//
// cjFIndexBuffer
//

cjFStream& SerializeStream(cjFStream& stream, cjFIndexBuffer** d)
{
	if(stream.IsObjOnly())
		return stream;

	if(stream.IsLoading())
	{
		cjAssert((*d) == NULL);
		(*d) = new cjFIndexBuffer;
	}

	(**d) << stream;
	stream << cjFStreamCmprI64((*d)->m_indexInterval);

	return stream;
}

cjFStream& cjFIndexBuffer::operator << (cjFStream& stream)
{
	if(stream.IsObjOnly())
		return stream;

	stream << m_indices;

	return stream;	
}

void cjFIndexBuffer::SetIndexDat(cjINT32 indexCount, void* indexBuf, cjINT32 stride)
{
	cjAssert(stride == GetStride());

	m_indices.RemoveAll(indexCount);
	m_indices.PushEmpty(indexCount); // set siz

	if(indexBuf)
		cjMemCpy(GetBuf(), indexBuf, stride * indexCount);

	IncCacheRev();
}

cjINT32 cjFIndexBuffer::CopyTo(void* dst, cjINT32 bytesStPos, cjINT32 bytesToCopy)
{
	if(bytesToCopy == -1)
		bytesToCopy = m_indices.Num() * sizeof(cjWORD);

	cjINT32 stPos = 0;
	if(bytesStPos)
		stPos = bytesStPos / sizeof(cjWORD);

	cjMemCpy(dst, &m_indices(stPos), bytesToCopy);

	return bytesToCopy;
}

void cjFIndexBuffer::OptimizeTriListIndexDatSequence()
{
#if defined(CJ_PLATFORM_WIN32)
	//PrimitiveGroup*	priGrps = NULL;
	//unsigned short	numPriGrps = 0;

	//	// using nvStrip
	//SetListsOnly(true); // do not generate tri-strip
	//GenerateStrips(&m_indices(0), m_indices.Num(), &priGrps, &numPriGrps);

	//m_indices.RemoveAll(priGrps->numIndices);
	//m_indices.PushEmpty(priGrps->numIndices);

	//cjMemCpy(&m_indices(0), priGrps->indices, m_indices.Num() * sizeof(cjWORD));

	//delete [] priGrps;

	//IncCacheRev();
#endif
}



//
// cjFShader
//

cjFShader::cjFShader(cjTFLAG flag, const char* name, cjINT32 ver)
{
	m_cacheID = cjResMakeCacheID(cjRSCID_Shader);

	m_flag = flag;
	m_comptiableVer = ver;

	m_name = name;
}

cjFShader::~cjFShader()
{
}

void cjFShader::GenerateFromMem(const cjBYTE* src, cjINT32 len)
{
	if(!src || !len)
	{
		m_flag |= EFLAG_NULL_SHADER;
		return;
	}

	m_shader.SetSiz(sizeof(cjBYTE)*len+1);
	cjMemCpy(&m_shader(0), src, sizeof(cjBYTE)*len);
	m_shader(len) = NULL; // as null
}

//
// cjFVertexShader
//
//cjFVertexShader::cjFVertexShader(cjGLRenDev* renDev, ERESFROM_FILE_TAG, const char* path,
//	const char* name, cjINT32 ver)
//	: cjFShader(renDev, EFLAG_VERTEX_SHADER | EFLAG_SRCFROM_FILE, name, ver)
//{	
//	cjAssert(0);
//}

cjFVertexShader::cjFVertexShader( ESHADER_FROM_STR_TAG,
	const char* shaderText, const char* name, cjINT32 ver)
	: cjFShader(EFLAG_VERTEX_SHADER | EFLAG_SRCFROM_MEM, name, ver)
{
	GenerateFromMem((const cjBYTE*)shaderText, (cjINT32)strlen(shaderText));
}

//cjFPixelShader::cjFPixelShader(cjGLRenDev* renDev, ERESFROM_FILE_TAG, const char* path)
//{
//}

cjFPixelShader::cjFPixelShader( ESHADER_FROM_STR_TAG, const char* shaderText,
	const char* name, cjINT32 ver)
	: cjFShader(EFLAG_PIXEL_SHADER | EFLAG_SRCFROM_MEM, name, ver)
{
	GenerateFromMem((const cjBYTE*)shaderText, (cjINT32)strlen(shaderText));
}


//
// cjFShaderProgram
//

cjFShaderProgram::cjFShaderProgram( const char* name)
{
	m_cacheID = cjResMakeCacheID(cjRSCID_Shader);

	if(name)
		m_name = name;

	m_vtxShader = NULL;
	m_delVtxShader = cjFalse;

	m_pxlShader = NULL;
	m_delPxlShader = cjFalse;
}

cjFShaderProgram::cjFShaderProgram(cjFVertexShader* vertexShader, cjFPixelShader* pixelShader,
	cjBool delOnDestroy)
{
	m_cacheID = cjResMakeCacheID(cjRSCID_Shader);


	SetShader(cjShader_VertexShader, vertexShader, delOnDestroy);
	SetShader(cjShader_PixelShader, pixelShader, delOnDestroy);
}

cjFShaderProgram::~cjFShaderProgram()
{
	if(m_vtxShader && m_delVtxShader)
    {
        delete m_vtxShader;
        m_vtxShader = NULL;
    }

	if(m_pxlShader && m_delPxlShader)
    {
        delete m_pxlShader;
        m_pxlShader = NULL;
    }
}

cjFShader* cjFShaderProgram::GetShader(cjRenShaderType shaderType)
{
	switch(shaderType)
	{
	case cjShader_VertexShader: return m_vtxShader;
	case cjShader_PixelShader: return m_pxlShader;
	default:
		break;
	}

	return NULL;
}

void cjFShaderProgram::SetShader(cjRenShaderType shaderType, cjFShader* shader, 
	cjBool delOnDestroy)
{	
	switch(shader->GetShaderType())
	{
	case cjShader_VertexShader: 
		{
			m_vtxShader = (cjFVertexShader*)shader;
			m_delVtxShader = delOnDestroy;
		}
		break;

	case cjShader_PixelShader:
		{
			m_pxlShader = (cjFPixelShader*)shader;
			m_delPxlShader = delOnDestroy;
		}
		break;

	default:
		break;
	}
}


//
// cjFRenTarget
//

cjFRenTarget::cjFRenTarget(cjINT32 width, cjINT32 height,
	cjRSurfaceFmt fmt, cjRSurfaceFmt dsFmt)
{
	m_cacheID = cjResMakeCacheID(cjRSCID_RenderTarget);

	m_width = width;
	m_height = height;
	m_format = fmt;
	m_dsformat = dsFmt;
}

cjFRenTarget::~cjFRenTarget()
{
}


//
// RenderTargetGroup
//

cjFRenTargetGrp::cjFRenTargetGrp()
{
	m_cacheID = cjResMakeCacheID(cjRSCID_RenderTarget);

	cjMemZero(m_renTarget, sizeof(m_renTarget));
}

cjFRenTargetGrp::~cjFRenTargetGrp()
{
	for(cjINT32 i=0; i<MAX_MULTI_RENDER_TARGETS; i++)
	{
		if(m_renTarget[i])
			delete m_renTarget[i];
	}
}

cjINT32 cjFRenTargetGrp::GetWidth(cjINT32 idxRenTarget)
{
	return m_renTarget[idxRenTarget] ? m_renTarget[idxRenTarget]->GetWidth() : 0;
}

cjINT32 cjFRenTargetGrp::GetHeight(cjINT32 idxRenTarget)
{
	return m_renTarget[idxRenTarget] ? m_renTarget[idxRenTarget]->GetHeight() : 0;
}

void cjFRenTargetGrp::AttachRenTarget(cjINT32 index, cjFRenTarget* rt)
{
	if(m_renTarget[index])
		delete m_renTarget[index];

	m_renTarget[index] = rt;
}


//
// cjVtxStreamComp
//

cjFStream& cjVtxCompStream::operator >>(cjFStream& stream)
{
	cjPODSerializeWithType<cjBYTE>(stream, m_type);
	cjPODSerializeWithType<cjBYTE>(stream, m_sizType);
	cjPODSerializeWithType<cjBYTE>(stream, m_elementSiz);
	stream << cjFStreamCmprI32(m_vertexCount);

	cjSLOG("type=%d , m_sizType=%d, m_elementSiz=%d ", m_type, m_sizType, m_elementSiz);

	stream << m_data;

	//cjtemp
	//if(m_type == cjVtxComp_TexCoord0)
	//{
	//	cjINT32 count = m_data.Count();
	//	cjVec2* p =(cjVec2*)m_data.GetData();

	//	for(cjINT32 i=0; i<count/8 ; i++)
	//	{
	//		cjLOG("[vtx-texuv0] %d , %f  %f ", i, p->x, p->y);
	//		p++;
	//	}
	//}
	/*if(m_type == cjVtxComp_Position)
	{
		int count = m_data.Count();
		cjVec3* p =(cjVec3*)m_data.GetData();

		for(int i=0; i<count/12 ; i++)
		{
			cjLOG("[vtx-pos] %d , %f  %f  %f  ", i, p->x, p->y, p->z);
			p++;
		}
	}*/
	//else if(m_type == cjVtxComp_BL4Index)
	//{
	//	int count = m_data.Count();
	//	cjBYTE* p =(cjBYTE*)m_data.GetData();

	//	for(int i=0; i<count/4 ; i++)
	//	{
	//		cjLOG("[vtx-blendIndices] %d , %d  %d  %d  %d  ", i, *p, *(p+1), *(p+2), *(p+3));
	//		p += 4;
	//	}
	//}
	//else if(m_type == cjVtxComp_BL4Weight)
	//{
	//	int count = m_data.Count();
	//	cjVec4* p =(cjVec4*)m_data.GetData();

	//	for(int i=0; i<count/16 ; i++)
	//	{
	//		cjLOG("[vtx-blendWeight] %d , %f  %f  %f  %f  ", i, p->x, p->y, p->z, p->w);
	//		p++;
	//	}
	//}	
	//cjSLOG("m_data=siz =%d ", m_data.Num());

	return stream;
}

void cjVtxCompStream::SetCompInfo(cjVertexCompType type, cjVertexCompSizType sizType, cjBYTE streamIndex)
{
	m_streamIndex = streamIndex;
	m_type = (cjBYTE)type;
	m_sizType = (cjBYTE)sizType;
	m_elementSiz = cjGetSizFromVertexCompSizType((cjVertexCompSizType)m_sizType);
}

cjBYTE* cjVtxCompStream::Lock()
{
	return &m_data(0);
}

void cjVtxCompStream::UnLock()
{
}

void cjVtxCompStream::SetVertexCount(cjINT32 count)
{
	m_vertexCount = count;
	m_data.RemoveAll(m_elementSiz * m_vertexCount, cjTrue);	
}

void cjVtxCompStream::Copy(cjBYTE* dst, cjINT32 preOffset, cjINT32 stride)
{
	cjBYTE* cur = dst + preOffset;
	cjBYTE* src = &m_data(0);

	for(cjINT32 i=0; i<m_vertexCount; i++)
	{
		cjMemCpy(cur, src, m_elementSiz);

		cur += stride;
		src += m_elementSiz;
	}
}

//
// cjFVertexStream
//

cjFVertexStream::cjFVertexStream()
{
    m_cacheID = cjResMakeCacheID(cjRSCID_VertexStream);
}

cjFVertexStream::~cjFVertexStream()
{
}

cjFVertexStream* cjFVertexStream::CreateInstance(cjBYTE clsID)
{
	switch(clsID)
	{
	case 1: return new cjFStaticVertexStream();
	default:
		break;
	}

	return NULL;
}

cjFStream& SerializeStream(cjFStream& stream, cjFVertexStream** d)
{
	if(stream.IsObjOnly())
		return stream;

	cjBYTE instClsID;

	if(stream.IsSaving())
		instClsID = (*d)->GetInstanceClsID();

	stream << instClsID;

	if(stream.IsLoading())
	{
		cjAssert((*d) == NULL);
		(*d) = cjFVertexStream::CreateInstance(instClsID);
	}

	(**d) << stream;

	return stream;
}


//
// cjFStaticVertexStream
//

cjFStream& cjFStaticVertexStream::operator<<(cjFStream& stream)
{
	if(stream.IsLoading())		
	{
		m_StreamDirtyFlag = 0;
		cjMemZero(m_compStream, sizeof(cjVtxCompStream*) * cjVtxComp_MAX);
	}

	stream << m_vtxDecl;
	stream << m_attr;
	cjPODSerializeWithType<cjBYTE>(stream, m_primitiveType);
	stream << cjFStreamCmprI32(m_numVertex);

	cjUINT32 compStreamFlag = 0; // MAX 32 kind
	if(stream.IsSaving())
	{
		cjAssert(cjVtxComp_MAX < 32);
		for(cjINT32 i=0; i<cjVtxComp_MAX; i++)
		{
			if(m_compStream[i])
				compStreamFlag |= 1 << i;
		}
	}
		
	cjPODSerialize(stream, compStreamFlag);
	cjSLOG("[%d] compStreamFlag=%x ", stream.IsLoading(), compStreamFlag);

	for(cjINT32 i=0; i<cjVtxComp_MAX; i++)
	{
		cjSLOG("[%d] vertexCompStream (%d/%d) ", stream.IsLoading(), i, cjVtxComp_MAX);

		if(stream.IsSaving())
		{
			cjVtxCompStream* vs = m_compStream[i];

			if(vs)
				*vs >> stream;
		}
		else if(stream.IsLoading())
		{
			if(compStreamFlag & (1 << i))
			{
				cjAssert(!m_compStream[i]);
				m_compStream[i] = new cjVtxCompStream;
				*m_compStream[i] >> stream;
			}
		}
	}	

	cjSLOG("[%d] end ", stream.IsLoading());

	return stream;
}

cjFStaticVertexStream::cjFStaticVertexStream()
{	
}


cjINT32 cjFStaticVertexStream::GetByteSiz() const
{
	return m_vtxDecl.GetStride() * m_numVertex;
}

cjVtxCompStream* cjFStaticVertexStream::CreateVtxCompStream(cjVertexCompType compType,
	cjVertexCompSizType compSizType, cjBool updateVtxDecl)
{
	if(m_compStream[compType])
		delete m_compStream[compType];

	cjVtxCompStream* stream = new cjVtxCompStream(); 
	m_compStream[compType] = stream;

	stream->SetCompInfo(compType, compSizType);
	stream->SetVertexCount(GetVertexCount());

	if(updateVtxDecl)
	{
		UpdateVertexDecl();
		RemoveStreamDirty(ESD_NEED_UPDATE_VERTEX_DECL);
	}

	m_StreamDirtyFlag |= ESD_NEED_UPDATE_VERTEX_DECL;

	return stream;
}

void cjFStaticVertexStream::SetVertexCount(cjINT32 count)
{
	if(GetVertexCount() == count)
		return;

	for(cjINT32 i=0; i<m_vtxDecl.GetNumComp(); i++)
		m_compStream[m_vtxDecl.GetComp(i)->m_type]->SetVertexCount(count);
}

void cjFStaticVertexStream::SetAttrib(cjUINT32 attrib)
{
	m_attr = attrib;
	m_StreamDirtyFlag |= ESD_NEED_CACHE_REV;
}

cjFStaticVertexStream::cjFStaticVertexStream(cjINT32 vtxCount, const cjFVertexDecl* decl,
    const void* rawVertexSrc,  cjUINT32 streamAttr)
{
	Init(cjFalse, vtxCount, decl, rawVertexSrc, streamAttr);
}

cjFStaticVertexStream::~cjFStaticVertexStream()
{
	Clean();
}

void cjFStaticVertexStream::Init(cjBool isSerialize, cjINT32 vtxCount, const cjFVertexDecl* decl,
	const void* rawVertexSrc, cjUINT32 streamAttr)
{
	if(decl)
		m_vtxDecl = *decl;

	m_numVertex = vtxCount;
	cjMemZero(m_compStream, sizeof(m_compStream));
	m_attr = streamAttr;
	m_StreamDirtyFlag = 0;

	cjINT32 numComp = m_vtxDecl.GetNumComp();
	cjINT32 vtxStride = m_vtxDecl.GetStride();

	for(cjINT32 i=0; i<numComp; i++)
	{
		const cjVertexComponent* curComp = m_vtxDecl.GetComp(i);
		cjVtxCompStream* curCompStrem = CreateVtxCompStream((cjVertexCompType)curComp->m_type, 
			(cjVertexCompSizType)curComp->m_sizType);

		if(rawVertexSrc)
		{
			cjBYTE* src = (cjBYTE*)rawVertexSrc;
			src += curComp->m_compOffset;

			cjINT32 curCompStride = curComp->m_stride;
			cjBYTE* dst = curCompStrem->Lock();

			for(cjINT32 k=0; k<vtxCount; k++)
			{
				cjMemCpy(dst, src, curCompStride);
				src += vtxStride;
				dst += curCompStride;
			}

			curCompStrem->UnLock();
		}
	}

	// don't rebuild vertex decl
	RemoveStreamDirty(ESD_NEED_UPDATE_VERTEX_DECL);
	RemoveStreamDirty(ESD_NEED_CACHE_REV);
}

void cjFStaticVertexStream::Clean()
{
	for(cjINT32 i=0; i<m_vtxDecl.GetNumComp(); i++)
	{
		const cjVertexComponent* comp = m_vtxDecl.GetComp(i);

		cjAssert(m_compStream[comp->m_type]);
		CJ_SAFE_DELETE(m_compStream[comp->m_type]);
	}

#if defined(CJDEBUG)
	{
		for(cjINT32 i=0; i<cjVtxComp_MAX; i++)
		{
			cjAssert(m_compStream[i] == NULL);
		}
	}
#endif	
}


void cjFStaticVertexStream::ClearStreamDirty()
{
	if(!m_StreamDirtyFlag)
		return;

	if(m_StreamDirtyFlag & ESD_NEED_UPDATE_VERTEX_DECL)
	{
		UpdateVertexDecl();
		m_StreamDirtyFlag |= ESD_NEED_CACHE_REV;
	}

	if(m_StreamDirtyFlag & ESD_NEED_CACHE_REV)
		IncCacheRev();

	m_StreamDirtyFlag = 0;
}

void cjFStaticVertexStream::UpdateVertexDecl()
{
	m_vtxDecl.MakeFromVtxCompStreamAry(m_compStream);
	SetStreamDirty(ESD_NEED_UPDATE_VERTEX_DECL);
}

void cjFStaticVertexStream::CopyCompStreamIntoSingleStream(cjBYTE* dst) const
{
	cjINT32 stride = m_vtxDecl.GetStride();
	for(cjINT32 i=0; i<m_vtxDecl.GetNumComp(); i++)
	{	
		const cjVertexComponent* comp = m_vtxDecl.GetComp(i);
		m_compStream[comp->m_type]->Copy(dst, comp->m_compOffset, stride);
	}
}

