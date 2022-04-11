
#include "cjEnginePCH.h"
#include "cjRenDef.h"


cjResCacheID cjGResMakeCacheIDIndex = 0;


cjINT32 cjGetSizFromVertexCompSizType(cjVertexCompSizType sizType)
{
    switch(sizType)
    {
    case cjVtxCompSiz_FLOAT     : return sizeof(cjFLOAT);
    case cjVtxCompSiz_FLOAT2    : return sizeof(cjFLOAT) * 2;
    case cjVtxCompSiz_FLOAT3    : return sizeof(cjFLOAT) * 3;
    case cjVtxCompSiz_FLOAT4    : return sizeof(cjFLOAT) * 4;
    case cjVtxCompSiz_UINT32    : return sizeof(cjUINT32);

	case cjVtxCompSiz_BYTE		: return sizeof(cjBYTE);
	case cjVtxCompSiz_BYTE2		: return sizeof(cjBYTE) * 2;
	case cjVtxCompSiz_BYTE3		: return sizeof(cjBYTE) * 3;
	case cjVtxCompSiz_BYTE4		: return sizeof(cjBYTE) * 4;
    
	case cjVtxCompSiz_SHORT		: return sizeof(cjSWORD);
	case cjVtxCompSiz_SHORT2	: return sizeof(cjSWORD) * 2;
	case cjVtxCompSiz_SHORT3	: return sizeof(cjSWORD) * 3;
	case cjVtxCompSiz_SHORT4	: return sizeof(cjSWORD) * 4;

    case cjVtxCompSiz_Unknown   : 
	default:
        cjError("Bad Parameter!");
        return 0;
    }
}

cjVertexCompFlag cjConvertToVertexCompFlag(cjVertexCompType type)
{
    switch(type)
    {
    case cjVtxComp_Position     : return cjVtxCompFlag_Position;
    case cjVtxComp_BL4Weight    : return cjVtxCompFlag_BL4Weight;
    case cjVtxComp_BL4Index     : return cjVtxCompFlag_BL4Index;
    case cjVtxComp_Diffuse      : return cjVtxCompFlag_Diffuse;
    case cjVtxComp_Specular     : return cjVtxCompFlag_Specular;
    case cjVtxComp_Normal       : return cjVtxCompFlag_Normal;
    case cjVtxComp_BiNormal     : return cjVtxCompFlag_BiNormal;
    case cjVtxComp_Tangent      : return cjVtxCompFlag_Tangent;
    case cjVtxComp_TexCoord0    : return cjVtxCompFlag_TexCoord0;
    case cjVtxComp_TexCoord1    : return cjVtxCompFlag_TexCoord1;
    case cjVtxComp_TexCoord2    : return cjVtxCompFlag_TexCoord2;
    case cjVtxComp_TexCoord3    : return cjVtxCompFlag_TexCoord3;
    case cjVtxComp_TexCoord4    : return cjVtxCompFlag_TexCoord4;
    case cjVtxComp_TexCoord5    : return cjVtxCompFlag_TexCoord5;
    case cjVtxComp_TexCoord6    : return cjVtxCompFlag_TexCoord6;
    case cjVtxComp_TexCoord7    : return cjVtxCompFlag_TexCoord7;
    
    default:
        cjError("Bad Parameter!");
        return (cjVertexCompFlag)0;
    }
}


//
// cjVertexComponent
//

CJ_IMPL_SERIALIZE_BEGIN(cjVertexComponent)
	cjPODSerializeWithType<cjBYTE>(stream, d.m_streamIndex);
	cjPODSerializeWithType<cjBYTE>(stream, d.m_type);
	cjPODSerializeWithType<cjBYTE>(stream, d.m_sizType);
	d.m_pad0 = 0;
	cjPODSerialize(stream, d.m_stride);
	cjPODSerialize(stream, d.m_compOffset);
CJ_IMPL_SERIALIZE_END(cjVertexComponent)


//
// cjFVertexDecl
//

const cjTFLAG cjFVertexDecl::COMP_FLAG_TEXUVS = \
	cjVtxCompFlag_TexCoord0 | \
	cjVtxCompFlag_TexCoord1 | \
	cjVtxCompFlag_TexCoord2 | \
	cjVtxCompFlag_TexCoord3 | \
	cjVtxCompFlag_TexCoord4 | \
	cjVtxCompFlag_TexCoord5 | \
	cjVtxCompFlag_TexCoord6 | \
	cjVtxCompFlag_TexCoord7;


CJ_IMPL_SERIALIZE_BEGIN(cjFVertexDecl)

	stream << *((cjTFLAG*)&(d.m_hash.m_compFlag));	
	stream << *((cjTFLAG*)&(d.m_hash.m_streamFlag));
	stream << *((cjTFLAG*)&(d.m_hash.m_sizFlag[0]));
	stream << *((cjTFLAG*)&(d.m_hash.m_sizFlag[1]));

	stream << d.m_numComp;
		
	if(stream.IsLoading())
	{
		cjAssert(!d.m_comp);
		if(d.m_numComp)
			d.m_comp = new cjVertexComponent[d.m_numComp];

	}

	cjSLOG("m_numComp=%d", d.m_numComp);

	for(cjINT32 i=0; i<d.m_numComp; i++)
		stream << d.m_comp[i];		

//	cjVertexComponent::SerializeAry(stream, &d.m_comp, d.m_numComp);

	stream << d.m_numTexUV;
	stream << d.m_stride;

	cjSLOG("m_numTexUV=%d , stride=%d", d.m_numTexUV, d.m_stride);
	
CJ_IMPL_SERIALIZE_END(cjFVertexDecl)


cjFVertexDecl::cjFVertexDecl()
{
    m_comp = cjNULL;
    Reset();
}

cjFVertexDecl::~cjFVertexDecl()
{
    Reset();
}

void cjFVertexDecl::Reset()
{
	m_hash.Reset();
    m_numComp = 0;
    
    if(m_comp)
    {
        delete [] m_comp;
        m_comp = cjNULL;
    }

	m_numTexUV = 0;
    
    m_stride = 0;
}

cjBool cjFVertexDecl::MakeFromOtherDecl(const cjFVertexDecl& srcDecl, cjINT32 numComp, ...)
{
    va_list args;
    
    Reset();
	va_start(args, numComp);

    if(numComp == 0)
        return cjFalse;
	
    m_comp = new cjVertexComponent[m_numComp];

    for(cjINT32 i=0; i<numComp; i++)
    {
        cjVertexCompType compType;

        compType = (cjVertexCompType)va_arg(args, cjTFLAG);
        
        cjINT32 foundIndex = -1;
        for(cjINT32 k=0; k<srcDecl.GetNumComp(); k++)
        {
            if(srcDecl.GetComp(k)->m_type == compType)
            {
                foundIndex = k;
                break;
            }
        }
        
        if(foundIndex == -1)
            continue;
        
        const cjVertexComponent* vtxComp = srcDecl.GetComp(foundIndex);
        m_comp[m_numComp] = *vtxComp;

		cjTFLAG compFlag = cjConvertToVertexCompFlag((cjVertexCompType)vtxComp->m_type);

		if(compFlag & COMP_FLAG_TEXUVS)
			m_numTexUV++;

		m_hash.m_compFlag |= compFlag;
		SetCompStream(m_hash.m_streamFlag, m_numComp, vtxComp->m_streamIndex);
		SetCompSizType(m_hash.m_sizFlag, m_numComp, (cjVertexCompSizType)vtxComp->m_sizType);

        m_stride += cjGetSizFromVertexCompSizType((cjVertexCompSizType)vtxComp->m_sizType);
        
        m_numComp++;
    }

    if(m_numComp == 0)
      Reset();
    
	va_end(args);
    
    return m_numComp ? cjTrue : cjFalse;
}

cjBool cjFVertexDecl::MakeDecl(cjINT32 numComp, ...)
{
    va_list args;

    Reset();

	va_start(args, numComp);

    if(numComp == 0)
        return cjFalse;

    m_numComp = numComp;
    m_comp = new cjVertexComponent[m_numComp];
    
    for(cjINT32 i=0; i<m_numComp; i++)
    {
		cjBYTE streamIndex;
        cjVertexCompType compType;
        cjVertexCompSizType compSizType;
    
		streamIndex = (cjBYTE)va_arg(args, cjTFLAG);
        compType = (cjVertexCompType)va_arg(args, cjTFLAG);
        compSizType = (cjVertexCompSizType)va_arg(args, cjTFLAG);
        
		cjVertexComponent* comp = &m_comp[i];

		comp->m_streamIndex	= (cjBYTE)streamIndex;
        comp->m_type		= (cjBYTE)compType;
        comp->m_sizType		= (cjBYTE)compSizType;		
        comp->m_stride		= cjGetSizFromVertexCompSizType(compSizType);
        comp->m_compOffset	= m_stride;

		cjTFLAG compFlag = cjConvertToVertexCompFlag(compType);
		if(compFlag & COMP_FLAG_TEXUVS)
			m_numTexUV++;

        m_hash.m_compFlag |= compFlag;
		SetCompStream(m_hash.m_streamFlag, i, comp->m_streamIndex);
		SetCompSizType(m_hash.m_sizFlag, i, (cjVertexCompSizType)comp->m_sizType);

        m_stride += comp->m_stride;
    }
   
	va_end(args);
    
    return cjTrue;
}

cjBool cjFVertexDecl::MakeFromVtxCompStreamAry(cjVtxCompStream** ary, cjINT32 numAry)
{
	Reset();

	if(numAry == 0)
		return cjFalse;

	cjINT32 numComp = 0;
	for(cjINT32 i=0; i<numAry; i++)
	{
		if(!ary[i]) continue;
		numComp++;
	}

	if(numComp == 0)
		return cjFalse;

    m_numComp = numComp;
    m_comp = new cjVertexComponent[m_numComp];

	cjINT32 insIndex = 0;    
    for(cjINT32 i=0; i<numAry && insIndex < m_numComp; i++)
    {
		cjVtxCompStream* cur = ary[i];
		
		if(!cur)
			continue;

		cjVertexComponent& comp = m_comp[insIndex];

		comp.m_streamIndex	= cur->GetStreamIndex();
        comp.m_type			= (cjBYTE)cur->GetCompType();
        comp.m_sizType		= (cjBYTE)cur->GetCompSizType();
        comp.m_stride		= cjGetSizFromVertexCompSizType(cur->GetCompSizType());
        comp.m_compOffset	= m_stride;
        
		cjTFLAG compFlag = cjConvertToVertexCompFlag(cur->GetCompType());
		if(compFlag & COMP_FLAG_TEXUVS)
			m_numTexUV++;

        m_hash.m_compFlag |= compFlag;
		SetCompStream(m_hash.m_streamFlag, insIndex, comp.m_streamIndex);
		SetCompSizType(m_hash.m_sizFlag, insIndex, (cjVertexCompSizType)comp.m_sizType);

        m_stride += m_comp[insIndex].m_stride;

		insIndex++;
    }

	return cjTrue;
}

cjFVertexDecl::cjFVertexDecl(const cjFVertexDecl& rhs)
{
    *this = rhs;
}

cjFVertexDecl& cjFVertexDecl::operator=(const cjFVertexDecl& rhs)
{
    if(this == &rhs)
        return *this;

    if(m_comp)
    {
        delete [] m_comp;
        m_comp = cjNULL;
    }

	m_hash = rhs.m_hash;
 
    m_numComp = rhs.m_numComp;
    if(m_numComp)
    {
        m_comp = new cjVertexComponent[m_numComp];
        cjMemCpy(m_comp, rhs.m_comp, sizeof(cjVertexComponent) * m_numComp);
    }

	m_numTexUV = rhs.m_numTexUV;
    m_stride = rhs.m_stride;
	
    return *this;
}

const cjVertexComponent* cjFVertexDecl::GetCompByShaderVtxAttrType(cjEShaderVtxAttr shaderVtxAttr) const
{
    //cjopti
    cjVertexCompType compType;
    
    switch(shaderVtxAttr)
    {
    case cjEShaderVtxAttr_Pos:          compType = cjVtxComp_Position;  break;
    case cjEShaderVtxAttr_Normal:       compType = cjVtxComp_Normal;    break;
    case cjEShaderVtxAttr_BlendIndices: compType = cjVtxComp_BL4Index;  break;
    case cjEShaderVtxAttr_BlendWeight:  compType = cjVtxComp_BL4Weight; break;
	case cjEShaderVtxAttr_Color:		compType = cjVtxComp_Diffuse;   break;
    case cjEShaderVtxAttr_BiNormal:     compType = cjVtxComp_BiNormal;  break;
    case cjEShaderVtxAttr_Tangent:      compType = cjVtxComp_Tangent;   break;
    case cjEShaderVtxAttr_TexCoord0:    compType = cjVtxComp_TexCoord0; break;
    case cjEShaderVtxAttr_TexCoord1:    compType = cjVtxComp_TexCoord1; break;
    default:
        return cjNULL;
    }
    
    for(cjINT32 i=0; i<m_numComp; i++)
    {
        if(m_comp[i].m_type == compType)
            return &m_comp[i];
    }
    
    return cjNULL;   
}

cjINT32 cjFVertexDecl::GetUVCompCount() const
{
	cjTFLAG flag = (m_hash.m_compFlag & cjVtxCompFlag_TexCoord);
	flag = flag >> cjVtxCompFlag_TexCoordOnly_Shift;

	cjINT32 uvCount = 0;

	for(cjINT32 i=0; i< 8; i++)
	{
		if((flag & 0x1))
			uvCount++;

		flag = flag >> 1;
	}

	return uvCount;
}

void cjFVertexDecl::SetCompStream(cjTFLAG& streamFlag, cjINT32 compIndex, cjINT32 streamIndex)
{
	cjAssert(streamIndex <= STREAM_FLAG_MASK);

	streamFlag &= ~(STREAM_FLAG_MASK << compIndex);
	streamFlag |= (streamIndex & STREAM_FLAG_MASK) << compIndex;
}

cjINT32 cjFVertexDecl::GetCompStreamIndex(const cjTFLAG& streamFlag, cjINT32 compIndex)
{
	return (cjINT32)((streamFlag & (STREAM_FLAG_MASK << compIndex)) >> compIndex);
}

void cjFVertexDecl::SetCompSizType(cjTFLAG sizFlag[], cjINT32 compIndex, cjVertexCompSizType sizType)
{
	cjAssert(sizType <= cjVtxCompSiz_FLAG_MASK);

	cjTFLAG& dst = sizFlag[compIndex / cjVtxCompSiz_FLAG_MASK_BIT];
	compIndex = compIndex % cjVtxCompSiz_FLAG_MASK_BIT;

	dst &= ~(cjVtxCompSiz_FLAG_MASK << compIndex);
	dst |= (sizType & cjVtxCompSiz_FLAG_MASK) << compIndex;
}

cjVertexCompSizType cjFVertexDecl::GetCompSizType(const cjTFLAG sizFlag[], cjINT32 compIndex)
{
	const cjTFLAG& dst = sizFlag[compIndex / cjVtxCompSiz_FLAG_MASK_BIT];
	compIndex = compIndex % cjVtxCompSiz_FLAG_MASK_BIT;

	return (cjVertexCompSizType)((dst & (cjVtxCompSiz_FLAG_MASK << compIndex)) >> compIndex);
}

