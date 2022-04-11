
#include "cjEnginePCH.h"
#include "cjSceneNode.h"

//
//CJSER_IMPL_CLASS(cjSceneNode, cjNode)
//
//
//void cjSceneNode::Update(cjEngineTime_t dt)
//{
//
//}
//
//cjSceneNode::cjSceneNode()
//{
//	m_renPropList = NULL;
//}
//
//cjSceneNode::~cjSceneNode()
//{
//	cjSceneProp::ReleaseRefPropList(m_renPropList);
//	m_renPropList = NULL;
//}
//
//void cjSceneNode::UpdateProperties()
//{
//
//}
//
//void cjSceneNode::UpdateEffects()
//{
//
//}
//
//void cjSceneNode::UpdateBound()
//{
//
//}
//
//void cjSceneNode::UpdateWorldData()
//{
//
//}
//
//void cjSceneNode::UpdateWorldBound()
//{
//
//}
//
//void cjSceneNode::AttachSceneProp(cjSceneProp* sceneProp)
//{
//	if(!sceneProp)
//		return;
//
//	if(m_renPropList)
//	{
//		cjAssert(sceneProp->m_nextProp == NULL);
//
//#ifdef CJ_SLOW_CHECK
//		cjAssert(!cjSceneProp::Find(m_renPropList, sceneProp));
//#endif
//		sceneProp->m_nextProp = m_renPropList; // moved from cjSceneNode to sceneProp
//
//		m_renPropList = sceneProp;
//		sceneProp->AddRef();			   // from cjSceneNode
//	}
//	else
//	{
//		m_renPropList = sceneProp;
//		sceneProp->AddRef();			  // from cjSceneNode
//	}
//}
//
//void cjSceneNode::DetachSceneProp(cjSceneProp* sceneProp)
//{
//	if(!m_renPropList || !sceneProp)
//		return;
//
//	if(m_renPropList == sceneProp)
//	{
//		if(sceneProp->m_nextProp)
//			m_renPropList = sceneProp->m_nextProp;
//
//		sceneProp->m_nextProp = NULL;
//		sceneProp->ReleaseRef();
//
//		return;
//	}
//	
//	cjSceneProp* prev;
//	if(!cjSceneProp::Find(m_renPropList, sceneProp, &prev))
//		return; // not found
//
//	prev->m_nextProp = sceneProp->m_nextProp;
//	sceneProp->m_nextProp = NULL;
//	sceneProp->ReleaseRef();
//}
//
//cjSceneProp* cjSceneNode::FindSceneProp(const cjRTTI* rtti)
//{
//	return cjSceneProp::FindByRtti(m_renPropList, rtti);
//}
