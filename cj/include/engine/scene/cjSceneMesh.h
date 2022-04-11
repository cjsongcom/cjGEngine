
#ifndef _CJSCENEMESH_H_
#define _CJSCENEMESH_H_


#include "cjSceneMeshModifier.h"


class CJENGINE_API cjSceneMesh : public cjSceneNode
{
	CJSER_DECL_CLASS(cjSceneMesh, cjSceneNode, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneMesh)
	CJSER_DECL_SERIALIZE_POST
	
public:
	enum 
	{ 
		MAX_NUM_MODIFIER = 4,
		MAX_STREAM_COUNT = 4,
	};

	const static cjINT32 INVALID_ACTIVE_SHMTL_INDEX = -1;


	cjSceneMesh();

	cjMesh*	GetOwnerMesh() { return m_ownerMesh; }
	void SetOwnerMesh(cjMesh* ownerMesh);
	
	cjFVertexStream** GetVertexStream(cjINT32* streamCount=cjNULL) 
	{ 
		if(streamCount) 
			*streamCount = m_streamCount; 

		return m_streams; 
	}

	void SetVertexStream(cjFVertexStream* stream, cjINT32 streamIndex=0);
	cjUINT32 GetVertexCount(cjINT32 streamIndex=0) { return m_streams[streamIndex] ? m_streams[streamIndex]->GetVertexCount() : 0; }

	cjFIndexBuffer* GetIndexBuffer(cjINT32 idxBufIndex=0) { return m_idxBufs[0]; }
	void SetIndexBuffer(cjFIndexBuffer* idxBuf, cjINT32 idxBufIndex=0);

	cjPrimitiveType GetPrimitiveType() { return m_primitiveType; }
	void SetPrimitiveType(cjPrimitiveType type) { m_primitiveType = type; }

	cjINT32 GetPrimitiveCount(cjINT32 idxBufIndex=0) { return m_idxBufs[idxBufIndex]->GetNumPrimitive(); }

	cjBool QueryVertexComp(cjVertexCompType vtxComp) const;
	cjINT32 QueryVertexUVSetCount() const;
	cjINT32 QueryVtxSkinningType() const; // 0:non-skin, 1:skinned, 2:instanced

	// Modifier
	cjBool AddModifier(cjSceneMeshMod* mod);

	cjSceneSkinMeshMod* GetSkinningModifier() 
	{
		cjSceneMeshMod* mod = m_modifier[0];
		return (cjSceneSkinMeshMod*)mod;
	}
		
	//
	// shader & shaderMaterial
	//

//	cjShader* GetShader() const;	
	cjShader* CacheShader(); // for active shader mtl

	void AddShaderMtl(cjShaderMtl* shaderMtl, cjBool active=cjFalse);

	//	// if not exist shaderMtl and return false
	//	// null param for disable active shaderMaterial
	cjUINT32 FindAndActiveShaderMtl(const cjShaderMtl* shaderMtl=cjNULL, cjBool findOnly=cjFalse);
	cjMaterial* GetActiveMtl();

	cjBool IsAlwaysUpdateShaderMtl() const { return m_alwaysUpdateShMtl; }
		

protected:
	cjObjRef<cjMesh>					m_ownerMesh;	// has vertex stream of this

	// 0: pos, uv, normal stream
	// 1: baked light color
	cjFVertexStream*					m_streams[MAX_STREAM_COUNT];	// ref, in owner mesh
	cjINT32								m_streamCount;

	cjFIndexBuffer*						m_idxBufs[MAX_STREAM_COUNT];		// ref, in owner mesh
	cjPrimitiveType						m_primitiveType;

	// modifier
	cjSceneMeshModRPtr					m_modifier[MAX_NUM_MODIFIER];
	cjBYTE								m_numModifier;

	// serialize: only save material names
	cjTUniqueAry<cjShaderMtlInst>		m_shMtlInsts;   

	cjINT32								m_activeShMtl;
	cjBool								m_alwaysUpdateShMtl;
		
};

cjRefPtrDefine(cjSceneMesh)
// typedef cjTAry<cjSceneMesh*> cjSceneMeshPAry;
typedef cjTAry<cjSceneMeshRPtr> cjSceneMeshRPtrAry;


#endif
