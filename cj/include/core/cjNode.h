
#ifndef _CJNODE_H_
#define _CJNODE_H_


//#define CJNODE_DECL_SERIALIZE(CLSNAME, REV)	\
//	friend class cjNodeSerializer; \
//	public: static const cjBYTE SER_REV=REV; \
//	friend cjFStream& operator<<(cjFStream& stream, CLSNAME& n);
//
//#define CJNODE_IMPL_SERIALIZE_BEGIN(CLSNAME) \
//	cjFStream& operator<<(cjFStream& stream, CLSNAME& n) \
//	{ \
//		if(stream.IsObjOnly()) \
//			return stream; \
//		cjBYTE rev = CLSNAME::SER_REV; \
//		if(rev) { stream << rev; }
//
//#define CJNODE_IMPL_SERIALIZE_END(CLSNAME) \
//		return stream; \
//	}
//
//#define CJNODE_DECL_SERIALIZE_INLINE_BEGIN(CLSNAME, REV) \
//	public: static const cjBYTE SER_REV=REV; \
//	friend cjFStream& operator<<(cjFStream& stream, CLSNAME& n) \
//	{ \
//		if(stream.IsObjOnly()) \
//			return stream; \
//        cjBYTE rev = CLSNAME::SER_REV; \
//        if(rev) { stream << rev; }
//
//#define CJNODE_DECL_SERIALIZE_INLINE_END(CLSNAME) \
//		return stream; \
//	}


/*
		NodeB (parent of NodeA)
		  |
          |
		NodeA (m_parent=NodeB)
		 -> childNodeA[m_child, GetHeadChild()] <====== child of NodeA
		 -> childNodeB[m_child(NodeA)->GetNextChild] <= sibling child , m_parent = NodeA
		 -> childNodeC[childNodeB->GetNextChild] <= sibling child, m_parent = NodeA

*/


/*

	node<root>
	 |
	 |- node<row00>
	 |
	 |- node<row01>
	 |   |
	 |   |- node<row01_row10>
	 |   |- node<row01_row11>
	 |   |- node<row01_row12>
	 |
	 |- node<row02>
	 

	 node<root>
	   firstChild = node<row00>
	   sibling   = 0

	 node<row00>
	   parent = node<root>
	   firstChild = 0
	   sibling   = node<row01>

	 node<row01>
	   parent = node<root>
	   firstChild = node<row01_row10>
	   sibling    = node<row02>

	 node<row02>
	   parent = node<root>
	   firstChild = 0;
	   sibling = 0

	  node<row01_row10>
	   parent = node<row01>
	   firstChild = 0;
	   sibling = node<row01_row11>

	  node<row01_row11>
	   parent = node<row01>
	   firstChild = 0;
	   sibling = node<row01_row12>

	  node<row01_row12>
	   parent = node<row01>
	   firstChild = 0;
	   sibling = 0

*/

enum cjNodeFlag
{
	CJNODE_FL_DONOT_SAVE							= 0x00000001,
};


//
// cjNodeUpdateProcess
//

class CJCORE_API cjNodeUpdateProcess : cjMem
{
//	CJSER_DECL_CLASS(cjNodeUpdateProcess, cjSerializable,0 , CJCORE_API)

public:
	cjNodeUpdateProcess();
	virtual ~cjNodeUpdateProcess() {}

	virtual void PreUpdate(cjNode* node) {}
	virtual void PostUpdate(cjNode* node) {}

    inline cjEngineTime_t GetTime() const { return m_time; }
    inline void SetTime(cjEngineTime_t _time) { m_time = _time; }

    inline cjBool GetUpdateAniCntrs() const { return m_updateControllers ? cjTrue : cjFalse; }
    inline void SetUpdateAniCntrs(cjBool updateControllers) 
		{ m_updateControllers= updateControllers; }

protected:
	cjEngineTime_t							m_time;
	cjBool									m_updateControllers;
};


//
// cjNode
//

class cjSceneNode;


class CJCORE_API cjNode : public cjRefObj
{
	CJSER_DECL_CLASS(cjNode, cjRefObj,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjNode)

	friend class cjSerializer;
		
public:
	//// [loading] node == 0 => newley created
	//cjINT32 _BuildSerializeInstances(cjNodeSerializer* info, cjNode* node);

	//virtual void OnBuildSerInstances(cjNodeSerializer* info, cjNode* node)
	//{
	//	if(node)
	//	{
	//		cjNode* curChild = m_child;
	//		while(curChild)
	//		{
	//			info->AddNode(curChild);
	//			OnBuildSerInstances(info, curChild);
	//			curChild = curChild->GetSibling();
	//		}
	//	}
	//}

	//virtual void _OnQuerySerializeString(cjNodeSerializeInfo* info) {}	
	//cjBool _OnSerializeLoad
	//cjBool _OnSerializeSave

private:

	CJFLAG_DECL_BEGIN

		MASK_SU_UPDATE_SELECTIVE			= 0x00000001,
		BPOS_SU_UPDATE_SELECTIVE			= 0,

		MASK_SU_PROP_ANICNTR				= 0x00000002,
		BPOS_SU_PROP_ANICNTR				= 1,

			// cjSceneNode Stuff
		MASK_SU_UPDATE_TM					= 0x00000004,
		BPOS_SU_UPDATE_TM					= 2,

		MASK_SU_FORCE_UPDATE_TM				= 0x00000008,
		BPOS_SU_FORCE_UPDATE_TM				= 3,

		MASK_SU_UPDATE_RIGID				= 0x00000010,
		BPOS_SU_UPDATE_RIGID				= 4,

		MASK_SU_RESERVED00					= 0x00000020,
		BPOS_SU_RESERVED00					= 5,

		MASK_SU_RESERVED01					= 0x00000040,
		BPOS_SU_RESERVED01					= 6,

		MASK_SU_RESERVED02					= 0x00000080,
		BPOS_SU_RESERVED02					= 7,

		// NODE_FLAG
		MASK_NF_CULLED						= 0x00000100,
		BPOS_NF_CULLED						= 8,

		MASK_NF_DISABLE_SORT				= 0x00000200,
		BPOS_NF_DISABLE_SORT				= 9,

	CJFLAG_DECL_END

public:
	cjNode();
    cjNode(const cjNameChar* name);

		// parent
	cjNode* GetParent(void) const { return m_parent; }		// just ref
	void SetParent(cjNode* parent) { m_parent = parent; }	// just ref
	void ChangeParent(cjNode* parent);

		// child management	
	cjNode* GetFirChild() { return m_firstChild; } // Get ChildNode , GetFirstChild is same with macro of win32
	cjBool IsEmptyChild() { return m_firstChild ? cjFalse : cjFalse; }
	
	cjINT32 cjSlow GetChildNumSlow();
	/*
		cjNode* child = m_firstChild;
		while(child)
			child = child->GetSibling();
		return cur;
	*/

	cjNode* cjVerySlow GetChildByIdxVerySlow(cjINT32 index);
	
	void AddChild(cjNode* node);
//	void AddChildOnHead(cjNode* node);

	void RemoveChild(cjNode* node); // lookup node in sibling child and remove
	void RemoveAllChild();

	cjBool IsNode() { return cjFalse; }
	cjBool IsLeaf() { return cjFalse; }

		// find
	cjNode* FindChildByName(const cjNameChar* name, cjBool recurFind=cjTrue);
	virtual cjINT32 GetNodeByType(const cjRTTI* /*typeRTTI*/, cjTAry<cjNode*>& /*rstAry*/) { return 0; }
	cjNode* GetSibling() { return m_sibling; } // Get Sibling Node(as same level child of same parent)

	virtual cjSceneNode* GetSceneNode() { return cjNULL; }
	virtual cjBool HasScenePropAniCntr() { return cjFalse; }
	
		//
		// Update
		//
	
	void Update(cjEngineTime_t etime, cjBool updateAniCntr=cjTrue);			
	void UpdateBySelectFlag(cjEngineTime_t etime); // update only stuff which is selected by SetNGetSeletiveUpdateFlags()

	virtual void UpdateScenePropAniCntr(cjEngineTime_t /*etime*/) {}

		//
		// Selective Update Flag functions
		//
			// UpdateRigidDownward, update boneBound
    cjBool	GetSUFlagUpdateSelective() const { return _GetFlagAsBool(MASK_SU_UPDATE_SELECTIVE, BPOS_SU_UPDATE_SELECTIVE); }
    void	SetSUFlagUpdateSelective(cjBool v) { _SetFlagAsBool(MASK_SU_UPDATE_SELECTIVE, BPOS_SU_UPDATE_SELECTIVE, v); }

			// UpdateAniCntr
    cjBool	GetSUFlagPropAniCntr() const { return _GetFlagAsBool(MASK_SU_PROP_ANICNTR, BPOS_SU_PROP_ANICNTR); }
    void	SetSUFlagPropAniCntr(cjBool v) { _SetFlagAsBool(MASK_SU_PROP_ANICNTR, BPOS_SU_PROP_ANICNTR, v); }

			// Update WorldData, WorldBound
    cjBool	GetSUFlagUpdateTM() const { return _GetFlagAsBool(MASK_SU_UPDATE_TM, BPOS_SU_UPDATE_TM); }
    void	SetSUFlagUpdateTM(cjBool v) { _SetFlagAsBool(MASK_SU_UPDATE_TM, BPOS_SU_UPDATE_TM, v); }

    cjBool	GetSUFlagForceUpdateTM() const { return _GetFlagAsBool(MASK_SU_FORCE_UPDATE_TM, BPOS_SU_FORCE_UPDATE_TM); }
    void	SetSUFlagForceUpdateTM(cjBool v) { _SetFlagAsBool(MASK_SU_FORCE_UPDATE_TM, BPOS_SU_FORCE_UPDATE_TM, v); }

    cjBool	GetSUFlagUpdateRigid() const { return _GetFlagAsBool(MASK_SU_UPDATE_RIGID, BPOS_SU_UPDATE_RIGID); }
    void	SetSUFlagUpdateRigid(cjBool v) { _SetFlagAsBool(MASK_SU_UPDATE_RIGID, BPOS_SU_UPDATE_RIGID, v); }
	
    virtual void SetNGetSeletiveUpdateFlags(cjBool& sgSUFlagUpdateSelective, cjBool& sgSUFlagUpdateRigid, 
		cjBool setSUFlagUpateTM);

	
		//
		// Internal use only update functions - do not call directly
		//
			// update sceneProp(updateProperties=cjTrue) and aniCntr
	void UpdateAniCntrs(cjEngineTime_t ti, cjBool updateProperties=cjTrue);

	void Update(cjNodeUpdateProcess& updateProcess);
	void UpdateBySelectFlag(cjNodeUpdateProcess& updateProcess);

	virtual void UpdateUpward();
	virtual void UpdateDownward(cjNodeUpdateProcess& updateProcess);
	virtual void UpdateBySelectFlagDownward(cjNodeUpdateProcess& updateProcess);
	virtual void UpdateRigidDownward(cjNodeUpdateProcess& updateProcess);

	void DoUpdateSelective(cjNodeUpdateProcess& updateProcess) 
	{
		if(GetSUFlagUpdateSelective())
		{
			if(GetSUFlagUpdateRigid())
			{
				UpdateRigidDownward(updateProcess);
			}
			else 
			{
				// update worldData, recursive do selective update
				UpdateBySelectFlagDownward(updateProcess); 
			}
		}
	}
	
private:
	void AddChildOnTail(cjNode* node);
	void SetSibling(cjNode* sibling);

	void CommonValueInit();

	void OnLinkNode();
	
protected:
	cjFLOAT							m_lifeTime;			// -1.0f = none;
	cjNode*							m_parent;

		// OnLinkNode at loading
	cjNode*							m_firstChild;
	cjNode*							m_sibling;

	// managing child node by sibling
	// benefit
	// 1. node in linked list can be disposable without rearrangement of all other childs
	// 2. fast insert/delete
	// weak
	// 1. count number of child
};


cjRefPtrDefine(cjNode);


//
// cjNode Template utility class
//

template<class T>
bool cjNodeIsSameOf(const cjNode* node)
{
	return T::cjrIsSameOf(T::_ms_cjnRTTI, node);
}

template<class T>
bool cjNodeIsKindOf(const cjNode* node)
{
	return T::cjrIsKindOf(T::_ms_cjnRTTI, node);
}

template<class T>
cjNode* cjNodeCast(const cjNode* node)
{
	return T::cjrCast(T::_ms_cjnRTTI, node);
}


template<class T>
T* cjNodeFind(cjNode* cur)
{
	if(!cur)
		return 0;

	if(cur->cjrRTTI() == T::cjrStaticRTTI())
		return (T*)cur;

	cjNode* child = cur->GetFirChild();

	if(!child)
		return 0;

	T* r = 0;
	r = cjNodeFind<T>(child);

	if(r)
		return r;	

	cjNode* sibilChild = child->GetSibling();
	for(;sibilChild; sibilChild = sibilChild->GetSibling())
	{
		r = cjNodeFind<T>(sibilChild);

		if(r) 
			return r;
	}

	return 0;
}


template<class T>
void cjNodeFindAll(cjNode* cur, cjTAry< cjTRefPtr<T> >& collected)
{
	if(!cur)
		return;

	if(cur->cjrRTTI() == T::cjrStaticRTTI())
	{
		collected.Push((T*)cur);
	}

	cjNode* child = cur->GetFirChild();

	if(!child)
		return;

	cjNodeFindAll<T>(child, collected);

	cjNode* sibilChild = child->GetSibling();

	for(;sibilChild; sibilChild = sibilChild->GetSibling())
		cjNodeFindAll<T>(sibilChild, collected);
}


//
// cjNodeSerializer
//

class CJCORE_API cjNodeSerializer : public cjMem
{
public:
	cjNodeSerializer();

	cjNode* Serialize(cjFStream& stream, cjNode** node);

	cjINT32 AddInstance(cjNode* node);


	cjUINT32									m_rev;

	cjUINT32									m_stringCount;
	cjStreamOffset								m_stringsOffset;	
	cjUINT32									m_instanceCount;
	cjStreamOffset								m_instanceOffset;	


	cjTAry<cjNode*>								m_instances;

	cjTAry<cjString>							m_strings;

};


//
// cjSerializeableObj - cjNode Container
//

//class CJCORE_API cjSerializeableObj : public cjObj
//{
//	CJOBJ_DECL_CLASS(cjNodeObj, cjObj, 0, CJCORE_API)
//	CJOBJ_DECL_SERIALIZE
//		
//public:
//	cjSerializeableObj();
//
//protected:
//	virtual void OnDestroy();
//
//private:
//	cjNodeSerializeInfo*				m_serInfo;
//	cjNode*								m_node;
//};


//
// cjRefObjHash
//

class CJCORE_API cjRefObjNameHash : public cjRef
{
	CJSER_DECL_CLASS(cjRefObjNameHash, cjRef,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjRefObjNameHash)

public:
    cjRefObjNameHash(cjRefObj* obj=cjNULL) 
		: m_root(obj), m_hash(40)
	{
		SetRoot(m_root);
	}

    cjRefObj* FindObjWithSetObjWhenRootIsSetted(const cjNameChar* name)
	{
		if(!name)
			return cjNULL;

		cjRefObj* obj = cjNULL;

		if(m_hash.Find(name, &obj))
		{
			return obj;
		}
		else if(m_root)
		{
			if(!m_rootIsNode)
				return cjNULL;

			cjNode* node = ((cjNode*)m_root)->FindChildByName(name);

			if(node)
			{
				SetObj(name, node);
				return (cjRefObj*)node;
			}
		}

		return cjNULL;
	}

    void SetObj(const cjNameChar* name, cjRefObj* obj)
	{
		if(!name)
			return;		

		if(!obj)
		{
			m_hash.Remove(name);
		}
		else
		{
			m_hash.Insert(name, obj);
		}
	}

    void SetRoot(cjRefObj* obj) 
	{ 
		m_root = obj; 
		m_rootIsNode = cjrIsKindOf<cjNode>(m_root);
	}

    cjRefObj* GetRoot() { return m_root; }

protected:
	cjRefObj*								m_root;	
	cjName2RefObjHash						m_hash;
	cjBool									m_rootIsNode;
};

cjRefPtrDefine(cjRefObjNameHash)


//
// cjPkgNodeSaveTest
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjPkgNodeSaveTest, cjObj, 0, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
	cjPkgNodeSaveTest();

protected:
	virtual void OnDestroy();

public:
	cjNodeRPtr						m_node;

CJOBJ_DECL_CLASS_END(cjPkgNodeSaveTest)


#endif
