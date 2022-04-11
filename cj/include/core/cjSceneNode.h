
#ifndef _CJSCENENODE_H_
#define _CJSCENENODE_H_


class cjSceneProp;
class cjSceneRenProp;


class CJCORE_API cjSceneNode : public cjNode
{
	CJSER_DECL_CLASS(cjSceneNode, cjNode,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjSceneNode)
	CJSER_DECL_CLONE_POST

public:
	static void SerializeAry(cjSerializer& ser, cjSceneNode*** d, cjINT32* count);
	static cjSerializer& PostSerializeAry(cjSerializer& ser, cjSceneNode** d);
	static void _CloneScenePropList(cjSceneNode* dst, cjScenePropRPtrList* srcList, cjSerObjCloner* cloner);
	static void _PostCloneScenePropList(cjScenePropRPtrList* srcList, cjSerObjCloner* cloner);

public:
		// cjNode
	virtual cjSceneNode* GetSceneNode() { return this; }	
	virtual cjBool HasScenePropAniCntr();
	virtual void UpdateScenePropAniCntr(cjEngineTime_t etime);

	virtual void UpdateUpward();
	virtual void UpdateDownward(cjNodeUpdateProcess& updateProcess);
    virtual void UpdateBySelectFlagDownward(cjNodeUpdateProcess& updateProcess);
    virtual void UpdateRigidDownward(cjNodeUpdateProcess& updateProcess);
	
public:
	cjSceneNode();

	void UpdateProperties(); // scene properties
	void UpdateEffects();

	cjSceneNode* GetFirstChildSceneNode();
	cjSceneNode* GetSibilSceneNode();
	cjSceneNode* GetParentSceneNode();

	virtual void UpdateBound();
	virtual void UpdateWorldData();
	virtual void UpdateWorldBound();
		
		// local transfor
	void SetScale(const cjVec3& scale) { m_local.m_scale = scale; }
	void SetScale(cjFLOAT x, cjFLOAT y, cjFLOAT z) { m_local.m_scale = cjVec3(x,y,z); }
	const cjVec3& GetScale() const { return m_local.m_scale; }
	
	const cjMatrix3& GetRotate() const { return m_local.m_rotate; }
	void GetRotate(cjQuat& rotQuat) const 
		{ rotQuat.MakeFromRotation(m_local.m_rotate); }
	void GetRotate(cjFLOAT& angle, cjFLOAT& x, cjFLOAT& y, cjFLOAT& z) const
		{ m_local.m_rotate.ToAngleAndAxis(angle, x, y, z); }

	void SetRotate(const cjMatrix3& rot) { m_local.m_rotate = rot; }
	void SetRotate(cjFLOAT angle, cjFLOAT x, cjFLOAT y, cjFLOAT z) { m_local.m_rotate.MakeRotationFromAxis(angle, x, y, z); }
	void SetRotate(const cjQuat& rotQuat) { rotQuat.ToRotateMatrix(m_local.m_rotate); }

	void SetPos(const cjVec3& pos) { m_local.m_pos = pos; }
	void SetPos(cjFLOAT x, cjFLOAT y, cjFLOAT z) { m_local.m_pos = cjVec3(x,y,z); }
	const cjVec3& GetPos() const { return m_local.m_pos; }

	const cjTransform& GetLocalTM() const { return m_local; }
	void SetLocalTM(const cjTransform& localTM) { m_local = localTM; }

		// world
	const cjVec3& GetWorldScale() const { return m_world.m_scale; }
	const cjMatrix3& GetWorldRotate() const { return m_world.m_rotate; }
	const cjVec3& GetWorldPos() const { return m_world.m_pos; }

	void SetWorldScale(const cjVec3& scale) { m_world.m_scale = scale; }
	void SetWorldRotate(const cjMatrix3& rotate) { m_world.m_rotate = rotate; }
	void SetWorldPos(const cjVec3& pos) { m_world.m_pos = pos; }

	const cjTransform& GetWorldTM() const { return m_world; }
	const cjFBound& GetWorldBound() const { return m_worldBound; }

		// collision info

		// intended only for overwriting world TM from physic lib result
	void SetWorldBound(const cjFBound& bound) { m_worldBound = bound; }
	cjBool IsWorldBoundZero() { return m_worldBound.GetRadius() != 0.0f ? cjTrue : cjFalse; }

		// scene prop list
	cjScenePropRPtrList& GetScenePropList() { return m_scenePropList; }
	const cjScenePropRPtrList& GetScenePropList() const { return m_scenePropList; }

	void AttachSceneProp(cjSceneProp* sceneProp);
	void DetachSceneProp(cjSceneProp* sceneProp);

	cjSceneProp* FindSceneProp(const cjRTTI* rtti) const;
	cjSceneProp* FindScenePropByRttiName(const cjNameChar* rttiName) const;

		// utility
	static cjNodeUpdateProcess* GetDefaultUpdateProcess() { return &ms_defaultUpdateProcess; }
	
protected:	
	cjTransform								m_local;
	cjTransform								m_world;
	cjFBound								m_worldBound;

	cjScenePropRPtrList						m_scenePropList;

	static cjNodeUpdateProcess				ms_defaultUpdateProcess;
};

cjRefPtrDefine(cjSceneNode);

typedef cjTHash<const cjNameChar*, cjSceneNode*, cjTHashSerRemover<cjSceneNode> > cjName2SceneNodeHash;


//
// cjSceneNodesHash
//

//class CJCORE_API cjSceneNodesHash : public cjRef
//{
//	CJSER_DECL_CLASS(cjSceneNodesHash, cjRef,0 , CJCORE_API)
//
//public:
//    cjSceneNodesHash(cjSceneNode* sceneRoot=cjNULL) 
//		: m_sceneRoot(sceneRoot), m_hash(40)
//	{
//	}
//
//    virtual cjSceneNode* GetNode(const cjNameChar* name)
//	{
//		if(!name)
//			return cjNULL;
//
//		cjSceneNode* sceneNode = cjNULL;
//
//		if(m_hash.Find(name, &sceneNode))
//		{
//			return sceneNode;
//		}
//		else if(m_sceneRoot)
//		{
//			cjNode* node = m_sceneRoot->FindChildByName(name);
//			if(node && node->GetSceneNode())
//			{
//				SetNode(name, node->GetSceneNode());
//				return (cjSceneNode*)node;
//			}
//		}
//
//		return cjNULL;
//	}
//
//    virtual void SetNode(const cjNameChar* name, cjSceneNode* sceneNode)
//	{
//		if(!name)
//			return;
//
//		if(!sceneNode)
//			m_hash.Remove(name);
//		else
//			m_hash.Insert(name, sceneNode);
//	}
//
//    virtual void SetScene(cjSceneNode* scene) { m_sceneRoot = scene; }
//    virtual cjSceneNode* GetScene() { return m_sceneRoot; }
//
//protected:
//	cjSceneNode*							m_sceneRoot;
//	cjName2SceneNodeHash					m_hash;
//};
//
//cjRefPtrDefine(cjSceneNodesHash)


#endif

