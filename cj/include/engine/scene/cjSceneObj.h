
#ifndef _CJSCENEOBJ_H_
#define _CJSCENEOBJ_H_

//
//class cjSceneProp;
//class cjSceneRenProp;
//
//
//class CJENGINE_API cjSceneNode : public cjNode
//{
//	CJSER_DECL_CLASS(cjSceneNode, cjNode, 0)
//
//public:
//	virtual void Update(cjEngineTime_t dt);
//
//public:
//	cjSceneNode();
//	virtual ~cjSceneNode();
//
//	void UpdateProperties(); // scene properties
//	void UpdateEffects();
//	virtual void UpdateBound();
//	virtual void UpdateWorldData();
//	virtual void UpdateWorldBound();
//		
//		// local transform
//	void SetTranslate(const cjVec3& pos) { m_local.m_pos = pos; }
//	void SetTranslate(cjFLOAT x, cjFLOAT y, cjFLOAT z) { m_local.m_pos = cjVec3(x,y,z); }
//	const cjVec3& GetTranslate() const { return m_local.m_pos; }
//
//	const cjMatrix3& GetRotate() const { return m_local.m_rotate; }
//	void GetRotate(cjQuat& rotQuat) const 
//		{ rotQuat.MakeFromRotation(m_local.m_rotate); }
//	void GetRotate(cjFLOAT& angle, cjFLOAT& x, cjFLOAT& y, cjFLOAT& z) const
//		{ m_local.m_rotate.ToAngleAndAxis(angle, x, y, z); }
//
//	void SetRotate(const cjMatrix3& rot) { m_local.m_rotate = rot; }
//	void SetRotate(cjFLOAT angle, cjFLOAT x, cjFLOAT y, cjFLOAT z) 
//		{ m_local.m_rotate.MakeRotation(angle, x, y, z); }
//	void SetRotate(const cjQuat& rotQuat) { rotQuat.ToRotateMatrix(m_local.m_rotate); }
//	void SetScale(cjFLOAT scale) { m_local.m_scale = scale; }
//	const cjTransform& GetLocalTM() const { return m_local; }
//	void SetLocalTM(const cjTransform& localTM) { m_local = localTM; }
//
//		// world
//	const cjTransform& GetWorldTM() const { return m_world; }
//	const cjFBound& GetWorldBound() const { return m_worldBound; }
//
//		// collision info
//
//		// intended only for overwriting world TM from physic lib result
//	void SetWorldTranslate(const cjVec3& pos) { m_world.m_pos = pos; }
//	void SetWorldRotate(const cjMatrix3& rot) { m_world.m_rotate = rot; }
//	void SetWorldScale(cjFLOAT scale) { m_world.m_scale = scale; }
//	void SetWorldBound(const cjFBound& bound) { m_worldBound = bound; }
//
//	void AttachSceneProp(cjSceneProp* sceneProp);
//	void DetachSceneProp(cjSceneProp* sceneProp);
//	cjSceneProp* FindSceneProp(const cjRTTI* rtti);
//
//protected:
//	cjTransform							m_local;
//	cjTransform							m_world;
//	cjFBound							m_worldBound;
//
//	cjSceneProp*						m_renPropList;
//};
//
//
//cjRefPtrDefine(cjSceneNode);


#endif

