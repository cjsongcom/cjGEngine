
#include "cjCorePCH.h"
#include "cjNode.h"


//
// cjNodeUpdateProcess
//

//CJSER_IMPL_CLASS(cjNodeUpdateProcess, cjSerializable)

cjNodeUpdateProcess::cjNodeUpdateProcess()
{
	m_time = 0.0f;
	m_updateControllers = cjTrue;
}


//
// cjNode
//

CJSER_IMPL_CLASS(cjNode, cjRefObj)

CJSER_IMPL_SERIALIZE_NOSUPER_BEGIN(cjNode)

	// prevent dupplicated calling 
	if(ser.IsQuerySerObjAndStr())
	{
		if(ser.FindQuerySerObj(this) == cjAryOff_Invalid)
		{
			CJSER_IMPL_CALL_SERIALIZE_SUPER

			cjNode* child = GetFirChild();

			if(child)
			{
				// skip when already traversed
				if(ser.FindQuerySerObj(child) == cjAryOff_Invalid) 
					(*child) >> ser;
			}
						
			cjNode* sibil = GetSibling();

			if(sibil) // no need while(sibil)
			{
				if(ser.FindQuerySerObj(sibil) == cjAryOff_Invalid)
					(*sibil) >> ser;
			}

		}

		return;
	}

	CJSER_IMPL_CALL_SERIALIZE_SUPER

	if(ser.IsSaving() || ser.IsLoading())
	{
		CJFLAG_SERIALIZE
		ser << m_lifeTime;

		if(ser.IsSaving())
		{	
			cjSerID serID = cjSerID_Invalid;

			if(m_parent)
				serID = ser.GetSerObjID(m_parent);

			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID parentNodeSerID;
			cjSerID_Serialize(ser, parentNodeSerID);

			if(parentNodeSerID != cjSerID_Invalid)
			{
				cjNode* parent = ser.GetSerObjNode(parentNodeSerID);

				cjAssert(parent != 0);

				if(parent)
					parent->AddChildOnTail(this);
			}
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjNode)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjNode)
	
	CJFLAG_CLONE
	d->m_lifeTime = m_lifeTime;
	d->m_parent = cjNULL;
	d->m_firstChild = cjNULL;
	d->m_sibling = cjNULL;
	
	// clone child nodes
	cjNode* srcChild = m_firstChild;
	cjNode* lastNode = cjNULL;
	
	while(srcChild)
	{
		if(lastNode)
		{
			lastNode->SetSibling((cjNode*)srcChild->Clone());
			lastNode = lastNode->GetSibling();
		}
		else
		{
			d->m_firstChild = (cjNode*)srcChild->Clone();
			CJ_REF_ADD(d->m_firstChild);

			lastNode = d->m_firstChild;
		}

		lastNode->SetParent(d);
		srcChild = srcChild->GetSibling();
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjNode)

CJSER_IMPL_CLONE(cjNode)


void cjNode::OnLinkNode()
{
}


//
// cjNode
//

cjNode::cjNode()
{
	CommonValueInit();
}

cjNode::cjNode(const cjNameChar* name)
{
	CommonValueInit();

	if(name)
		SetName(name);
}

cjNode::~cjNode()
{
	RemoveAllChild();
}

void cjNode::CommonValueInit()
{
	CJFLAG_IMPL_INIT
	m_lifeTime = 0.0f;	
	m_parent = 0;

	m_firstChild = 0;
	m_sibling = 0;
}

void cjNode::ChangeParent(cjNode* newParent)
{
	CJ_REF_ADD(this);

	if(GetParent())
		GetParent()->RemoveChild(this);

	if(newParent)
		newParent->AddChild(this);

	CJ_REF_MUST_RELEASE(this);
}


cjINT32 cjNode::GetChildNumSlow()
{
	cjNode* cur = m_firstChild;
	cjINT32 numChild = 0;

	while(cur)
	{
		numChild++;
		cur = cur->GetSibling(); // get next sibling child
	}

	return numChild;
}

cjNode* cjVerySlow cjNode::GetChildByIdxVerySlow(cjINT32 index)
{
	cjNode* cur = m_firstChild;

	for(cjINT32 i=0; i<index; i++)
		cur = cur->GetSibling();

	return cur;
}

void cjNode::SetSibling(cjNode* sibling)
{
	CJ_REF_CHANGE_TO(m_sibling, sibling);
}

void cjNode::AddChild(cjNode* node)
{
	AddChildOnTail(node);
}

//void cjNode::AddChildOnHead(cjNode* node)
//{
//	if(!node)
//		return;
//
//	if(node->m_parent)
//		node->m_parent->RemoveChild(this);
//
//	node->SetSibling(m_firstChild);
//	CJ_REF_CHANGE_TO(m_firstChild, node);
//
//	m_firstChild->SetParent(this);
//}

void cjNode::AddChildOnTail(cjNode* node)
{
	cjAssert(node);

	if(!node)
		return;

	CJ_REF_ADD(node); // #ref0

	if(node->GetParent())
		node->GetParent()->RemoveChild(node);

	if(m_firstChild)
	{  
		// add node on next of last child		
		cjNode* cur = m_firstChild;

		// lookup last child
		while(cur->GetSibling())
			cur = cur->GetSibling();

		cjAssert(cur != node);
		cur->SetSibling(node);
	}
	else
	{
		// newly first child
		m_firstChild = node;
		CJ_REF_ADD(m_firstChild);
	}

	node->SetParent(this);

	CJ_REF_RELEASE(node); // #ref0
}

void cjNode::RemoveChild(cjNode* node)
{
	cjAssert(node);
	node->SetParent(NULL);

	if(m_firstChild == node) // founded on head child
	{
		m_firstChild = node->GetSibling();
		CJ_REF_SAFE_ADD(m_firstChild); // can be null
	}
	else
	{
		cjNode* curPrev = m_firstChild;

		// find previous childNode from given node
		while(curPrev && (curPrev->GetSibling() != node))
			curPrev = curPrev->GetSibling();

		if(!curPrev)
		{
			cjAssert(0 && "ERR! - given child node is not founded..");
			return;
		}

		CJ_REF_ADD(node);

		// adjust next sibling(as same level child) link
		curPrev->SetSibling(node->GetSibling());
	}

	node->SetSibling(NULL);
	CJ_REF_RELEASE(node);
}

void cjNode::RemoveAllChild()
{
	cjNode *cur, *curNext;

	for(cur=m_firstChild; cur; )
	{
		curNext = cur->GetSibling();

		cur->SetParent(0);
		CJ_REF_SAFE_RELEASE(cur);

		cur = curNext;
	}

	m_firstChild = NULL;
}

cjNode* cjNode::FindChildByName(const cjNameChar* name, cjBool recur)
{
	cjNode* cur = m_firstChild;
	cjNode* rst = NULL;

	while(cur)
	{
		if(cur->GetName().IsValid())
		{
			if(cur->GetName().Cmp(name))
				return cur;
		}

		if(recur)
		{
			rst = cur->FindChildByName(name, cjTrue);

			if(rst)
				return rst;
		}

		cur = cur->GetSibling();
	}

	return NULL;
}

//
// cjNode - update
//

void cjNode::Update(cjEngineTime_t etime, cjBool updateAniCntr)
{
	cjNodeUpdateProcess* updateProcess = cjSceneNode::GetDefaultUpdateProcess();

	updateProcess->SetTime(etime);
	updateProcess->SetUpdateAniCntrs(updateAniCntr);

	Update(*updateProcess);
}

void cjNode::UpdateBySelectFlag(cjEngineTime_t etime)
{
	cjNodeUpdateProcess* updateProcess = cjSceneNode::GetDefaultUpdateProcess();

	updateProcess->SetTime(etime);
	updateProcess->SetUpdateAniCntrs(cjTrue);

	UpdateBySelectFlag(*updateProcess);
}

void cjNode::SetNGetSeletiveUpdateFlags(cjBool& sgSUFlagUpdateSelective, cjBool& sgSUFlagUpdateRigid, 
	cjBool setSUFlagUpateTM)
{
    cjBool hasAniTMCntr = HasTMAniCntr();
	
	if(!setSUFlagUpateTM)
        setSUFlagUpateTM = hasAniTMCntr || GetSUFlagForceUpdateTM();

    sgSUFlagUpdateSelective = setSUFlagUpateTM;
    sgSUFlagUpdateRigid = cjTrue;

	cjNode* child = GetFirChild();
	while(child)
	{
		cjBool tmpUpdateSelective = cjFalse;
        cjBool tmpUpdateRigid = cjTrue;

        child->SetNGetSeletiveUpdateFlags(tmpUpdateSelective, tmpUpdateRigid, setSUFlagUpateTM);

        if(tmpUpdateSelective)
			sgSUFlagUpdateSelective = cjTrue;

        if(!tmpUpdateRigid)
			sgSUFlagUpdateRigid = cjFalse;

		child = child->GetSibling();
    }

    cjBool hasScenePropAniCntr = HasScenePropAniCntr();
    
    if(hasScenePropAniCntr || GetFirstAniCntr())
        sgSUFlagUpdateSelective = cjTrue;

    SetSUFlagUpdateSelective(sgSUFlagUpdateSelective);
    SetSUFlagPropAniCntr(hasScenePropAniCntr);
	SetSUFlagUpdateTM(setSUFlagUpateTM);    
    SetSUFlagUpdateRigid(sgSUFlagUpdateRigid);
    
    if(hasAniTMCntr)
        sgSUFlagUpdateRigid = cjFalse;
}


//
// cjNode - update - internal use
//

void cjNode::UpdateAniCntrs(cjEngineTime_t ti, cjBool updateProperties)
{
    if(updateProperties)
		UpdateScenePropAniCntr(ti);

	cjAniCntr* curAniCntr = GetFirstAniCntr();
	while(curAniCntr)
	{
		curAniCntr->Update(ti);
		curAniCntr = curAniCntr->GetNextCntr();
	}
}

void cjNode::Update(cjNodeUpdateProcess& updateProcess)
{
	updateProcess.PreUpdate(this);
	
	UpdateDownward(updateProcess);
	
	if(m_parent)
		m_parent->UpdateUpward();

	updateProcess.PostUpdate(this);
}

void cjNode::UpdateBySelectFlag(cjNodeUpdateProcess& updateProcess)
{
	updateProcess.PreUpdate(this);
	
	UpdateBySelectFlagDownward(updateProcess);
	
	if(m_parent)
		m_parent->UpdateUpward();

	updateProcess.PostUpdate(this);
}

void cjNode::UpdateUpward()
{
	cjNode* parent = GetParent();

	if(parent)
		parent->UpdateUpward();
}

void cjNode::UpdateDownward(cjNodeUpdateProcess& updateProcess)
{
	if(updateProcess.GetUpdateAniCntrs())
		UpdateAniCntrs(updateProcess.GetTime());
		
	cjNode* child = GetFirChild();
	while(child)
	{
		child->UpdateDownward(updateProcess);
		child = child->GetSibling();
	}
}

void cjNode::UpdateBySelectFlagDownward(cjNodeUpdateProcess& updateProcess)
{
    UpdateAniCntrs(updateProcess.GetTime(), GetSUFlagPropAniCntr());
}

void cjNode::UpdateRigidDownward(cjNodeUpdateProcess& updateProcess)
{
    UpdateAniCntrs(updateProcess.GetTime(), GetSUFlagPropAniCntr());
}

//
// cjRefObjNameHash
// 

CJSER_IMPL_CLASS(cjRefObjNameHash, cjRef)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjRefObjNameHash)
	d->m_root = m_root;
	m_hash.CloneTo(d->m_hash);
	d->m_rootIsNode = m_rootIsNode;
CJSER_IMPL_CLONE_PROPCOPY_END(cjRefObjNameHash)

CJSER_IMPL_CLONE(cjRefObjNameHash)


cjRefObjNameHash::~cjRefObjNameHash()
{
}


//
// cjPkgNodeSaveTest
//


CJOBJ_IMPL_CLASS(cjPkgNodeSaveTest)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjPkgNodeSaveTest)
	
	if(stream.IsObjOnly())
		return;
	
	cjSerObjStreamBroker ser(&stream);
	ser.Serialize(m_node);

CJOBJ_IMPL_SERIALIZE_END(cjPkgNodeSaveTest)


cjPkgNodeSaveTest::cjPkgNodeSaveTest()
{
}

void cjPkgNodeSaveTest::OnDestroy()
{
	m_node = cjNULL;
	cjSuper::OnDestroy();
}



































//void cjNode::Update(cjEngineTime_t dt)
//{
//}


//
// cjNodeObj
//

//CJOBJ_IMPL_CLASS(cjNode)
//
//CJOBJ_IMPL_SERIALIZE_BEGIN(cjNode)
//
//	if(stream.IsObjOnly())
//		return;
//
//	cjNode::Serialize(stream, &m_node);
//
//	cjBYTE valid = m_node ? 1 : 0;
//	stream << valid;
//
//	if(stream.IsLoading())
//	{
//		CJ_REF_SAFE_RELEASE(m_node);
//		
//		if(valid)
//			
//	} 
//	else if(stream.IsSaving())
//	{
//		if(valid(
//	}
//
//CJOBJ_IMPL_SERIALIZE_END(cjNode)

//
//cjNode::cjNode() : m_node(0)
//{
//	
//}
//
//void cjNode::OnDestroy()
//{
//	cjSuper::OnDestroy();
//
//}


//
// cjNodeSerializer
//
//
//cjNodeSerializer::cjNodeSerializer()
//{
//
//}
//
//cjNode* cjNodeSerializer::Serialize(cjFStream& stream, cjNode** node)
//{
//	if(stream.IsLoading())
//	{
//		return 0;
//	}
//	else if(stream.IsSaving())
//	{
//		cjNode* dstNode = 0;
//	
//		// collect all serialize object
//		dstNode->_SerCollectObjects(this);
//
//		// build string table
//		cjNodeSerializer* ser;
//		cjNode* curNode;
//
//		cjUni* rttiName = curNode->cjrRTTI()->cjrGetNameW();
//		ser->AddString(rttiName);
//
//		CollectNodeString(this, curNode->GetChild());
//	
//		curNode->GetSibling()
//
//		return dstNode;
//	}
//}
//
//cjINT32 cjNodeSerializer::AddInstance(cjNode* node)
//{
//	if(!node)
//		return -1;
//
//	cjAryOff index = m_instances.Find(node);
//
//	if(index == cjAryOff_Invalid)
//		return m_instances.Push(node);
//
//	return index;
//}
