
#ifndef _CJANIPOSE_H_
#define _CJANIPOSE_H_


class cjResAniSeq;

class cjAniPoseBind;
class cjAniEvaluator;
class cjAniEvalSPDat;
class cjAniCntrManager;
class cjAniSeq;
class cjAniCntrSeq;
class cjAniCntrManager;

cjRefPtrDefine(cjAniCntrSeq)



//
// cjAniPoseBindInfo
//

class CJCORE_API cjAniPoseBindInfo : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjAniPoseBindInfo)
		stream << d.m_id << d.m_propRttiName << d.m_cntrRttiName << d.m_cntrID << d.m_evaluatorID;
		
		stream << d.m_nextHash;
		stream << d.m_lod;

		cjAssert(d.m_lod < 10);
		cjAssert((cjSWORD)d.m_lod > -10);

		stream << d.m_pbHandle;

		stream << d.m_pbIndicesForTM[0];
		stream << d.m_pbIndicesForTM[1];

	CJ_DECL_SERIALIZE_INLINE_END(cjAniPoseBindInfo)

public:
    cjAniPoseBindInfo();

    void					SetPBHandleInfo(const cjAniPoseBufHandle& pbHandle);
    
	cjAniPoseBufHandle		GetFirstPBHandle() const;
    cjAniPoseBufHandle		GetPBHandle(cjAniPoseBufChanItem pbChannelType) const;

    cjBool					MatchesIDTag(const cjAniEvaluatorTag& tag) const;
    cjBool					MatchesIDTag(const cjName& id, const cjName& propName, const cjName& cntrRttiName, const cjName& cntrID, const cjName& evaluatorID) const;

    cjBool					IsTransformData() const;
    cjBool					IsEqual(const cjAniPoseBindInfo& dst);

public:
    cjName					m_id;
    cjName					m_propRttiName;
    cjName					m_cntrRttiName;
    cjName					m_cntrID;
    cjName					m_evaluatorID;

	cjWORD					m_nextHash;
	cjWORD					m_lod;
    cjAniPoseBufHandle		m_pbHandle;

	// index for transform	
	/* 
	    case #0
		m_pbHandle          : rotation
		m_pbIndicesForTM[0] : pos
		m_pbIndicesForTM[1] : scale

	*/
    cjWORD					m_pbIndicesForTM[2]; // 0:pos,  1:scale
};


//
// cjAniPoseBind
//

class CJCORE_API cjAniPoseBind : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniPoseBind, cjRefObj,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjAniPoseBind)
	CJSER_DECL_CLONE_POST

public:
    enum
    {
        ACCUMENDINDEX			= 2, // index 0: accum value; index 1: accum delta
    };

    cjAniPoseBind(cjAniCntrManager* cntrMan, const cjRefObj* accumRoot);

	// AccumInfo
	cjBool						GetCumulativeAni() const { return (m_nonAccumStIndex > 0) ? cjTrue : cjFalse; }
    const cjName&				GetAccumRootName() const { return m_accumRootName; }
	cjUINT32					GetNonAccumStIndex() const { return m_nonAccumStIndex; }
	
	// BindInfo Management
	void						SyncBindInfoToSeqEvaluator(const cjAniSeq* seqDat, cjAniCntrManager* cntrMan);
    cjAniPoseBufHandle			_AddBind(cjAniPoseBindInfo* bindInfo, cjAniPoseBufChanItem pbChanType);

	cjAniPoseBindInfo*			GetBindInfoAry() { return m_bindInfoAry; }
	cjUINT32					GetBindInfoCount() const { return m_bindInfoCount; }
	cjWORD*						GetBindInfoIndexList() { return m_bindInfoIndexList; }

    const cjAniPoseBindInfo*	GetBindInfo(const cjAniPoseBufHandle& pbHandle) const;
    cjAniPoseBindInfo*			GetBindInfo(const cjAniEvaluatorTag& tag) const;
    cjAniPoseBindInfo*			GetBindInfo(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, 
									const cjName& cntrID, const cjName& evaluatorID) const;

    cjUINT32					GetTotalBindCount() const	{ return m_totalBindCount;	}
	cjUINT32					GetColorBindCount() const	{ return m_colorBindCount;	}
	cjUINT32					GetBoolBindCount()	const	{ return m_boolBindCount;	}
    cjUINT32					GetFloatBindCount() const	{ return m_floatBindCount;	}
    cjUINT32					GetVec3SBindCount() const	{ return m_vec3SBindCount;	}
	cjUINT32					GetVec3PBindCount() const	{ return m_vec3PBindCount;	}
    cjUINT32					GetRotBindCount()	const	{ return m_rotBindCount;	}
    cjUINT32					GetRefDatBindCount() const	{ return m_refDatBindCount; }

	// Handle
    cjBool						GetHandleAndLOD(const cjName& id, const cjName& propName, const cjName& cntrRttiName, const cjName& cntrID, const cjName& evaluatorID, cjAniPoseBufChanItem pbChanType, cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const;
    cjBool						GetHandleAndLOD(const cjAniEvaluatorTag& tag, cjAniPoseBufChanItem pbChanType, cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const;

protected:
    cjAniPoseBind();

    void						Init(cjAniCntrManager* cntrMan, const cjRefObj* accumRoot);
    void						Shutdown();

    void						InitAccumRootBind(cjAniCntrManager* cntrMan, const cjRefObj* accumRoot);

    void						ReallocBinds(cjUINT32 colorCount, cjUINT32 boolCount, cjUINT32 floatCount, cjUINT32 vec3SCount, cjUINT32 vec3PCount, cjUINT32 rotCount, cjUINT32 refDatCount, cjUINT32 bindInfoCount);
    void						ReallocHashTable(cjUINT32 hashBucketSiz);
	
protected:	
	// item hash
	cjWORD*						m_hash;
    cjWORD*						m_bindInfoIndexList;
	cjAniPoseBindInfo*			m_bindInfoAry;

	cjWORD						m_hashBucketSiz;
    cjWORD						m_bindInfoCount;

	cjName						m_accumRootName;    
    cjWORD						m_nonAccumStIndex;  // 0: no accum root, ACCUMENDINDEX: accum root 
    cjWORD						m_totalBindCount;

	cjWORD						m_lodCount;

	// bind info
    cjWORD						m_colorBindCount;
    cjWORD						m_colorStIndex;

    cjWORD						m_boolBindCount;
    cjWORD						m_boolStIndex;

    cjWORD						m_floatBindCount;
    cjWORD						m_floatStIndex;

    cjWORD						m_vec3SBindCount;		// scale
    cjWORD						m_vec3SStIndex;

    cjWORD						m_vec3PBindCount;		// pos
    cjWORD						m_vec3PStIndex;

    cjWORD						m_rotBindCount;			// rot
    cjWORD						m_rotStIndex;

    cjWORD						m_refDatBindCount;
    cjWORD						m_refDatStIndex;
};

cjRefPtrDefine(cjAniPoseBind)


//
// cjAniPoseBuffer
//

class CJCORE_API cjAniPoseBuf : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniPoseBuf, cjRefObj,0 , CJCORE_API)
	//CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjAniPoseBuf)
	CJSER_DECL_CLONE_POST

public:	
	// weight data format
	// [31~25][24  ~    0]
	
	// FLAG[31~25]: Valid / Culled
	//        WEIGHT[24~0]: 24bit cjFLOAT weight

	enum
    {	
		// 25bit ~ 31bit (7)
		WDAT_FLAG_VALID			= 0x80000000,	// 31
		WDAT_FLAG_CULLED		= 0x40000000,	// 30
		WDAT_FLAG_MASK			= 0xfe000000,
		WDAT_FLAG_BITS			= 7,
		
		// 0~24bit		 (25) : final weight * WDAT_WEIGHT_CONV_INT_SCALER
		WDAT_WEIGHT_MASK		= 0x01ffffff,
		WDAT_WEIGHT_BITS		= 25,

		// 1 << (25-1) : 0x01000000, 16777216
		WDAT_WEIGHT_CONV_INT_SCALER	= 1 << (WDAT_WEIGHT_BITS-1)
    };

	// must keep 4byte alignment
	struct ColorDat	{ cjColorF		m_color;	};
    struct BoolDat	{ cjFLOAT		m_bool;		};
	struct FloatDat	{ cjFLOAT		m_float;	};
	struct Vec3SDat	{ cjVec3		m_vec3;		};
	struct Vec3PDat	{ cjVec3		m_vec3;		};
	struct RotDat	{ cjQuat		m_rotQuat;  };
    struct RefDat
    {
		cjAniCntrSeq*			m_cntrSeq;
        cjAniEvaluator*			m_refEvaluator;
        cjAniEvalSPDat*			m_evalSPDat[cjAniEvalChan_PRS_Max];
    };
	
public:
	cjAniPoseBuf(cjAniPoseBind* poseBind);
	
	
	// WeightDat Access	
    static cjBool			IsValidWeight(cjUINT32 weight) { return (weight & WDAT_FLAG_VALID) != 0; }
    static cjBool			IsValidWeightAndNotCulled(cjUINT32 weight) { return ((weight & (WDAT_FLAG_VALID | WDAT_FLAG_CULLED)) == WDAT_FLAG_VALID); }

    static cjFLOAT			GetUnitScaledWeight(cjUINT32 weight)
							{
								cjAssert(IsValidWeightAndNotCulled(weight));
								// 1 << 24: 0x01000000, 1.0f/16777216.0f
								const cjFLOAT WDAT_INVSCALER = 1.0f / (cjFLOAT)WDAT_WEIGHT_CONV_INT_SCALER;
								return (cjFLOAT)(weight & WDAT_WEIGHT_MASK) * WDAT_INVSCALER;
							}

    static void				SetUnitScaledWeight(cjFLOAT unitScaledWeight, cjUINT32& weight)
							{
								cjAssert(unitScaledWeight >= 0.0f && unitScaledWeight <= 1.0f);
								cjAssert(IsValidWeightAndNotCulled(weight));
								// unitScaledWeight(0.0~1.0) * (1<<24)
								weight = WDAT_FLAG_VALID | (cjUINT32)(unitScaledWeight * (cjFLOAT)WDAT_WEIGHT_CONV_INT_SCALER);
							}

	void					InvalidateAllDats() {
								if(m_totalDatCount > 0) {
									cjAssert(m_weightAry);
									cjMemSet(m_weightAry, 0, m_totalDatCount * sizeof(cjUINT32));
									m_validDatCount = 0; }
							}

    cjUINT32*				GetWeightAry() { return m_weightAry; }


    cjAniPoseBind*			GetPoseBind() const { return m_poseBind; }
    cjBool					IsFullBodyPose() const
							{
								cjAssert(m_validDatCount <= m_totalDatCount);
								cjAssert(m_poseBind || m_totalDatCount == 0);
								cjAssert(!m_poseBind || m_totalDatCount <= m_poseBind->GetTotalBindCount());
								return (!m_poseBind || m_validDatCount == m_poseBind->GetTotalBindCount());
							}

	//
	// data management
	//

	cjUINT32				GetTotalDatCount() const { return m_totalDatCount; }
    cjUINT32				GetValidDatCount() const { return m_validDatCount; }
	cjUINT32				GetNonAccumStartIndex() const { return m_nonAccumStIndex; }
	
		// Color
    cjUINT32				GetColorCount() const { return m_colorCount; }
    cjBool					GetColor(cjAniPoseBufHandle pbHandle, cjColorF& v, cjBool notCulledOnly) const;
    cjBool					IsColorValid(cjAniPoseBufHandle pbHandle) const;
    void					SetColor(cjAniPoseBufHandle pbHandle, const cjColorF& v);
    void					SetColorValid(cjAniPoseBufHandle pbHandle, cjBool valid);

		// Bool
	cjUINT32				GetBoolCount() const { return m_boolCount; }
    cjBool					GetBool(cjAniPoseBufHandle pbHandle, cjFLOAT& v, cjBool notCulledOnly) const;
    cjBool					IsBoolValid(cjAniPoseBufHandle pbHandle) const;
    void					SetBool(cjAniPoseBufHandle pbHandle, cjFLOAT v);
    void					SetBoolValid(cjAniPoseBufHandle pbHandle, cjBool isValid);

		// Float
	cjUINT32				GetFloatCount() const { return m_floatCount; }
    cjBool					GetFloat(cjAniPoseBufHandle pbHandle, cjFLOAT& v, cjBool notCulledOnly) const;
    cjBool					IsFloatValid(cjAniPoseBufHandle pbHandle) const;
    void					SetFloat(cjAniPoseBufHandle pbHandle, cjFLOAT v);
    void					SetFloatValid(cjAniPoseBufHandle pbHandle, cjBool isValid);

		// Vec3S
	cjUINT32				GetVec3SCount() const { return m_vec3SCount; }
    cjBool					GetVec3S(cjAniPoseBufHandle pbHandle, cjVec3& v, cjBool notCulledOnly) const;
    cjBool					IsVec3SValid(cjAniPoseBufHandle pbHandle) const;
    void					SetVec3S(cjAniPoseBufHandle pbHandle, const cjVec3& v);
    void					SetVec3SValid(cjAniPoseBufHandle pbHandle, cjBool isValid);

		// Vec3P
	cjUINT32				GetVec3PCount() const { return m_vec3PCount; }
    cjBool					GetVec3P(cjAniPoseBufHandle pbHandle, cjVec3& v, cjBool notCulledOnly) const;
    cjBool					IsVec3PValid(cjAniPoseBufHandle pbHandle) const;
    void					SetVec3P(cjAniPoseBufHandle pbHandle, const cjVec3& v);
    void					SetVec3PValid(cjAniPoseBufHandle pbHandle, cjBool isValid);

		// Rot
	cjUINT32				GetRotCount() const { return m_rotCount; }
    cjBool					GetRot(cjAniPoseBufHandle pbHandle, cjQuat& v, cjBool notCulledOnly) const;
    cjBool					IsRotValid(cjAniPoseBufHandle pbHandle) const;
    void					SetRot(cjAniPoseBufHandle pbHandle, const cjQuat& v);
    void					SetRotValid(cjAniPoseBufHandle pbHandle, cjBool isValid);
	
		// ReferencedDat
	cjUINT32				GetRefDatCount() const { return m_refDatCount; }
    cjBool					GetRefDat(cjAniPoseBufHandle pbHandle, RefDat& v, cjBool notCulledOnly) const;
    cjBool					IsRefDatValid(cjAniPoseBufHandle pbHandle) const;
    void					SetRefDat(cjAniPoseBufHandle pbHandle, const RefDat& v);
    void					SetRefDatValid(cjAniPoseBufHandle pbHandle, cjBool isValid);

    cjFLOAT*				GetDat()		{ return m_dats;	}
	ColorDat*				GetColorDats()	{ return m_colors;	}
	BoolDat*				GetBoolDats()	{ return m_bools;	}
    FloatDat*				GetFloatDat()	{ return m_floats;	}
	Vec3SDat*				GetVec3SDat()	{ return m_vec3Ss;	}
	Vec3PDat*				GetVec3PDat()	{ return m_vec3Ps;	}
	RotDat*					GetRotDats()	{ return m_rots;	}
	RefDat*					GetRefDat()		{ return m_refDats;	} // avoid name conflict with m_refCount

		// AniPoseBufHandle
    cjFLOAT*				GetDat(cjAniPoseBufHandle pbHandle) const;
	cjBool					IsValid(cjAniPoseBufHandle pbHandle) const;
    void					SetValid(cjAniPoseBufHandle pbHandle, cjBool valid);

    cjBool					GetHandle(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, const cjName& cntrID, 
										const cjName& evaluatorID, cjAniPoseBufChanItem pbChanItem, cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const;
    cjBool					GetHandle(const cjAniEvaluatorTag& tag, cjAniPoseBufChanItem pbChanItem, cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const;

	void					ValidateAccumDelta();
    cjBool					AddNewDatsFromPoseBind(cjBool copyFloatWeights, cjBool copyDats);

protected:
	cjAniPoseBuf();

	void					Init(cjAniPoseBind* poseBind);
    void					Shutdown();

	void					_InitCommon();

protected:
	cjAniPoseBindRPtr				m_poseBind;

	cjWORD							m_totalDatCount;
	cjWORD							m_validDatCount;
    cjUINT32						m_nonAccumStIndex;	// 0:no accum, 1~:accum index

	// 7bit(Flag) + 24bit cjFLOAT-weight data array
    cjUINT32*						m_weightAry;

	// all the composite data(ColorDat,BoolDat,FloatDat...) format is cjFLOAT
//	cjUINT32						m_datSiz;
	cjFLOAT*						m_dats; 
	
	cjWORD							m_colorCount;
	cjWORD							m_colorStIndex;
	ColorDat*						m_colors;

	cjWORD							m_boolCount;
	cjWORD							m_boolStIndex;
	BoolDat*						m_bools;

	cjWORD							m_floatCount;
	cjWORD							m_floatStIndex;
	FloatDat*						m_floats;

	cjWORD							m_vec3SCount;
	cjWORD							m_vec3SStIndex;
	Vec3SDat*						m_vec3Ss;

	cjWORD							m_vec3PCount;
	cjWORD							m_vec3PStIndex;
	Vec3PDat*						m_vec3Ps;

	cjWORD							m_rotCount;
	cjWORD							m_rotStIndex;
	RotDat*							m_rots;				// quaternion	

    cjWORD							m_refDatCount;
    cjWORD							m_refDatStIndex;
    RefDat*							m_refDats;				//ref
};

cjRefPtrDefine(cjAniPoseBuf)


//
// cjAniMultiTargetPoseHdr
//

class cjAniMTPoseItpCntrInfo : public cjMem
{
public:
    enum
    {
        LODSKIPINTERP = -2,
    };

	cjAniMTPoseItpCntrInfo() {	ClearValues(); }

    void ClearValues()
	{
		m_lod						= LODSKIPINTERP;
		m_interpolatorIndex			= 0;
		m_dstAsInterpolationCntr	= 0;
	//	m_dstAsObj			= 0;
	}

    cjBool IsTransformEvaluator() const { return (m_interpolatorIndex == cjInterpolationAniCntr_INVALID_INDEX); }

public:
    cjSWORD								m_lod;
    cjWORD								m_interpolatorIndex;		// INVALID_INDEX: transform

    union
    {
        cjInterpolationAniCntr*			m_dstAsInterpolationCntr;	// non transform controller
        cjRefObj*						m_dstAsObj;					// transform controller
    };
};


//
// cjAniMTargetPoseHandler
//

class CJCORE_API cjAniMTargetPoseHandler : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniMTargetPoseHandler, cjRefObj,0 , CJCORE_API)

public:
	enum { LOD_SKIP_INTERPOLATION = -2 };

public:
    cjAniMTargetPoseHandler(cjAniCntrManager* owner, cjBool moveAccumTMToChild = cjTrue);

		// AccumNode Management
    const cjName&			GetAccumRootName()	const { return m_accumRootName; }
	cjRefObj*				GetAccumRoot()		const { return m_accumRoot; }
    const cjQuatTM&			GetAccumTM()		const { return m_accumTM; }
    void					SetAccumTM(const cjQuatTM& tm);
    void					ClearAccumTM();

    cjAniCntrManager*		GetOwner() const { return m_owner; }
    cjFLOAT					GetLastUpdateTime() const { return m_lastUpdateTime; }

    void					Update(cjFLOAT etime, cjINT32 lod, cjAniPoseBuf* poseBuf, cjAniCntrSeq* aniCntrSeq);
    cjBool					FindTarget(const cjName& id, const cjName& prop, const cjName& cntrType, const cjName& kCtlrID, const cjName& interpolatorID, 
								cjInterpolationAniCntr*& pkCtlr, cjSceneNode*& pkAVObject);

    void					AddNewTargetsFromPoseBind() { AddInterpControllerInfos(); }

	
	void					FillPoseSeqDat(const cjAniSeq* seqDatToSync, cjAniSeq* poseSeqDat);
	void					OnRemoveSequence(cjAniCntrSeq* cntrSeq) { if(cntrSeq == m_soleCntrSeq) m_soleCntrSeq = cjNULL; }

	void					_CommInit();

protected:
    cjAniMTargetPoseHandler();

	void					Init(cjAniCntrManager* cntrMan, cjBool moveAccumTMToChild);
    void					Shutdown();

    void					AddInterpControllerInfos();
    void					RemoveAllInterpControllerInfos();


protected:
    cjAniCntrManager*				m_owner;
    cjFLOAT							m_lastUpdateTime;

    cjName							m_accumRootName;
    cjRefObj*						m_accumRoot;
	cjBool							m_moveAccumTMToChild;

	cjQuatTM						m_accumTM;
    cjQuatTM						m_stAccumTM;
    cjMatrix3						m_stAccumRot;
    cjAniCntrSeqRPtr				m_soleCntrSeq;
   
	cjUINT32						m_interpCntrInfoCount;
    cjAniMTPoseItpCntrInfo*			m_interpCntrInfoAry;

    cjWORD							m_colorStIndex;
    cjWORD							m_boolStIndex;
    cjWORD							m_floatStIndex;
    cjWORD							m_vec3SStIndex;
	cjWORD							m_vec3PStIndex;
    cjWORD							m_rotStIndex;
    cjWORD							m_refDatStIndex;
};

cjRefPtrDefine(cjAniMTargetPoseHandler)


//
// cjAniPoseBlend
//

class CJCORE_API cjAniPoseActInfo : public cjMem
{
public:
    cjAniPoseActInfo();

    cjAniPoseBuf*					m_poseBuf;
    cjUINT32*						m_flagAndWeightAry;
    cjBool							m_hasBlendItems;
    cjINT32							m_priority;
    cjFLOAT							m_weight;
    cjAniCntrSeq*					m_cntrSeq;
};


class CJCORE_API cjAniPoseBlend : public cjRefObj
{
	CJSER_DECL_CLASS(cjAniPoseBlend, cjRefObj,0 , CJCORE_API)

public:
	cjAniPoseBlend();
	cjAniPoseBlend(cjAniCntrManager* cntrMan);

    cjAniCntrManager*				GetOwner() const { return m_owner; }
    cjBool							Update(cjUINT32 blendCntrSeqCount, cjINT32 lod, cjAniPoseBuf*& poseBuf, cjAniCntrSeq*& rstCntrSeq);

protected:
	void							Init(cjAniCntrManager* cntrMan);
	void							Shutdown();

	void							AddNewLODsFromPoseBind();

	void							_InitCommon();

protected:
	cjAniCntrManager*				m_owner;
	cjAniPoseBufRPtr				m_poseBuf;

	cjUINT32						m_lodCount;
	cjSWORD*						m_psLODs;

	cjUINT32						m_actPoseCount;
	cjAniPoseActInfo*				m_actPoses;

	cjUINT32						m_bitPtnCount;
	cjUINT32						m_bitPtns[cjAniPoseBlend_MaxBitPatterns];
	cjUINT32						m_bitMasks[cjAniPoseBlend_MaxBitPatterns];

	cjUINT32						m_flagWeightCount;
	cjUINT32*						m_flagAndWeightAry;
};

cjRefPtrDefine(cjAniPoseBlend)

#endif

