
#include "cjCorePCH.h"
#include "cjSerialize.h"


//
// cjSerializer
//
 
cjSerializer::cjSerializer() : m_cjObjPreLoaded(128)
{
	cjMemZero(m_freeMem, sizeof(m_freeMem));
	cjMemZero(m_freeMemSiz, sizeof(m_freeMemSiz));
	m_numFreeMem = 0;

	Reset();
}

cjSerializer::~cjSerializer()
{
	FlushFreeMem();	
}

cjAryOff cjSerializer::AddString(const cjUni* str)
{
	if(!str)
		return cjAryOff_Invalid;

	return m_collectedStr.Push(str);
}

cjAryOff cjSerializer::AddString(const cjChar* str)
{
	if(!str)
		return cjAryOff_Invalid;

	cjChar2Uni c2u(str);
	return m_collectedStr.Push(*c2u);
}

cjAryOff cjSerializer::AddCJObj(cjObj* obj)
{
	if(!obj)
		return cjAryOff_Invalid;
		
	return m_collectedCJObj.Push(obj);
}

cjAryOff cjSerializer::AddSerObj(cjSerializable* serObj)
{
	if(!serObj)
		return cjAryOff_Invalid;
		
	m_collectedSerObjRtti.PushUnique(serObj->cjrRTTI());

	return m_collectedSerObj.PushUnique(serObj);
}

cjAryOff cjSerializer::FindQuerySerObj(cjSerializable* serObj)
{
	if(!serObj)
		return cjAryOff_Invalid;

	return m_collectedSerObj.Find(serObj);
}

cjSerID cjSerializer::GetCJObjID(cjObj* obj)
{
	if(!obj)
		return cjSerID_Invalid;

	cjAryOff off = m_serCJObj.Find(obj);

	cjAssert(obj && off != cjAryOff_Invalid);

	if(off == cjAryOff_Invalid)
		return  cjSerID_Invalid;

	return cjSerializable::GenerateCJObjID(obj, off);
}

cjSerID cjSerializer::GetSerObjID(cjSerializable* obj)
{
	if(!obj)
		return cjSerID_Invalid;

	cjAryOff off = m_serSerObj.Find(obj);

	if(off == cjAryOff_Invalid)
		return  cjSerID_Invalid;

	return cjSerializable::GenerateSerObjID(obj, off);
}

//cjdeprecate
//void cjSerializer::SaveSerObj(cjSerializable* serObj)
//{
//	cjSerID serID = GetSerObjID(serObj);
//	cjSerID_Serialize(*this, serID);
//}

cjSerID cjSerializer::PopPreloadSerObj()
{	
	cjSerID serID;

	if(!PopPostSerializeSerID(serID))
		return cjSerID_Invalid;

	return serID;
}

cjBool cjSerializer::EnqueuePostSerializeSerID(cjSerID serID)
{
	cjAryOff insOff = 0;
	cjAryOff deferSetObjCount = m_deferSetObjs.Count();
	
	if(m_deferSetObjIndex >= m_deferSetObjs.Count())
	{
		insOff = m_deferSetObjs.PushEmpty();
	}
	else
	{
		return cjFalse;
	}
	//else
	//{
	//	if(deferSetObjCount == m_deferSetObjs.GetMaxCount())
	//		insOff = m_deferSetObjs.PushEmpty();
	//	else
	//		insOff = m_deferSetObjIndex;
	//}

	DeferObjSetDat& cur = m_deferSetObjs(insOff);

	cur.m_serID = serID;
//	cur.m_dstObj = GetSerObj(serID);

	//cjtemp
	/*cjMLOG("[EP]  insOff=%d  serID=%x ", insOff, serID);*/

	m_deferSetObjIndex++;
	cjAssert(m_deferSetObjIndex <= (m_deferSetObjs.Count()+1));

	return cjTrue;
}

cjBool cjSerializer::PopPostSerializeSerID(cjSerID& serID)
{	
	if(m_deferSetObjPopIndex >= m_deferSetObjs.Count())
	{
		cjFatal("m_deferSetObjIndex is invalid..");

		serID = cjSerID_Invalid;
		return cjFalse;
	}

	DeferObjSetDat& cur = m_deferSetObjs(m_deferSetObjPopIndex);

	serID = cur.m_serID;

	cur.m_serID = cjSerID_Invalid;
//	cur.m_dstObj = ;

	//cjtemp
	//cjMLOG("[PO]  popOff=%d  serID=%x ", m_deferSetObjPopIndex, serID);

	m_deferSetObjPopIndex++;

	return cjTrue;
}

cjObj* cjSerializer::GetCJObj(cjSerID serID, cjObjCls* verifyCls)
{
	if(serID != cjSerID_Invalid)
	{
		cjSerOrdinal ord = cjSerID_GetOrdinal(serID);

		if(m_serCJObj.Num() > ord)
		{
			if(!verifyCls)
				return m_serCJObj(ord);

			cjObj* obj = m_serCJObj(ord);

			if(!obj)
				return 0;

			if(obj->cjObjHasKindOfCls(verifyCls))
				return obj;
			else
				return 0;
		}
	}

//	cjAssert(!"[cjSerializer] ERR! - Invalid Ordinal");
	return 0;
}

cjSerializable* cjSerializer::GetSerObj(cjSerID serID)
{
	if(serID != cjSerID_Invalid)
	{
		cjSerOrdinal ord = cjSerID_GetOrdinal(serID);

		if(m_serSerObj.Num() > ord)
		{
			m_serSerObjHitNum(ord)++;
			return m_serSerObj(ord);
		}
	}

//	cjAssert(!"[cjSerializer] ERR! - Invalid Ordinal");
	return 0;
}

cjNode* cjSerializer::GetSerObjNode(cjSerID serID)
{
	cjSerializable* obj = GetSerObj(serID);

	if(!obj)
		return 0;

	if(obj->cjsIsKindOf<cjNode>())
		return (cjNode*)obj;

	return 0;
}

void cjSerializer::Reset()
{
	SetLoading(cjFalse);
	SetSaving(cjFalse);
	SetObjOnly(cjFalse);
	SetSerOnly(cjTrue);

	m_flag = 0;

	m_collectedCJObj.RemoveAll();
	m_collectedSerObj.RemoveAll();
	m_collectedSerObjRtti.RemoveAll();
	m_collectedStr.RemoveAll();

	m_serCJObj.RemoveAll();
	m_serSerObj.RemoveAll();
	m_serSerObjHitNum.RemoveAll();
	m_serSerObjRttiStr.RemoveAll();
	m_serSerObjClsStrIdx.RemoveAll();	
	m_serStr.RemoveAll();
	m_serStrIndex.RemoveAll();
	m_serRtti.RemoveAll();

	m_deferSetObjs.RemoveAll();
	m_deferSetObjIndex = 0;
	m_deferSetObjPopIndex = 0;

	cjMemZero(m_freeMem, sizeof(m_freeMem));
	cjMemZero(m_freeMemSiz, sizeof(m_freeMemSiz));
	m_numFreeMem = 0;
	m_lastFreeMem = 0;
}

void cjSerializer::PostQuery()
{
	// unique object pointer
	// build unique obj string pointer ary
	// ser.AddSerString(cjrRTTI()->cjrGetNameW())

	// compare collect str to unique objstr
	// collect valid string

	// build index to pointer ary
	// build index to string ary

	// remove dupplicate instance
	for(cjAryOff i=0; i<m_collectedCJObj.Num(); i++)
		m_serCJObj.PushUnique(m_collectedCJObj(i));

	for(cjAryOff i=0; i<m_collectedSerObj.Num(); i++)
		m_serSerObj.PushUnique(m_collectedSerObj(i));
	
	// remove dupplicate rtti name
	m_serSerObjClsStrIdx.RemoveAll(m_serSerObj.Num(), cjTrue);

	for(cjAryOff i=0; i<m_serSerObj.Num(); i++)
	{
		const cjChar* rttiName = m_serSerObj(i)->cjrRTTI()->cjrGetNameA();
		cjAryOff index = m_serSerObjRttiStr.PushUnique(rttiName);

		m_serSerObjClsStrIdx(i) = index;
	}
	
	// remove dupplicate str
	for(cjAryOff i=0; i<m_serStr.Num(); i++)
	{
		const cjUni* src = *m_collectedStr(i);
		
		cjAryOff found = -1;
		for(cjAryOff k=0; k<m_serStr.Num(); k++)
		{
			const cjUni* dst = *m_serStr(i);
			if(!cjStrcmp(src, dst))
			{
				found = k;
				break;
			}
		}

		if(found == -1)
		{
			found = m_serStr.Push(src);
			m_serStrIndex(i) = found;
		}
		else
		{
			m_serStrIndex(i) = found;
		}
	}

}

void cjSerializer::WriteHeader()
{
	//
	// info
	//

	cjWORD rev = CJSERIALIZER_REV_MAKE(CJSERIALIZER_REV_MAJOR, CJSERIALIZER_REV_MINOR);
	cjUINT32 info = ((0xefef) << 16) | rev;
	*this << info;


	// write cjObjName (xxx.yyy)
	cjINT32 numCJObj = m_serCJObj.Num();
	*this << cjFStreamCmprI32(numCJObj);

	cjNameChar buf[cjMAX_PATH];
	for(cjINT32 i=0; i<numCJObj; i++)
	{
		cjObj* cur = m_serCJObj(i);

			// class pkg name
		const cjNameChar* clsPkgName = cur->GetCls()->GetPkgNameW();
		cjSerializeNameCharStr(*this, &clsPkgName, 0);		

			// class name
		const cjNameChar* clsName = *(cur->GetCls()->GetName());
		cjSerializeNameCharStr(*this, &clsName, 0);		

			// obj name
		const cjNameChar* objName = cur->cjObjGetPathName(buf, cjMAX_PATH);
		cjSerializeNameCharStr(*this, &objName, 0);
	}	

	// write obj instance name string	
	cjINT32 numRttiStr = m_serSerObjRttiStr.Num();
	*this << cjFStreamCmprI32(numRttiStr);

	for(cjINT32 i=0; i<numRttiStr; i++)
	{
		const char* name = m_serSerObjRttiStr(i);
		cjSerializeStrChar(*this, &name, 0);
	}

	cjINT32 numStr = m_serStr.Num();
	*this << cjFStreamCmprI32(numStr);
	for(cjINT32 i=0; i<numStr; i++)
	{
		const cjNameChar* str = *m_serStr(i);
		cjSerializeNameCharStr(*this, &str, 0);
	}

	// write object
	cjINT32 numObj = m_serSerObj.Num();
	*this << cjFStreamCmprI32(numObj);
			
	for(cjAryOff i=0; i<numObj; i++)
	{		
		cjINT32 idx = m_serSerObjClsStrIdx(i);
		(*this) << cjFStreamCmprI32(idx);
	}

	// write object content
	for(cjAryOff i=0; i<numObj; i++)
	{		
		cjSerializable* cur = m_serSerObj(i);
		(*cur) >> *this;
	}
}

void cjSerializer::ReadHeader()
{
	if(!IsLoading())
		return;

	//
	// info
	//
	cjUINT32 info = 0;
	*this << info;
	
	//
	// cjObj
	//
	{
		const cjNameChar* clsPkgName = 0;
		const cjNameChar* objName = 0;
		const cjNameChar* clsName = 0;
		cjINT32 numCJObj = 0;
		cjINT32 numPreloadedFind = 0;
		cjObj* rstPreLoaded;
		
		*this << cjFStreamCmprI32(numCJObj);
		m_serCJObj.RemoveAll(numCJObj, cjTrue);

		for(cjINT32 i=0; i<numCJObj; i++)
		{
			cjSerializeNameCharStr(*this, &clsPkgName, 0, &cjSerializer::RecycleMAlloc, 
				&cjSerializer::RecycleMFree);

			cjSerializeNameCharStr(*this, &clsName, 0, &cjSerializer::RecycleMAlloc, 
				&cjSerializer::RecycleMFree);

			cjSerializeNameCharStr(*this, &objName, 0, &cjSerializer::RecycleMAlloc, 
				&cjSerializer::RecycleMFree);

			cjString _objNameStr;
			if(objName)
				_objNameStr = objName;

			rstPreLoaded = 0;
			if(numPreloadedFind < m_cjObjPreLoaded.GetNum())
			{				
				if(m_cjObjPreLoaded.Find(_objNameStr, &rstPreLoaded))
					numPreloadedFind++;
			}

			cjObj* loadObj = 0;
			if(rstPreLoaded)
			{
				loadObj = rstPreLoaded;
			}
			else
			{
				cjObjPackage* pkg = (cjObjPackage*)cjObj::cjObjFind(cjObjPackage::cjObjGetStaticCls(), 0, clsPkgName);
				cjObjCls* cls = cjObjFindObject<cjObjCls>(pkg, clsName);
				if(cls)
				{
					loadObj = cjObj::cjObjFind(cls, 0, objName);

					if(!loadObj)
						loadObj = cjObj::_cjObjLoadPkgObj(cls, 0, objName, 0, 0);

					if(!loadObj)
					{						
						cjLOG(_CT("[Warn!] failed to load object (%s)"), objName);
						cjAssert(!"failed to load object");
					}
				}
			}
			
			m_serCJObj(i) = loadObj;

			cjSerializer::RecycleMFree((void*)clsPkgName, this);
			cjSerializer::RecycleMFree((void*)objName, this);
			cjSerializer::RecycleMFree((void*)clsName, this);
		}
	}
	

	//
	// cjSerObj
	//

	cjINT32 numRttiStr = 0;
	*this << cjFStreamCmprI32(numRttiStr);
	m_serSerObjRttiStr.RemoveAll(numRttiStr, cjTrue);
	m_serRtti.RemoveAll(numRttiStr, cjTrue);

	for(cjINT32 i=0; i<numRttiStr; i++)
	{
		const char* name = 0;

		// cjSerializer::MAlloc for free memory pool
		cjSerializeStrChar(*this, &name, 0, &cjSerializer::RecycleMAlloc, &cjSerializer::RecycleMFree);
		const cjRTTI* rtti = cjRTTI::FindByNameA(name);

		if(rtti)
		{
			m_serRtti(i) = rtti;
		}
		else
		{
			cjLOGE("ERR! - Invalid rtti name = %s", name);			
			cjAssert(!"invalid rtti");
		}

		cjSerializer::RecycleMFree((void*)name, this); // recycled 
	}

	cjINT32 numStr = 0;
	*this << cjFStreamCmprI32(numStr);
	
	for(cjINT32 i=0; i<numStr; i++)
		*this << m_serStr(i);
	

	// read object
	cjINT32 numObj = 0;
	*this << cjFStreamCmprI32(numObj);
	m_serSerObj.RemoveAll(numObj, cjTrue);
	m_serSerObjHitNum.RemoveAll();
	m_serSerObjHitNum.PushZero(numObj);

	// cjdefer : no need to load all object
	// pre create all object
	for(cjAryOff i=0; i<numObj; i++)
	{		
		cjINT32 rttiIdx;
		(*this) << cjFStreamCmprI32(rttiIdx);

		const cjRTTI* rtti = m_serRtti(rttiIdx);
		cjSerializable* inst = (cjSerializable*)rtti->NewInst();
		m_serSerObj(i) = inst;

		cjRef* ref = inst->GetInstAScjRef();
		if(ref) 
			ref->AddRef();
	}

	// serialize all object
	for(cjAryOff i=0; i<m_serSerObj.Num(); i++)
	{		
		cjSerializable* inst = m_serSerObj(i);
		(*inst) >> *this;				
	}

	// post serialize
	for(cjAryOff i=0; i<m_serSerObj.Num(); i++)
	{		
		cjSerializable* inst = m_serSerObj(i);
		(*inst).PostSerialize(*this);
	}
}

void cjSerializer::PostReadHeader()
{
	for(cjAryOff i=0; i<m_serSerObj.Num(); i++)
	{
		cjSerializable* ser = m_serSerObj(i);
		cjRef* ref = ser->GetInstAScjRef();

		if(ref) 
		{
			if(ref->GetRef() < 2)
			{
				const cjNameChar* nameW = ser->cjrRTTI()->cjrGetNameW();

				/*
						case #0
						... 
						if(ser.IsQuerySerObjAndStr())
						{
							ser << m_aniNotePtr;
						}
						else if(ser.IsSaving() || ser.IsLoading())
						{
							ser << m_aniNotePtr;   << it occurs problem that wrting same object twice
													fix) ser.SerializeSerObjPtr(&m_aniNotePtr);
						}
				*/

				/*
					#0. ref : 0->1 , in cjSerializer::ReadHeader()

							const cjRTTI* rtti = m_serRtti(rttiIdx);
							cjSerializable* inst = (cjSerializable*)rtti->NewInst();  // ref 0
							m_serSerObj(i) = inst;

							cjRef* ref = inst->GetInstAScjRef();
							if(ref) 
								ref->AddRef();   // ref 0->1

					#1. ref : 1->2 , TRefPtr in instance, loading on serialize


				*/

				cjLOG(_CT("ERR! - may be unreferenced object is saved..! ord[%d] rtti[%s] "), i, nameW);
				//cjAssert(!"ERR! - serialize failed..");

				if(ref->GetRef() == 1)
					ref->ReleaseRef(); // dispose dangling object
			}
			else
			{
				ref->ReleaseRef();
			}

			m_serSerObj(i) = 0;
		}
	}

	for(cjAryOff i=0; i<m_serSerObjHitNum.Num(); i++)
	{
		if(m_serSerObjHitNum(i) == 0) // unreferenced
		{			
			cjSerializable* ser = m_serSerObj(i);

			if(!ser)
				continue;

			cjRef* ref = ser->GetInstAScjRef();

			if(ref)
				ref->ReleaseRef();
			else
				ser->DelInst();
		}
	}
}

void cjSerializer::FlushFreeMem()
{
	for(cjINT32 i=0; i<NUM_MAX_RECYLE_MEM; i++)
	{
		if(!m_freeMem[i])
			continue;

		cjMemDeAlloc(m_freeMem[i]);
	}

	cjMemZero(m_freeMem, sizeof(m_freeMem));
	cjMemZero(m_freeMemSiz, sizeof(m_freeMemSiz));
}

void cjSerializer::AddPreLoadedCJObj(cjObj* obj)
{
	if(!obj)
		return;

	cjNameChar name[cjMAX_PATH]; 
	obj->cjObjGetPathName(name, cjMAX_PATH);

	cjAssert((cjStrlen(name) != 0));

	cjString _nameStr = name;
	m_cjObjPreLoaded.Insert(_nameStr, obj);
}

void cjSerializer::ResetPreloadedCJObj()
{
	m_cjObjPreLoaded.RemoveAll();
}

void* cjSerializer::RecycleMAlloc(cjSiz_t siz, void* owner)
{
	cjSerializer* o = (cjSerializer*)owner;

	if(!o || o->m_numFreeMem == 0)
		return cjMemAlloc(siz);

	for(cjINT32 i=0; i<NUM_MAX_RECYLE_MEM; i++)
	{
		if(!o->m_freeMem[i])
			continue;

		if(o->m_freeMemSiz[i] < siz)
			continue;
					
		o->m_freeMemSiz[i] = 0;
		void* r = o->m_freeMem[i];
		o->m_freeMem[i] = 0;
		o->m_numFreeMem--;
		o->m_lastFreeMem = i;

		cjAssert(o->m_numFreeMem >= 0);

		return r;
	}

	return cjMemAlloc(siz);
}

void cjSerializer::RecycleMFree(void* m, void* owner)
{
	cjSerializer* o = (cjSerializer*)owner;

	if(!o || o->m_numFreeMem >= NUM_MAX_RECYLE_MEM)
		cjMemDeAlloc(m);

	cjINT32 pos = o->m_lastFreeMem;
	if(o->m_freeMem[pos])
	{
		pos = -1;
		for(cjINT32 i=0; i<NUM_MAX_RECYLE_MEM; i++)
		{
			if(!o->m_freeMem[i])
			{
				pos = i;
				break;
			}
		}

		if(pos == -1)
		{
			// must be not occur..
			cjAssert(!"Warn! - invalid freeMemory management state..");
			cjMemDeAlloc(m);
			return;
		}
	}
		
	o->m_freeMemSiz[pos] = CJMEM_GETSIZ(m);
	o->m_freeMem[pos] = m;

	o->m_numFreeMem++;
}


//
// cjSerObjStreamBroker
//

cjBool cjSerObjStreamBroker::Flush()
{
	if(m_dstStream)
		m_dstStream->Flush();

	return cjTrue;
}

cjSerObjStreamBroker::cjSerObjStreamBroker(cjFStream* dstSteram)
	: m_dstStream(dstSteram)
{
	if(dstSteram)
	{		
		if(dstSteram->GetSerializerBroker())
		{
			cjAssert(!"ERR! - given dstStream is derived from cjSerObjStreamBroker..");

			m_dstStream = cjNULL;
			return;
		}

//		SetObjOnly(dstSteram->IsObjOnly());
		SetLoading(dstSteram->IsLoading());
		SetSaving(dstSteram->IsSaving());
		SetPersist(dstSteram->IsPersist());
	}
}

cjSerObjStreamBroker::~cjSerObjStreamBroker()
{
}

cjFStream& cjSerObjStreamBroker::operator<< (cjName& name)
{
	if(m_dstStream->IsObjOnly())
		return *this;

	//todo: make string table, for remove dupplicated string
	name >> *this;
	return *this;
}

cjStreamOffset cjSerObjStreamBroker::SerializeStream(void* v, cjStreamOffset siz)
{
	if(IsQuerySerObjAndStr())
		return 0;

	if(m_dstStream->IsObjOnly())
		return 0;

	return m_dstStream->SerializeStream(v, siz);
}

cjBool cjSerObjStreamBroker::Serialize(cjSerializable** serObj)
{
	if(!m_dstStream)
		return cjFalse;

	if(m_dstStream->IsObjOnly())
		return cjTrue;

	//cjopti - need to skip at this when called form cjFStreamMarkExportTag::opearator<<
	Reset();

	if(m_dstStream->IsLoading())
	{
		cjRef* ref = cjNULL;

		if((*serObj))
			ref = (*serObj)->GetInstAScjRef();

		if(ref)
			CJ_REF_SAFE_RELEASE(ref);

		SetLoading(cjTrue);

		ReadHeader();

		*serObj = m_serSerObj(0);
		
		ref = (*serObj) ? (*serObj)->GetInstAScjRef() : cjNULL;

		if(ref)
			ref->AddRef();
		
//		(**node) >> *this;
		PostReadHeader();
	}
	else if(m_dstStream->IsSaving())
	{		
		// query information to obj to save
		SetQuerySerObjAndStr(cjTrue);

		if(*serObj) // collect cjSerializeble object
			**serObj >> *this;

		//(**node) >> *this; // just querying
		SetQuerySerObjAndStr(cjFalse);
		SetSaving(cjTrue);

		PostQuery();

		WriteHeader();
//		(**node) >> *this;

		m_dstStream->Flush(); // save
	}

	return cjTrue;
}

//
// cjSerializable
//

CJSER_IMPL_CLASS_ROOT(cjSerializable)

cjSerializable::~cjSerializable()
{
}

void cjSerializable::operator >> (cjSerializer& ser) 
{ 
	if(ser.IsQuerySerObjAndStr())
	{
		ser.AddSerObj(this);
		
	//	cjNode* n = (cjNode*)this;

	//	cjNameChar buf[cjMAX_PATH];
	//	cjSprintf(buf, cjMAX_PATH, _CT("[cjSer] %p - %s \n"), this, *(n->GetName()));
	//	OutputDebugStringW( *n->GetName());
	//	OutputDebugStringW(buf);

		// processed in cjSerializer::FinishQueryObjStr
		// ser.AddSerString(cjrRTTI()->cjrGetNameW())
	}
	else
	{
		cjTFLAG serFlag = 0;
	
		if(ser.IsSaving())
			serFlag = (_m_serFlag & CJRTTI_CF_MASK_SERIALIZE) >> 4;
		
		ser << cjFStreamCmprI32(serFlag);

		if(ser.IsLoading())
			_m_serFlag = (_m_serFlag & (0x0000ffff)) | (serFlag << 4);
	}
}

//cjSerObjCloner& operator <<(cjSerObjCloner& cloner, cjSerializable* d)
//{
//	d->_m_serFlag = cloner.GetSrc<cjSerializable>()._m_serFlag;
//	return cloner;
//}

cjSerID cjSerializable::GenerateSerObjID(cjSerializable* obj, cjSerOrdinal ord)
{
	cjSerID serID = cjSerID_MakeOrdinal(ord);

	if(obj->_m_serFlag & CJSER_FL_EXTERN)
		cjSerID_SetExtern(serID, cjTrue);

	return serID;
}

cjSerID cjSerializable::GenerateCJObjID(cjObj* obj, cjSerOrdinal ord)
{
	cjSerID serID = cjSerID_MakeOrdinal(ord);
	cjSerID_SetExtern(serID, cjTrue);

	return serID;
}


//
// cjSerObjCloner
//

const cjBool cjSerObjCloner::Cloned = cjTrue;

cjSerObjCloner::cjSerObjCloner(cjPtrHasherValType bucketSiz)
{
	m_clones		= new cjTPtrHash<cjSerializable*, cjSerializable*>(bucketSiz);
	m_postCloned	= new cjTPtrHash<cjSerializable*, cjBool>(bucketSiz);
}

cjSerObjCloner::~cjSerObjCloner()
{
	if(m_clones)
		delete m_clones;

	if(m_postCloned)
		delete m_postCloned;
}

cjSerializable*	cjSerObjCloner::GetCloned(cjSerializable* serObj) 
{ 
	if(!serObj)
		return cjNULL;

	cjSerializable* rst;

	if(m_clones->GetValue(serObj, rst))
		return rst;

	return cjNULL;
}

cjRef* cjSerObjCloner::GetClonedAsRef(cjSerializable* serObj) 
{ 
	if(!serObj)
		return cjNULL;

	cjSerializable* rst;
	
	if(m_clones->GetValue(serObj, rst))
		return rst->GetInstAScjRef();

	return cjNULL;
}

cjRefObj* cjSerObjCloner::GetClonedAsRefObj(cjSerializable* serObj) 
{ 
	if(!serObj)
		return cjNULL;

	cjSerializable* rst;
	
	if(m_clones->GetValue(serObj, rst))
		return rst->GetInstAsRefObj();

	return cjNULL;
}

void cjSerObjCloner::SetCloned(cjSerializable* src, cjSerializable* dst) 
{
	cjAssert(src);
	cjAssert(dst);

	if(!src || !dst)
		return;

	m_clones->Set(src, dst);
}

cjBool cjSerObjCloner::IsPostCloned(cjSerializable* serObj)
{
	if(!serObj)
		return cjFalse;

	cjBool rst;

	if(m_postCloned->GetValue(serObj, rst))
		return rst;

	return cjFalse;
}

void cjSerObjCloner::SetPostCloned(cjSerializable* serObj)
{
	if(!serObj)
		return;

	m_postCloned->Set(serObj, Cloned);
}
