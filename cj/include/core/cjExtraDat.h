
#ifndef _CJEXTRADAT_H_
#define _CJEXTRADAT_H_


class cjAniTxtKey;
class cjExtraDat;


//
// cjAniNote
//

class CJCORE_API cjAniNote : public cjMem
{
	CJ_DECL_SERIALIZE(cjAniNote)
	CJTYPEINFO_SUPPORT_DECL(cjAniNote)	

public:
	cjAniNote() {}
	cjAniNote(const cjNameChar* name, cjFLOAT timeSec)
	{
		m_note = name;
		m_timeSec = timeSec;
	}

	cjBool operator== (const cjAniNote& note) const { return note.m_note == m_note && note.m_timeSec == m_timeSec ? cjTrue : cjFalse; }
	cjBool operator!= (const cjAniNote& note) const { return !operator==(note); }

	void SetNote(const cjNameChar* note) { if(note) { m_note = note; } else { m_note.Clear(); } }
	const cjNameChar* GetNote() const { return m_note.GetStr(); }

	void SetTime(cjFLOAT timeSec) { m_timeSec = timeSec; }
	cjFLOAT GetTime() const { return m_timeSec; }

	static cjAniNote* NewNoteAry(cjINT32 numNote);
	static void DelNoteAry(cjAniNote* noteAry);

	//static cjAniNote* NewNote(cjFLOAT timeSec, const cjNameChar* str);
	//static void DelNote(cjAniNote* note) { note->DelInst(); }

	static void Serialize(cjSerializer& ser, cjAniNote** ary, cjINT32* _count=0);
	
protected:
	cjString							m_note;		// first position for padding
	cjFLOAT								m_timeSec;	

public:
	CJTYPEINFO_SUPPORT_DECL(cjAniNote)
//	CJCORE_API friend struct cjTypeInfo<cjAniNote>;
};

CJTYPEINFO_SUPPORT_IMPL(cjAniNote);


//
// cjNameMatch
//

class CJCORE_API cjNameMatch : public cjRef
{
	CJSER_DECL_CLASS_X(cjNameMatch, cjRef, 0, CJCORE_API)
	CJSER_DECL_CLONE(cjNameMatch)

public:
	cjNameMatch() {}
	cjNameMatch(const cjName& name) : m_name(name) {}
	
    virtual cjBool IsMatch(const cjName& name)
	{
		if(m_name == name)
		{
			SetLastMatchedName(name);
			return cjTrue;
		}
		return cjFalse;
	}

	operator const cjNameChar*(void) { return *m_name; }
	void SetName(const cjName& name) { m_name = name; }
	
    const cjName& GetLastMatchedName() const { return m_lastMatched; }

protected:
    void SetLastMatchedName(const cjName& name) { m_lastMatched = name; }

protected:
    cjName								m_lastMatched;
    cjName								m_name;
};

cjRefPtrDefine(cjNameMatch)


//
// cjExtraDat
//

typedef cjExtraDat* (*CJExtraDatCreatorType)();


class CJCORE_API cjExtraDat : public cjRef
{
	CJSER_DECL_CLASS_ABSTRACT(cjExtraDat, cjRef,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE 
	CJSER_DECL_CLONE_ABSTRACT(cjExtraDat)
	
public:
	enum ESERID
	{
			// core		
		SERID_UNKNOWN					= 0,
		SERID_CORE_EXTDAT_START			= 1,
		SERID_BOOL						= SERID_CORE_EXTDAT_START,	// cjExtDatBool
		SERID_INT,						// cjExtDatInt
		SERID_STR,						// cjExtDatStr
		SERID_ANINOTE,					// cjExtDatAniNote
		SERID_BINARY,					// cjExtDatBinary
		SERID_VECTOR,					// cjExtDatVector
		SERID_MATRIX,					// cjExtDatMatrix			

			// engine
		SERID_ENGINE_EXTDAT_START		= 101,
		SERID_COLOR						= SERID_ENGINE_EXTDAT_START, // cjExtDatColor

		SERID_MAX						= 255,
	};
	
		// serialize stuff
	static cjSerializer& SerializeAry(cjSerializer& ser, cjExtraDat*** d, cjINT32* count=0);

	static cjExtraDat* CreateFromSerClsID(cjBYTE serClsID)
	{
		if(serClsID > cjExtraDat::SERID_MAX)
			return 0;
		return (ms_creator[serClsID])();
	}

	virtual cjBYTE GetSerClsID() { return SERID_UNKNOWN; }
	static void RegisterCreator(cjINT32 id, CJExtraDatCreatorType creator)
		{	if(id < SERID_MAX) ms_creator[id] = creator; }

	const cjName& GetName() const { return m_name; }
	void SetName(const cjName& name) { m_name = name; }
#ifdef CJUNICODE
	void SetName(const cjChar* name);
#endif

public:
	static void* AllocMem(cjMemSizT siz) { return cjMemAlloc(siz); }
	static void DeAllocMem(void* x) { if(x) cjMemDeAlloc(x); }

	static void StaticInit();
	static void StaticShutdown();

protected:
	cjExtraDat(const cjName& name) : m_name(name) {}

protected:
	cjName								m_name;

private:
	static CJExtraDatCreatorType		ms_creator[SERID_MAX];
};

inline cjExtraDat::cjExtraDat() : m_name(cjName_None) {}
inline cjExtraDat::~cjExtraDat() {}


//cjRefPtrDefine(cjExtraDat);


//
// cjBool
//

class CJCORE_API cjExtDatBool : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatBool, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatBool)
	CJSER_DECL_SERIALIZE

public:
	enum { EXTDAT_SERID = SERID_BOOL };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }

public:
	cjExtDatBool() {}
	cjExtDatBool(cjBool val) : m_value(val) {}

	inline void SetValue(cjBool val) { m_value = val; }
	inline cjBool GetValue(void) const { return m_value; }

	static void StaticInit();

private:
	cjBool							m_value;
};

inline cjExtDatBool::~cjExtDatBool() {}

cjRefPtrDefine(cjExtDatBool);


//
// int
//

class CJCORE_API cjExtDatInt : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatInt, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatInt)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_INT };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }
	
public:
	cjExtDatInt() {}
	cjExtDatInt(cjINT32 val) : m_value(val) { }

	inline void SetValue(cjINT32 val) { m_value = val; }
	inline cjINT32 GetValue(void) const { return m_value; }

	static void StaticInit();

private:
	cjINT32							m_value;
};

inline cjExtDatInt::~cjExtDatInt() {}

cjRefPtrDefine(cjExtDatInt);


//
// string
//

class CJCORE_API cjExtDatStr : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatStr, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatStr)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_STR };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }	

public:
	cjExtDatStr() { m_numRow = 0; m_buf = 0; }

#ifdef CJUNICODE
	cjExtDatStr(const cjChar* str);
#endif
	cjExtDatStr(const cjNameChar* str) { CreateSingleString(str); }
	
	void SetMultiString(const cjNameChar** strAry, cjINT32 numRow) 
		{ CreateMultiString(strAry, numRow); }
	cjBool SetString(const cjNameChar* str, cjINT32 row=0);

	const cjNameChar* GetValue(cjINT32 row=0) const;

	cjINT32 GetValueRowCount() { return m_numRow; }

	static void StaticInit();

private:
	void CreateSingleString(const cjNameChar* str);
	void CreateMultiString(const cjNameChar** strAry, cjINT32 numRow);
	void CleanString();

private:
	cjINT32								m_numRow;	  // 0 for single string,  >0 : multi row string
	cjNameChar**						m_buf;		  // m_buf = cjNameChar*[row]
};

cjRefPtrDefine(cjExtDatStr);


//
// aniNote
//

class CJCORE_API cjExtDatAniNote : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatAniNote, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatAniNote)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_ANINOTE };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }
	
public:
	cjExtDatAniNote();
	cjExtDatAniNote(cjAniNote* noteAry, cjINT32 numNoteInAry)
		: m_noteAry(noteAry), m_numNotes(numNoteInAry) {}
	
	void SetNotes(cjAniNote* noteAry, cjINT32 numNoteInAry)
		{ m_noteAry = noteAry, m_numNotes = numNoteInAry; }

	cjAniNote* GetNoteAry(cjINT32* rstNumNotesInAry=cjNULL)
	{ 
		if(rstNumNotesInAry)
			*rstNumNotesInAry = m_numNotes; 

		return m_noteAry;
	}

	static void StaticInit();

protected:
	cjINT32								m_numNotes;
	cjAniNote*							m_noteAry;
};

cjRefPtrDefine(cjExtDatAniNote)


//
// cjBinary Extra Data
//

class CJCORE_API cjExtDatBinary : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatBinary, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatBinary)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_BINARY };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }
	
public:
	cjExtDatBinary() { m_siz = 0; m_binaryDat = NULL; }
	cjExtDatBinary(const cjUINT32 siz, cjBYTE* binaryDat) { SetValue(siz, binaryDat); }
	
	void GetValue(cjUINT32& siz, cjBYTE*& binaryDat) const { siz = m_siz; binaryDat = m_binaryDat; }
	void SetValue(const cjUINT32 siz, cjBYTE* binaryDat);

	static void StaticInit();

protected:	
	cjUINT32							m_siz;
	cjBYTE*								m_binaryDat;	
};

cjRefPtrDefine(cjExtDatBinary);


//
// Vector4
//

class CJCORE_API cjExtDatVector : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatVector, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatVector)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_VECTOR };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }

public:
	cjExtDatVector() {}
	cjExtDatVector(const cjVec2& vec2) { cjMemCpy(m_vec, &vec2, sizeof(cjVec2)); }
	cjExtDatVector(const cjVec3& vec3) { cjMemCpy(m_vec, &vec3, sizeof(cjVec3)); }	
	cjExtDatVector(const cjVec4& vec4) { cjMemCpy(m_vec, &vec4, sizeof(cjVec4)); }

	void SetValue(const cjVec2& rhs) { cjMemCpy(m_vec, &rhs, sizeof(cjVec2)); }
	void SetValue(const cjVec3& rhs) { cjMemCpy(m_vec, &rhs, sizeof(cjVec3)); }
	void SetValue(const cjVec4& rhs) { cjMemCpy(m_vec, &rhs, sizeof(cjVec4)); }
	
	const cjFLOAT* GetValue() const { return m_vec; }
	void SetValue(cjFLOAT* ary, cjINT32 numElement) 
		{ cjMemCpy(m_vec, ary, sizeof(cjFLOAT) * numElement); }

	cjFLOAT GetX() const { return m_vec[0]; }
	cjFLOAT GetY() const { return m_vec[1]; }
	cjFLOAT GetZ() const { return m_vec[2]; }
	cjFLOAT GetW() const { return m_vec[3]; }

	static void StaticInit();

protected:
	cjFLOAT m_vec[4];
};

inline cjExtDatVector::~cjExtDatVector() {}

cjRefPtrDefine(cjExtDatVector);


//
// Matrix(4x4)
//

class CJCORE_API cjExtDatMatrix: public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatMatrix, cjExtraDat,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjExtDatMatrix)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_MATRIX };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }

public:
	cjExtDatMatrix() {}

	void SetRow(cjINT32 row, const cjVec4& v4) { cjMemCpy(&m_mat[row][0], &v4, sizeof(cjVec4)); }
	void SetMatrix(const cjMatrix& mat) { cjMemCpy(m_mat, mat.m, sizeof(cjFLOAT)*4*4); }

	const cjMatrix* GetMatrix() const { return (cjMatrix*)m_mat; }
	const cjFLOAT* GetRow(cjINT32 row) const { return &m_mat[row][0]; }
	const cjVec4& GetRowVec(cjINT32 row) { return *((cjVec4*)&m_mat[row]); }

	static void StaticInit();

protected:
	cjFLOAT						m_mat[4][4];
};

inline cjExtDatMatrix::~cjExtDatMatrix() {}

cjRefPtrDefine(cjExtDatMatrix);


#endif
