
#include "cjCorePCH.h"
#include "cjSceneProp.h"


//
// cjSceneProp
//

CJSER_IMPL_CLASS(cjSceneProp, cjSerializable)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneProp)

	if(ser.IsLoading() || ser.IsSaving())
	{
		cjPODSerializeWithType<cjBYTE>(ser, m_cate);
	}

CJSER_IMPL_SERIALIZE_END(cjSceneProp)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneProp)
	d->m_cate = m_cate;
//	d->_nextProp = cjNULL; // cjreview, do not clone, managed by only in cjSceneNode	
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneProp)

CJSER_IMPL_CLONE(cjSceneProp)


cjSerializer& cjSceneProp::SerializeList(cjSerializer& ser, cjScenePropRPtrList* scenePropList, 
	cjINT32* count)
{
	if(ser.IsQuerySerObjAndStr())
	{
		if(scenePropList->IsEmpty())
			return ser;
		
		cjScenePropRPtrList::Iter iter = scenePropList->Begin();
		cjScenePropRPtr v;

		while(scenePropList->GetNextV(iter, v))
			*v >> ser;
	}	
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjINT32 numProp;
		
		if(ser.IsSaving())
			numProp = scenePropList->Count();

		ser << cjFStreamCmprI32(numProp);

		if(ser.IsSaving())
		{
			cjScenePropRPtrList::Iter iter = scenePropList->Begin();
			cjScenePropRPtr v;

			while(scenePropList->GetNextV(iter, v))
			{
				cjAssert(v);
				ser.SerializeSerObjPtr(&v);
			}
		}
		else if(ser.IsLoading())
		{
			ser.EnqueuePostSerializeObjCount(numProp);

			cjScenePropRPtr scenepropPtr;
			cjScenePropRPtr v;

			// prepare space
			for(cjINT32 i=0; i<numProp; i++)
				ser.SerializeObjForPaddingOnLoad();
		}
	}

	return ser;
}

cjSerializer& cjSceneProp::PostSerializeList(cjSerializer& ser, cjScenePropRPtrList* scenePropList)
{
	if(ser.IsLoading())
	{
		cjAryOff numProp;
		
		if(ser.PopPostSerializeObjCount(numProp))
		{
			for(cjAryOff i=0; i<numProp; i++)
			{
				cjScenePropRPtr v;

				ser.PostSerializeSerObjPtr(v);
				scenePropList->PushBack(v);
			}
		}
	}

	return ser;
}


//cjSerializer& cjSceneProp::SerializeAry(cjSerializer& ser, cjSceneProp** d, cjINT32* )
//{
//	if(ser.IsQuerySerObjAndStr())
//	{
//		if(*d == 0)
//			return ser;
//
//		cjSceneProp* cur = *d;
//		while(cur)
//		{
//			*cur >> ser;
//			cur = cur->_nextProp;
//		}
//	}	
//	else if(ser.IsLoading() || ser.IsSaving())
//	{
//		cjINT32 numProp;
//		
//		if(ser.IsSaving())
//			numProp = cjSceneProp::GetNumProp(*d);
//
//		ser << cjFStreamCmprI32(numProp);
//		
//		cjSerID serID = cjSerID_Invalid;
//
//		if(ser.IsLoading())
//		{	
//			cjSceneProp* propList = 0;		
//
//			for(;;) 
//			{
//				cjSerID_Serialize(ser, serID);
//			
//				if(serID == cjSerID_Invalid)
//					break;
//
//				cjSceneProp* s = ser.GetSerObj<cjSceneProp>(serID);
//				s->AddRef();
//				
//				if(propList)
//					s->_nextProp = propList;					
//				
//				propList = s;
//
//			}
//
//			*d = propList;
//		}
//		else if(ser.IsSaving())
//		{
//			cjSceneProp* cur = *d;
//			serID = cjSerID_Invalid;
//
//			for(;;) 
//			{
//				if(cur)
//				{
//					serID = ser.GetSerObjID(cur);
//					cjAssert(serID != cjSerID_Invalid);
//				}
//				else
//					serID = cjSerID_Invalid;
//
//				cjSerID_Serialize(ser, serID);
//
//				if(cur == 0)
//					break;
//
//				cur = cur->_nextProp;
//
//			}
//		}
//	}
//
//	return ser;
//}

//void cjSceneProp::CloneList(cjSceneProp*& dstPropList, const cjSceneProp* srcPropList)
//{
//	if(!srcPropList || !dstPropList)
//		return;
//
//	cjSceneProp* lastCloneProp = cjNULL;
//	const cjSceneProp* src = srcPropList;
//
//	while(src)	
//	{
//		if(lastCloneProp)
//		{			
//			lastCloneProp->_nextProp = (cjSceneProp*)src->Clone(0);
//			lastCloneProp = lastCloneProp->_nextProp;
//		}
//		else
//		{
//			lastCloneProp = (cjSceneProp*)src->Clone(0);
//		}
//
//		src = src->_nextProp;
//	}
//
//	dstPropList = lastCloneProp;
//}

cjSceneProp::~cjSceneProp()
{
	// _nextProp is release ~cjSceneNode
}

void cjSceneProp::Update(cjFLOAT deltaTime)
{
}

//cjBool cjSceneProp::Find(cjSceneProp* list, cjSceneProp* dst, cjSceneProp** prev)
//{
//	if(!list || !dst)
//		return cjFalse;
//
//	if(prev)
//	{
//		cjSceneProp* curPrev = NULL;
//
//		while(list)
//		{
//			if(list == dst)
//			{
//				if(prev) *prev = curPrev;
//				return cjTrue;
//			}
//
//			curPrev = list;
//			list = list->_nextProp;
//		}
//	}
//	else
//	{
//		while(list)
//		{
//			if(list == dst)
//				return cjTrue;
//
//			list = list->_nextProp;
//		}
//	}
//
//	return cjFalse;
//}
//
//cjSceneProp* cjSceneProp::FindByRtti(cjSceneProp* list, const cjRTTI* rtti)
//{
//	if(!list || !rtti)
//		return cjNULL;
//
//	cjSceneProp* cur = list;
//	while(cur)
//	{
//		if(cur->cjrRTTI() == rtti)
//			return cur;
//
//		cur = cur->_nextProp;
//	}
//
//	return cjNULL;
//}
//
//cjSceneProp* cjSceneProp::FindByRttiName(cjSceneProp* list, const cjNameChar* rttiName)
//{
//	if(!list || !rttiName)
//		return cjNULL;
//
//	cjSceneProp* cur = list;
//	while(cur)
//	{
//		if(!cjStrcmp(cur->cjrRTTI()->cjrGetNameW(), rttiName))
//			return cur;
//
//		cur = cur->_nextProp;
//	}
//
//	return cjNULL;
//}

//void cjSceneProp::ReleaseRefPropList(cjSceneProp* list)
//{
//	cjSceneProp* curNext;
//
//	while(list)
//	{
//		curNext = list->_nextProp;
//		list->ReleaseRef();
//		list = curNext;
//	}
//}

//cjINT32 cjSceneProp::GetNumProp(cjSceneProp* list)
//{
//	cjINT32 num = 0;
//	cjSceneProp* cur = list;
//
//	while(cur)
//	{
//		num++;
//		cur = cur->_nextProp;
//	}
//
//	return num;
//}

