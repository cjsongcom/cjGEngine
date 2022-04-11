
#ifndef _CJSCENEMESHMODIFIER_H_
#define _CJSCENEMESHMODIFIER_H_


class cjSceneMesh;
class cjSceneNode;


//
// cjSceneMeshMod
//

class CJENGINE_API cjSceneMeshMod : public cjRefObj
{
	CJSER_DECL_CLASS_ABSTRACT(cjSceneMeshMod, cjRefObj, 0, CJENGINE_API)		
	CJSER_DECL_CLONE_ABSTRACT(cjSceneMeshMod)

public:
	virtual cjBool Attach(cjSceneMesh* sceneMesh) = 0;
	virtual cjBool Detach(cjSceneMesh* sceneMesh) = 0;
};

inline cjSceneMeshMod::cjSceneMeshMod() {}
inline cjSceneMeshMod::~cjSceneMeshMod() {}

cjRefPtrDefine(cjSceneMeshMod)


//
// cjSceneSkinMeshMod
//

class CJENGINE_API cjSceneSkinMeshMod : public cjSceneMeshMod
{
	CJSER_DECL_CLASS(cjSceneSkinMeshMod, cjSceneMeshMod, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_NO_CLONEABLE

public:
	virtual cjBool Attach(cjSceneMesh* sceneMesh);
	virtual cjBool Detach(cjSceneMesh* sceneMesh);

public:
	cjSceneSkinMeshMod();
	cjSceneSkinMeshMod(cjUINT32 boneCount);

	cjFLOAT Update(cjEngineTime_t t, cjBool force=cjFalse);

	cjUINT32 GetBoneCount() const;
//	cjBound* GetBoneBounds() const;
	cjSceneNode** GetBoneAry() { return m_bones; }
	cjTransform* GetSkinToBoneTMAry() { return m_skinToBoneTMAry; }

	const cjMatrix* GetBoneMatrix() { return m_boneMatrices; }
	const cjVec4* GetBoneMatrixAsVec4Ary() { return (const cjVec4*)m_boneMatrices; }
	cjINT32 GetBoneMatrixCount() { return m_numBones; }

	//cjINT32 GetGPUSkinningMatrixCount();
	//cjVec4* GetGPUSkinningMatrixAry();

	cjSceneNode* GetTopNodeForRootBone() { return m_topNodeForRootBone; }
	void SetTopNodeForRootBone(cjSceneNode* topNodeForRootBone)
		{ m_topNodeForRootBone = topNodeForRootBone; }

	const cjTransform& GetInvSkinTM() { return m_invSkinTM; }
	void SetInvSkinTM(const cjTransform& tm) { m_invSkinTM = tm; }

	//void DeformVertex(cjFVertexStream* outVtxStream, cjFVertexStream* srcVtxStream);
	cjSceneNode* GetAttachNode() { return m_attachNode; }

private:
		// created from exporter, Convert
	cjINT32									m_numBones;
	cjSceneNode**							m_bones;				// pary
	cjTransform*							m_skinToBoneTMAry;		// fary

	cjSceneNode*							m_topNodeForRootBone;
	cjTransform								m_invSkinTM; // m_rot is transposed
	
	cjEngineTime_t							m_lastUpdateTime;

	// attach/detach(), for gpu render, created from m_bones	
	cjMatrix*								m_boneMatrices;
	cjTransform**							m_boneWorldTMAry;	// created from m_bones

	// mostly scene mesh
	cjSceneNode*							m_attachNode; // mostly scene mesh
};

cjRefPtrDefine(cjSceneSkinMeshMod)


#endif

