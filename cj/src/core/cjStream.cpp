
#include "cjCorePCH.h"
#include "cjStream.h"

#include "third/tinyxml/tinyxml2.h"


#define CJ_USE_CJFSTREAM_SAVE_CHECK


//
// cjFStreamDBGSerialDat / cjFStreamDBGInfo
//

class cjFStreamDBGSerialDat;

class CJCORE_API cjFStreamDBGSerialDat : public cjMem
{		
	CJTYPEINFO_SUPPORT_DECL(cjFStreamDBGSerialDat)

public:
	cjFStreamDBGSerialDat();
	~cjFStreamDBGSerialDat();

	cjStreamOffset					m_offsetBegin;
	cjStreamOffset					m_offsetEnd;
		
	const cjChar*					m_funcName;
	const cjChar*					m_fileName;
		
	cjWORD							m_line;
	cjWORD							m_serial;		// 0x7fff

	cjUINT32						m_sequence;
	cjStringA						m_rttiName;
	cjStringA						m_name;

	cjFStreamDBGSerialDat*			m_parent;
	cjTAry<cjFStreamDBGSerialDat*>*	m_child;
};

CJTYPEINFO_SUPPORT_IMPL(cjFStreamDBGSerialDat);


class CJCORE_API cjFStreamDBGInfo : public cjMem
{	
	CJTYPEINFO_SUPPORT_DECL(cjFStreamDBGInfo)

public:
	typedef cjTHash<const cjChar* , cjUINT32> FuncName2Seq;  // key=function name, val=sequence
	typedef cjTHash<const cjChar* , cjUINT32> FileName2Seq;  // key=src file name, val=sequence

	cjFStreamDBGInfo();
	~cjFStreamDBGInfo();

	cjWORD SetBeginInfo(const cjChar* rttiName,  const cjChar* name, const cjChar* func, 
		const cjUINT32 line, const cjChar* fname, cjStreamOffset offset, cjWORD lastSerial);
	void SetEndInfo(cjWORD serial, cjStreamOffset offset);

	void Report(const cjNameChar* path);
		
public:
	cjBool							m_isBegin;
	cjWORD							m_dbgSerial;
	FuncName2Seq					m_funcName2Seq;
	cjUINT32						m_funcNameSeq;

	FileName2Seq					m_fileName2Seq;
	cjUINT32						m_fileNameSeq;

	cjUINT32						m_sequence;
	
	cjTAry<cjFStreamDBGSerialDat*>*	m_dbgSDat;
	cjTHash<cjWORD, cjFStreamDBGSerialDat*>* m_serial2SDat;
};

CJTYPEINFO_SUPPORT_IMPL(cjFStreamDBGInfo);


//
// Compressed Numeric Value Serializer
//

//cjFStream& operator<< (cjFStream& stream, cjFStreamCmprNum32& i32)
//{
//	cjBool isSave = stream.IsSaving();
//
//	if(!stream.IsLoading() && !isSave)
//	{
//		// no loading and no saving, just in serialze
//		stream << i32.m_v;
//	}
//	else
//	{
//		// loading or saving
//#ifdef CJ_USE_CJFSTREAM_SAVE_CHECK
//		cjINT32			original = 0;
//#endif
//		cjUINT32		absV = 0;
//		cjBYTE			c0 = 0;
//
//		if(isSave)
//		{
//			// saving
//
//#ifdef CJ_USE_CJFSTREAM_SAVE_CHECK
//			original = i32.m_v;
//#endif
//			absV = cjAbs(i32.m_v);
//			c0 = ((i32.m_v >=0 ) ? 0 : 0x80);  // determine sign bit
//
//			// if absolute value smaller than 64(0x40), add value to c0 and end
//			// else greater than 63(0x3f), add (absV & 0x3f) to c0, add 0x40(0x64) flag to continue compress
//			c0 += ((absV < 0x40) ? absV : ((absV & 0x3f) + 0x40));
//		}
//
//		if(isLoad)
//			i32.m_v = 0;
//
//		stream << c0; // read byte or write c0
//
//		// handle absolute value 64(0x40) ~ 127
//		if(c0 & 0x40)
//		{
//			absV >>= 6; // truncate 0~63 range bit of c0
//
//			// absolute value is smaller than 128(0x80), use value 
//			// else greater than 127(0x7f), add (absV & 0x7f) to c0, add 0x80 flag to continue compress
//			cjBYTE c1;
//
//			if(isSave) 
//				c1 = (absV < 0x80) ? absV : ((absV & 0x7f) + 0x80);
//			else
//				c1 = 0;
//
//			stream << c1; // read and write
//						
//			if(c1 & 0x80)
//			{
//				absV >>= 7; // 7bit shift to drop value of c1
//
//				cjBYTE c2;
//				
//				if(isSave)
//					c2 = (absV < 0x80) ? absV : ((absV & 0x7f)+0x80);
//				else
//					c2 = 0;
//
//				stream << c2;
//
//				if(c2 & 0x80)
//				{
//					absV >>= 7;
//
//					cjBYTE c3;
//					
//					if(isSave)
//						c3 = (absV < 0x80) ? absV : ((absV & 0x7f)+0x80);
//					else
//						c3 = 0;
//
//					stream << c3;
//
//					if(c3 & 0x80)
//					{
//						absV >>= 7;
//
//						cjBYTE c4 = isSave ? absV : 0;
//						stream << c4;
//
//						i32.m_v = c4;  // rebuild value #0
//					}
//
//					i32.m_v = (i32.m_v << 7) + (c3 & 0x7f);  // rebuild value #1
//				}
//
//				i32.m_v = (i32.m_v << 7) + (c2 & 0x7f);  // rebuild value #2
//			}
//
//			i32.m_v = (i32.m_v << 7) + (c1 & 0x7f); // rebuild value #3
//		}
//
//		i32.m_v = (i32.m_v << 6) + (c0 & 0x3f);  //// rebuild value #4
//
//		// apply sign bit
//		if(c0 & 0x80)
//			i32.m_v = -i32.m_v;
//
//#ifdef CJ_USE_CJFSTREAM_SAVE_CHECK
//		if(stream.IsSaving() && i32.m_v != original)
//		{
//			cjLOG("error occured on saving value, value=%08x, ori=%08x", i32.m_v, original);
//			cjAssert(0);
//		}
//#endif
//	}
//
//	return stream;
//}

//cjFStream& operator<< (cjFStream& stream, _cjFStreamCompressUI32& ui32)
//{
//	if(!stream.IsLoading() && !stream.IsSaving())
//	{	
//		stream << ui32.m_v; // no loading and no saving, just in serialze
//	}
//	else
//	{
//		// loading or saving
//		cjUINT32	V = ui32.m_v;
//		cjBYTE		c0 = 0;
//
//		if(stream.IsSaving())
//		{
//			// saving
//			//absV = cjAbs(ui32.m_v);
//			//c0 = ((ui32.m_v >=0 ) ? 0 : 0x80);  // determine sign bit
//
//			// 128(0x80), 64(0x40), 32(0x20)
//			c0 += (cjBYTE)((V < 0x80) ? V : ((V & 0x7f) + 0x80)); // 0x80 = compress sign bit
//
//			stream << c0;
//			if(c0 & 0x80)
//			{  
//				V >>= 7;
//
//				cjBYTE c1 = (cjBYTE)((V < 0x80) ? V : ((V & 0x7f) + 0x80));
//
//				stream << c1;
//				if(c1 & 0x80)
//				{
//					V >>= 7;
//					cjBYTE c2 = cjBYTE((V < 0x80) ? V : ((V & 0x7f)+0x80));
//
//					stream << c2;
//					if(c2 & 0x80)
//					{
//						V >>= 7;
//						cjBYTE c3 = (cjBYTE)((V < 0x80) ? V : ((V & 0x7f)+0x80));
//
//						stream << c3;
//						if(c3 & 0x80)
//						{
//							V >>= 7;
//
//							cjBYTE c4 = (cjBYTE)V;
//							stream << c4;  // 0xf
//						}
//					}
//				}
//			}
//		}
//		else
//		{
//			ui32.m_v = 0;
//
//			// loading
//			stream << c0;
//			if(c0 & 0x80)
//			{
//				cjBYTE c1; stream << c1;
//				if(c1 & 0x80)
//				{
//					cjBYTE c2; stream << c2;
//					if(c2 & 0x80)
//					{
//						cjBYTE c3; stream << c3;
//						if(c3 & 0x10)
//						{
//							cjBYTE c4; stream << c4;
//							ui32.m_v = c4 & 0xf;
//						}
//						ui32.m_v = (ui32.m_v << 7) + (c3 & 0x7f);
//					}
//					ui32.m_v = (ui32.m_v << 7) + (c2 & 0x7f);
//				}
//				ui32.m_v = (ui32.m_v << 7) + (c1 & 0x7f);
//			}
//			ui32.m_v = (ui32.m_v << 7) + (c0 & 0x7f);
//		}
//
//	} // end of if(!stream.IsLoading() && !isSave)
//
//	return stream;
//}


cjFStream& operator<< (cjFStream& stream, _cjFStreamCompressI32& i32)
{
	if(!stream.IsLoading() && !stream.IsSaving())
	{	
		stream << i32.m_v; // no loading and no saving, just in serialze
	}
	else
	{
		// loading or saving
		cjUINT32	absV = 0;
		cjBYTE		c0 = 0;

		if(stream.IsSaving())
		{
			// saving
			absV = cjAbs(i32.m_v);
			c0 = ((i32.m_v >=0 ) ? 0 : 0x80);  // determine sign bit

			// if absolute value smaller than 64(0x40), add value to c0 and end
			// else greater than 63(0x3f), add (absV & 0x3f) to c0, add 0x40(0x64) flag to continue compress
			c0 += (cjBYTE)((absV < 0x40) ? absV : ((absV & 0x3f) + 0x40));

			stream << c0;			
			if(c0 & 0x40) // handle absolute value 64(0x40) ~ 127
			{
				absV >>= 6; // truncate 0~63 range bit of c0

				// absolute value is smaller than 128(0x80), use value 
				// else greater than 127(0x7f), add (absV & 0x7f) to c0, add 0x80 flag to continue compress
				cjBYTE c1 = (cjBYTE)((absV < 0x80) ? absV : ((absV & 0x7f) + 0x80));

				stream << c1;
				if(c1 & 0x80)
				{
					absV >>= 7; // 7bit shift to drop value of c1
					cjBYTE c2 = cjBYTE((absV < 0x80) ? absV : ((absV & 0x7f)+0x80));

					stream << c2;
					if(c2 & 0x80)
					{
						absV >>= 7;
						cjBYTE c3 = (cjBYTE)((absV < 0x80) ? absV : ((absV & 0x7f)+0x80));

						stream << c3;
						if(c3 & 0x80)
						{
							absV >>= 7;

							cjBYTE c4 = (cjBYTE)absV;
							stream << c4;
						}
					}
				}
			}
		}
		else
		{
			i32.m_v = 0;

			// loading
			stream << c0;
			if(c0 & 0x40) // handle absolute value 64(0x40) ~ 127
			{
				cjBYTE c1; stream << c1;
				if(c1 & 0x80)
				{
					cjBYTE c2; stream << c2;
					if(c2 & 0x80)
					{
						cjBYTE c3; stream << c3;
						if(c3 & 0x80)
						{
							cjBYTE c4; stream << c4;
							i32.m_v = c4;
						}
						i32.m_v = (i32.m_v << 7) + (c3 & 0x7f);
					}
					i32.m_v = (i32.m_v << 7) + (c2 & 0x7f);
				}
				i32.m_v = (i32.m_v << 7) + (c1 & 0x7f);
			}
			i32.m_v = (i32.m_v << 6) + (c0 & 0x3f);
						
			if(c0 & 0x80) // apply sign bit
				i32.m_v = -i32.m_v;
		}

	} // end of if(!stream.IsLoading() && !isSave)

	return stream;
}


cjFStream& operator<< (cjFStream& stream, _cjFStreamCompressI64& i64)
{
	if(!stream.IsLoading() && !stream.IsSaving())
	{	
		stream << i64.m_v; // no loading and no saving, just in serialze
	}
	else
	{
		// loading or saving
		cjUINT64	absV = 0;
		cjBYTE		c0 = 0;

		if(stream.IsSaving())
		{
			// saving
			absV = cjAbs(i64.m_v);
			c0 = ((i64.m_v >=0 ) ? 0 : 0x80);  // determine sign bit

			// if absolute value smaller than 64(0x40), add value to c0 and end
			// else greater than 63(0x3f), add (absV & 0x3f) to c0, add 0x40(0x64) flag to continue compress
			c0 += ((absV < 0x40) ? absV & 0xff : ((absV & 0x3f) + 0x40));

			stream << c0;			
			if(c0 & 0x40) // handle absolute value 64(0x40) ~ 127
			{
				absV >>= 6; // truncate 0~63 range bit of c0

				// absolute value is smaller than 128(0x80), use value 
				// else greater than 127(0x7f), add (absV & 0x7f) to c0, add 0x80 flag to continue compress
				cjBYTE c1 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f) + 0x80);

				stream << c1;
				if(c1 & 0x80)
				{
					absV >>= 7; // 7bit shift to drop value of c1
					cjBYTE c2 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f)+0x80);

					stream << c2;
					if(c2 & 0x80)
					{
						absV >>= 7;
						cjBYTE c3 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f)+0x80);

						stream << c3;
						if(c3 & 0x80)
						{
							absV >>= 7;
							cjBYTE c4 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f)+0x80);

							stream << c4;
							if(c4 & 0x80)
							{
								absV >>= 7;
								cjBYTE c5 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f)+0x80);

								stream << c5;
								if(c5 & 0x80)
								{
									absV >>= 7;
									cjBYTE c6 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f)+0x80);

									stream << c6;
									if(c6 & 0x80)
									{
										absV >>= 7;
										cjBYTE c7 = (absV < 0x80) ? absV & 0xff : ((absV & 0x7f)+0x80);

										stream << c7;
										if(c7 & 0x80)
										{
											absV >>= 7;

											cjBYTE c8 = absV & 0xff;
											stream << c8;
										}										
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			i64.m_v = 0;

			// loading
			stream << c0;
			if(c0 & 0x40) // handle absolute value 64(0x40) ~ 127
			{
				cjBYTE c1; stream << c1;
				if(c1 & 0x80)
				{
					cjBYTE c2; stream << c2;
					if(c2 & 0x80)
					{
						cjBYTE c3; stream << c3;
						if(c3 & 0x80)
						{
							cjBYTE c4; stream << c4;
							if(c4 & 0x80)
							{
								cjBYTE c5; stream << c5;
								if(c5 & 0x80)
								{
									cjBYTE c6; stream << c6;
									if(c6 & 0x80)	
									{
										cjBYTE c7; stream << c7;
										if(c7 & 0x80)
										{
											cjBYTE c8; stream << c8;
											i64.m_v = c8;
										}
										i64.m_v = (i64.m_v << 7) + (c7 & 0x7f);
									}
									i64.m_v = (i64.m_v << 7) + (c6 & 0x7f);
								}
								i64.m_v = (i64.m_v << 7) + (c5 & 0x7f);
							}
							i64.m_v = (i64.m_v << 7) + (c4 & 0x7f);
						}
						i64.m_v = (i64.m_v << 7) + (c3 & 0x7f);
					}
					i64.m_v = (i64.m_v << 7) + (c2 & 0x7f);
				}
				i64.m_v = (i64.m_v << 7) + (c1 & 0x7f);
			}
			i64.m_v = (i64.m_v << 6) + (c0 & 0x3f);

			if(c0 & 0x80) // apply sign bit
				i64.m_v = -i64.m_v;
		}

	} // end of if(!stream.IsLoading() && !isSave)

	return stream;
}


//
// cjFStream
//

cjFStream::cjFStream(cjTFLAG flags)
{	
	m_maxSerializeSiz = 0;
	m_streamFlag = flags;

	m_dbgInfo = 0;
	m_dbgCurSerial = 0;
	m_dbgLastDBGSerial = 0;
	m_dbgLastEndOffset = 0;

	//SetLoading( flags & ESF_READ ? cjTrue : cjFalse);
	//SetSaving(  flags & ESF_WRITE ? cjTrue : cjFalse);
	//SetObjOnly( flags & ESF_CJOBJONLY ? cjTrue : cjFalse);
	//SetSerOnly( flags & ESF_CJSERONLY ? cjTrue : cjFalse);
	//SetPersist( flags & ESF_PERSIST ? cjTrue : cjFalse);

	SetErr(ERR_CLOSED);
}

cjFStream::~cjFStream()
{
	if(m_dbgInfo)
	{
		delete m_dbgInfo;
		m_dbgInfo = cjNULL;
	}
}

cjBool cjFStream::Open()
{
	ERR_STATUS ERR_STATUS = GetErr();

	if(ERR_STATUS != ERR_CLOSED)
		return cjFalse;

	return cjTrue;
}

void cjFStream::Close()
{
	SetErr(ERR_CLOSED);
}

// debug stuff

void cjFStream::ValidateStreamDBGInfo()
{
	if(!m_dbgInfo)
		m_dbgInfo = new cjFStreamDBGInfo;
}

// ret: new serial
cjWORD cjFStream::BeginDBGSerInfo(cjWORD* rstDbgSerial, const cjNameChar* rttiName, const cjNameChar* name, const cjChar* func, 
	const cjUINT32 line, const cjChar* fname)
{
	if(!m_dbgInfo)
		return 0;
		
	cjStreamOffset offset = GetDebugCurContentPos();
	cjWORD lastSerial = m_dbgCurSerial;

	cjUni2Char u2cRttiName(rttiName);
	cjUni2Char u2cName(name);
	m_dbgCurSerial = m_dbgInfo->SetBeginInfo(*u2cRttiName, *u2cName, func, line, fname, offset, m_dbgCurSerial);
	
	*rstDbgSerial = (m_dbgCurSerial & 0x7fff);

	return lastSerial;
}

void cjFStream::EndDBGSerInfo(const cjChar* func, const cjUINT32 line, const cjChar* fname, cjWORD lastSerial)
{
	cjAssert(lastSerial != m_dbgCurSerial);

	if(lastSerial == m_dbgCurSerial)
		return;

	cjStreamOffset offset = GetDebugCurContentPos();
	m_dbgInfo->SetEndInfo(m_dbgCurSerial, offset);

	cjWORD suffix = m_dbgCurSerial | (cjWORD)0xf000;
	*this << suffix;

	m_dbgCurSerial = lastSerial;
}

void cjFStream::Report(const cjNameChar* filepath)
{
	if(m_dbgInfo)
		m_dbgInfo->Report(filepath);
}

// friend operation

CJCORE_API cjFStream& operator<<(cjFStream& stream, unsigned char& B)
{
	stream.SerializeStream(&B, 1);
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, char& SB)
{
	stream.SerializeStream(&SB, 1);
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, cjSBYTE& SB)
{
	stream.SerializeStream(&SB, 1);
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, short& SW)
{
	stream.SerializeStream(&SW, sizeof(SW));
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, unsigned short& W)
{
	stream.SerializeStream(&W, sizeof(W));
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, unsigned int& UI)
{
	stream.SerializeStream(&UI, sizeof(UI));
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, int& I)
{
	stream.SerializeStream(&I, sizeof(I));
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, cjFLOAT& F)
{
	stream.SerializeStream(&F, sizeof(F));
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, cjINT64& SI64)
{
	stream.SerializeStream(&SI64, sizeof(SI64));
	return stream;
}

CJCORE_API cjFStream& operator<<(cjFStream& stream, cjUINT64& UI64)
{
	stream.SerializeStream(&UI64, sizeof(UI64));
	return stream;	
}

#ifdef CJ_PLATFORM_WIN32
CJCORE_API cjFStream& operator<<(cjFStream& stream, wchar_t& W)
{
	stream.SerializeStream(&W, sizeof(wchar_t));
	return stream;	
}
#endif

CJCORE_API cjFStream& cjFStream::operator<<(const cjRTTI*& rtti)
{	
	if(IsSaving() || IsLoading())
	{				
		cjName rttiName(rtti->cjrGetNameW()); //cjopti cjName rttiName = cjName rtti->m_nameW);
		*this << rttiName;
	}
		
	return *this;
}


// type : [0] 32bit cjFLOAT  [1] 8bit(0~255),unit cjFLOAT(0.0~1.0),  [2] 16bit cjFLOAT
CJCORE_API void cjFloatSerHelper(cjFStream& stream, cjFLOAT& v, cjINT32 type)
{

	if(type == 0)
	{
		stream << v;
	}
	else if(type == 1)
	{
		cjBYTE b;

		if(stream.IsSaving())
		{
			if(v > 1.0f)
				b = 255;
			else
				b = (cjBYTE)(255 * v);
		}
				
		stream << b;

		if(stream.IsLoading())
			v = (cjFLOAT)b / 255.0f;
	}
	else if(type == 2)
	{
		cjAssert(!"not impl");
	}
}


void cjSerializeNameCharStr(cjFStream& stream, const cjNameChar** src, cjINT32* sizStr,
	cjMemAllocPF _ma, cjMemDeallocPF _mf)
{
	cjINT32 siz = 0;
	cjINT32 serSiz = 0;
	cjBool allAnsiChar = cjTrue;

	if(stream.IsLoading())
	{
		stream << cjFStreamCmprI32(serSiz);				
		siz = cjAbs(serSiz);

		// (+1 for null string)
//		cjAssert(*src == 0);
		cjNameChar* buf = 0;
		
		if(siz > 0)
		{
			buf = _ma ? (cjNameChar*)_ma(sizeof(cjNameChar)*(siz+1), &stream) 
				: (cjNameChar*)cjMemAlloc(sizeof(cjNameChar)*(siz+1));
		}

		if(serSiz > 0)
		{	
			// read as compressed unicode
			cjINT32 i=0;
			for(; i<siz; i++)
			{			
				cjINT32 uch;
				stream << cjFStreamCmprI32(uch);

				buf[i] = (cjUni)uch;
			}

			buf[i] = (cjUni)0; // add null
		}
		else if(serSiz < 0)
		{
			// read as only ansi string
			cjINT32 i=0;
			for(; i<siz; i++)
			{			
				cjBYTE ch;
				stream << *((cjBYTE*)&ch);
				buf[i] = ch;
			}

			buf[i] = 0;  // add null
		}
		else
		{
		}

		*src = buf;

		if(sizStr)
			*sizStr = siz;
	}
	else if(stream.IsSaving())
	{	
		// save
		siz = sizStr ? *sizStr : cjStrlen(*src); // except null
		serSiz = siz;

		if(siz > 0)
		{
			allAnsiChar =cjTestAllCharIsAnsi(*src);

			if(allAnsiChar)
				serSiz = -serSiz;
		}

		stream << cjFStreamCmprI32(serSiz);

		const cjNameChar* _src = *src;

		if(serSiz > 0)
		{
			// write as compressed unicode
			for(cjINT32 i=0; i<siz; i++)
			{			
				cjINT32 uch = (cjINT32)_src[i];
				stream << cjFStreamCmprI32(uch);
			}
		}
		else
		{
			// write as ansi
			for(cjINT32 i=0; i<siz; i++)
			{			
				cjUni uni = _src[i];
				cjAssert(uni < 0x100);
				stream << *((cjBYTE*)&uni);
			}
		}
	}
}

void cjSerializeStrChar(cjFStream& stream, const cjChar** src, cjINT32* sizStr,
	cjMemAllocPF _ma, cjMemDeallocPF _mf)
{
	cjINT32 siz = 0;

	if(stream.IsLoading())
	{
		stream << cjFStreamCmprI32(siz);

		// (+1 for null string)
		cjAssert(src != 0);
		cjChar* buf = 0;
		
		if(siz > 0)
		{
			buf = _ma ? (cjChar*)_ma(siz+1, &stream) : (cjChar*)cjMemAlloc(siz+1); 

			cjINT32 i=0;
			for(; i<siz; i++)
				stream << buf[i]; //cjslow

			buf[i] = 0;  // add null
		}

		*src = buf;

		if(sizStr)
			*sizStr = siz;
	}
	else if(stream.IsSaving())
	{	
		siz = sizStr ? *sizStr : cjStrlen(*src); // except null
		stream << cjFStreamCmprI32(siz);

		cjChar* s = const_cast<cjChar*>(*src);
		for(cjINT32 i=0; i<siz; i++)
			stream << s[i];
	}
}


//
// cjFCacheBaseStream
//

cjFCacheBaseStream::cjFCacheBaseStream(cjTFLAG fnStreamFlag)
	: cjFStream(fnStreamFlag)
{
	m_siz = 0;
	m_curPos = 0;
	m_cacheBuf = NULL;
	m_cacheBasePos = 0;
	m_cachedCnt = 0;
	m_cacheBufSiz = 0;
}

cjFCacheBaseStream::~cjFCacheBaseStream()
{
	if(m_cacheBuf)
	{
		cjMemDeAlloc(m_cacheBuf);
		m_cacheBuf = NULL;
	}
}

cjBool cjFCacheBaseStream::Open()
{
	cjBool rst = cjSuper::Open();

	return rst;
}

void cjFCacheBaseStream::Close()
{
	if(GetErr() == ERR_CLOSED)
		return;

	if(m_cacheBuf)
	{
		cjMemDeAlloc(m_cacheBuf);
		m_cacheBuf = NULL;
	}

	m_cachedCnt = 0;

	cjSuper::Close();
}

cjBool cjFCacheBaseStream::ResetCache(cjStreamOffset readerSiz)
{
	cjAssert(!m_cacheBuf);

	if(IsReadonly())
	{
		m_siz = (cjStreamOffset)readerSiz;
		m_cacheBufSiz = CACHESIZ_READ;
	}
	else
	{
		m_siz = 0;
		m_cacheBufSiz = CACHESIZ_WRITE;
	}

	if(m_cacheBuf)
	{
		cjMemDeAlloc(m_cacheBuf);
		m_cacheBuf = NULL;
	}

	m_curPos = 0;
	m_cacheBasePos = 0;
	m_cachedCnt = 0;

	m_cacheBuf = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE)* m_cacheBufSiz);

	if(!m_cacheBuf)
		return cjFalse;

	return cjTrue;
}

cjBool cjFCacheBaseStream::PreCache(cjStreamOffset cacheSizHint)
{
	if(m_err || ! IsLoading())
		return cjFalse;
	
	m_cacheBasePos = m_curPos;

	cjStreamOffset readByte = m_curPos & (m_cacheBufSiz-1);
			 readByte = cjMin(cacheSizHint, m_cacheBufSiz - readByte);
			 readByte = cjMin(readByte, m_siz-m_curPos);

	if(!OnCacheRead(m_curPos, m_cacheBuf, readByte, NULL))
	{
		SetErr(ERR_CACHING);
		m_cachedCnt = 0;

		return cjFalse;
	}

	m_cachedCnt = readByte;

	return cjTrue;
}

cjBool cjFCacheBaseStream::SetPos(cjStreamOffset pos) 
{
	if(m_err)
		return false;

	if(IsLoading())
	{
		// Reader
		if(m_curPos > m_siz)
			return cjFalse;

		if(!OnCacheSetSrcPos(pos))
			return cjFalse;

		m_curPos = pos;
		m_cacheBasePos = pos;
		m_cachedCnt = 0;
	}
	else
	{
		// Writer
		if(!Flush())
			return cjFalse;

		if(!OnCacheSetSrcPos(pos))
			return cjFalse;

		m_curPos = pos;
	}

	return cjTrue;
}

cjStreamOffset cjFCacheBaseStream::SerializeStream(void* v, cjStreamOffset siz)
{
	if(m_err || !v)
		return 0;

	cjStreamOffset serializeByte = 0;

	if(IsLoading())
	{
		// loop, until size is going to zero
		while(siz > 0)
		{
			// choose minimum between siz or availCachedByte
			cjStreamOffset copyBytes = cjMin(siz, m_cacheBasePos + m_cachedCnt - m_curPos);

			cjAssert(copyBytes >= 0);

			if(copyBytes == 0) // cache is empty
			{ 
				// if size to read is bigger than cache buf siz
				// sending read data to buf directly, not to cacheBuf
				if(siz >= m_cacheBufSiz)
				{
					cjStreamOffset numCached = 0;

					// read from archive and write on buffer(v) directly
					if(!OnCacheRead(m_curPos, v, siz, &numCached))
						SetErr(ERR_READ);

					m_curPos += numCached;					
					m_cacheBasePos += numCached;
					m_cachedCnt = m_curPos - m_cacheBasePos;

					//cjserCheck
					cjAssert((m_cacheBasePos + m_cachedCnt - m_curPos) >= 0);

					serializeByte += numCached;

					return serializeByte;
				}

				PreCache(CJ_INT_MAX); // cache file data as maximum buffer siz
				copyBytes = cjMin(siz, m_cacheBasePos + m_cachedCnt - m_curPos);

				if(copyBytes <= 0)
				{
					SetErr(ERR_CACHING);
					return serializeByte;
				}

				if(m_err)
					return serializeByte;
			}

			//cjserCheck
			cjAssert((m_cacheBuf + m_curPos - m_cacheBasePos) >= 0);

			memcpy(v, m_cacheBuf + m_curPos - m_cacheBasePos, copyBytes);
			m_curPos += copyBytes;
			serializeByte += copyBytes;
			v = (cjBYTE*)v + copyBytes;

			siz -= copyBytes;


		} // end of while

		cjAssert(siz == 0);
	}
	else
	{
		m_curPos += siz;
		cjStreamOffset writeCopySiz = 0;

		while(siz > (writeCopySiz = (m_cacheBufSiz - m_cachedCnt)))
		{
			memcpy(m_cacheBuf + m_cachedCnt, v, writeCopySiz);
			m_cachedCnt += writeCopySiz;

			siz -= writeCopySiz;
			v = (cjBYTE*)v + writeCopySiz;
			serializeByte += writeCopySiz;

			// write
			if(!Flush())
			{
				SetErr(ERR_FLUSH);
				return serializeByte;
			}
		}
		
		if(siz) // copy remaining write data to cache
		{
			memcpy(m_cacheBuf + m_cachedCnt, v, siz);
			m_cachedCnt += siz;
		}

		serializeByte += siz;
	}

	return serializeByte;
}

cjBool cjFCacheBaseStream::Flush()
{
	if(m_err || IsLoading())
		return cjFalse;

	if(m_cachedCnt > 0)
	{
		cjStreamOffset numRstFlushed = 0;
		if(!OnCacheFlush(m_cacheBuf, m_cacheBasePos, m_cachedCnt, &numRstFlushed))
		{
			m_cacheBasePos += numRstFlushed;
			SetErr(ERR_FLUSH);
			return false;
		}

		m_cacheBasePos += m_cachedCnt;
	}

	m_cachedCnt = 0;

	return cjTrue;
}


//
// cjFileStream
//

cjFileStream::cjFileStream()
	: cjFCacheBaseStream(0)
{
	m_file = NULL;
}

cjFileStream::cjFileStream(cjFile* file, cjTFLAG baseFlag, cjStreamOffset baseFilePos, 
	cjStreamOffset givenSiz)
	: cjFCacheBaseStream(baseFlag)
{
	InitStuff();

	if(!file || !file->IsValid())
	{
		SetErr(ERR_INVALID_INIT_PARAM);
		return;
	}

	m_file = file;
	m_basePos = baseFilePos;
	m_localFlag |= EFSLF_GIVEN_FILE;

	if(givenSiz)
	{
		m_localFlag |= EFSLF_GIVEN_FILE;
		m_fileSiz = givenSiz;
	}

	SetErr(ERR_CLOSED);
}

cjFileStream::cjFileStream(const cjChar* path, cjFilePathHint hint, cjTFLAG baseFlag, 
	cjStreamOffset baseFilePos, cjStreamOffset givenSiz)
	: cjFCacheBaseStream(baseFlag)
{
	InitStuff();

	if(!path)
	{
		SetErr(ERR_INVALID_INIT_PARAM);
		return;
	}

	cjFile::ResolvePath(m_path, cjMAX_PATH, path, hint);
	//	cjStrcpy(m_path, cjMAX_PATH, path);

	cjTFLAG openFlag = 0;
	cjTFLAG shareFlag = 0;

	if(IsReadonly())
		openFlag |= cjFile::EFF_READ; // | cjFile::EFF_CREATE_ONLY_NOT_EXIST;
	else
		openFlag |= cjFile::EFF_WRITE | cjFile::EFF_CREATE;

	cjFile* file = cjFile::CreateDefer(m_path, openFlag, cjFilePathHint_PassThru, shareFlag);

	m_file = file;
	m_basePos = baseFilePos;

	if(givenSiz)
	{
		m_localFlag |= EFSLF_GIVEN_FILE;
		m_fileSiz = givenSiz;
	}
}

cjFileStream::cjFileStream(const cjUni* path, cjFilePathHint hint, cjTFLAG baseFlag, 
	cjStreamOffset baseFilePos, cjStreamOffset givenSiz)
	: cjFCacheBaseStream(baseFlag)
{
	InitStuff();

	if(!path)
	{
		SetErr(ERR_INVALID_INIT_PARAM);
		return;
	}
		
	cjUni tmpPath[cjMAX_PATH];
	cjFile::ResolvePath(tmpPath, cjMAX_PATH, path, hint);

	cjUni2Char uni2ch(tmpPath);				// cjSlow
	cjStrcpy(m_path, cjMAX_PATH, uni2ch);	// cjSlow

	cjTFLAG openFlag = 0;
	cjTFLAG shareFlag = 0;

	if(IsReadonly())
		openFlag |= cjFile::EFF_READ; // | cjFile::EFF_CREATE_ONLY_NOT_EXIST;
	else
		openFlag |= cjFile::EFF_WRITE | cjFile::EFF_CREATE;
	
	cjFile* file = cjFile::CreateDefer(m_path, openFlag, cjFilePathHint_PassThru, shareFlag);

	m_file = file;
	m_basePos = baseFilePos;

	if(givenSiz)
	{
		m_localFlag |= EFSLF_GIVEN_FILE;
		m_fileSiz = givenSiz;
	}
}


cjFileStream::~cjFileStream()
{
	Close();
}

cjBool cjFileStream::Open()
{
	cjTFLAG streamFlag = GetStreamFlag();

	if(streamFlag & ESF_OPENED)
		return cjTrue;

	if(!m_file)
		return cjFalse;

	if(streamFlag & ESF_OPEN_FAIL) // already failed at previous
		return cjFalse;

	cjBool rst = cjSuper::Open();

	if(!rst)
		return cjFalse;

	if(!m_file->IsValid())
	{
		if(!m_file->Open())
		{
			delete m_file;
			m_file = NULL;

			SetStreamFlag(ESF_OPEN_FAIL);
			SetErr(ERR_OPEN_FAIL);

			return cjFalse;
		}

		cjStreamOffset desiredSiz = 0;

		if(IsReadonly() && !(m_localFlag & EFSLF_GIVEN_FILE))
			desiredSiz = m_file->GetSiz();

		cjSuper::ResetCache((cjStreamOffset)desiredSiz);

		SetErr(ERR_OK);
		SetStreamFlag(ESF_OPENED);
	}

	return cjTrue;
}

void cjFileStream::Close()
{
	if(GetErr() == ERR_CLOSED)
		return;

	if(m_file)
	{
		if(IsWriteable())
			Flush();
	}

	if(m_file && !(m_localFlag & EFSLF_GIVEN_FILE))
	{
		// if(m_localFlag & ESF_PERSIST) {}
		delete m_file;
		m_file = NULL;
	}

	SetErr(ERR_CLOSED);

	cjSuper::Close();
}

cjStreamOffset cjFileStream::SerializeStream(void* v, cjStreamOffset siz)
{
	return cjSuper::SerializeStream(v, siz);
}

cjBool cjFileStream::SetPos(cjStreamOffset pos) 
{
	return cjSuper::SetPos(pos);
}

const cjChar* cjFileStream::GetPath()
{
	if(m_file)
		return m_file->GetPath();

	return NULL;
}

const cjChar* cjFileStream::GetName()
{
	if(m_file)
		return m_file->GetName();

	return NULL;
}

cjStreamOffset cjFileStream::GetDebugCurContentPos(cjWORD serial)
{
	serial = serial;

	if(!m_file)
		return -1;

//	cjStreamOffset fileOff = m_file->GetPos();
	cjStreamOffset logicalOff = GetDebugLogicalPos();

	if(IsLoading())
	{
		return logicalOff;
	}
	else if(IsSaving())
	{
		return logicalOff;
	}	

	return -1;
}

void cjFileStream::InitStuff()
{
	m_file = NULL;
	m_fileSiz = 0;
	m_basePos = 0;
	m_localFlag = 0;

//#pragma message ("DELETEME! - TEMP - EFSLF_ALWAYS_FLUSH")
//	m_localFlag |= EFSLF_ALWAYS_FLUSH;

	m_name = NULL;
	m_path[0] = NULL;
}

cjBool cjFileStream::OnCacheSetSrcPos(cjStreamOffset cachePos)
{
	if(!m_file && m_err)
		return cjFalse;

	return m_file->SetPos(m_basePos + cachePos);
}

cjBool cjFileStream::OnCacheFlush(const void* srcBuf, cjStreamOffset cacheWritePos, 
	cjStreamOffset numWrite, cjStreamOffset* numRstWrited)
{
	if(m_err || IsLoading())
	{
		SetErr(ERR_INVALID_CALL);
		return cjFalse;
	}

	cjStreamOffset writePos = m_basePos + cacheWritePos;

	if(!m_file->Write(srcBuf, numWrite, numRstWrited, &writePos, (m_localFlag & EFSLF_ALWAYS_FLUSH) ? cjTrue : cjFalse))
	{
		SetErr(ERR_WRITE);
		return cjFalse;
	}

	return cjTrue;
}

cjBool cjFileStream::OnCacheRead(cjStreamOffset cacheReadPos, void* dstBuf, 
	cjStreamOffset numRead,	cjStreamOffset* numRstReaded)
{
	if(m_err || !IsLoading())
	{
		SetErr(ERR_INVALID_CALL);
		return cjFalse;
	}
	
	cjStreamOffset readPos = m_basePos + cacheReadPos;
	if(!m_file->Read(dstBuf, numRead, numRstReaded, &readPos))
	{
		if(m_file->GetLastErr() == cjFile::ERR_REACHED_EOF)
			SetErr(ERR_REACHED_EOF);
		else
			SetErr(ERR_READ);

		return cjFalse;
	}

	return cjTrue;
}


//
// cjFDeferLoad
//

cjBool cjFDeferLoad::ms_enableDeferLoading;

cjBool cjFDeferLoad::GetDeferLoadingState()
{
	return ms_enableDeferLoading;
}

void cjFDeferLoad::SetDeferLoadState(cjBool enableDeferLoading)
{
	ms_enableDeferLoading = enableDeferLoading;
}


//
// cjFStreamDBGInfo
//

cjFStreamDBGSerialDat::cjFStreamDBGSerialDat()
{
	m_offsetBegin = cjNULL;
	m_offsetEnd = cjNULL;
	
	m_funcName = cjNULL;
	m_fileName = cjNULL;
	
	m_line = 0;
	m_serial = cjNULL;

	m_sequence = 0;
	m_rttiName = "none";
	
	m_parent = cjNULL;
	m_child = cjNULL;
}

cjFStreamDBGSerialDat::~cjFStreamDBGSerialDat()
{
	if(m_child)
	{
		delete m_child;
		m_child = cjNULL;
	}
}


cjFStreamDBGInfo::cjFStreamDBGInfo() : m_funcName2Seq(40), m_fileName2Seq(40)
{	
	m_isBegin = cjFalse;
	m_dbgSerial = 0;

	m_funcNameSeq = 0;
	m_fileNameSeq = 0;

	m_sequence = 0;

	m_dbgSDat = new cjTAry<cjFStreamDBGSerialDat*>;
	m_serial2SDat = new cjTHash<cjWORD, cjFStreamDBGSerialDat*>(128);
}

cjFStreamDBGInfo::~cjFStreamDBGInfo()
{
	if(m_serial2SDat)
	{
		m_serial2SDat->TraverseAllPair(cjTHash<cjWORD, cjFStreamDBGSerialDat*>::PFDefaultRemovePairValue);
		delete m_serial2SDat;
	}

	if(m_dbgSDat)
	{
		// deleting item of m_dbgSDat is performed ~cjFStreamDBGSerialDat
		//for(cjAryOff i=0; i<m_dbgSDat->Count(); i++)
			//delete (*m_dbgSDat)(i);

		delete m_dbgSDat;
	}

}

cjWORD cjFStreamDBGInfo::SetBeginInfo(const cjChar* rttiName, const cjChar* name,
	const cjChar* func, const cjUINT32 line, const cjChar* fname, 
	cjStreamOffset offset, cjWORD lastSerial)
{
	cjUINT32 funcSeq = 0;
	cjUINT32 fileNameSeq = 0;

	if(func && *func)
	{
		if(!m_funcName2Seq.Find(func, &funcSeq))
		{
			funcSeq = ++m_funcNameSeq;
			m_funcName2Seq.Insert(func, funcSeq);
		}
	}

	if(fname && *fname)
	{
		if(!m_fileName2Seq.Find(fname, &fileNameSeq))
		{
			fileNameSeq = ++m_fileNameSeq;
			m_fileName2Seq.Insert(fname, fileNameSeq);
		}
	}

	++m_dbgSerial;

	cjAssert(funcSeq < 0xffff);
	cjAssert(fileNameSeq < 0xffff);


	// Info
	cjFStreamDBGSerialDat* sdat = new cjFStreamDBGSerialDat;

	sdat->m_offsetBegin		= offset;
	sdat->m_offsetEnd		= 0;
	sdat->m_funcName		= func;
	sdat->m_fileName		= fname;
	sdat->m_line			= (cjWORD)line;

	sdat->m_serial			= m_dbgSerial;
	sdat->m_rttiName		= rttiName;
	sdat->m_name			= name ? name : "";
	sdat->m_sequence		= ++m_sequence;

	cjFStreamDBGSerialDat* parentSDat = cjNULL;

	if(lastSerial != 0)
	{
		cjTHash<cjWORD, cjFStreamDBGSerialDat*>::Pair* pair = m_serial2SDat->Find(lastSerial);

		if(pair)
			parentSDat = pair->m_val;
	}

	sdat->m_parent			= parentSDat;
	if(parentSDat)
	{
		if(!parentSDat->m_child)
			parentSDat->m_child = new cjTAry<cjFStreamDBGSerialDat*>;

		parentSDat->m_child->Push(sdat);
	}

	m_dbgSDat->Push(sdat);
	m_serial2SDat->Insert(m_dbgSerial, sdat);

	return m_dbgSerial;
}

void cjFStreamDBGInfo::SetEndInfo(cjWORD serial, cjStreamOffset offset)
{
	cjTHash<cjWORD, cjFStreamDBGSerialDat*>::Pair* pair = m_serial2SDat->Find(serial);

	if(pair)
		pair->m_val->m_offsetEnd = offset;
}


static void RecursiveMakeXML(cjUINT32 index, cjFStreamDBGSerialDat* cur, tinyxml2::XMLDocument& doc, tinyxml2::XMLNode* parentEl)
{
	cjChar buf[512];

	++index;
	
	cjSprintf(buf, 512, "N%x-SER%x_%s", cur->m_sequence, cur->m_serial, *cur->m_rttiName);
	tinyxml2::XMLElement* newElement = doc.NewElement(buf);

	cjUINT32 offBegin = (cjUINT32)cur->m_offsetBegin;
	cjUINT32 offEnd = (cjUINT32)cur->m_offsetEnd;

	cjSprintf(buf, 512, "%x-%x", offBegin, offEnd);
	
	newElement->SetAttribute("name", *(cur->m_name));
	newElement->SetAttribute("A2funcName", cur->m_funcName);
	newElement->SetAttribute("A3line", cur->m_line);
	newElement->SetAttribute("A4file", cur->m_fileName);
	newElement->SetAttribute("A0Pos", buf);
		
	parentEl->InsertEndChild(newElement);
	
	if(cur->m_child)
	{
		for(cjAryOff k=0; k<cur->m_child->Count(); k++)
			RecursiveMakeXML(++index, (*(cur->m_child))(k), doc, newElement);
	}
}


void cjFStreamDBGInfo::Report(const cjNameChar* path)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* rootEl = doc.InsertEndChild(doc.NewElement("Root"));

	cjTAry<cjFStreamDBGSerialDat*> rootSDats;

	for(cjAryOff i=0; i<m_dbgSDat->Count(); i++)
	{
		cjFStreamDBGSerialDat* curSDat = (*m_dbgSDat)(i);

		if(!curSDat->m_parent)
			rootSDats.Push(curSDat);
	}
		
	for(cjAryOff i=0; i<rootSDats.Count(); i++)
		RecursiveMakeXML(0, rootSDats(i), doc, rootEl);
		
	cjUni2Char u2cPath(path);
	doc.SaveFile(*u2cPath);
}