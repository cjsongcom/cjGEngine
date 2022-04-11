
#include "cjCorePCH.h"


//
// cjPropHdrSerialzer
//

cjPropHdrSerialzer::cjPropHdrSerialzer() 
	: m_aryIndex(cjIndex_None) 
{
}

cjPropHdrSerialzer::cjPropHdrSerialzer(cjFStream& stream, cjProperty* prop, 
	cjINT32 aryIndex, cjBYTE* v)
	:	m_propType(prop->GetPropertyType()), 
		m_propName(prop->GetName()),
		m_propSiz(0), 
		m_aryIndex(aryIndex),
		m_propSerFlag(prop->GetPropertyType())
{
	cjPropertySizCounter propSizCounter(stream);
	Serialize(propSizCounter, prop, v, 0);

	m_propSiz = propSizCounter.GetPropSiz();
	
	/* 
		m_propSerFlag
		8421 8421
		    [0~3bit property type]
			1= cjName_ByteProperty  // match order in predefined cjName
			2= cjName_IntProperty 
			3= cjName_BoolProperty
			4= cjName_FloatProperty
			5= cjName_ObjProperty
			6= cjName_NameProperty
			7= cjName_StringProperty 
			8= cjName_ClassProperty(cjObjCls)
			9= cjName_AryProperty
			10= cjName_HashProperty
			11= cjName_VectorProperty
			12= cjName_RotateProperty
			13= cjName_cjSerObjRefProperty		// cjSerialzeablePtr
			14= 
			15=
	   
		[4~6bit : property siz : 1,2,4,12,16,byte,word,int
			0 = 1byte / 1 = 2byte / 2 = 4byte / 3 = 12byte / 4 = 16byte
			5 = 0x50 + real size byte
			6 = 0x60 + real size word
			7 = 0x70 + real size int

		[7bit :  (0x80 && type != cjBoolProperty(1)) => array flag & ary index (1,2,3,4 byte array index) ]
		[7bit :  (0x00 or type == cjBoolProperty(1)) => none array (aryindex=0)
	*/
	m_propSerFlag |= 
		(m_propSiz==1 ? 0x00 : 
			m_propSiz==2 ? 0x10 : 
				m_propSiz==4 ? 0x20 : 
					m_propSiz==12 ? 0x30 : 
						m_propSiz==16 ? 0x40 : 
							m_propSiz <= 255 ? 0x50 : // 0xff=255
								m_propSiz <= 65536 ? 0x60 : 0x70 ); // 0x10000=65536,  0xffff=65535
		
	// bool property (1bit not a 1byte)
	cjBoolProperty* boolProp = prop->GetBoolProperty();

	if(aryIndex || (boolProp && (*(cjBITFIELD*)v & boolProp->m_bitMask)))
		m_propSerFlag |= 0x80;
}

cjFStream& operator<< (cjFStream& stream, cjPropHdrSerialzer& propHdrSerializer)
{
	cjBYTE sizBytes;
	cjWORD sizWord;
	cjINT32 sizInt;

	stream << propHdrSerializer.m_propName;

	if(propHdrSerializer.m_propName == cjName_None)
		return stream;

	// write property type[0~3bit] + siz[4~6bit] + array flag[7bit]
	stream << propHdrSerializer.m_propSerFlag;
	
	// property type
	propHdrSerializer.m_propType = propHdrSerializer.m_propSerFlag & 0x0f;

	// property size
	switch(propHdrSerializer.m_propSerFlag & 0x70)
	{
	case 0x00: propHdrSerializer.m_propSiz = 1;	break;
	case 0x10: propHdrSerializer.m_propSiz = 2; break;
	case 0x20: propHdrSerializer.m_propSiz = 4; break;
	case 0x30: propHdrSerializer.m_propSiz = 12; break;
	case 0x40: propHdrSerializer.m_propSiz = 16; break;

	case 0x50:  sizBytes = (cjBYTE)propHdrSerializer.m_propSiz;  
				stream << sizBytes; // writing byte siz
				propHdrSerializer.m_propSiz = sizBytes;
				break;

	case 0x60:	sizWord = (cjWORD)propHdrSerializer.m_propSiz;	
				stream << sizWord; // writing word siz
				propHdrSerializer.m_propSiz = sizWord;
				break;

	case 0x70:  sizInt = propHdrSerializer.m_propSiz;
				stream << sizInt; // writing int siz
				propHdrSerializer.m_propSiz = sizInt;
				break;
	}

	// calculate array property index
	if( (propHdrSerializer.m_propSerFlag & 0x80) && 
		(propHdrSerializer.m_propType != cjName_cjBoolProperty))
	{
		// for writing
		// 0x80 = 2 byte array index flag
		// 0xc0 = 4 byte array index flag
		cjBYTE a = (cjBYTE)( (propHdrSerializer.m_aryIndex <= 127) ? (propHdrSerializer.m_aryIndex) : 
								(propHdrSerializer.m_aryIndex <= 16383) ? (propHdrSerializer.m_aryIndex >> 8) + 0x80 : 
																			(propHdrSerializer.m_aryIndex >> 24) + 0xC0 );
		stream << a;		

		if((a & 0x80)==0)
		{
			// ary range is 0~254 , 1byte
			propHdrSerializer.m_aryIndex = a;	
		}
		else if( (a & 0xc0) == 0x80 ) 				
		{
			// array range (0~32767)[0111 1111 1111 1111]
			// 0x7f (0111 1111)
			//	    0xff
				
			cjBYTE b = propHdrSerializer.m_aryIndex & 255;  // for writing, ( 2 byte array index flag(0x80) )

			stream << b;
								
			propHdrSerializer.m_aryIndex = ((cjINT32)(a & 0x7F) << 8) + ((cjINT32)b); // for loading
		}
		else
		{				
			cjBYTE b = (cjBYTE)(propHdrSerializer.m_aryIndex >> 16);
			cjBYTE c = (cjBYTE)(propHdrSerializer.m_aryIndex >> 8);
			cjBYTE d = (cjBYTE)propHdrSerializer.m_aryIndex;

			stream << b << c << d;

			/*
				array range (0~1073741823)
				0011 1111 1111 1111 1111 1111 1111 1111
				0x3f
					0xff
						0xff
							0xff
			*/

			propHdrSerializer.m_aryIndex = 
				((cjINT32)(a & 0x3F) << 24) + 
				((cjINT32) b         << 16) + 
				((cjINT32) c         <<  8) + ((cjINT32)d);
		}
	}
	else
	{
		propHdrSerializer.m_aryIndex = 0;
	}

	return stream;
}

void cjPropHdrSerialzer::Serialize(cjFStream& stream, cjProperty* prop, 
	cjBYTE* v, cjINT32 maxReadBytes)
{
	cjBoolProperty* boolProp = prop->GetBoolProperty();

	if(boolProp)
	{
		cjAssert(boolProp->m_bitMask !=0);

		if(stream.IsLoading())
		{
			if(m_propSerFlag & 0x80)
				(*(cjBITFIELD*)v) |=  boolProp->m_bitMask;
			else
				(*(cjBITFIELD*)v) &= ~boolProp->m_bitMask;
		}
	}
	else
	{
		prop->SerializeItem(stream, v, maxReadBytes);
	}
}


//
// cjProperty
//

CJOBJ_IMPL_CLASS(cjProperty);

CJOBJ_IMPL_SERIALIZE_BEGIN(cjProperty)

	stream << m_aryNum << m_propFlag << m_cateName;

	if(stream.IsLoading())
	{
		m_nativeOffset = 0;
		m_nextConstructorProp = cjNULL;
	}

CJOBJ_IMPL_SERIALIZE_END(cjProperty)


cjProperty::cjProperty()
	: cjElement(cjNULL)
{
    m_aryNum = 1;

	/*
    m_nextProp = cjNULL;
	m_nextConstructorProp = cjNULL;
	m_nextConfigProp = cjNULL;
	*/

	m_nextRefProp = cjNULL;

	/*
	m_elemSiz = 0;
	m_nativeOffset = 0;
	m_propFlag = 0;	
	*/	
}

cjProperty::cjProperty(cjObjEPropertyTag, cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag)
	: cjElement(cjNULL)
{
    m_aryNum = 1;
	m_propFlag = propFlag;
    m_cateName = cateName;
    m_nativeOffset = nativeOffset;
	m_nextRefProp = cjNULL;
    
    GetPropStruct()->AddNativeProperty(this);
}

cjBYTE cjProperty::GetPropertyType() const
{
	if(!CheckPropertyNames())
	{
		cjAssert(!"[ERR!] property clsss name and predefined cjName index for one is different");
		return 0xff;
	}

	return (cjBYTE)GetCls()->GetName().GetIndex();
}

cjBool cjProperty::CheckPropertyNames()
{
	static cjBool checked = 0;
	static cjBool rst = cjFalse;

	if(checked)
		return rst;

	checked = cjTrue;

	//cjdefer
	if( 
	//	(cjName_cjByteProperty		!= cjByteProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjIntProperty		!= cjIntProperty::cjObjGetStaticCls()->GetName().GetIndex())  ||
		(cjName_cjBoolProperty		!= cjBoolProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
	//	(cjName_cjFloatProperty		!= cjFloatProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjObjProperty		!= cjObjProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjNameProperty		!= cjNameProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjStrProperty		!= cjStrProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjObjClsProperty	!= cjObjClsProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjAryProperty		!= cjAryProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
	//	(cjName_cjHashProperty		!= cjHashProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
	//	(cjName_cjVecProperty		!= cjVecProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
	//	(cjName_cjRotProperty		!= cjRotProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
		(cjName_cjSerObjRefProperty	!= cjSerObjRefProperty::cjObjGetStaticCls()->GetName().GetIndex()) ||
	//	(cjName_ReservedProperty1	!= cjReservedProperty1::cjObjGetStaticCls()->GetName().GetIndex()) || //fixedary
	//	(cjName_ReservedProperty2	!= cjReservedProperty2::cjObjGetStaticCls()->GetName().GetIndex()) ||
		0
	  )
	{		
		rst = cjFalse;
		return cjFalse;
	}

	rst = cjTrue;
	return cjTrue;
}

//
// cjBoolProperty
//

CJOBJ_IMPL_CLASS(cjBoolProperty)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjBoolProperty)
	if(!stream.IsLoading() && !stream.IsSaving())
		stream << m_bitMask;
CJOBJ_IMPL_SERIALIZE_END(cjBoolProperty)

void cjBoolProperty::Link(cjFStream& stream, cjProperty* prev)
{
	cjSuper::Link(stream, prev);

	cjBoolProperty* prevBool = prev->GetBoolProperty();

	if(GetPropStruct()->MergeBools() && prevBool && CJ_NEXT_BITFIELD(prevBool->m_bitMask))
	{
		m_nativeOffset = prev->m_nativeOffset;
		m_bitMask = CJ_NEXT_BITFIELD(prevBool->m_bitMask);
	}
	else
	{
		m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertySiz(), sizeof(cjBITFIELD));
		m_bitMask = CJ_FIRST_BITFIELD;
	}

	m_elemSiz = sizeof(cjBITFIELD);
}

void cjBoolProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes) const
{
	cjBYTE b = (*(cjBITFIELD*)v & m_bitMask) ? 1 : 0;
	stream << b;

	if( b ) *(cjBITFIELD*)v |=  m_bitMask;
	else    *(cjBITFIELD*)v &= ~m_bitMask;
}

cjBool cjBoolProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	return ((*(cjBITFIELD*)src ^ (dst ? *(cjBITFIELD*)dst : 0)) & m_bitMask) == 0;
}

void cjBoolProperty::CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj) const
{
	*(cjBITFIELD*)dst = (*(cjBITFIELD*)dst & ~m_bitMask) | (*(cjBITFIELD*)src & m_bitMask);
}

void cjBoolProperty::ClearValue(cjBYTE* dst) const
{
	*(cjBITFIELD*)dst &= ~m_bitMask;
}

cjBool cjBoolProperty::HasValue(const cjBYTE* dst) const
{
	return (*(cjBITFIELD*)dst & m_bitMask) == 0;
}


//
// cjIntProperty
//

CJOBJ_IMPL_CLASS(cjIntProperty);

void cjIntProperty::Link(cjFStream& stream, cjProperty* prev)
{
    cjSuper::Link(stream, prev);
    cjPropStruct* outerPropStruct = cjObjCast<cjPropStruct>(GetOuter());
    
    m_elemSiz = sizeof(cjINT32);
    
    /*
        previous m_nativeOffset is used in 'cjPropStruct::Link(..)'        
        prevPropNativeOffset = cjProperty->m_nativeOffset;
        cjProperty->Link(..);
        cjProperty->m_nativeOffset = PrevPropNativeOffset; <<
    */    
	// GetPropertySiz() -> lastest property size
    m_nativeOffset = cjAlignAddr(outerPropStruct->GetPropertySiz(), sizeof(cjINT32)); 
}

void cjIntProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 /*maxReadBytes*/) const
{
	stream << *(cjINT32*)v;
}

cjBool cjIntProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	return *(cjINT32*)src == (dst ? *(cjINT32*)dst : 0);
}

void cjIntProperty::CopyValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
	*(cjINT32*)dst = *(cjINT32*)src;
}

void cjIntProperty::CopyAllValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
	if(m_aryNum == 1)
    {
		*(cjINT32*)dst = *(cjINT32*)src;
    }
	else
    {
		for(cjINT32 i=0; i<m_aryNum; i++)
			((cjINT32*)dst)[i] = ((cjINT32*)src)[i];
    }
}

void cjIntProperty::ClearValue(cjBYTE* dst) const
{
    *(cjINT32*)dst = 0;
}

cjBool cjIntProperty::HasValue(const cjBYTE* dst) const
{
	return *(cjINT32*)dst != 0;
}


//
// cjStrProperty
//

CJOBJ_IMPL_CLASS(cjStrProperty);

//CJOBJ_IMPL_SERIALIZE_BEGIN(cjStrProperty)
//CJOBJ_IMPL_SERIALIZE_END(cjStrProperty)


void cjStrProperty::Link(cjFStream& stream, cjProperty* prev)
{
	cjSuper::Link(stream, prev);

	m_elemSiz = sizeof(cjString);
	m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertySiz(), cjObj_ClsPropertySizAlignByte);
	
	if((m_propFlag & cjPropF_Export))
		m_propFlag |= cjPropF_NeedConstructorLink;
}

void cjStrProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes) const
{
	stream << *(cjString*)v;
}

cjBool cjStrProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	return cjStrcmp(**(const cjString*)src, dst ? **(const cjString*)dst : _CT("")) == 0;
}

void cjStrProperty::ClearValue(cjBYTE* dst) const
{
	(*(cjString*)dst).Clear();
}

cjBool cjStrProperty::HasValue(const cjBYTE* dst) const
{
	return (*(cjString*)dst).Len() > 0;
}

void cjStrProperty::DestroyValue(void* dst) const
{
    if(m_nativeOffset == 0 && m_elemSiz == 0)
    {
		cjAssert(!"invalid function call, cjStrProperty::DestroyValue.");
        return;
    }

	for(cjINT32 i=0; i<m_aryNum; i++)
		(*(cjString*)((cjBYTE*)dst + i*m_elemSiz)).~cjString();
}

void cjStrProperty::CopyValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
	*(cjString*)dst = *(cjString*)src;
}


//
// cjObjProperty
//

CJOBJ_IMPL_CLASS(cjObjProperty);

CJOBJ_IMPL_SERIALIZE_BEGIN(cjObjProperty)
	stream << m_propCls;
CJOBJ_IMPL_SERIALIZE_END(cjObjProperty)

void cjObjProperty::Link(cjFStream& stream, cjProperty* prev)
{
    cjSuper::Link(stream, prev);
    
    m_elemSiz = sizeof(cjObj*);
    m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertySiz(), cjObj_ClsPropertySizAlignByte);
    
	if((m_propFlag & cjPropF_Export))
		m_propFlag |= cjPropF_NeedConstructorLink;
}

void cjObjProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes) const
{
	stream << *(cjObj**)v;
}

cjBool cjObjProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	return *(cjObj**)src == (dst ? *(cjObj**)dst : cjNULL);
}

void cjObjProperty::CopyValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
    // pointer copy
    *(cjObj**)dst = *(cjObj**)src;
}

void cjObjProperty::CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj) const
{
	if(isInstanceSubObj && (m_propFlag & cjPropF_NeedConstructorLink) && superObj)
	{
		for(cjINT32 i=0; i<m_aryNum; i++)
		{
			if(((cjObj**)src)[i])
			{
				cjObjCls* cls = ((cjObj**)src)[i]->GetCls();
                cjObj* outerObj= superObj->GetOuter();
                
				((cjObj**)dst)[i] = __cjObjAlloc(cls, outerObj, cjName_None, 0, 0, ((cjObj**)src)[i], 0, superObj);
			}
			else
            {
				((cjObj**)dst)[i] = ((cjObj**)src)[i];
            }
		}
	}
	else
	{
		for(cjINT32 i=0; i<m_aryNum; i++)
        {
			((cjObj**)dst)[i] = ((cjObj**)src)[i];
        }
	}
}

void cjObjProperty::ClearValue(cjBYTE* dst) const
{
	*(cjObj**)dst = cjNULL;
}

cjBool cjObjProperty::HasValue(const cjBYTE* dst) const
{
	return *(cjObj**)dst != cjNULL;
}

void cjObjProperty::RemoveRefForPendingKillObj(cjBYTE* dst) const
{
	for(cjINT32 i=0; i<m_aryNum; i++)
	{
		cjObj*& obj=*(cjObj**)(dst + i*m_elemSiz);
        
		if(obj)
		{
			obj = cjNULL; // for garbage collection
		}
	}
}


//
// cjObjClsProperty
//

CJOBJ_IMPL_CLASS(cjObjClsProperty)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjObjClsProperty)
	stream << m_metaCls;
CJOBJ_IMPL_SERIALIZE_END(cjObjClsProperty)

cjObjClsProperty::cjObjClsProperty(cjObjEPropertyTag, cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag, cjObjCls* cls)
	: cjObjProperty(cjObjEProperty, nativeOffset, cateName, propFlag, cjObjCls::cjObjGetStaticCls()), m_metaCls(cls) 
{
}

void cjObjClsProperty::ClearValue(cjBYTE* dst) const
{
	*(cjObjCls**)dst = cjNULL;
}

cjBool cjObjClsProperty::HasValue(const cjBYTE* dst) const
{
	return *(cjObjCls**)dst != cjNULL;
}


//
// cjNameProperty
//

CJOBJ_IMPL_CLASS(cjNameProperty);


void cjNameProperty::Link(cjFStream& stream, cjProperty* prev)
{
	cjSuper::Link(stream, prev);

	m_elemSiz = sizeof(cjName);
	m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertySiz(), cjObj_ClsPropertySizAlignByte);
//	m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertiesSize(), sizeof(cjName));
}

void cjNameProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes) const
{
	stream << *(cjName*)v;
}

cjBool cjNameProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	return *(cjName*)src == (dst ? *(cjName*)dst : cjName(cjName_None));
}

void cjNameProperty::ClearValue(cjBYTE* dat) const
{
	*(cjName*)dat = cjName_None;
}

cjBool cjNameProperty::HasValue(const cjBYTE* dat) const
{
	return *(cjName*)dat != cjName_None;
}

void cjNameProperty::CopyValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
	*(cjName*)dst = *(cjName*)src;
}

void cjNameProperty::CopyAllValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
	if(m_aryNum == 1)
	{
		*(cjName*)dst = *(cjName*)src;
	}
	else
	{
		for(cjAryOff i=0; i<m_aryNum; i++ )
			((cjName*)dst)[i] = ((cjName*)src)[i];
	}
}


//
// cjAryProperty
//

CJOBJ_IMPL_CLASS(cjAryProperty);

CJOBJ_IMPL_SERIALIZE_BEGIN(cjAryProperty)

	/*
	   array item type, cjObjCls
	   save : save current element count
	   register object index in cjPkgLoad,cjPkgSave 
	*/

	stream << m_aryItem;

CJOBJ_IMPL_SERIALIZE_END(cjAryProperty)


void cjAryProperty::Link(cjFStream& stream, cjProperty* prev)
{
    cjSuper::Link(stream, prev);

	stream.Preload(m_aryItem);
	    
    m_aryItem->Link(stream, cjNULL);
    m_elemSiz = sizeof(cjAry);
    
    m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertySiz(), cjObj_ClsPropertySizAlignByte);

	// need calling constructor for property that is not created from native
	if(!(m_propFlag & cjPropF_Native))
		m_propFlag |= cjPropF_NeedConstructorLink;
}

void cjAryProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes) const
{
	cjAssert(m_aryItem);

	cjINT32 elemSiz = m_aryItem->m_elemSiz;
	cjINT32 count = ((cjAry*)v)->Count();

	stream << cjFStreamCmprI32(count);

	if(stream.IsLoading())
	{
		((cjAry*)v)->RemoveAll(elemSiz);
		((cjAry*)v)->AddZeroed(elemSiz, count);
	}

	cjBYTE* s = (cjBYTE*)((cjAry*)v)->GetData();

	for(cjINT32 i=0; i<count; i++)
		m_aryItem->SerializeItem(stream, s+i*elemSiz, (maxReadBytes > 0) ? maxReadBytes/count : 0 );
}

cjBool cjAryProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	cjINT32 count = ((cjAry*)src)->Count();

	if(count != (dst ? ((cjAry*)dst)->Count() : 0))
		return cjFalse;

	cjINT32 elemSiz = m_aryItem->m_elemSiz;
	cjBYTE* s = (cjBYTE*)((cjAry*)src)->GetData();
	
	if(dst)
	{
		cjBYTE* d = (cjBYTE*)((cjAry*)dst)->GetData();

		for(cjINT32 i=0; i<count; i++)
		{
			if(!m_aryItem->IsIdenticalValue(s+i*elemSiz, d+i*elemSiz) )
				return cjFalse;
		}
	}
	else
	{
		for(cjINT32 i=0; i<count; i++)
		{
			if(!m_aryItem->IsIdenticalValue(s+i*elemSiz, 0))
				return cjFalse;
		}
	}

	return cjTrue;
}

void cjAryProperty::ClearValue(cjBYTE* dst) const
{
	(*(cjAry*)dst).RemoveAll(m_elemSiz);
}

cjBool cjAryProperty::HasValue(const cjBYTE* dst) const
{
	return (*(cjAry*)dst).Num() != 0;
}

void cjAryProperty::DestroyValue(void* dst) const
{
    if( (m_nativeOffset == 0) && GetPropStruct()->cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()) )
    {
        cjAssert(!"ERR! - this property is corrupted..");
        return;
    }

	cjAry* dstAry = (cjAry*)dst;
	
    if(m_aryItem->m_propFlag & cjPropF_NeedConstructorLink)
	{
		cjBYTE* dstData = (cjBYTE*)dstAry->GetData();
		cjINT32 siz = m_aryItem->m_elemSiz;
        
		for(cjAryOff i=0; i<dstAry->Num(); i++)
			m_aryItem->DestroyValue(dstData + i*siz);
	}
	
    dstAry->~cjAry();
}

void cjAryProperty::CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj) const
{
    cjAry* srcAry = (cjAry*)src;
    cjAry* dstAry = (cjAry*)dst;    
    cjINT32 elemSiz = m_aryItem->m_elemSiz;
    
    if(m_aryItem->m_propFlag & cjPropF_NeedConstructorLink)
        DestroyValue(dst);
    else
        dstAry->RemoveAll(elemSiz, srcAry->Num());
    
    if(dstAry->Num() == 0)
        return;

    if(m_aryItem->m_propFlag & cjPropF_NeedConstructorLink)
    {
        dstAry->AddZeroed(elemSiz, srcAry->Num());
        
        cjBYTE* srcDat = (cjBYTE*)srcAry->GetData();
        cjBYTE* dstDat = (cjBYTE*)dstAry->GetData();
        
        for(cjAryOff i=0; i<dstAry->Num(); i++)
			m_aryItem->CopyAllValue(dstDat+i*elemSiz, srcDat+i*elemSiz, superObj, isInstanceSubObj);
    }
    else
    {
        dstAry->PushEmpty(srcAry->Num(), elemSiz);
		cjMemCpy(dstAry->GetData(), srcAry->GetData(), srcAry->Num()*elemSiz);
    }
}

void cjAryProperty::RemoveRefForPendingKillObj(cjBYTE* dst) const
{
	for(cjINT32 i=0; i<m_aryNum; i++)
	{
		cjAry* ary = (cjAry*)(dst + i*m_elemSiz);
        
		for(cjAryOff k=0; k<ary->Num(); k++)
			m_aryItem->RemoveRefForPendingKillObj((cjBYTE*)ary->GetData() + k*m_aryItem->GetSiz());
	}
}

void cjAryProperty::AddNativeProperty(cjProperty* prop)
{
	m_aryItem = prop;
}



//
// cjSerObjRefProperty
//

CJOBJ_IMPL_CLASS(cjSerObjRefProperty);

CJOBJ_IMPL_SERIALIZE_BEGIN(cjSerObjRefProperty)
	stream << m_propRTTI;
CJOBJ_IMPL_SERIALIZE_END(cjSerObjRefProperty)

void cjSerObjRefProperty::Link(cjFStream& stream, cjProperty* prev)
{
    cjSuper::Link(stream, prev);

    m_elemSiz = sizeof(cjRefPtr); // cjTRefPtr<cjRef> has one member variable(cjSerRefObj ptr)
    m_nativeOffset = cjAlignAddr(GetPropStruct()->GetPropertySiz(), cjObj_ClsPropertySizAlignByte);
    
//	if((m_propFlag & cjPropF_Export))
	m_propFlag |= cjPropF_NeedConstructorLink; // cjTRefRPtr need to execute constructor
}

void cjSerObjRefProperty::SerializeItem(cjFStream& stream, void* v, cjINT32 /*maxReadBytes*/) const
{	
	cjRefPtr& s = *((cjRefPtr*)v);

	cjSerObjStreamBroker ser(&stream);
	ser.Serialize(s);
}

cjBool cjSerObjRefProperty::IsIdenticalValue(const void* src, const void* dst) const
{
	cjRefPtr& s = *((cjRefPtr*)src);
	cjRefPtr& d = *((cjRefPtr*)dst);

	return s == d;
}

void cjSerObjRefProperty::CopyValue(void* dst, void* src, cjObj* /*superObj*/, cjBool /*isInstanceSubObj*/) const
{
	cjRefPtr& s = *((cjRefPtr*)src);
	cjRefPtr& d = *((cjRefPtr*)dst);

	s = d;
}

void cjSerObjRefProperty::CopyAllValue(void* dst, void* src, cjObj* /*superObj*/, cjBool isInstanceSubObj) const
{	
	if(isInstanceSubObj && (m_propFlag & cjPropF_NeedConstructorLink)) // && superObj)
	{
		for(cjINT32 i=0; i<m_aryNum; i++)
		{
			cjRefPtr& s = ((cjRefPtr*)src)[i];
			cjRefPtr& d = ((cjRefPtr*)dst)[i];

			if(s)
			{
				const cjRTTI* rtti = s->cjrRTTI();
				d = (cjRef*)(rtti->NewInst());
			}
			else
            {
				d = cjNULL;
            }
		}
	}
	else
	{
		for(cjINT32 i=0; i<m_aryNum; i++)
		{
			cjRefPtr& s = ((cjRefPtr*)src)[i];
			cjRefPtr& d = ((cjRefPtr*)dst)[i];
			
			d = s;
		}
	}
}

void cjSerObjRefProperty::ClearValue(cjBYTE* dst) const
{
	cjRefPtr& d = *((cjRefPtr*)dst);
	d = cjNULL;
}

cjBool cjSerObjRefProperty::HasValue(const cjBYTE* dst) const
{
	cjRefPtr& d = *((cjRefPtr*)dst);
	return d != cjNULL;
}

void cjSerObjRefProperty::DestroyValue(void* dst) const
{
 	for(cjINT32 i=0; i<m_aryNum; i++)
	{
		cjRefPtr& r = *((cjRefPtr*)((cjBYTE*)dst + i*m_elemSiz));
		r = cjNULL;
	}
}

void cjSerObjRefProperty::RemoveRefForPendingKillObj(cjBYTE* dst) const
{
	for(cjINT32 i=0; i<m_aryNum; i++)
	{
		cjRefPtr& r = *((cjRefPtr*)(dst + i*m_elemSiz));

		if(r)
			r = cjNULL;
	}
}

