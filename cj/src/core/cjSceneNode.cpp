
#include "cjCorePCH.h"
#include "cjSceneNode.h"


cjNodeUpdateProcess cjSceneNode::ms_defaultUpdateProcess;

CJSER_IMPL_CLASS(cjSceneNode, cjNode)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneNode)

	if(ser.IsQuerySerObjAndStr())
	{
		if(!m_scenePropList.IsEmpty())
			cjSceneProp::SerializeList(ser, &m_scenePropList);
	}
	else if(ser.IsSaving() || ser.IsLoading())
	{
		ser << m_local;
				
		ser << m_world;			//cjremove - no need to serialize
		ser << m_worldBound;	//cjremove - no need to serialize
		
		cjSceneProp::SerializeList(ser, &m_scenePropList);
	}

CJSER_IMPL_SERIALIZE_END(cjSceneNode)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjSceneNode)

	if(ser.IsLoading())
	{
		//cjdefer
		cjSceneProp::PostSerializeList(ser, &m_scenePropList);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjSceneNode)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneNode)
	d->m_local = m_local;
//	d->m_world = m_world;
//	d->m_worldBound = m_worldBound;
	_CloneScenePropList(d, &m_scenePropList, cloner);
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneNode)

CJSER_IMPL_CLONE(cjSceneNode)

CJSER_IMPL_CLONE_POST_BEGIN(cjSceneNode)
    if(!m_scenePropList.IsEmpty())
        _PostCloneScenePropList(&m_scenePropList, cloner);
CJSER_IMPL_CLONE_POST_END(cjSceneNode)

	
void cjSceneNode::SerializeAry(cjSerializer& ser, cjSceneNode*** d, cjINT32* count)
{
	if(ser.IsQuerySerObjAndStr())
	{
		for(cjINT32 i=0; i< *count; i++)
		{
			cjSceneNode* curSceneNode = (*d)[i];
			*curSceneNode >> ser;
		}
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
	//	cjSerID serID;
		cjINT32 datCount = count ? *count : 0;

		ser << cjFStreamCmprI32(datCount);
		
		if(ser.IsLoading())
		{
			cjAssert(*d == 0);

			ser.EnqueuePostSerializeObjCount(datCount);
								
			if(datCount)
			{
				*d = new cjSceneNode*[datCount];
				for(cjINT32 i=0; i<datCount; i++)
				{
					ser.SerializeObj((*d)[i]);

//					cjSerID_Serialize(ser, serID);
//					(*d)[i] = ser.GetSerObj<cjSceneNode>(serID);
				}
			}

			if(count)
				*count = datCount;			
		}
		else if(ser.IsSaving())
		{
			for(cjINT32 i=0; i<datCount; i++)
			{
				ser.SerializeObj((*d)[i]);

//				serID = ser.GetSerObjID((*d)[i]);
//				cjSerID_Serialize(ser, serID);
			}
		}
	}
}

cjSerializer& cjSceneNode::PostSerializeAry(cjSerializer& ser, cjSceneNode** d)
{
	if(ser.IsLoading())
	{
		cjAryOff count;

		if(ser.PopPostSerializeObjCount(count))
		{			
			for(cjAryOff i=0; i<count; i++)
				ser.PostSerializeSerObj(d[i]);
		}
	}

	return ser;
}

void cjSceneNode::_CloneScenePropList(cjSceneNode* dst, cjScenePropRPtrList* srcList, cjSerObjCloner* cloner)
{
	cjScenePropRPtrList::Iter iter = srcList->End();
	cjScenePropRPtr v;

	while(srcList->GetPrevV(iter, v))
	{		
		cjAssert(v);

		cjSceneProp* clonedSceneProp = (cjSceneProp*)(v->_CloneShared(cloner));
		dst->AttachSceneProp(clonedSceneProp);		
	}
}

void cjSceneNode::_PostCloneScenePropList(cjScenePropRPtrList* srcList, cjSerObjCloner* cloner)
{
	cjScenePropRPtrList::Iter iter = srcList->End();
	cjScenePropRPtr v;

	while(srcList->GetPrevV(iter, v))
	{
		cjAssert(v);

		if(!cloner->IsPostCloned(v))
			v->_PostClone(cloner);
	}
}

cjBool cjSceneNode::HasScenePropAniCntr()
{
	cjScenePropRPtrList::Iter iter = m_scenePropList.Begin();
	cjScenePropRPtr v;
	
	while(m_scenePropList.GetNextV(iter, v))
	{
		if(v && v->GetFirstAniCntr())
			return cjTrue;
	}

	return cjFalse;
}

void cjSceneNode::UpdateScenePropAniCntr(cjEngineTime_t etime)
{
	cjScenePropRPtrList::Iter iter = m_scenePropList.Begin();
	cjScenePropRPtr v;
	
	while(m_scenePropList.GetNextV(iter, v))
	{
		if(v && v->GetFirstAniCntr())
			v->Update(etime);
	}
}

void cjSceneNode::UpdateUpward()
{
	cjNode* parent = GetParent();

	UpdateWorldBound();

	if(parent)
		parent->UpdateUpward();
}

void cjSceneNode::UpdateDownward(cjNodeUpdateProcess& updateProcess)
{
	if(updateProcess.GetUpdateAniCntrs())
		UpdateAniCntrs(updateProcess.GetTime());
		
	UpdateWorldData();  // update worldTM, collision object
	m_worldBound.SetRadius(0.0f);

	cjNode* child = GetFirChild();
	while(child)
	{
		child->UpdateDownward(updateProcess);

		cjSceneNode* scNode = child->GetSceneNode();
		if(scNode && !scNode->IsWorldBoundZero())
		{
			if(IsWorldBoundZero())
			{
				m_worldBound = scNode->GetWorldBound();
			}
			else
			{
				//cjtemp
				// m_worldBound.Merge(&scNode->GetWorldBound());
			}
		}

		child = child->GetSibling();
	}
}

void cjSceneNode::UpdateBySelectFlagDownward(cjNodeUpdateProcess& updateProcess)
{
    UpdateAniCntrs(updateProcess.GetTime(), GetSUFlagPropAniCntr());

    if(GetSUFlagUpdateTM())
        UpdateWorldData();

	m_worldBound.SetRadius(0.0f);

	cjNode* child = GetFirChild();
	while(child)
	{
		child->DoUpdateSelective(updateProcess);

		cjSceneNode* scNode = child->GetSceneNode();
		if(scNode&& !scNode->IsWorldBoundZero())
		{
			if(IsWorldBoundZero())
			{
				m_worldBound = scNode->GetWorldBound();
			}
			else
			{
				//cjtemp
				// m_worldBound.Merge(&scNode->GetWorldBound());
			}
		}

		child = child->GetSibling();
	}
}

void cjSceneNode::UpdateRigidDownward(cjNodeUpdateProcess& updateProcess)
{
    UpdateAniCntrs(updateProcess.GetTime(), GetSUFlagPropAniCntr());

    if(GetSUFlagUpdateTM())
    {
        UpdateWorldData();
        UpdateWorldBound();
    }

	cjNode* child = GetFirChild();
	while(child)
	{
		if(child->GetSUFlagUpdateSelective())
			child->UpdateRigidDownward(updateProcess);

		child = child->GetSibling();
	}
}

cjSceneNode::cjSceneNode()
{
	m_local.MakeIdentity();
	m_world.MakeIdentity();

	m_worldBound.MakeIdentity();
}

cjSceneNode::~cjSceneNode()
{	
//	cjSceneProp::ReleaseRefPropList(m_renPropList);
//	m_renPropList = 0;
}

void cjSceneNode::UpdateProperties()
{

}

void cjSceneNode::UpdateEffects()
{

}

cjSceneNode* cjSceneNode::GetFirstChildSceneNode()
{
	cjNode* firstChild = GetFirChild();

	if(!firstChild)
		return 0;

	if(cjrIsKindOf<cjSceneNode>(firstChild))
		return (cjSceneNode*)firstChild;

	return 0;
}

cjSceneNode* cjSceneNode::GetSibilSceneNode()
{
	cjNode* sibil = GetSibling();

	if(!sibil)
		return 0;

	if(cjrIsKindOf<cjSceneNode>(sibil))
		return (cjSceneNode*)sibil;

	return 0;
}

cjSceneNode* cjSceneNode::GetParentSceneNode()
{
	cjNode* parent = GetParent();

	if(!parent)
		return 0;

	if(cjrIsKindOf<cjSceneNode>(parent))
		return (cjSceneNode*)parent;

	return 0;
}

void cjSceneNode::UpdateBound()
{

}

void cjSceneNode::UpdateWorldData()
{
	cjSceneNode* parent = GetParentSceneNode();

	if(parent)
	{
		m_world = parent->m_world * m_local;
	}
	else
	{
		m_world = m_local;
	}

//	if(m_collisionObject)
//		m_collisionObject->UpdateWorldData();
}

void cjSceneNode::UpdateWorldBound()
{

}

void cjSceneNode::AttachSceneProp(cjSceneProp* sceneProp)
{
	if(!sceneProp)
		return;

	m_scenePropList.PushBack(sceneProp);
}

void cjSceneNode::DetachSceneProp(cjSceneProp* sceneProp)
{
	if(!sceneProp)
		return;

	m_scenePropList.Remove(sceneProp);
}

cjSceneProp* cjSceneNode::FindSceneProp(const cjRTTI* rtti) const
{
	if(!rtti)
		return cjNULL;

	cjScenePropRPtrList::Iter iter = m_scenePropList.Begin();
	cjScenePropRPtr v;
	
	while(m_scenePropList.GetNextV(iter, v))
	{
		if(v->cjrRTTI() == rtti)
			return v;
	}

	return cjNULL;
}

cjSceneProp* cjSceneNode::FindScenePropByRttiName(const cjNameChar* rttiName) const
{
	if(!rttiName)
		return cjNULL;

	cjScenePropRPtrList::Iter iter = m_scenePropList.Begin();
	cjScenePropRPtr v;

	while(m_scenePropList.GetNextV(iter, v))
	{
		if(v->cjrRTTI()->CmpName(rttiName))
			return v;
	}

	return cjNULL;
}

