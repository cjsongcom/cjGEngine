
#include "cjEnginePCH.h"
#include "cjSceneMesh.h"


CJSER_IMPL_CLASS(cjSceneMesh, cjSceneNode)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneMesh)

	if(ser.IsQuerySerObjAndStr())
	{
		ser.AddCJObj(m_ownerMesh);

		for(cjAryOff i=0; i<m_shMtlInsts.Count(); i++)
		{
			cjShaderMtlInst& cur = m_shMtlInsts(i);				
			ser.AddCJObj(cur.GetShaderMtlObjRef());
		}

		for(cjINT32 i=0; i<m_numModifier; i++)
			(*m_modifier[i]) >> ser;
	}
	else if(ser.IsSaving() || ser.IsLoading())
	{
		cjINT32 meshStreamCount;
		cjBYTE serNumModifier;
		
		ser.SerializeCJObjRef(m_ownerMesh);

		if(ser.IsSaving())
		{
			meshStreamCount = m_streamCount;

			if(!m_ownerMesh)
			{
				cjAssert(m_streamCount == 0);
			}

			serNumModifier = m_numModifier;
		}

		ser << cjFStreamCmprI32(meshStreamCount);

		if(ser.IsLoading())
		{
			if(meshStreamCount)
			{
				cjAssert(m_ownerMesh);
			}

			cjINT32 streamIndex = 0;
			cjINT32 ownerMeshStreamIndex = 0;
			cjINT32 ownerMeshIdxBufIndex = 0;

			cjMemSet(m_streams, 0, sizeof(m_streams));
			cjMemSet(m_idxBufs, 0, sizeof(m_idxBufs));

			cjFStreamEnum(ser, m_primitiveType);
			
			for(cjINT32 i=0; i<meshStreamCount; i++)
			{
				ser << cjFStreamCmprI32(streamIndex);

				ser << cjFStreamCmprI32(ownerMeshStreamIndex);
				ser << cjFStreamCmprI32(ownerMeshIdxBufIndex);

				m_streams[streamIndex] = m_ownerMesh->GetVertexStream(ownerMeshStreamIndex);
				m_idxBufs[streamIndex] = m_ownerMesh->GetIndexBuffer(ownerMeshIdxBufIndex);

				if(m_streams[streamIndex])
					m_streamCount++;
			}
		}
		else if(ser.IsSaving())
		{
			cjINT32 streamIndex = 0;
			cjINT32 ownerMeshStreamIndex = 0;
			cjINT32 ownerMeshIdxBufIndex = 0;

			cjFVertexStream** curStream = &m_streams[0];
			cjFIndexBuffer** curIdxBuf = &m_idxBufs[0];

			if(meshStreamCount)
			{
				cjAssert(m_ownerMesh);
			}

			cjFStreamEnum(ser, m_primitiveType);

			for(cjINT32 i=0; i<meshStreamCount; i++)
			{
				while(!(*curStream))
				{
					curStream++;
					curIdxBuf++;

					streamIndex++;
				}
				
				ser << cjFStreamCmprI32(streamIndex);
				
				ownerMeshStreamIndex = m_ownerMesh->GetVertexStreamIndex(*curStream);
				ownerMeshIdxBufIndex = m_ownerMesh->GetIndexBufIndex(*curIdxBuf);

				ser << cjFStreamCmprI32(ownerMeshStreamIndex);				
				ser << cjFStreamCmprI32(ownerMeshIdxBufIndex);
			}
		}

		//
		// Modifier / Material
		//
		
		ser << serNumModifier;

		for(cjINT32 i=0; i<serNumModifier; i++)
			ser.SerializeSerObjPtr(&m_modifier[i]);

		{
			cjINT32 shMtlCount = m_shMtlInsts.Count();
			ser << cjFStreamCmprI32(shMtlCount);

			for(cjAryOff i=0; i<m_shMtlInsts.Count(); i++)
			{
				cjShaderMtlInst& cur = m_shMtlInsts(i);				
				ser.SerializeCJObjRef(cur.GetShaderMtlObjRef());
			}
		}
		
		cjFStreamCmprI32(m_activeShMtl);
		ser << m_alwaysUpdateShMtl;

		if(ser.IsLoading())
		{
			m_numModifier = serNumModifier;
		}

	}

CJSER_IMPL_SERIALIZE_END(cjSceneMesh)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjSceneMesh)

	if(ser.IsLoading())
	{
		for(cjINT32 i=0; i<m_numModifier; i++)
		{
			ser.PostSerializeSerObjPtr(m_modifier[i]);
			m_modifier[i]->Attach(this);
		}
	}

CJSER_IMPL_SERIALIZE_POST_END(cjSceneMesh)

CJSER_IMPL_CLONE(cjSceneMesh)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneMesh)

	d->m_ownerMesh = m_ownerMesh;

	cjMemCpy(d->m_streams, m_streams, sizeof(cjFVertexStream*) * MAX_STREAM_COUNT);
	d->m_streamCount = m_streamCount;
	cjMemCpy(d->m_idxBufs, m_idxBufs, sizeof(cjFIndexBuffer*) * MAX_STREAM_COUNT);

	d->m_primitiveType = m_primitiveType;

	for(cjAryOff i=0; i<MAX_NUM_MODIFIER; i++)
		d->m_modifier[i] = m_modifier[i];
	d->m_numModifier = m_numModifier;

	m_shMtlInsts.RemoveAll(d->m_shMtlInsts.Count(), cjTrue);
	for(cjAryOff i=0; i<m_shMtlInsts.Count(); i++)
		m_shMtlInsts(i) = d->m_shMtlInsts(i);
	
	m_activeShMtl = d->m_activeShMtl;
	m_alwaysUpdateShMtl = d->m_alwaysUpdateShMtl;
		
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneMesh)
	

cjSceneMesh::cjSceneMesh()
{
	m_ownerMesh = 0;

	cjMemZero(m_streams, sizeof(m_streams));
	cjMemZero(m_idxBufs, sizeof(m_idxBufs));

	m_primitiveType = cjPT_TriangleList;

	m_streamCount = 0;
	m_numModifier = 0;
	m_activeShMtl = INVALID_ACTIVE_SHMTL_INDEX;
	m_alwaysUpdateShMtl = cjFalse;
}

cjSceneMesh::~cjSceneMesh()
{
	//for(cjINT32 i=0; i<MAX_NUM_MODIFIER; i++)
	//{
	//	CJ_REF_SAFE_RELEASE(m_modifier[m_numModifier]);
	//}
}

void cjSceneMesh::SetOwnerMesh(cjMesh* ownerMesh)
{
	m_ownerMesh = ownerMesh;
	cjMemZero(m_streams, sizeof(m_streams));
	cjMemZero(m_idxBufs, sizeof(m_idxBufs));

	m_streamCount = 0;
}

void cjSceneMesh::SetVertexStream(cjFVertexStream* stream, cjINT32 streamIndex)
{
	cjAssert(m_ownerMesh);
	cjAssert(m_ownerMesh->GetVertexStreamIndex(stream) != -1);

	if(streamIndex < MAX_STREAM_COUNT)
	{
		m_streams[streamIndex] = stream;
		m_streamCount = 0;

		for(cjINT32 i=0; i<MAX_STREAM_COUNT; i++)
		{
			if(m_streams[i])
				m_streamCount++;
		}
	}
}

void cjSceneMesh::SetIndexBuffer(cjFIndexBuffer* idxBuf, cjINT32 idxBufIndex)
{
	cjAssert(m_ownerMesh);
	cjAssert(m_ownerMesh->GetIndexBufIndex(idxBuf) != -1);
	m_idxBufs[idxBufIndex] = idxBuf;
}

cjBool cjSceneMesh::QueryVertexComp(cjVertexCompType vtxComp) const
{
	const cjFVertexDecl* vtxDecl = m_streams[0]->GetVertexDecl();

	if(!vtxDecl)
		return cjFalse;

	return vtxDecl->HasComp(vtxComp);
}

cjINT32 cjSceneMesh::QueryVertexUVSetCount() const
{
	const cjFVertexDecl* vtxDecl = m_streams[0]->GetVertexDecl();

	if(!vtxDecl)
		return 0;
	
	return vtxDecl->GetUVCompCount();
}

cjINT32 cjSceneMesh::QueryVtxSkinningType() const
{
	const cjFVertexDecl* vtxDecl = m_streams[0]->GetVertexDecl();

	if(!vtxDecl)
		return 0;
	
	cjBool hasBlendInfo = vtxDecl->HasComp(cjVtxComp_BL4Index) && vtxDecl->HasComp(cjVtxComp_BL4Weight);

	if(hasBlendInfo)
		return 1;

	return 0;
}

cjBool cjSceneMesh::AddModifier(cjSceneMeshMod* mod)
{
	if(!mod)
		return cjFalse;

	if((m_numModifier+1) >= MAX_NUM_MODIFIER)
		return cjFalse;

	m_modifier[m_numModifier] = mod;

//	CJ_REF_CHANGE_TO(m_modifier[m_numModifier], mod);
//	m_modifier[m_numModifier] = mod;
	m_numModifier++;

//	mod->Attach(this);

	return cjTrue;
}

//
// shaderMaterial
//

//cjShader* cjSceneMesh::GetShader() const
//{
//    if (m_activeShMtl >= m_shMtlInsts.Count())
//        return cjNULL;
//
//	const cjShaderMtlInst& shMtlInst = m_shMtlInsts(m_activeShMtl);
//
//    return shMtlInst.EvaluateCachedShader(this, &GetScenePropList());
//}

//cjShader* cjSceneMesh::CacheShader()
//{
//	if(m_activeShMtl >= m_shMtlInsts.Count())
//		return cjNULL;
//
//	cjShaderMtlInst& shMtlInst = m_shMtlInsts(m_activeShMtl);
//
//	return shMtlInst.CacheShader(this, &GetScenePropList());
//}
//
void cjSceneMesh::AddShaderMtl(cjShaderMtl* shaderMtl, cjBool active)
{
	if(!shaderMtl)
		return;

	cjAryOff shaderMtlIndex = FindAndActiveShaderMtl(shaderMtl, cjTrue); // true for find only

	if(shaderMtlIndex == cjShaderMtl::NO_SHMTL_INDEX) 
	{
		cjShaderMtlInst newMtlInst(shaderMtl);
		newMtlInst.SetMtlDirtyState(cjShaderMtlInst::EMDS_DIRTY);

		shaderMtlIndex = (cjAryOff)m_shMtlInsts.Push(newMtlInst);
	}

	if(active)
		m_activeShMtl = shaderMtlIndex;
}

cjUINT32 cjSceneMesh::FindAndActiveShaderMtl(const cjShaderMtl* shaderMtl, cjBool findOnly)
{
	cjAryOff activeShMtlIndex = cjShaderMtl::NO_SHMTL_INDEX;

	if(!shaderMtl)
	{
		if(!findOnly)
			m_activeShMtl = activeShMtlIndex;

		return m_activeShMtl;
	}
	
	cjAryOff mtlInstCounts = m_shMtlInsts.Count();

	for(cjAryOff i=0; i<mtlInstCounts; i++) 
	{
		const cjShaderMtlInst& shMtlInst = m_shMtlInsts(i);

		if(shMtlInst.GetShaderMtl() == shaderMtl)
		{
			activeShMtlIndex = i;
			break;
		}
	}

	if(!findOnly)
		m_activeShMtl = activeShMtlIndex;

	return m_activeShMtl;
}

cjMaterial* cjSceneMesh::GetActiveMtl()
{
	if(m_activeShMtl == INVALID_ACTIVE_SHMTL_INDEX)
		return cjNULL;

	if(m_activeShMtl >= m_shMtlInsts.Count())
		return cjNULL;

	const cjShaderMtlInst& shMtlInst = m_shMtlInsts(m_activeShMtl);

	return shMtlInst.GetShaderMtl();
}
