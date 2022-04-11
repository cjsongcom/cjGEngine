
#ifndef _CJANIDEF_H_
#define _CJANIDEF_H_


class cjAniEvaluator;


//
// Animation State
//

//
// cjAniSeqCB
//

enum cjAniSeqState
{
	cjAniSeqState_InActive,
    cjAniSeqState_Animating,
    cjAniSeqState_EaseIn,
    cjAniSeqState_EaseOut,
    cjAniSeqState_TransitionSrc,
    cjAniSeqState_TransitionDst,
	cjAniSeqState_MorphSrc,
	cjAniSeqState_Max
};


const cjFLOAT CJANI_SEQUENCE_DAT_FREQ	= -1.0f;
const cjFLOAT CJANI_SEQUENCE_CUR_FRAME	= CJ_FLT_INVALID;

typedef cjUINT32 cjAniSeqID;
typedef cjTAry<cjAniSeqID> cjAniSeqIDAry;

const cjAniSeqID	cjAniSeqID_AnySeq			= (cjUINT32) -1;
const cjAniSeqID	cjAniSeqID_InvalidSeq		= (cjUINT32) -2;

const cjAniSeqID	cjAnimatorAnyEvtCode		= cjAniSeqID_AnySeq;
const cjAniSeqID	cjAnimatorInvalidEvtCode	= cjAniSeqID_InvalidSeq;

//const cjUINT32		cjAnimatorSyncSeqID_None = (cjUINT32)-2;


enum cjAniChannel
{
	cjAniChan_Base		= 0,
    cjAniChan_Pos		= cjAniChan_Base,
    cjAniChan_Rot		= 1,
    cjAniChan_Scale		= 2
};


//
// cjAniEvaluator
//

enum cjAniEvalChannel
{	
	// most of evaluator use only base channel
	// item:  cjAniEvalChanItem_Float, cjAniEvalChanItem_Color, cjAniEvalChanItem_Bool 
	cjAniEvalChan_Base	= 0,					
	cjAniEvalChan_Pos	= cjAniEvalChan_Base,	// position channel for transform evaluator
		
	// rotation, scale channel for cjAniTransfromEvaluator
	// item : cjAniEvalChanItem_Rot, cjAniEvalChanItem_Float
	cjAniEvalChan_Rot	= 1,					// Rotation Channel for transform evaluator
	cjAniEvalChan_Scl	= 2,					// Scale Channel	for transform evaluator
	
	// actual channel count
	cjAniEvalChan_PRS_Max,		// Pos,Rot,Scale Channel Max
	
	// flag channel - cjAniEvalutor::m_channelItem[cjAniEvalChan_Flag]
	// flag values : cjAniEvalFlagChanVal_Referenced, cjAniEvalFlagChanVal_Transform, ..
	cjAniEvalChan_Flag  = 3,					// special channel to store flags
	cjAniEvalChan_Item_MAX
};


//
// flag value for cjAniEvaluator::m_channelItem[cjAniEvalChan_Flag]
//
enum cjAniEvalFlagChannelVal
{
	cjAniEvalFlagChanVal_Referenced		= 0x1,
	cjAniEvalFlagChanVal_Transform		= 0x2,
	cjAniEvalFlagChanVal_AlwaysUpdate	= 0x4,
	cjAniEvalFlagChanVal_ShutDown		= 0x8
};


//
// itemvalues that are stored in cjAniEvaluator::m_channelItem[cjAniEvalChan_Base or _Pos, _Rot, _Scl]
//

enum cjAniEvalChanItem
{
    cjAniEvalChanItem_NoData		= 0x00,
    cjAniEvalChanItem_Color			= 0x01,
    cjAniEvalChanItem_Bool			= 0x02,
    cjAniEvalChanItem_Float			= 0x03,
    cjAniEvalChanItem_Vec3S			= 0x04,
	cjAniEvalChanItem_Vec3P			= 0x05,
    cjAniEvalChanItem_Rot			= 0x06,

	// mask flag
		// only one key frame is there, channel item is static
	cjAniEvalChanItemMask_Constant = 0x80 // 0x80 + (cjAniEvalChanItem_Color ~ cjAniEvalChanItem_Rot)
};


enum cjAniEvalGrp
{	
	cjAniEvalGrp_Color				= 1,
	cjAniEvalGrp_Bool				= 2,
	cjAniEvalGrp_Float				= 3,
	cjAniEvalGrp_Vec3S				= 4,	// scale
	cjAniEvalGrp_Vec3P				= 5,	// pos
	cjAniEvalGrp_Rot				= 6,
	cjAniEvalGrp_Transform			= 7,
	cjAniEvalGrp_Referenced			= 8,
	cjAniEvalGrp_Max				= 9,

	cjAniEvalGrp_Invalid			= 0xff
};

enum cjAniEvaluatorConstants
{
    cjAniEvalHashBucketSizSmall		= 31,
    cjAniEvalHashBucketSizLarge		= 127,
};


//
// cjAniEvaluatorTag
//

class CJCORE_API cjAniEvaluatorTag : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjAniEvaluatorTag)
		stream << d.m_id << d.m_propRttiName << d.m_cntrRttiName << d.m_cntrID << d.m_evaluatorID << d.m_hashS << d.m_hashL;
	CJ_DECL_SERIALIZE_INLINE_END(cjAniEvaluatorTag)

public:
	enum
    {
        SMALLHASHTABLESIZE = 31,
        LARGEHASHTABLESIZE = 127,
    };

	cjAniEvaluatorTag();
    cjAniEvaluatorTag(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, 
		const cjName& cntrID, const cjName& evaluatorID);
    cjAniEvaluatorTag(const cjAniEvaluatorTag& rhs);

    cjAniEvaluatorTag& operator=(const cjAniEvaluatorTag& rhs);

    cjBool IsEqual(const cjAniEvaluatorTag& dst);

    const cjName& GetID() const { return m_id; }
    void SetID(const cjName& id) { m_id = id; }

	const cjName& GetPropRttiName() const { return m_propRttiName; }
	void SetPropRttiName(const cjName& propRttiName) { m_propRttiName = propRttiName; }

    const cjName& GetCntrRttiName() const { return m_cntrRttiName; }
    void SetCntrRttiName(const cjName& cntrRttiName) { m_cntrRttiName = cntrRttiName; }

    const cjName& GetCntrID() const { return m_cntrID; }
    void SetCntrID(const cjName& cntrID) { m_cntrID = cntrID; }

    const cjName& GetEvaluatorID() const { return m_evaluatorID; }
    void SetEvaluatorID(const cjName& evaluatorID) { m_evaluatorID = evaluatorID; }

    void Reset();

public:
    static cjWORD GetHash(const cjNameChar* id, const cjNameChar* propRttiName, 
		const cjNameChar* cntrRttiName,	const cjNameChar* cntrID, 
		const cjNameChar* evaluatorID);

	void DoHash();

    cjWORD GetHashSmall() const { return m_hashS; }
    cjWORD GetHashLarge() const { return m_hashL; }

protected:
    cjName									m_id;				// id
	cjName									m_propRttiName;		// property rtti name
    cjName									m_cntrRttiName;		// rtti name of class which is derived from cjInterpolationCntr
    cjName									m_cntrID;			// interpolator id
    cjName									m_evaluatorID;		//

    cjWORD									m_hashS;
    cjWORD									m_hashL;
};



//
// cjAniPose
//

// Conversion between cjAniEvalutorChanItem with cjAniPoseChanItem
/*
	cjAniEvalChannel::m_channelItem[cjAniEvalChan_Pos] 
		= cjAniEvalChanItem_Float, cjAniEvalChanItem_Color, cjAniEvalChanItem_XXX, ..

	cjAniPoseBufChannel pbChan = 
		1 << 
			(  (cjAniEvalChannel::m_channelItem[cjAniEvalChan_Pos] & ~cjAniEvalChanItemMask_Constant)-1)  );
*/

enum cjAniPoseBufChanItem // must be a power of 2
{
	cjAniPoseBufChanItem_Color		= 0x01,   //  1 << ((cjAniEvalChanItem_Color & 7f) -1)
	cjAniPoseBufChanItem_Bool		= 0x02,   //  1 << ((cjAniEvalChanItem_Bool  & 7f) -1)
	cjAniPoseBufChanItem_Float		= 0x04,   //  1 << ((cjAniEvalChanItem_Float & 7f) -1)
	cjAniPoseBufChanItem_Vec3S		= 0x08,	  //  1 << ((cjAniEvalChanItem_Vec3S & 7f) -1)    for scale
	cjAniPoseBufChanItem_Vec3P		= 0x10,   //  1 << ((cjAniEvalChanItem_Vec3P & 7f) -1)    for pos
	cjAniPoseBufChanItem_Rot		= 0x20,
	cjAniPoseBufChanItem_Ref		= 0x40,

	cjAniPoseBufChanItem_Invalid	= 0xffff   // for cjWORD cjAniPoseBufHandle::m_poseBufChanItemType
};

enum cjAniPoseConstants
{
	cjAniPoseAccumEndIndex			= 2,		// 0:accum value, 1:accum delta
	cjAniPoseInvalidChanIndex		= 0xffff,
	cjAniPoseInvalidHashIndex		= 0xffff
};


//
// cjAniPoseBufHandle
//

class CJCORE_API cjAniPoseBufHandle : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjAniPoseBufHandle)
		stream << d.m_poseBufChanItem;
		stream << d.m_auxIndex;
	CJ_DECL_SERIALIZE_INLINE_END(cjAniPoseBufHandle)

public:
	cjAniPoseBufHandle(cjAniPoseBufChanItem channel, cjWORD auxIndex) : m_poseBufChanItem((cjWORD)channel), m_auxIndex(auxIndex) {}
    cjAniPoseBufHandle() : m_poseBufChanItem((cjWORD)cjAniPoseBufChanItem_Invalid), m_auxIndex(0) {}

    void					Init(cjAniPoseBufChanItem channel, cjWORD auxIndex) { m_poseBufChanItem = (cjWORD)channel; m_auxIndex = auxIndex; }
    
	void					Invalidate() { m_poseBufChanItem = cjAniPoseBufChanItem_Invalid; m_auxIndex = 0; }
    cjBool					IsValid() const { return m_poseBufChanItem != cjAniPoseBufChanItem_Invalid; }

	cjAniPoseBufChanItem	GetChannelItem() const { return (cjAniPoseBufChanItem)m_poseBufChanItem; }
    cjWORD					GetAuxIndex() const { return m_auxIndex; }

    cjBool operator== (const cjAniPoseBufHandle& rhs) const { return (m_poseBufChanItem == rhs.m_poseBufChanItem && m_auxIndex == rhs.m_auxIndex); }
    cjBool operator!= (const cjAniPoseBufHandle& rhs) const { return !operator==(rhs); }

protected:
	cjWORD					m_poseBufChanItem;	// cjAniPoseBufChanItem
	cjWORD					m_auxIndex;
};


//
// cjAniPoseBlend
//

enum
{
	cjAniPoseBlend_MaxBitPatterns	= 16,
	cjAniPoseBlend_MaxBlendSeqs		= 32,
};    



//
// cjAniEvalSPad
//

class cjAniEvalSPDat;

typedef cjBool (*cjAniEvalSPadFillFunc)(cjFLOAT etime, cjAniEvalSPDat* evalSPadDat);


enum cjAniSPadBlock
{
    cjAniSPADB_FillDat			= 0,
		   
    cjAniSPADB_EvaluatorDat,

    cjAniSPADB_BSBasisDat,

    cjAniSPADB_LinearColorSeg,
    cjAniSPADB_BSColorSeg,

    cjAniSPADB_CBoolSeg,

    cjAniSPADB_CubicFloatSeg,
    cjAniSPADB_BSFloatSeg,

    cjAniSPADB_CubicVec3SSeg,
    cjAniSPADB_BSVec3SSeg,

    cjAniSPADB_CubicVec3PSeg,
    cjAniSPADB_BSVec3PSeg,

    cjAniSPADB_SlerpRotSeg,
    cjAniSPADB_SquadRotSeg,
    cjAniSPADB_BSRotSeg,

	cjAniSPADB_Max  // MaxScratchPadBlocks
};


//
// cjAniEvalSPDat
//

// keep POD Structure
class CJCORE_API cjAniEvalSPDat : public cjMem
{
public:
    enum EFLAG
    {
        EFL_ALWAYS_UPDATE			= 0x1, 
        EFL_STEP_FUNCTION			= 0x2, 
        EFL_ALTERNATE_SPDATA		= 0x4,
    };

    cjAniEvalSPDat()
	{
		m_evaluator				= cjNULL; 
		m_evalChannel			= cjAniEvalChan_Base;

		m_segMinTime			= CJ_FTIME_INFINITY;
		m_segMaxTime			= CJ_FTIME_INVALID;

		m_fillFunc				= cjNULL;

		m_fillDat				= cjNULL;
		m_segDat				= cjNULL;

		m_lod					= 0;
		m_flag					= 0;
		m_pad					= 0;
	}

	const cjAniEvaluator*		GetEvaluator() const { return m_evaluator; }
    cjAniEvaluator*				GetEvaluator() { return m_evaluator; }
    void						SetEvaluator(cjAniEvaluator* evaluator) { m_evaluator = evaluator; }

    cjAniEvalChannel			GetEvalChannel() const { return (cjAniEvalChannel)m_evalChannel; }
    void						SetEvalChannel(cjAniEvalChannel channel) { m_evalChannel = channel; }

    cjBYTE						GetFlag() const { return m_flag; }
    void						SetFlag(cjBYTE flag) { m_flag = flag; }

	cjSWORD						GetLOD() const { return m_lod; }
	void						SetLOD(cjSWORD lod) { m_lod = lod; }

    cjBool						IsAlwaysUpdate() const { return (m_flag & EFL_ALWAYS_UPDATE) != 0; }
	cjBool						IsStepFunction() const { return (m_flag & EFL_STEP_FUNCTION) != 0; }
    void						SetStepFunction(cjBool enabled = cjTrue) { 
									if(enabled) m_flag |= EFL_STEP_FUNCTION;
									else m_flag &= ~EFL_STEP_FUNCTION;	 }

    cjBool						IsAlternateSPData() const { return (m_flag & EFL_ALTERNATE_SPDATA) != 0; }
    void						SetAlternateSPData(cjBool enabled = cjTrue)	{
									if(enabled) m_flag |= EFL_ALTERNATE_SPDATA;
									else m_flag &= ~EFL_ALTERNATE_SPDATA;	}

    cjFLOAT						GetSegMinTime() const { return m_segMinTime; }
	cjFLOAT						GetSegMaxTime() const { return m_segMaxTime; }
    void						SetSegTimeRange(cjFLOAT minTime, cjFLOAT maxTime) { m_segMinTime = minTime; m_segMaxTime = maxTime; }
    cjBool						IsSegDatValid(cjFLOAT etime) const { return (etime >= m_segMinTime && etime <= m_segMaxTime); }
	void*						GetSegDat() const { return m_segDat; }
    void						SetSegDat(void* segDat) { m_segDat = segDat; }

    cjAniEvalSPadFillFunc		GetFillFunc() const { return m_fillFunc; }
    void						SetFillFunc(cjAniEvalSPadFillFunc fillFunc) { m_fillFunc = fillFunc; }
	void*						GetFillDat() const { return m_fillDat; }
    void						SetFillDat(void* fillDat) { m_fillDat = fillDat;}
	
protected:
	// keep pod, 4byte alignment
    cjAniEvaluator*				m_evaluator;
    cjAniEvalChannel			m_evalChannel;
    
    cjFLOAT						m_segMinTime;
    cjFLOAT						m_segMaxTime;

    cjAniEvalSPadFillFunc		m_fillFunc;

    void*						m_fillDat;
    void*						m_segDat;

	cjSWORD						m_lod;
	cjBYTE						m_flag;
	cjBYTE						m_pad;
};


//
// cjAniConstants
//

class CJCORE_API cjAniConstants
{
public:
	static const cjName&	GetTransformCntrID()		{ return ms_transformCntrID;		}
							
	static const cjName&	GetNonAccumSuffixName()		{ return ms_nonAccumSuffix;			}
	static const cjINT32	GetNonAccumSuffixNameLen()	
	{ 
		cjAssert( (cjName(cjName_cjAniNonAccumSuffix).GetLen()) == ms_nonAccumSuffixNameLen );
		return ms_nonAccumSuffixNameLen;	
	}
							
    static const cjName&	GetStartTextKey()			{ return ms_stTxtKey;				}
    static const cjName&	GetEndTextKey()				{ return ms_enTxtKey;				}

	static const cjName&	GetMorphTextKey()			{ return ms_morphTextKey;			}
    static const cjINT32	GetMorphTextKeyLen()		{ return ms_morphTextKeyNameLen;	}

	static const cjName&	GetFlipAniCntrName()		{ return ms_flipAniCntrName;		}

protected:
    static cjName					ms_transformCntrID;

	static cjName					ms_nonAccumSuffix;
    static const cjINT32			ms_nonAccumSuffixNameLen;

    static cjName					ms_stTxtKey;
    static cjName					ms_enTxtKey;

    static cjName					ms_morphTextKey;
    static const cjINT32			ms_morphTextKeyNameLen;

	static cjName					ms_flipAniCntrName;
};


//
// cjAnimator
//

enum cjAnimatorRst
{
	cjAnimatorRst_Ok,
	cjAnimatorRst_Err,
	cjAnimatorRst_Err_Invalid_Sequence,	
};

enum cjAnimatorEvtType
{
    cjAnimatorAnim_Activated,
    cjAnimatorAnim_Deactivated,
    cjAnimatorAnim_Completed,
    cjAnimatorAnim_TxtKeyEvt,
    cjAnimatorAnim_EndOfSeq
};

enum cjAnimatorTransState
{
	cjAnimatorTS_NoTrans,
	cjAnimatorTS_Blend,
	cjAnimatorTS_Morph,
	cjAnimatorTS_CrossFade,
};

const cjFLOAT		cjAnimatorInvalidTime		= CJ_FTIME_INVALID;
const cjFLOAT		cjAnimatorSeqDatFrequency	= CJANI_SEQUENCE_DAT_FREQ;
const cjFLOAT		cjAnimatorMaxDuration		= -1.0f;




enum cjAniTransitionType
{
	cjAniTransitionType_Change,			// [txtMarker]   0: immediate change, duration= time amount for mixing src seq with dst seq from begin
										// [txtMarker]  >0: delayed change  , duration= time amount for mixing src seq with dst seq from end of src seq,  txtMarker: (ex: "end"(src) to "start"(dst))
	cjAniTransitionType_CrossFade,		// no txtMarker need,  duration = time for src(1.0->0.0), dst(0.0->1.0)
	cjAniTransitionType_Chain,			// srcSeq, chain seq#0, chain seq#1 .. , dstSeq : duration0: chain #0 / duration1: chain #1
	cjAniTransitionType_DefaultSync,	
	cjAniTransitionType_DefaultNoSync,
//	cjAniTransitionType_Morph,
	cjAniTransitionType_Invalid
};


//
// cjAnimatorCB
//

class cjAnimator;


class CJCORE_API cjAnimatorCB : public cjMem
{
public:
	virtual ~cjAnimatorCB() {}

	virtual void Animator_OnAniActivated(cjAnimator* animator, cjAniSeqID seqID, cjFLOAT curTime, cjFLOAT evtTime) = 0;
	virtual void Animator_OnAniDeactivated(cjAnimator* animator, cjAniSeqID seqID, cjFLOAT curTime, cjFLOAT evtTime) = 0;
	virtual void Animator_OnAniCompleted(cjAnimator* animator, cjAniSeqID seqID, cjFLOAT curTime, cjFLOAT evtTime) = 0;

	virtual void Animator_OnTxtKeyEvt(cjAnimator* animator,	cjAniSeqID seqID, const cjName& txtKeyName, 
								const cjNameMatch* nameMatch, cjFLOAT curTime, cjFLOAT evtTime) = 0;

	virtual void Animator_OnEndOfSeq(cjAnimator* animator, cjAniSeqID seqID, cjFLOAT curTime, cjFLOAT evtTime) = 0;
};


class CJCORE_API cjAnimatorCBDat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAnimatorCBDat, cjRefObj,0 , CJCORE_API)

public:
	cjAnimatorCBDat() {}
    cjAnimatorCBDat(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* nameMatch=cjNULL)
	{
		m_evtType = evtType;
		m_seqID = seqID;
		m_nameMatch = nameMatch;
	}

    cjNameMatch* GetNameMatch() const { return m_nameMatch; }
    void SetNameMatch(cjNameMatch* nameMatch);

public:
    cjAnimatorEvtType				m_evtType;
    cjAniSeqID						m_seqID;

protected:
    cjNameMatchRPtr					m_nameMatch;
};

inline cjAnimatorCBDat::~cjAnimatorCBDat()
{
}

cjRefPtrDefine(cjAnimatorCBDat)


class CJCORE_API cjAniTxtMarkerPair : public cjMem
{
public:
	cjAniTxtMarkerPair(const cjName& startMarkerAtSrcSeq, const cjName& endMarkerAtDstSeq)
	{
		cjAssert(startMarkerAtSrcSeq.IsValid() || endMarkerAtDstSeq.IsValid());

		m_startMarker = startMarkerAtSrcSeq;
		m_endMarker = endMarkerAtDstSeq;
	}

	const cjName& GetStMarker() { return m_startMarker; }
	const cjName& GetEnMarker() { return m_endMarker;   }


	cjName							m_startMarker;
	cjName							m_endMarker;
};
typedef cjTAry<cjAniTxtMarkerPair*>	cjAniTxtMarkerPairs;
	


class CJCORE_API cjAniSeqIDChainInfo : public cjMem
{
public:
	cjAniSeqID						m_seqID;
	cjFLOAT							m_duration;
};
typedef cjTAry<cjAniSeqIDChainInfo> cjAniSeqIDChainInfos;



class CJCORE_API cjAniTransition : public cjMem
{
public:
	cjAniTransitionType				m_type;
	cjFLOAT							m_duration;

	cjAniTxtMarkerPairs				m_markers; // for cjAniTransitionType_Change
	cjAniSeqIDChainInfos			m_chains;  // for cjAniTransitionType_Chain

public:
	cjAniTransitionType GetType() { return m_type; }
	cjAniTxtMarkerPairs& GetBlendPairs() { return m_markers; }
	cjFLOAT	GetDuration() { return m_duration; }
	cjAniSeqIDChainInfos& GetChains() { return m_chains; }

	const cjAniTransition& GetDefault();
	
private:
	static cjAniTransition			ms_default;
};


class CJCORE_API cjAniSeqInfo : public cjMem
{
public:
	cjAniSeqID					m_seqID;
	cjName						m_seqName;		// xpkg.yname

	cjTHash<cjAniSeqID, cjAniTransition*> m_transitions;
};

typedef cjTHash<cjAniSeqID, cjAniSeqInfo*> AniID2SeqInfoMap;



class CJCORE_API cjAnimatorDat : public cjRefObj
{
	CJSER_DECL_CLASS(cjAnimatorDat, cjRefObj,0 , CJCORE_API)

public:	
	const static cjUINT32 MAX_CHAIN_SEQID_COUNT;

	class Sequence : public cjMem
	{
	public:
		cjAniSeqID						m_seqID;
		cjName							m_seqName;
		cjName							m_fileName;

		typedef cjTHash<cjAniSeqID, cjAniTransition*> Transitions;
		Transitions						m_transitions;
	};
	typedef cjTHash<cjAniSeqID, Sequence*> AniID2SeqMap;
		
public:
	cjAnimatorDat();

	cjAniTransition* GetTransition(cjAniSeqID srcID, cjAniSeqID dstID) const;
	cjAnimatorRst IsTransitionNeed(cjAniSeqID srcID, cjAniSeqID dstID, cjBool& rstIsTransitionNeed) const;

	cjBool IsValidChainTransition(cjAniSeqID srcID, cjAniSeqID dstID, cjAniTransition* transition);
	void RecursiveCollectChainIDs(cjAniSeqID srcID, cjAniSeqID dstID, cjAniTransition* transition, cjAniSeqIDAry& chainIDs);

	cjAnimatorRst UpdateSeqID(cjAniSeqID oldSeqID, cjAniSeqID newSeqID);
	void UpdateTransitionsContainingSeq(cjAniSeqID oldSeqID, cjAniSeqID newSeqID);
		
protected:
	AniID2SeqMap						m_sequenceMap;
};

cjRefPtrDefine(cjAnimatorDat)



class CJCORE_API cjAnimatorChainCompletionInfo : public cjRefObj
{
	CJSER_DECL_CLASS(cjAnimatorChainCompletionInfo, cjRefObj,0 , CJCORE_API)

public:
    cjAnimatorChainCompletionInfo() {}

    void SetName(const cjName& name) { m_name = name; }
    void SetNextName(const cjName& nextName) { m_nextName = nextName; }

    cjFLOAT							m_seqStart;
    cjFLOAT							m_inSeqBeginTime;
   
	cjFLOAT							m_transStTime;
    cjFLOAT							m_transEnTime;

    cjName							m_name;
    cjName							m_nextName;

	cjAniTransition*				m_transition;

    cjAniSeqID						m_srcSeqID;
    cjAniSeqID						m_dstSeqID;
};

inline cjAnimatorChainCompletionInfo::~cjAnimatorChainCompletionInfo() 
{
}

cjRefPtrDefine(cjAnimatorChainCompletionInfo)



class CJCORE_API cjAnimatorCompletionInfo : public cjRefObj
{
	CJSER_DECL_CLASS_X(cjAnimatorCompletionInfo, cjRefObj,0 , CJCORE_API)

public:
    cjFLOAT							m_dstTimeTransitionComplete;
    cjFLOAT							m_timeToCompleteTransition;

    cjFLOAT							m_srcTransitionOccurTime;
    cjFLOAT							m_chainTimeToComplete;

    cjAniTxtMarkerPair*				m_blendPair;

	//cjopti : cjTAry<cjAnimatorChainCompletionInfoPtr> -> cjSet<..>
    cjTAry<cjAnimatorChainCompletionInfoRPtr> m_chainCompletionInfoSet;

	void AddChainCompletionInfo(cjAnimatorChainCompletionInfo* info)
	{
		if(!info)
			return;
		
		m_chainCompletionInfoSet.PushUnique(info);
	}
};

inline cjAnimatorCompletionInfo::~cjAnimatorCompletionInfo()
{
}

cjRefPtrDefine(cjAnimatorCompletionInfo)



class CJCORE_API cjAnimatorTimeline : public cjMem
{
	CJTYPEINFO_SUPPORT_DECL(cjAnimatorTimeline)	

public:
	cjAnimatorTimeline() {} 
	~cjAnimatorTimeline() {} 

    void SetValues(cjFLOAT evtTime, cjAnimatorEvtType evtType, cjAniSeqID seqID, 
		cjNameMatch* nameMatch, const cjName& txtKeyMatched)
	{
		m_evtTime = evtTime;
		m_evtType = evtType;
		m_seqID = seqID;
		m_nameMatch = nameMatch;
		m_txtKeyMatched = txtKeyMatched;
	}

    cjFLOAT							m_evtTime;
    cjAnimatorEvtType				m_evtType;
    cjAniSeqID						m_seqID;
    cjNameMatchRPtr					m_nameMatch;
    cjName							m_txtKeyMatched;
};

CJTYPEINFO_SUPPORT_IMPL(cjAnimatorTimeline);


#endif
