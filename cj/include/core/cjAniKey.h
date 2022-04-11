
#ifndef _CJANIKEY_H_
#define _CJANIKEY_H_

/*
   cjAniKey
     cjAniKeyFloat           (FLOATKEY)
       cjAniKeyLinFloat        (LINKEY)
       cjAniKeyBezFloat        (BEZKEY)
       cjAniKeyTCBFloat        (EKT_TCBKEY)
       cjAniKeyStepFloat       (STEPKEY)
     cjAniKeyVec3             (EKVAL_VEC3)
       cjAniKeyLinVec3          (LINKEY)
       cjAniKeyBezVec3          (BEZKEY)
       cjAniKeyTCBVec3          (EKT_TCBKEY)
       cjAniKeyStepVec3         (STEPKEY)
     cjAniKeyRot             (ROTKEY)
       cjAniKeyLinRot          (LINKEY)
       cjAniKeyBezRot          (BEZKEY)
       cjAniKeyTCBRot          (EKT_TCBKEY)
       cjAniKeyEulerRot        (EULERKEY)
       cjAniKeyStepRot         (STEPKEY)
     cjAniKeyColor           (COLORKEY)
       cjAniKeyLinColor        (LINKEY)
       cjAniKeyStepColor       (STEPKEY)
     cjAniKeyText            (TEXTKEY)
     cjAniKeyBool            (BOOLKEY)
       cjAniKeyStepBool       (STEPKEY)
*/


// declare in class header
#define CJ_ANIKEYOPER_DECL \
	public: \
		static cjBool		RegisterAniKeyOperator(); \
		static void			RegisterCustomAniKeyOperation(EKEY_VALTYPE keyValType, EKEY_TYPE keyType); \
		\
		static cjBool		IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount); \
		static cjBool		EqualKey(const cjAniKey& key0, const cjAniKey& key1); \
		\
		static void			SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount); \
		static cjAniKey*	LoadKey(cjFStream& stream, cjUINT32 keyCount); \
		static cjAniKey*	CreateKey(cjUINT32 keyCount); \
		static void			DeleteKey(cjAniKey* keys); \
		\
		static void			CopyKey(cjAniKey* dstKeys, const cjAniKey* srcKeys); \
		static cjBool		InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount); \
		\
		static cjFLOAT		CaclCurveRatioKey(cjAniKey* keys, cjUINT32 keyCount); \
		static void			FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE siz); \
		static void			CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, void* v0, void* vOutTangent0, void* va0, void* vb0); \
		static void			InterpolateKey(cjFLOAT rtime, const cjAniKey* k0, const cjAniKey* k1, void* rst); \
		static void			InterpolateD1Key(cjFLOAT rtime, const cjAniKey* k0, const cjAniKey* k1, void* rst); \
		static void			InterpolateD2Key(cjFLOAT rtime, const cjAniKey* k0, const cjAniKey* k1, void* rst);


	// declare at below of class declare
#define CJ_ANIKEYOPER_REG(CLS_NAME) \
	static cjBool CLS_NAME##_ANIKEYOPER_REG_RST = CLS_NAME::RegisterAniKeyOperator();

	// at class implement start
#define CJ_ANIKEYOPER_IMPL(CLS_NAME, EKEY_VALTYPE, EKEY_TYPE) \
	cjBool CLS_NAME::RegisterAniKeyOperator() \
	{ \
		static cjBool isRegistered = cjFalse; \
		if(isRegistered) \
			return cjFalse; \
		isRegistered = cjTrue; \
		SetSaveKeyPF		(EKEY_VALTYPE, EKEY_TYPE, CLS_NAME::SaveKey);	\
		SetLoadKeyPF		(EKEY_VALTYPE, EKEY_TYPE, CLS_NAME::LoadKey);	\
		SetCreateKeyPF		(EKEY_VALTYPE, EKEY_TYPE, CLS_NAME::CreateKey);	\
		SetDeleteKeyPF		(EKEY_VALTYPE, EKEY_TYPE, CLS_NAME::DeleteKey);	\
		SetCopyKeyPF		(EKEY_VALTYPE, EKEY_TYPE, CLS_NAME::CopyKey);	\
		SetKeySiz			(EKEY_VALTYPE, EKEY_TYPE, sizeof(CLS_NAME));	\
		RegisterCustomAniKeyOperation(EKEY_VALTYPE, EKEY_TYPE); \
		return cjTrue; \
	}


//
// cjAniKey
//

class CJCORE_API cjAniKey : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjAniKey)
		stream << d.m_time;
	CJ_DECL_SERIALIZE_INLINE_END(cjAniKey)

public:
	enum EKEY_VALTYPE
    {
        EKVAL_FLOAT,
        EKVAL_VEC3,
        EKVAL_ROT,
        EKVAL_COLOR,
        EKVAL_TEXT,
        EKVAL_BOOL,

		EKVAL_MAX
    };

	enum EKEY_TYPE
    {
        EKT_NOINTERP,
        EKT_LINKEY,
        EKT_BEZKEY,
        EKT_TCBKEY,
        EKT_EULERKEY,
        EKT_STEPKEY,

		EKT_MAX
    };
	
	enum { KEYOPER_SIZ = EKVAL_MAX * EKT_MAX };


	// function pointer type for animation key operator
	typedef cjBool		(*PFIsPosed)		(const cjAniKey* keys, cjUINT32 keyCount);
    typedef cjBool		(*PFEqual)			(const cjAniKey& key0, const cjAniKey& key1);

    typedef void		(*PFSave)			(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount);
	typedef cjAniKey*	(*PFLoad)			(cjFStream&, cjUINT32);
    typedef cjAniKey*	(*PFCreate)			(cjUINT32 keyCount);
	typedef void		(*PFDelete)			(cjAniKey* keys);
	typedef void		(*PFCopy)			(cjAniKey* dst, const cjAniKey* src);
	typedef cjBool		(*PFInsert)			(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount);

    typedef cjFLOAT		(*PFCurvedRatio)	(cjAniKey* keys, cjUINT32 keyCount);

	// generate m_dd, m_df, m_a, m_b for TCB from m_value
	typedef void		(*PFFillDerivedVals)(cjAniKey* keys, cjUINT32 keyCount, cjBYTE Siz);        

    typedef void		(*PFCubicCoefs)		(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, void* val0, void* outTangent0, void* va0, void* vb0);
    typedef void		(*PFInterpolation)	(cjFLOAT etime, const cjAniKey* key0, const cjAniKey* key1, void* rst);
	
public:
    ~cjAniKey() {}

    void SetTime(cjFLOAT etime) { m_time = etime; }
    cjFLOAT GetTime() const { return m_time; }
    
    cjAniKey*					GetKey(cjUINT32 index, cjBYTE keySiz) const
									{ return (cjAniKey*) ((cjSBYTE*)this + index * keySiz); }
    static cjBYTE				GetKeySiz(EKEY_VALTYPE valType, EKEY_TYPE keytype)
									{ return ms_keysiz[valType * EKT_MAX + keytype]; } //cjneed resolve valType*EKT_MAX

	static void					CloneKeysByRange(EKEY_VALTYPE valType, EKEY_TYPE keyType, const cjAniKey* srcKeys, cjUINT32 srcKeyCount, 
									cjFLOAT srcStTime, cjFLOAT srcEnTime, cjAniKey*& dstKeys, cjUINT32& dstKeyCount);
    static void					RecursiveGetKeysByRange(EKEY_VALTYPE valType, EKEY_TYPE keyType, cjAniKey*& rstKeys, 
									cjUINT32& rstKeyCount, cjFLOAT stTime, cjFLOAT enTime);

	// operators
    static PFIsPosed			GetIsPosedKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);    
    static PFEqual				GetEqualKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    
	static PFSave				GetSaveKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFLoad				GetLoadKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);	
	static PFCreate				GetCreateKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFDelete				GetDeleteKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);
	static PFCopy				GetCopyKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFInsert				GetInsertKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);

    static PFCurvedRatio		GetCurvedRatioKeyPF	(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFFillDerivedVals	GetFillDerivedValsKeyPF (EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFCubicCoefs			GetCubicCoefsKeyPF	(EKEY_VALTYPE valType, EKEY_TYPE keyType);
	static PFInterpolation		GetInterpKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFInterpolation		GetInterpD1KeyPF	(EKEY_VALTYPE valType, EKEY_TYPE keyType);
    static PFInterpolation		GetInterpD2KeyPF	(EKEY_VALTYPE valType, EKEY_TYPE keyType);    
	
protected:
	cjAniKey() {}
	cjAniKey(cjFLOAT etime) { m_time = etime; };

	static cjBool TestKeyInsertable(cjFLOAT etime, const cjAniKey* keys, cjUINT32 keyCount, cjBYTE keySiz, cjUINT32& insertIndex);

	// operators
    static void SetIsPosedKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFIsPosed			pfIsPosed);
	static void SetEqualKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFEqual			pfEqual);
	
	static void SetSaveKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFSave			pfSave);
    static void SetLoadKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFLoad			pfLoad);
	static void SetCreateKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFCreate			pfCreate);
    static void SetDeleteKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFDelete			pfDelete);
    static void SetCopyKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFCopy			pfCopy);
    static void SetInsertKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFInsert			pfInsert);

    static void SetCurvedRatioKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFCurvedRatio		pfCurvedRatio);
    static void SetFillDerivedValsKeyPF (EKEY_VALTYPE valType, EKEY_TYPE keyType, PFFillDerivedVals	pfFillDerivedVals);
	static void SetCubicCoefsKeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFCubicCoefs		pfCubicCoefs);
	static void SetInterpKeyPF			(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFInterpolation	pfInterp);
    static void SetInterpD1KeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFInterpolation	pfInterp);
    static void SetInterpD2KeyPF		(EKEY_VALTYPE valType, EKEY_TYPE keyType, PFInterpolation	pfInterp);
	
    static void SetKeySiz				(EKEY_VALTYPE valType, EKEY_TYPE keyType, cjBYTE keySiz)
											{ ms_keysiz[valType * EKT_MAX + keyType] = keySiz; }

protected:
	// operators - default implementation
	static cjBool		IsPosedKey			(const cjAniKey*, cjUINT32) { cjAssert(cjFalse); return cjFalse; }
	static cjBool		EqualKey			(const cjAniKey& k0, const cjAniKey& k1) { return k0.m_time == k1.m_time; }
	
	static void			SaveKey				(cjFStream& , cjAniKey* , cjUINT32 ) { cjAssert(cjFalse); }
	static cjAniKey*	LoadKey				(cjFStream& , cjUINT32 ) {  cjAssert(cjFalse); return 0; }
	
	static cjAniKey*	CreateKey			(cjUINT32) { cjAssert(cjFalse); return 0; }
	static void			DeleteKey			(cjAniKey*) { cjAssert(cjFalse); }
	static void			CopyKey				(cjAniKey* , const cjAniKey* ) { cjAssert(cjFalse); }
	static cjBool		InsertKey			(cjFLOAT , cjAniKey*& , cjUINT32& ) { cjAssert(cjFalse); return cjFalse;}

	static cjFLOAT		CaclCurveRatioKey	(cjAniKey* , cjUINT32 ) { cjAssert(cjFalse); return 0.0f; }
	static void			FillDerivedValsKey	(cjAniKey*, cjUINT32, cjBYTE ) { cjAssert(cjFalse); }
	static void			CubicCoefsKey		(const cjAniKey*, cjUINT32, cjUINT32, void*, void*, void*, void* ) { cjAssert(cjFalse); }
	static void			InterpolateKey		(cjFLOAT, const cjAniKey*, const cjAniKey*, void*) { cjAssert(cjFalse); }

protected:
	static cjBYTE						ms_keysiz		[KEYOPER_SIZ];

	// operators
	static PFIsPosed					ms_isposedPF	[KEYOPER_SIZ];
	static PFEqual						ms_equalPF		[KEYOPER_SIZ];
														
    static PFSave						ms_savePF		[KEYOPER_SIZ];
    static PFLoad						ms_loadPF		[KEYOPER_SIZ];
    static PFCreate						ms_createPF		[KEYOPER_SIZ];
	static PFDelete						ms_deletePF		[KEYOPER_SIZ];
    static PFCopy						ms_copyPF		[KEYOPER_SIZ];
	static PFInsert						ms_insertPF		[KEYOPER_SIZ];

	static PFCurvedRatio				ms_curvePF		[KEYOPER_SIZ];
	static PFFillDerivedVals			ms_fillderivedPF[KEYOPER_SIZ];
	static PFCubicCoefs					ms_cubiccoefsPF	[KEYOPER_SIZ];
    static PFInterpolation				ms_interpPF		[KEYOPER_SIZ];
    static PFInterpolation				ms_interpd1PF	[KEYOPER_SIZ];
    static PFInterpolation				ms_interpd2PF	[KEYOPER_SIZ];  

public:
    cjFLOAT								m_time;
};


//
// cjAniKeyFloat
//

class CJCORE_API cjAniKeyFloat : public cjAniKey
{
    CJ_ANIKEYOPER_DECL

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyFloat, cjAniKey)
		stream << d.m_value;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyFloat, cjAniKey)

public:
    void SetValue(cjFLOAT value) { m_value = value; }
	cjFLOAT GetValue() const { return m_value; }

    static cjFLOAT GenInterp(cjFLOAT etime, cjAniKeyFloat* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz);
    static void GenCubicCoefs(cjFLOAT etime, cjAniKeyFloat* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
    cjFLOAT& val0, cjFLOAT& outTan, cjFLOAT& valA, cjFLOAT& valB);
    
    cjAniKeyFloat* GetKey(cjUINT32 index, cjBYTE keyCount);
    static cjBYTE GetKeySiz(EKEY_TYPE keyType)  { return cjAniKey::GetKeySiz(EKVAL_FLOAT, keyType); }
    static void SetDefault(cjFLOAT valDefault)  { ms_default = valDefault; }
	
	//
	// operators
	//
	static PFIsPosed			GetIsPosedKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetIsPosedKeyPF(		EKVAL_FLOAT, keyType); }
    static PFEqual				GetEqualKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetEqualKeyPF(		EKVAL_FLOAT, keyType); }
																												
    static PFSave				GetSaveKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetSaveKeyPF(		EKVAL_FLOAT, keyType); }
	static PFLoad				GetLoadKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetLoadKeyPF(		EKVAL_FLOAT, keyType); }
    static PFCreate				GetCreateKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetCreateKeyPF(		EKVAL_FLOAT, keyType); }
    static PFDelete				GetDeleteKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetDeleteKeyPF(		EKVAL_FLOAT, keyType); }
    static PFCopy				GetCopyKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetCopyKeyPF(		EKVAL_FLOAT, keyType); }
    static PFInsert				GetInsertKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetInsertKeyPF(		EKVAL_FLOAT, keyType); }	

    static PFFillDerivedVals	GetFillDerivedValsKeyPF(EKEY_TYPE keyType) { return cjAniKey::GetFillDerivedValsKeyPF(EKVAL_FLOAT, keyType); }
    static PFCubicCoefs			GetCubicCoefsKeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetCubicCoefsKeyPF(	EKVAL_FLOAT, keyType); }
    static PFInterpolation		GetInterpKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetInterpKeyPF(		EKVAL_FLOAT, keyType); }
	
protected:
    cjAniKeyFloat() {}
    cjAniKeyFloat(cjFLOAT etime, cjFLOAT value) : cjAniKey(etime) , m_value(value) {}
    static cjFLOAT GenInterpDefault(cjFLOAT etime, cjAniKeyFloat* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjBYTE siz);

protected:
    cjFLOAT									m_value;
    static cjFLOAT							ms_default;
};

inline void			cjAniKeyFloat::SaveKey(cjFStream&, cjAniKey*, cjUINT32) { cjAssert(0); }
inline cjAniKey*	cjAniKeyFloat::LoadKey(cjFStream&, cjUINT32 )			{ cjAssert(0); return cjNULL; }
inline cjAniKey*	cjAniKeyFloat::CreateKey(cjUINT32)						{ cjAssert(0); return cjNULL; }
inline void			cjAniKeyFloat::DeleteKey(cjAniKey*)						{ cjAssert(0); }
inline void			cjAniKeyFloat::CopyKey(cjAniKey* , const cjAniKey* )	{ cjAssert(0); }
					
inline void			cjAniKeyFloat::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE) { cjAssert(cjFalse); }					
inline void			cjAniKeyFloat::InterpolateKey(cjFLOAT, const cjAniKey*, const cjAniKey*, void* )  { cjAssert(0); }

inline cjAniKeyFloat* cjAniKeyFloat::GetKey(cjUINT32 index, cjBYTE keySiz) { return (cjAniKeyFloat*)cjAniKey::GetKey(index, keySiz); }


CJ_ANIKEYOPER_REG(cjAniKeyFloat);


//
// cjAniKeyFloat
//   - cjAniKeyLinFloat
//

class CJCORE_API cjAniKeyLinFloat : public cjAniKeyFloat
{
    CJ_ANIKEYOPER_DECL;

public:
	cjAniKeyLinFloat() {}
    cjAniKeyLinFloat(cjFLOAT etime, cjFLOAT v) : cjAniKeyFloat(etime, v) {}
};

// custom Inline AniKey Operation
inline cjBool cjAniKeyLinFloat::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    return cjAniKeyFloat::EqualKey(key0, key1);
}

inline void cjAniKeyLinFloat::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE)
{
}

CJ_ANIKEYOPER_REG(cjAniKeyLinFloat);


//
// cjAniKeyFloat
//   - cjAniKeyBezFloat
//

class CJCORE_API cjAniKeyBezFloat : public cjAniKeyFloat
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyBezFloat, cjAniKeyFloat)
		stream << d.m_inTan;
		stream << d.m_outTan;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyBezFloat, cjAniKeyFloat)

public:     
	cjAniKeyBezFloat() {}
    cjAniKeyBezFloat(cjFLOAT etime, cjFLOAT val, cjFLOAT inTan, cjFLOAT outTan) 
		: cjAniKeyFloat(etime, val)
	{
		m_inTan = inTan;
		m_outTan = outTan;
	}
    
    inline void SetInTan(cjFLOAT inTangent) { m_inTan = inTangent; }
    inline void SetOutTan(cjFLOAT outTangent) { m_outTan = outTangent; }
    inline cjFLOAT GetInTan() const { return m_inTan; }
    inline cjFLOAT GetOutTan() const { return m_outTan; }

protected:
    cjFLOAT							m_inTan;
    cjFLOAT							m_outTan;
};

inline void cjAniKeyBezFloat::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE ) 
{
}

CJ_ANIKEYOPER_REG(cjAniKeyBezFloat);


//
// cjAniKeyFloat
//   - cjAniKeyTCBFloat
//

class CJCORE_API cjAniKeyTCBFloat : public cjAniKeyFloat
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyTCBFloat, cjAniKeyFloat)
		stream << d.m_tension;
		stream << d.m_continuity;
		stream << d.m_bias;
		stream << d.m_diffSrc;
		stream << d.m_diffDst;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyTCBFloat, cjAniKeyFloat)

public: 
	cjAniKeyTCBFloat() {}
    cjAniKeyTCBFloat(cjFLOAT etime, cjFLOAT value, cjFLOAT tension, cjFLOAT continuity, cjFLOAT bias) 
		: cjAniKeyFloat(etime, value)
	{
		m_tension = tension;
		m_continuity = continuity;
		m_bias = bias;
	}

    void SetTension (cjFLOAT tension) { m_tension = tension; }
    void SetContinuity (cjFLOAT continuity) { m_continuity = continuity; }
    void SetBias (cjFLOAT bias) { m_bias = bias; }
    cjFLOAT GetTension () const { return m_tension; }
    cjFLOAT GetContinuity () const { return m_continuity; }
    cjFLOAT GetBias () const { return m_bias; }
    
protected:
    static void CalculateDiffVals(cjAniKeyTCBFloat& rst, cjFLOAT valSub1, cjFLOAT valPlus1, 
		cjFLOAT preLen, cjFLOAT nextLen);

	// default tension=0, continuity=0, bias=0.
    cjFLOAT							m_tension;
    cjFLOAT							m_continuity;
    cjFLOAT							m_bias;

    cjFLOAT							m_diffSrc;
    cjFLOAT							m_diffDst;
};

CJ_ANIKEYOPER_REG(cjAniKeyTCBFloat);


//
// cjAniKeyVec3
//

class CJCORE_API cjAniKeyVec3 : public cjAniKey
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyVec3, cjAniKey)
		stream << d.m_value;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyVec3, cjAniKey)

public:    
    void SetValue(const cjVec3& vec3) { m_value = vec3; }
    const cjVec3& GetValue() const { return m_value; }
    
    static cjVec3 GenInterp(cjFLOAT etime, cjAniKeyVec3* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz);
	    
    static void GenCubicCoefs(cjFLOAT etime, cjAniKeyVec3* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1,
		cjVec3& val0, cjVec3& outTan, cjVec3& valA, cjVec3& valB);

    static void SetMaxCurvatureSampleSiz(cjINT32 sampleSiz);
    static cjFLOAT GetMaxCurvatureSampleDelta();

    cjAniKeyVec3* GetKey(cjUINT32 index, cjBYTE keyCount);
    static cjBYTE GetKeySiz(EKEY_TYPE keyType) { return cjAniKey::GetKeySiz(EKVAL_VEC3, keyType); }
    static void SetDefault(const cjVec3& valDefault)  { ms_default = valDefault; }

	//
	// operators
	//
    static PFIsPosed			GetIsPosedKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetIsPosedKeyPF(		EKVAL_VEC3, keyType); }
	static PFEqual				GetEqualKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetEqualKeyPF(		EKVAL_VEC3, keyType); }
																												
	static PFSave				GetSaveKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetSaveKeyPF(		EKVAL_VEC3, keyType); }
    static PFLoad				GetLoadKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetLoadKeyPF(		EKVAL_VEC3, keyType); }
    static PFCreate				GetCreateKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetCreateKeyPF(		EKVAL_VEC3, keyType); }
    static PFDelete				GetDeleteKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetDeleteKeyPF(		EKVAL_VEC3, keyType); }
	static PFCopy				GetCopyKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetCopyKeyPF(		EKVAL_VEC3, keyType); }
	static PFInsert				GetInsertKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetInsertKeyPF(		EKVAL_VEC3, keyType); }

    static PFCurvedRatio		GetCurvedRatioKeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetCurvedRatioKeyPF(	EKVAL_VEC3, keyType); }
    static PFFillDerivedVals	GetFillDerivedValsKeyPF(EKEY_TYPE keyType) { return cjAniKey::GetFillDerivedValsKeyPF(EKVAL_VEC3, keyType); }
	static PFCubicCoefs			GetCubicCoefsKeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetCubicCoefsKeyPF(	EKVAL_VEC3, keyType); }
    static PFInterpolation		GetInterpKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetInterpKeyPF(		EKVAL_VEC3, keyType); }
	static PFInterpolation		GetInterpD1KeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetInterpD1KeyPF(	EKVAL_VEC3, keyType); }
    static PFInterpolation		GetInterpD2KeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetInterpD2KeyPF(	EKVAL_VEC3, keyType); }

	//
	// cjAniKeyVec3 specialize
	//
    static void CoordinateFrame(cjFLOAT etime, const cjAniKeyVec3* key0, const cjAniKeyVec3* key1, EKEY_TYPE keyType, 
		cjVec3& tangent, cjVec3& normal, cjVec3& binormal, cjFLOAT& curvature);

protected:
    cjAniKeyVec3() {}
    cjAniKeyVec3(cjFLOAT etime, const cjVec3& pos) : cjAniKey(etime), m_value(pos) {}
    static cjVec3 GenInterpDefault(cjFLOAT etime, cjAniKeyVec3* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjBYTE siz);

protected:
	// curvature sampling size
    static cjINT32							ms_sampleSiz;
    static cjFLOAT							ms_sampleDelta;

    static cjVec3							ms_default;

public:
	cjVec3									m_value;
};

inline void			cjAniKeyVec3::SaveKey(cjFStream&, cjAniKey*, cjUINT32)  { cjAssert(0); }
inline cjAniKey*	cjAniKeyVec3::LoadKey(cjFStream&, cjUINT32 )			{ cjAssert(0); return cjNULL; }
inline cjAniKey*	cjAniKeyVec3::CreateKey(cjUINT32)						{ cjAssert(0); return cjNULL; }
inline void			cjAniKeyVec3::DeleteKey(cjAniKey*)						{ cjAssert(0); }
inline void			cjAniKeyVec3::CopyKey(cjAniKey* , const cjAniKey* )		{ cjAssert(0); }

inline cjFLOAT		cjAniKeyVec3::CaclCurveRatioKey(cjAniKey*, cjUINT32)	{ cjAssert(cjFalse); return 0.0f; }
inline void			cjAniKeyVec3::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE) { cjAssert(cjFalse); }					

inline void			cjAniKeyVec3::InterpolateKey(cjFLOAT /*rtime*/, const cjAniKey*, const cjAniKey*, void* rst)    { cjAssert(0); *(cjVec3*)rst = cjVec3::ZERO; }
inline void			cjAniKeyVec3::InterpolateD1Key(cjFLOAT , const cjAniKey*, const cjAniKey*, void* rst)  { cjAssert(0); *(cjVec3*)rst = cjVec3::ZERO; }
inline void			cjAniKeyVec3::InterpolateD2Key(cjFLOAT , const cjAniKey*, const cjAniKey*, void* rst)  { cjAssert(0); *(cjVec3*)rst = cjVec3::ZERO; }

inline cjAniKeyVec3* cjAniKeyVec3::GetKey(cjUINT32 index, cjBYTE keySiz) { return (cjAniKeyVec3*)cjAniKey::GetKey(index, keySiz); }


CJ_ANIKEYOPER_REG(cjAniKeyVec3);


//
// cjAniKeyVec3 
//    - cjAniKeyLinVec3
//

class CJCORE_API cjAniKeyLinVec3 : public cjAniKeyVec3
{
    CJ_ANIKEYOPER_DECL;

public:
    cjAniKeyLinVec3() {}
    cjAniKeyLinVec3(cjFLOAT etime, const cjVec3& pos) : cjAniKeyVec3(etime, pos) {}
		
};

// custom Inline AniKey Operation
inline cjBool cjAniKeyLinVec3::EqualKey(const cjAniKey& key0, const cjAniKey& key1) { return cjAniKeyVec3::EqualKey(key0,key1); }
inline cjFLOAT cjAniKeyLinVec3::CaclCurveRatioKey(cjAniKey*, cjUINT32) { return 0.0f; }
inline void cjAniKeyLinVec3::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE) {}

CJ_ANIKEYOPER_REG(cjAniKeyLinVec3);


//
// cjAniKeyVec3 
//    - cjAniKeyBezVec3
//

class CJCORE_API cjAniKeyBezVec3 : public cjAniKeyVec3
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyBezVec3, cjAniKeyVec3)
		stream << d.m_inTan;
		stream << d.m_outTan;
		stream << d.m_a;
		stream << d.m_b;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyBezVec3, cjAniKeyVec3)

public:
	cjAniKeyBezVec3() {}
    cjAniKeyBezVec3(cjFLOAT etime, const cjVec3& pos, const cjVec3& inTan,
        const cjVec3& outTan) : cjAniKeyVec3(etime, pos), m_inTan(inTan), m_outTan(outTan) { }

    void SetInTan(const cjVec3& inTan) { m_inTan = inTan; }
    void SetOutTan(const cjVec3& outTan) { m_outTan = outTan; }
    const cjVec3& GetInTan() const { return m_inTan; }
    const cjVec3& GetOutTan() const { return m_outTan; }
    
protected:
    cjVec3							m_inTan;
    cjVec3							m_outTan;
    cjVec3							m_a;
    cjVec3							m_b;
};

CJ_ANIKEYOPER_REG(cjAniKeyBezVec3);


//
// cjAniKeyVec3 
//    - cjAniKeyTCBVec3
//

class CJCORE_API cjAniKeyTCBVec3 : public cjAniKeyVec3
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyTCBVec3, cjAniKeyVec3)
		stream << d.m_tension;
		stream << d.m_continuity;
		stream << d.m_bias;

		//cjdefer
		// skip m_DS, m_DD, m_a, m_b
		//stream << d.m_DS;
		//stream << d.m_DD;

		//stream << d.m_a;
		//stream << d.m_b;

		// m_DS, m_DD : cjAniKeyTCBVec3::FillDerivedValsKey , tcbVec3Keys[0].CalculateDiffVals
		// m_a, m_b   : cjAniKeyTCBVec3::FillDerivedValsKey

	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyTCBVec3, cjAniKeyVec3)

public:
	cjAniKeyTCBVec3() {}
    cjAniKeyTCBVec3(cjFLOAT etime, const cjVec3& pos, cjFLOAT tension,
        cjFLOAT continuity, cjFLOAT bias) : cjAniKeyVec3(etime,pos)
	{
		m_tension = tension;
		m_continuity = continuity;
		m_bias = bias;
	}

    void SetTension (cjFLOAT tension) { m_tension = tension; }
    void SetContinuity (cjFLOAT continuity) { m_continuity = continuity; }
    void SetBias (cjFLOAT bias) { m_bias = bias; }
    cjFLOAT GetTension () const { return m_tension; }
    cjFLOAT GetContinuity () const { return m_continuity; }
    cjFLOAT GetBias () const { return m_bias; }

protected:
    void CalculateDiffVals(const cjVec3& ptSub1, const cjVec3& ptPlus1, cjFLOAT fPreLen, cjFLOAT fNextLen);

    cjFLOAT							m_tension;
    cjFLOAT							m_continuity;
    cjFLOAT							m_bias;

    cjVec3							m_DS; // diff src
    cjVec3							m_DD; // diff dst

    cjVec3							m_a;
    cjVec3							m_b;
};

CJ_ANIKEYOPER_REG(cjAniKeyTCBVec3);


//
// cjAniKeyRot
//

class CJCORE_API cjAniKeyRot : public cjAniKey
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyRot, cjAniKey)
		stream << d.m_value;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyRot, cjAniKey)

public:
    void SetValue(const cjQuat& quat) { m_value = quat; }
    const cjQuat& GetValue() const { return m_value; }

    static cjQuat GenInterp (cjFLOAT etime, cjAniKeyRot* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz);
    static void GenInterp (cjFLOAT etime, cjAniKeyRot* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjQuat* rst);
    static void GenCubicCoefs(cjFLOAT rtime, cjAniKeyRot* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
		cjQuat& quat0, cjQuat& quat1, cjQuat& outQuat0, cjQuat& inQuat1);

    cjAniKeyRot* GetKey(cjUINT32 index, cjBYTE keyCount);
    static cjBYTE GetKeySiz(EKEY_TYPE keyType) { return cjAniKey::GetKeySiz(EKVAL_ROT, keyType); }
    static void SetDefault(const cjQuat& valDefault)  { ms_default = valDefault; }

	//
	// operators
	//
    static PFIsPosed			GetIsPosedKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetIsPosedKeyPF(		EKVAL_ROT, keyType); }
	static PFEqual				GetEqualKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetEqualKeyPF(		EKVAL_ROT, keyType); }
																												
	static PFSave				GetSaveKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetSaveKeyPF(		EKVAL_ROT, keyType); }
    static PFLoad				GetLoadKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetLoadKeyPF(		EKVAL_ROT, keyType); }
    static PFCreate				GetCreateKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetCreateKeyPF(		EKVAL_ROT, keyType); }
    static PFDelete				GetDeleteKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetDeleteKeyPF(		EKVAL_ROT, keyType); }
    static PFCopy				GetCopyKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetCopyKeyPF(		EKVAL_ROT, keyType); }
    static PFInsert				GetInsertKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetInsertKeyPF(		EKVAL_ROT, keyType); }

    static PFFillDerivedVals	GetFillDerivedValsKeyPF(EKEY_TYPE keyType) { return cjAniKey::GetFillDerivedValsKeyPF(EKVAL_ROT, keyType); }
	static PFCubicCoefs			GetCubicCoefsKeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetCubicCoefsKeyPF(	EKVAL_ROT, keyType); }
    static PFInterpolation		GetInterpKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetInterpKeyPF(		EKVAL_ROT, keyType); }

protected:
    cjAniKeyRot() : m_value(0,0,0,0) {}
	cjAniKeyRot(cjFLOAT etime, const cjQuat& quat) : cjAniKey(etime), m_value(quat) {}
    cjAniKeyRot(cjFLOAT etime, cjFLOAT angle, const cjVec3& axis) : cjAniKey(etime), m_value(angle, axis) {}
    static cjQuat GenInterpDefault(cjFLOAT etime, cjAniKeyRot* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjBYTE siz);
	
public:
    cjQuat									m_value;
    static cjQuat							ms_default;
};

inline void			cjAniKeyRot::SaveKey(cjFStream&, cjAniKey*, cjUINT32)	{ cjAssert(0); }
inline cjAniKey*	cjAniKeyRot::LoadKey(cjFStream&, cjUINT32 )				{ cjAssert(0); return cjNULL; }
inline cjAniKey*	cjAniKeyRot::CreateKey(cjUINT32)						{ cjAssert(0); return cjNULL; }
inline void			cjAniKeyRot::DeleteKey(cjAniKey*)						{ cjAssert(0); }
inline void			cjAniKeyRot::CopyKey(cjAniKey* , const cjAniKey* )		{ cjAssert(0); }

inline cjAniKeyRot* cjAniKeyRot::GetKey(cjUINT32 index, cjBYTE keySiz) { return (cjAniKeyRot*)cjAniKey::GetKey(index, keySiz); }

CJ_ANIKEYOPER_REG(cjAniKeyRot);


//
// cjAniKeyRot
//   - cjAniKeyLinRot
//

class CJCORE_API cjAniKeyLinRot : public cjAniKeyRot
{
    CJ_ANIKEYOPER_DECL;

public:
    cjAniKeyLinRot() {}
    cjAniKeyLinRot(cjFLOAT etime, const cjQuat& quat) : cjAniKeyRot(etime, quat) {}
	cjAniKeyLinRot(cjFLOAT etime, cjFLOAT angle, const cjVec3& axis) : cjAniKeyRot(etime, angle, axis) {}

};

// inline custom operation
inline cjBool cjAniKeyLinRot::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    return cjAniKeyRot::EqualKey(key0,key1);
}

inline void cjAniKeyLinRot::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE keySiz)
{
    cjAniKeyRot::FillDerivedValsKey(keys, keyCount, keySiz);
}


CJ_ANIKEYOPER_REG(cjAniKeyLinRot);


//
// cjAniKeyRot
//   - cjAniKeyBezRot
//

class CJCORE_API cjAniKeyBezRot : public cjAniKeyRot
{
    CJ_ANIKEYOPER_DECL;

//	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyBezRot, cjAniKeyRot)
//	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyBezRot, cjAniKeyRot)
	
public:    
	cjAniKeyBezRot() {}
    cjAniKeyBezRot(cjFLOAT etime, cjFLOAT angle, const cjVec3& axis) : cjAniKeyRot(etime,angle,axis) {}
    cjAniKeyBezRot(cjFLOAT etime, const cjQuat& quat) : cjAniKeyRot(etime, quat) {}
    
protected:
    cjQuat							m_intermediateQuat; // for interpolation, do not need serialize
};

CJ_ANIKEYOPER_REG(cjAniKeyBezRot);


//
// cjAniKeyRot
//   - cjAniKeyTCBRot
//

class CJCORE_API cjAniKeyTCBRot : public cjAniKeyRot
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyTCBRot, cjAniKeyRot)		
		stream << d.m_tension;
		stream << d.m_continuity;
		stream << d.m_bias;

		stream << d.m_a;
		stream << d.m_b;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyTCBRot, cjAniKeyRot)

public:    
	cjAniKeyTCBRot() {}
    cjAniKeyTCBRot(cjFLOAT etime, cjFLOAT angle, const cjVec3& axis) : cjAniKeyRot(etime,angle,axis) {}
    cjAniKeyTCBRot(cjFLOAT etime, const cjQuat& quat) : cjAniKeyRot(etime,quat) {}
        
    void SetTension(cjFLOAT tension) { m_tension = tension; }
    void SetContinuity(cjFLOAT continuity) { m_continuity = continuity; }
    void SetBias(cjFLOAT bias) { m_bias = bias; }

    cjFLOAT GetTension() const { return m_tension; }
    cjFLOAT GetContinuity() const { return m_continuity; }
    cjFLOAT GetBias() const { return m_bias; }
    
protected:
    void CalculateDiffVals(const cjAniKeyTCBRot* pPrev, const cjAniKeyTCBRot* pNext);

    cjFLOAT							m_tension;
    cjFLOAT							m_continuity;
    cjFLOAT							m_bias;

    cjQuat							m_a;
    cjQuat							m_b;
};

CJ_ANIKEYOPER_REG(cjAniKeyTCBRot);


//
// cjAniKeyRot
//   - cjAniKeyEulerRot
//

class CJCORE_API cjAniKeyEulerRot : public cjAniKeyRot
{
    CJ_ANIKEYOPER_DECL;

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyEulerRot, cjAniKeyRot)

		stream << d.m_keyCount[0] << d.m_keyCount[1] << d.m_keyCount[2];

		cjFStreamEnum(stream, d.m_keyType[0]);
		cjFStreamEnum(stream, d.m_keyType[1]);
		cjFStreamEnum(stream, d.m_keyType[2]);

		stream << d.m_keySiz[0]   << d.m_keySiz[1]   << d.m_keySiz[2];
		stream << d.m_lastIdx[0]  << d.m_lastIdx[1]  << d.m_lastIdx[2];

		cjAniKeyFloat::PFSave pfSave = cjNULL;
		cjAniKeyFloat::PFLoad pfLoad = cjNULL;

		for(cjINT32 i=0; i<3; i++)
		{
			if(stream.IsLoading())
				d.m_keys[i] = cjNULL;

			if(!d.m_keyCount[i])
				continue;

			if(stream.IsLoading())
			{
				pfLoad = cjAniKeyFloat::GetLoadKeyPF(d.m_keyType[i]);
				d.m_keys[i] = (cjAniKeyFloat*)pfLoad(stream, d.m_keyCount[i]);
			}
			else if(stream.IsSaving())
			{
				pfSave = cjAniKeyFloat::GetSaveKeyPF(d.m_keyType[i]);
				pfSave(stream, d.m_keys[i], d.m_keyCount[i]);
			}
		}	 	
		
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyEulerRot, cjAniKeyRot)

public:
    cjAniKeyEulerRot();

    cjAniKeyEulerRot(
		cjAniKeyFloat* keysX, cjUINT32 keyCountX, cjAniKeyFloat::EKEY_TYPE keyTypeX,
		cjAniKeyFloat* keysY, cjUINT32 keyCountY, cjAniKeyFloat::EKEY_TYPE keyTypeY, 
		cjAniKeyFloat* keysZ, cjUINT32 keyCountZ, cjAniKeyFloat::EKEY_TYPE keyTypeZ);

    void Initialize(
		cjAniKeyFloat* keysX, cjUINT32 keyCountX, cjAniKeyFloat::EKEY_TYPE keyTypeX,
		cjAniKeyFloat* keysY, cjUINT32 keyCountY, cjAniKeyFloat::EKEY_TYPE keyTypeY,
		cjAniKeyFloat* keysZ, cjUINT32 keyCountZ, cjAniKeyFloat::EKEY_TYPE keyTypeZ);

    void CleanUp();

    void SetKeys(cjBYTE index, cjAniKeyFloat* keys) { cjAssert(index <= 2); m_keys[index] = keys; m_lastIdx[index] = 0; }
    void SetKeyCount(cjBYTE index, cjUINT32 keyCount) { cjAssert(index <= 2); m_keyCount[index] = keyCount; m_lastIdx[index] = 0; }
    void SetKeyType(cjBYTE index, cjAniKeyFloat::EKEY_TYPE keyType) { cjAssert(index <= 2); 
		m_keyType[index] = keyType; m_lastIdx[index] = 0; m_keySiz[index] = cjAniKeyFloat::GetKeySiz(keyType); }
    
    void ReplaceKeys(cjBYTE index, cjUINT32 keyCount, cjAniKeyFloat* keys);

    cjAniKeyFloat*				GetKeys(cjBYTE index) const		{ cjAssert(index <= 2);	return m_keys[index];		}
    cjBYTE						GetKeySiz(cjBYTE index) const	{ cjAssert(index <= 2);	return m_keySiz[index];		}
    cjUINT32					GetKeyCount(cjBYTE index) const	{ cjAssert(index <= 2);	return m_keyCount[index];	}
    cjAniKeyFloat::EKEY_TYPE	GetType(cjBYTE index) const		{ cjAssert(index <= 2); return m_keyType[index];	}
      
protected:    
	cjUINT32									m_keyCount[3];	
	cjAniKeyFloat*								m_keys[3]; // x,y,z	
    cjAniKeyFloat::EKEY_TYPE					m_keyType[3];
    cjBYTE										m_keySiz[3];
    cjUINT32									m_lastIdx[3];
};

//inline void			cjAniKeyEulerRot::SaveKey(cjFStream&, cjAniKey*, cjUINT32)	{ cjAssert(0); }
//inline cjAniKey*	cjAniKeyEulerRot::LoadKey(cjFStream&, cjUINT32 )			{ cjAssert(0); return cjNULL; }


CJ_ANIKEYOPER_REG(cjAniKeyEulerRot)


//
// cjAniKeyColor
//

class CJCORE_API cjAniKeyColor : public cjAniKey
{
    CJ_ANIKEYOPER_DECL

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyColor, cjAniKey)
		stream << d.m_color;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyColor, cjAniKey)

public:
	~cjAniKeyColor();

    void SetValue(const cjColorF& color) { m_color = color; }
    const cjColorF& GetValue() const { return m_color; }

    static cjColorF GenInterp(cjFLOAT etime, cjAniKeyColor* keys, EKEY_TYPE keyType, cjUINT32 keyCount, 
		cjUINT32& lastIdx, cjBYTE siz);

    static void GenCubicCoefs(cjFLOAT etime, cjAniKeyColor* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
		cjColorF& val0, cjColorF& outTan, cjColorF& valA, cjColorF& valB);

    static void SetDefault(const cjColorF& valDefault) { ms_default = valDefault; }

    cjAniKeyColor* GetKey(cjUINT32 index, cjBYTE keyCount) { return (cjAniKeyColor*)cjAniKey::GetKey(index, keyCount); }
    static cjBYTE GetKeySiz(EKEY_TYPE keyType) { return cjAniKey::GetKeySiz(EKVAL_COLOR, keyType); }

	//
	// operator
	//
    static PFIsPosed			GetIsPosedKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetIsPosedKeyPF(		EKVAL_COLOR, keyType); }
	static PFEqual				GetEqualKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetEqualKeyPF(		EKVAL_COLOR, keyType); }

	static PFSave				GetSaveKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetSaveKeyPF(		EKVAL_COLOR, keyType); }
    static PFLoad				GetLoadKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetLoadKeyPF(		EKVAL_COLOR, keyType); }    
	static PFCreate				GetCreateKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetCreateKeyPF(		EKVAL_COLOR, keyType); }
	static PFDelete				GetDeleteKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetDeleteKeyPF(		EKVAL_COLOR, keyType); }
    static PFCopy				GetCopyKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetCopyKeyPF(		EKVAL_COLOR, keyType); }	
    static PFInsert				GetInsertKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetInsertKeyPF(		EKVAL_COLOR, keyType); }

    static PFCubicCoefs			GetCubicCoefsKeyPF	(EKEY_TYPE keyType)	{ return cjAniKey::GetCubicCoefsKeyPF(	EKVAL_COLOR, keyType); }
    static PFInterpolation	GetInterpKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetInterpKeyPF(		EKVAL_COLOR, keyType); }

protected:
    cjAniKeyColor();
    cjAniKeyColor(cjFLOAT etime, const cjColorF& col);

    static cjColorF				GenInterpDefault(cjFLOAT etime, cjAniKeyColor* keys, EKEY_TYPE keyType, cjUINT32 keyCount, cjBYTE siz);

protected:
    cjColorF								m_color;
    static cjColorF							ms_default;

};

inline void			cjAniKeyColor::SaveKey(cjFStream&, cjAniKey*, cjUINT32) { cjAssert(0); }
inline cjAniKey*	cjAniKeyColor::LoadKey(cjFStream& , cjUINT32 )			{ cjAssert(0); return cjNULL; }
inline cjAniKey*	cjAniKeyColor::CreateKey(cjUINT32)						{ cjAssert(0); return cjNULL; }
inline void			cjAniKeyColor::DeleteKey(cjAniKey*)						{ cjAssert(0); }
inline void			cjAniKeyColor::CopyKey(cjAniKey* , const cjAniKey* )	{ cjAssert(0); }
inline void			cjAniKeyColor::InterpolateKey(cjFLOAT, const cjAniKey*, const cjAniKey*, void* rst) { cjAssert(0);  *(cjColorF*)rst = cjColorF::BLACK; }


CJ_ANIKEYOPER_REG(cjAniKeyColor);


//
// cjAniKeyLinColor
//

class CJCORE_API cjAniKeyLinColor : public cjAniKeyColor
{
    CJ_ANIKEYOPER_DECL
		
public:
    cjAniKeyLinColor();
    cjAniKeyLinColor(cjFLOAT etime, const cjColorF& color);

};

CJ_ANIKEYOPER_REG(cjAniKeyLinColor);


//
// cjAniKeyBool
//

class CJCORE_API cjAniKeyBool : public cjAniKey
{
    CJ_ANIKEYOPER_DECL

	CJ_DECL_SERIALIZE_EX_INLINE_BEGIN(cjAniKeyBool, cjAniKey)
		stream << d.m_value;
	CJ_DECL_SERIALIZE_EX_INLINE_END(cjAniKeyBool, cjAniKey)


public:
    void SetValue(const cjBool v) { m_value = v; }
    const cjBool GetValue() const { return m_value; }

    static cjBool GenInterp(cjFLOAT etime, cjAniKeyBool* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz);
        
    static void GenCubicCoefs(cjFLOAT etime, cjAniKeyBool* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
		cjFLOAT& val0, cjFLOAT& outTan, cjFLOAT& valA, cjFLOAT& valB);

    cjAniKeyBool* GetKey(cjUINT32 index, cjBYTE keyCount);
	static cjBYTE GetKeySiz(EKEY_TYPE keyType) { return cjAniKey::GetKeySiz(EKVAL_BOOL, keyType); }

    static void SetDefault(const cjBool valDefault) { ms_default = valDefault; }

	//
	// operators
	//
    static PFIsPosed			GetIsPosedKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetIsPosedKeyPF(		EKVAL_BOOL, keyType); }
	static PFEqual				GetEqualKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetEqualKeyPF(		EKVAL_BOOL, keyType); }
																												
	static PFSave				GetSaveKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetSaveKeyPF(		EKVAL_BOOL, keyType); }
    static PFLoad				GetLoadKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetLoadKeyPF(		EKVAL_BOOL, keyType); }
    static PFCreate				GetCreateKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetCreateKeyPF(		EKVAL_BOOL, keyType); }
    static PFDelete				GetDeleteKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetDeleteKeyPF(		EKVAL_BOOL, keyType); }
    static PFCopy				GetCopyKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetCopyKeyPF(		EKVAL_BOOL, keyType); }
    static PFInsert				GetInsertKeyPF		(EKEY_TYPE keyType)	{ return cjAniKey::GetInsertKeyPF(		EKVAL_BOOL, keyType); }

	static PFFillDerivedVals	GetFillDerivedValsKeyPF(EKEY_TYPE keyType) { return cjAniKey::GetFillDerivedValsKeyPF(EKVAL_BOOL, keyType); }
	static PFCubicCoefs			GetCubicCoefsKeyPF	(EKEY_TYPE keyType) { return cjAniKey::GetCubicCoefsKeyPF(	EKVAL_BOOL, keyType); }
    static PFInterpolation		GetInterpKeyPF		(EKEY_TYPE keyType) { return cjAniKey::GetInterpKeyPF(		EKVAL_BOOL, keyType); }

protected:
    cjAniKeyBool() {}
    cjAniKeyBool(cjFLOAT etime, const cjBool v) : cjAniKey(etime) { m_value = v; }

    static cjBool GenInterpDefault(cjFLOAT etime, cjAniKeyBool* keys, EKEY_TYPE keyType, 
		cjUINT32 keyCount, cjBYTE siz);

protected:
    cjBool									m_value;
    static cjBool							ms_default;
};

inline void			cjAniKeyBool::SaveKey(cjFStream&, cjAniKey*, cjUINT32)  { cjAssert(0); }
inline cjAniKey*	cjAniKeyBool::LoadKey(cjFStream&, cjUINT32 )			{ cjAssert(0); return cjNULL; }
inline cjAniKey*	cjAniKeyBool::CreateKey(cjUINT32)						{ cjAssert(0); return cjNULL; }
inline void			cjAniKeyBool::DeleteKey(cjAniKey*)						{ cjAssert(0); }
inline void			cjAniKeyBool::CopyKey(cjAniKey* , const cjAniKey* )		{ cjAssert(0); }

inline cjFLOAT		cjAniKeyBool::CaclCurveRatioKey(cjAniKey*, cjUINT32)	{ cjAssert(cjFalse); return 0.0f; }
inline void			cjAniKeyBool::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE) { cjAssert(cjFalse); }

inline void			cjAniKeyBool::InterpolateKey(cjFLOAT, const cjAniKey*, const cjAniKey*, void* rst)	 { cjAssert(0);  *(cjBool*)rst = ms_default; }
inline void			cjAniKeyBool::InterpolateD1Key(cjFLOAT, const cjAniKey*, const cjAniKey*, void* rst) { cjAssert(0);  *(cjBool*)rst = ms_default; }
inline void			cjAniKeyBool::InterpolateD2Key(cjFLOAT, const cjAniKey*, const cjAniKey*, void* rst) { cjAssert(0);  *(cjBool*)rst = ms_default; }

inline cjAniKeyBool* cjAniKeyBool::GetKey(cjUINT32 index, cjBYTE keySiz) { return (cjAniKeyBool*)cjAniKey::GetKey(index, keySiz); }


CJ_ANIKEYOPER_REG(cjAniKeyBool);


//
// cjAniKeyBool
//  - cjAniKeyStepBool
//

class CJCORE_API cjAniKeyStepBool : public cjAniKeyBool
{
    CJ_ANIKEYOPER_DECL

public:
    cjAniKeyStepBool() {}
    cjAniKeyStepBool(cjFLOAT etime, const cjBool v) : cjAniKeyBool(etime, v) {}
};

inline cjBool cjAniKeyStepBool::EqualKey(const cjAniKey& k0, const cjAniKey& k1) { return cjAniKeyBool::EqualKey(k0, k1); }
inline void cjAniKeyStepBool::FillDerivedValsKey(cjAniKey*, cjUINT32, cjBYTE) {}

CJ_ANIKEYOPER_REG(cjAniKeyStepBool);


//
// cjAniFloatDat
//

cjRefPtrDefine(cjAniFloatDat)

class CJCORE_API cjAniFloatDat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniFloatDat, cjRefObj,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	virtual cjSerializable* _CloneAsCopy();

public:
    cjAniFloatDat() { m_keyCount=0; m_keys=0; m_keyType=cjAniKey::EKT_NOINTERP; m_keySiz=0; }

    cjUINT32						GetKeyCount() const { return m_keyCount; }
	cjAniKeyFloat*					GetKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
	void							ReplaceKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);

    void							CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    cjAniFloatDatRPtr				GetDat(cjFLOAT stTime, cjFLOAT enTime);

protected:
	cjAniKeyFloat*					m_keys;
    cjUINT32						m_keyCount;    
    cjAniKeyFloat::EKEY_TYPE		m_keyType;
    cjBYTE							m_keySiz;	
};


//
// cjAniBoolDat
//

cjRefPtrDefine(cjAniBoolDat)

class CJCORE_API cjAniBoolDat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniBoolDat, cjRefObj,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	virtual cjSerializable* _CloneAsCopy();

public:
    cjAniBoolDat() { m_keyCount=0; m_keys=0; m_keyType=cjAniKey::EKT_NOINTERP; m_keySiz=0; }

    cjUINT32						GetKeyCount() const { return m_keyCount; }
	cjAniKeyBool*					GetKeys(cjUINT32& keyCount, cjAniKeyBool::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
	void							ReplaceKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType);

    void							CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    cjAniBoolDatRPtr				GetDat(cjFLOAT stTime, cjFLOAT enTime);

protected:
	cjAniKeyBool*					m_keys;
    cjUINT32						m_keyCount;    
    cjAniKeyBool::EKEY_TYPE			m_keyType;
    cjBYTE							m_keySiz;
	
};


//
// cjAniRotDat
//

cjRefPtrDefine(cjAniRotDat)

class CJCORE_API cjAniRotDat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniRotDat, cjRefObj, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	virtual cjSerializable* _CloneAsCopy();

public:
    cjAniRotDat();    

    cjUINT32						GetKeyCount() const { return m_keyCount; }
    cjAniKeyRot*					GetKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);

    void							CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    cjAniRotDatRPtr					GetDat(cjFLOAT stTime, cjFLOAT enTime);

protected:
    cjUINT32						m_keyCount;
    cjAniKeyRot*					m_keys;
    cjAniKeyRot::EKEY_TYPE			m_keyType;
    cjBYTE							m_keySiz;
};


//
// cjAniVec3Dat
//

cjRefPtrDefine(cjAniVec3Dat)

class CJCORE_API cjAniVec3Dat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniVec3Dat, cjRefObj, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	virtual cjSerializable* _CloneAsCopy();

public:
    cjAniVec3Dat();

    cjUINT32						GetKeyCount() const { return m_keyCount; }
    cjAniKeyVec3*					GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);

    void							CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    cjAniVec3DatRPtr				GetDat(cjFLOAT stTime, cjFLOAT enTime);

protected:
    cjUINT32						m_keyCount;
    cjAniKeyVec3*					m_keys;
    cjAniKeyVec3::EKEY_TYPE			m_keyType;
    cjBYTE							m_keySiz;
};


//
// cjAniTMDat
//

cjRefPtrDefine(cjAniTMDat)


class CJCORE_API cjAniTMDat : public cjRef
{
	CJSER_DECL_CLASS(cjAniTMDat, cjRef,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	virtual cjSerializable* _CloneAsCopy();

public:
    cjAniTMDat();
	
	// Scale
	cjUINT32 GetScaleKeyCount() const { return m_scaleKeyCount; }    
	cjAniKeyVec3* GetScaleKeys(cjUINT32& rstKeyCount, cjAniKeyVec3::EKEY_TYPE& rstKeyType, cjBYTE& rstKeySiz) const;
    void ReplaceScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void SetScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	
	// rot
    cjUINT32 GetRotKeyCount () const { return m_rotKeyCount; }
    cjAniKeyRot* GetRotKeys(cjUINT32& rstKeyCount, cjAniKeyRot::EKEY_TYPE& rstKeyType, cjBYTE& rstKeySiz) const;
    void ReplaceRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
    void SetRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
	
	// pos
    cjUINT32 GetPosKeyCount() const { return m_posKeyCount; }
    cjAniKeyVec3* GetPosKeys(cjUINT32& rstKeyCount, cjAniKeyVec3::EKEY_TYPE& rstKeyType, cjBYTE& rstKeySiz) const;
    void ReplacePosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void SetPosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	
	void CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    cjAniTMDatRPtr GetDat(cjFLOAT stTime, cjFLOAT enTime);

protected:
	// to delete euler rotation key specially
    void CleanRotKeys(cjAniKeyRot* rotKeys, cjUINT32 rotKeyCount, cjAniKeyRot::EKEY_TYPE keyType);
    
protected:
	cjAniKeyVec3*							m_scaleKeys;
	cjAniKeyRot*							m_rotKeys;
	cjAniKeyVec3*							m_posKeys;

    cjUINT32								m_scaleKeyCount;
	cjUINT32								m_rotKeyCount;
	cjUINT32								m_posKeyCount;

    cjAniKeyVec3::EKEY_TYPE					m_scaleKeyType;
	cjAniKeyRot::EKEY_TYPE					m_rotKeyType;
	cjAniKeyVec3::EKEY_TYPE					m_posKeyType;

    cjBYTE									m_scaleKeySiz;
    cjBYTE									m_rotKeySiz;
    cjBYTE									m_posKeySiz;
};


//
// cjAniColorDat
//

cjRefPtrDefine(cjAniColorDat)

class CJCORE_API cjAniColorDat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniColorDat, cjRefObj, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	virtual cjSerializable* _CloneAsCopy();

public:
	cjAniColorDat();
	
    cjUINT32						GetKeyCount() const { return m_keyCount; }
    cjAniKeyColor*					GetKeys(cjUINT32& keyCount, cjAniKeyColor::EKEY_TYPE& keyType, cjBYTE& ksySiz) const;
    void							ReplaceKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType);

    void							CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    cjAniColorDatRPtr				GetDat(cjFLOAT stTime, cjFLOAT enTime);

protected:
    cjUINT32						m_keyCount;
    cjAniKeyColor*					m_keys;
    cjAniKeyColor::EKEY_TYPE		m_keyType;
    cjBYTE							m_keySiz;

};


#endif
