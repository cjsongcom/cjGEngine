
#include "cjCorePCH.h"
#include "cjExtraDat.h"


//
// cjAniNote
//

CJ_IMPL_SERIALIZE_BEGIN(cjAniNote)
	stream << d.m_timeSec;
	stream << d.m_note;
CJ_IMPL_SERIALIZE_END(cjAniNote)


void cjAniNote::Serialize(cjSerializer& ser, cjAniNote** ary, cjINT32* _count)
{
	cjINT32 count = _count ? *_count: 0;
	
	ser << cjFStreamCmprI32(count);
	
	if(ser.IsLoading())
	{
		cjAssert(*ary == 0);

		if(count > 0)
		{
			*ary = cjAniNote::NewNoteAry(count);
		}
				
		if(_count) 
			*_count = count;
	}

	for(cjINT32 i=0; i<count; i++)
		ser << ((*ary)[i]);
}

cjAniNote* cjAniNote::NewNoteAry(cjINT32 numNote)
{
	if(numNote == 0)
		return 0;

	cjAniNote* ary = new cjAniNote[numNote];

//	cjAniNote* ary = (cjAniNote*)cjMemAlloc(sizeof(cjAniNote) * numNote);
//	cjMemZero(ary, sizeof(cjAniNote) * numNote);

	return ary;
}

void cjAniNote::DelNoteAry(cjAniNote* noteAry)
{
	if(!noteAry)
		return;

	delete [] noteAry;
//	cjMemDeAlloc(noteAry);
}

//cjAniNote* cjAniNote::NewNote(cjFLOAT timeSec, const cjNameChar* note)
//{
//	cjAniNote* noteInst = cjAniNote::NewInst();
//
//	noteInst->m_timeSec = timeSec;
//
//	if(note)
//		noteInst->m_note = note;
//
//	return noteInst;
//}


//
// cjNameMatch
//

CJSER_IMPL_CLASS(cjNameMatch, cjRef)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjNameMatch)

	d->m_lastMatched = m_lastMatched;
	d->m_name = m_name;

CJSER_IMPL_CLONE_PROPCOPY_END(cjNameMatch)

CJSER_IMPL_CLONE(cjNameMatch)


cjNameMatch::~cjNameMatch() 
{
}


//
// cjExtraDat
//

CJSER_IMPL_CLASS(cjExtraDat, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtraDat)
	ser << m_name;
CJSER_IMPL_SERIALIZE_END(cjExtraDat)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtraDat)
	d->m_name = m_name;
CJSER_IMPL_CLONE_PROPCOPY_END(cjExtraDat)


cjSerializer& cjExtraDat::SerializeAry(cjSerializer& ser, cjExtraDat*** d, cjINT32* count)
{	
	if(ser.IsQuerySerObjAndStr())
	{
		for(cjINT32 i=0; i< *count; i++)
			*((*d)[i]) >> ser;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjBYTE serClsID;
		cjBYTE datCount = count ? (cjBYTE)(*count) : 0;

		cjPODSerializeWithType<cjBYTE>(ser, datCount);

		if(ser.IsLoading())
		{
			cjAssert(*d == 0);
		
			if(datCount)
			{
				*d = (cjExtraDat**)cjMemAlloc(sizeof(cjExtraDat*) * datCount);
				for(cjINT32 i=0; i<datCount; i++)
				{
					ser << serClsID;
					(*d)[i] = cjExtraDat::CreateFromSerClsID(serClsID);
					((*d)[i])->AddRef();
					*((*d)[i]) >> ser;
				}
			}

			if(count)
				*count = datCount;
		}
		else if(ser.IsSaving())
		{
			for(cjINT32 i=0; i<datCount; i++)
			{
				serClsID = (*d)[i]->GetSerClsID();
				ser << serClsID;
				*((*d)[i]) >> ser;
			}
		}
	}

	return ser;	
}


CJExtraDatCreatorType cjExtraDat::ms_creator[cjExtraDat::SERID_MAX];

#ifdef CJUNICODE
void cjExtraDat::SetName(const cjChar* name)
{
	if(!name)
		m_name = cjName::NullName;

	cjChar2Uni uni(name);
	m_name.SetName(*uni);		
}
#endif

void cjExtraDat::StaticInit()
{
	cjExtDatBool::StaticInit();
	cjExtDatInt::StaticInit();
	cjExtDatStr::StaticInit();
	cjExtDatAniNote::StaticInit();
	cjExtDatBinary::StaticInit();
	cjExtDatVector::StaticInit();
	cjExtDatMatrix::StaticInit();
}

void cjExtraDat::StaticShutdown()
{
}


//
// cjExtDatBool
//

CJSER_IMPL_CLASS(cjExtDatBool, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatBool)
	ser << m_value;
CJSER_IMPL_SERIALIZE_END(cjExtDatBool)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatBool)
	d->m_value = m_value;	
CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatBool)

CJSER_IMPL_CLONE(cjExtDatBool)


void cjExtDatBool::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}

//
// cjExtDatInt
//

CJSER_IMPL_CLASS(cjExtDatInt, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatInt)
	ser << m_value;
CJSER_IMPL_SERIALIZE_END(cjExtDatInt)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatInt)
	d->m_value = m_value;	
CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatInt)

CJSER_IMPL_CLONE(cjExtDatInt)


void cjExtDatInt::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}


//
// cjExtDatStr
//

CJSER_IMPL_CLASS(cjExtDatStr, cjExtraDat)

//cjopti
CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatStr)
	cjINT32 numRow = 0;

	if(ser.IsSaving() && m_buf)
		numRow = m_numRow > 1 ? m_numRow : 1;

	ser << cjFStreamCmprI32(numRow);

	if(ser.IsLoading())
	{
		cjAssert(!m_buf);
		cjAssert(m_numRow == 0);

		m_buf = 0;
		m_numRow = numRow == 1 ? 0 : numRow;

		if(numRow > 0)
		{
			if(numRow == 1)
			{
				cjSerializeNameCharStr(ser, (const cjNameChar**)&m_buf, 0);
			}
			else
			{
				// m_numRow > 1
				m_buf = (cjNameChar**)cjMemAlloc(sizeof(cjNameChar*) * numRow);

				for(cjINT32 i=0; i<numRow; i++)
					cjSerializeNameCharStr(ser, (const cjNameChar**)&m_buf[i], 0);
			}
		}
	}
	else if(ser.IsSaving())
	{
		if(numRow > 0)
		{
			if(m_numRow == 0) // single string
			{
				cjSerializeNameCharStr(ser, (const cjNameChar**)&m_buf, 0);
			}
			else
			{
				for(cjINT32 i=0; i<m_numRow; i++)
				{
					const cjNameChar* cur = m_buf[i];
					cjSerializeNameCharStr(ser, &cur, 0);
				}
			}
		}
	}
CJSER_IMPL_SERIALIZE_END(cjExtDatStr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatStr)

	cjINT32 numRow = m_numRow; 
	cjINT32 len = 0;

	d->m_numRow = numRow;
	
	if(numRow == 0) // single line
	{ 
		if(m_buf)
		{
			len = cjStrlen((cjNameChar*)m_buf);

			if(len > 0)
			{
				d->m_buf = (cjNameChar**)cjMemAlloc(sizeof(cjNameChar)*(len+1));
				cjStrcpy((cjNameChar*)d->m_buf, (cjNameChar*)m_buf);
			}
			else
			{
				d->m_buf = cjNULL;
			}
		}
		else
		{
			d->m_buf = cjNULL;
		}
	}
	else
	{
		d->m_buf = (cjNameChar**)cjMemAlloc(sizeof(cjNameChar*) * numRow);

		for(cjINT32 i=0; i<numRow; i++)
		{
			len = cjStrlen(m_buf[i]);

			if(len > 0)
			{
				d->m_buf[i] = (cjNameChar*)cjMemAlloc(sizeof(cjNameChar)*(len+1));
				cjStrcpy(d->m_buf[i], m_buf[i]);
			}
			else
			{
				d->m_buf[i] = cjNULL;
			}
		}
	}
	
CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatStr)

CJSER_IMPL_CLONE(cjExtDatStr)


void cjExtDatStr::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}


#ifdef CJUNICODE

static void _InnerStrAlloc(cjNameChar** buf, const cjChar* str)
{
	if(*buf)
	{
		cjAssert(!"[_InnerStrAlloc] ERR! - buf must be 0 !!");
		return;
	}

	// cjChar2Uni::GetConvLen() returns string length which is contained null pointer
	cjINT32 len = cjChar2Uni::GetConvLen(str);
	if(len)
	{		
		(*buf) = (cjNameChar*)cjMemAlloc(sizeof(cjNameChar*)*len); // not len+1
		cjChar2Uni::Conv((*buf), str, len); //(*buf)[len] = NULL; , cjChar2Uni::Conv does
	}
	else
		(*buf) = 0;
}

cjExtDatStr::cjExtDatStr(const cjChar* str)
{
	m_numRow = 0;
	m_buf = 0;

	if(str)
		_InnerStrAlloc((cjNameChar**)&m_buf, str);
	else
		m_buf = 0;	
}

#endif


static void _InnerStrAlloc(cjNameChar** buf, const cjNameChar* str)
{
	//if(*buf)
	//{
	//	cjAssert(!"[_InnerStrAlloc] ERR! - buf must be 0 !!");
	//	return;
	//}

	cjINT32 len = cjStrlen(str);

	if(len)
	{
		(*buf) = (cjNameChar*)cjMemAlloc(sizeof(cjNameChar)*(len+1));
		cjStrcpy((*buf), str);
		(*buf)[len] = 0;
	}
	else
		(*buf) = 0;
}

void cjExtDatStr::CreateSingleString(const cjNameChar* str)
{
	CleanString();

	m_numRow = 0;

	if(str)
		_InnerStrAlloc((cjNameChar**)&m_buf, str);
}

void cjExtDatStr::CreateMultiString(const cjNameChar** strAry, cjINT32 numRow)
{
	if(numRow == 1)
	{
		CreateSingleString(strAry[0]);
		return;
	}

	CleanString();

	m_numRow = numRow;
	m_buf = 0;

	if(m_numRow == 0 || !strAry)
		return;

	m_buf = (cjNameChar**)cjMemAlloc(sizeof(cjNameChar*)* m_numRow);

	for(cjINT32 i=0; i<m_numRow; i++)
	{
		const cjNameChar* str = strAry[i];

		if(str)
			_InnerStrAlloc(&m_buf[i], str);
		else
			m_buf[i] = 0;
	}
}

void cjExtDatStr::CleanString()
{
	if(m_buf)
	{
		if(m_numRow == 0)
		{
			cjMemDeAlloc(m_buf);
		}
		else
		{
			for(cjINT32 i=0; i<m_numRow; i++)
			{
				if(m_buf[i])
					cjMemDeAlloc(m_buf[i]);
			}

			cjMemDeAlloc(m_buf);
		}

		m_buf = 0;
	}

	m_numRow = 0;
}

cjExtDatStr::~cjExtDatStr()
{
	CleanString();
}

cjBool cjExtDatStr::SetString(const cjNameChar* str, cjINT32 row)
{
	if(m_numRow > 0)
	{
		if(row >= m_numRow)
			return cjFalse;

		cjMemDeAlloc(m_buf[row]);
		m_buf[row] = 0;

		_InnerStrAlloc(&m_buf[row], str);
	}
	else
	{
		if(row != 0)
			return cjFalse;

		cjMemDeAlloc(m_buf);
		m_buf = 0;

		_InnerStrAlloc((cjNameChar**)&m_buf, str);
	}

	return cjTrue;
}

const cjNameChar* cjExtDatStr::GetValue(cjINT32 row) const
{
	if(m_numRow == 0)
		return (cjNameChar*)m_buf;

	if(m_numRow <= row)
		return 0;

	return m_buf[row];
}


//
// cjExtDatName
//

CJSER_IMPL_CLASS(cjExtDatAniNote, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatAniNote)
	cjAniNote::Serialize(ser, &m_noteAry, &m_numNotes);
CJSER_IMPL_SERIALIZE_END(cjExtDatAniNote)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatAniNote)

	cjINT32 noteCount = m_numNotes;

	d->m_noteAry = cjNULL;
	d->m_numNotes = noteCount;	

	if(m_noteAry)
	{
		if(noteCount)
		{
			d->m_noteAry = cjAniNote::NewNoteAry(noteCount);
			for(cjINT32 i=0; i<noteCount; i++)
				d->m_noteAry[i] = m_noteAry[i]; //cjReview
		}
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatAniNote)

CJSER_IMPL_CLONE(cjExtDatAniNote)


void cjExtDatAniNote::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}

cjExtDatAniNote::cjExtDatAniNote()
{
	m_numNotes = 0;
	m_noteAry = 0;
}

//cjExtDatAniNote::cjExtDatAniNote(cjAniNote** keyAry, cjINT32 numKeys)
//{
//	m_numNotes = numKeys;
//	m_noteAry = keyAry;
//}

cjExtDatAniNote::~cjExtDatAniNote()
{
	cjAniNote::DelNoteAry(m_noteAry);

	m_noteAry = 0;
	m_numNotes = 0;
}


//
// cjExtDatBinary
//

CJSER_IMPL_CLASS(cjExtDatBinary, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatBinary)
	ser << cjFStreamCmprI32(m_siz);
	
	if(ser.IsLoading())
	{
		cjAssert(!m_binaryDat);

		if(m_siz > 0)
			m_binaryDat = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * m_siz);
	}

	ser.SerializeStream(m_binaryDat, sizeof(cjBYTE) * m_siz);
CJSER_IMPL_SERIALIZE_END(cjExtDatBinary)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatBinary)

	d->m_siz = m_siz;

	if(d->m_siz > 0)
	{
		d->m_binaryDat = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * m_siz);
		cjMemCpy(d->m_binaryDat, m_binaryDat, m_siz);
	}
	else
	{
		d->m_binaryDat = cjNULL;
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatBinary)

CJSER_IMPL_CLONE(cjExtDatBinary)
	

void cjExtDatBinary::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}


cjExtDatBinary::~cjExtDatBinary()
{
	cjMemDeAlloc(m_binaryDat);
	m_binaryDat = 0;
}

void cjExtDatBinary::SetValue(const cjUINT32 siz, cjBYTE* binaryDat)
{
	if(m_binaryDat)
	{
		cjMemDeAlloc(m_binaryDat);
		m_binaryDat = 0;
	}
	m_siz = siz;

	if(m_siz > 0)
	{
		m_binaryDat = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE)*m_siz);
		cjMemCpy(m_binaryDat, binaryDat, sizeof(cjBYTE)*siz);
	}
}


//
// cjExtDatVector
//

CJSER_IMPL_CLASS(cjExtDatVector, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatVector)
	ser << m_vec[0] << m_vec[1] << m_vec[2] << m_vec[3];	
CJSER_IMPL_SERIALIZE_END(cjExtDatVector)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatVector)

	//cjReview, sizeof(m_vec) size
	cjMemCpy(d->m_vec, m_vec, sizeof(m_vec));

CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatVector)

CJSER_IMPL_CLONE(cjExtDatVector)


void cjExtDatVector::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}


//
// cjExtDatMatrix
//

CJSER_IMPL_CLASS(cjExtDatMatrix, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatMatrix)
	cjMatrix& mat = *((cjMatrix*)this);
	ser << mat;
CJSER_IMPL_SERIALIZE_END(cjExtDatMatrix)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatMatrix)
	//cjReview, sizeof(m_mat[4][4]) size
	cjMemCpy(d->m_mat, m_mat, sizeof(m_mat));
CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatMatrix)

CJSER_IMPL_CLONE(cjExtDatMatrix)


void cjExtDatMatrix::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}

