
#ifndef _CJANISPAD_H_
#define _CJANISPAD_H_


class cjAniCntrSeq;
class cjAniEvaluator;
class cjAniPoseBuf;
class cjAniCntrManager;

cjRefPtrForwardDefine(cjSceneNode)


//
// cjAniSPad
//

class CJCORE_API cjAniSPad : public cjRef
{
	CJSER_DECL_CLASS(cjAniSPad, cjRef, 0, CJCORE_API)

public:
    cjAniSPad(cjAniCntrSeq* seq);

    enum
    {
        EPOS					= cjAniEvalChan_Pos,
        EROT					= cjAniEvalChan_Rot,
        ESCALE					= cjAniEvalChan_Scl,
        EMAX_ACCUM_CHANS		= cjAniEvalChan_PRS_Max,
        EINVALID_OUTPUT_INDEX	= 0xffff,
    };
	
    struct SegHdr
    {
        cjSWORD						m_lod;
        cjWORD						m_outputPBIndex;
    };


	//
	// Linear/BSpline-Color
	//

    struct LColorSeg // 4byte align
    {
        SegHdr						m_hdr;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        cjColorF					m_value0;
        cjColorF					m_outTangent0;
    };

    struct LColorFill
    {
        cjUINT32					m_lastIndex;
        cjUINT32					m_keyCount;
        cjAniKeyColor*				m_keys;				// ref
        cjAniKeyColor::EKEY_TYPE	m_keyType;
        cjBYTE						m_keySiz;
        cjChar						m_pad[3];			// for 4byte align
    };

    struct BSColorSeg // 4byte align
    {
        SegHdr						m_hdr;
        cjBSplineBasisF3Dat*		m_bsBasisDat;
        cjFLOAT						m_src[16];			// 4 cjColorF control points
    };

    struct BSColorFill
    {
        cjBSplineDat::BSOffset		m_cpOffset;
		cjBSplineDat*				m_cpAry;

        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        cjINT32						m_lastMin;        
    };

    struct BSCompColorFill // half cjFLOAT version of BSColorFill
    {
        BSColorFill					m_bsColorDat;

		// to decompress control points
        cjFLOAT						m_halfOffset;	
        cjFLOAT						m_halfRange;
    };
	

	//
	// Const Bool
	//

    struct CBoolSeg	// 4byte align
    {
        SegHdr						m_hdr;
        cjFLOAT						m_val0;
    };

    struct CBoolFill // 4byte align
    {
        cjUINT32					m_lastIndex;
        cjUINT32					m_keyCount;
        cjAniKeyBool*				m_keys;
        cjAniKeyBool::EKEY_TYPE		m_keyType;
        cjBYTE						m_keySiz;
        cjBYTE						m_pad[3];
    };

    struct CBoolTimelineFill
    {
        CBoolFill					m_baseDat;
        cjUINT32					m_prevIndex;
        cjBool						m_prevValue;
    };
	

	//
	// Cubic-1 Float
	//

    struct CubicFloatSeg  // 4byte align
    {
        SegHdr						m_hdr;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        cjFLOAT						m_value0;
        cjFLOAT						m_outTangent0;
        cjFLOAT						m_a0;
        cjFLOAT						m_b0;
    };

    struct CubicFloatFill // 4byte align
    {
        cjUINT32					m_lastIndex;
        cjUINT32					m_keyCount;
        cjAniKeyFloat*				m_keys;
        cjAniKeyFloat::EKEY_TYPE	m_keyType;
        cjBYTE						m_keySiz;
        cjBYTE						m_pad[3];
    };
	

	//
	// BSpline-1 Float
	//

    struct BSFloatSeg // 4byte align
    {
        SegHdr						m_hdr;
        cjBSplineBasisF3Dat*		m_bsBasisDat;
        cjFLOAT						m_src[4]; // 4 control points
    };

    struct BSFloatFill
    {
        cjBSplineDat::BSOffset		m_cpOffset;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        int							m_lastMin;
        cjBSplineDat*				m_bsDat; // pointer to source control point data
    };

    struct BSCompFloatFill	// half cjFLOAT version of BSFloatFill
    {
        BSFloatFill					m_baseDat;
        cjFLOAT						m_halfOffset;
        cjFLOAT						m_halfRange;
    };


	//
    // Cubic-Vec3(3Float)
	//

    struct CubicVec3Seg // 4byte align
    {
        SegHdr						m_hdr;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;

        cjVec3						m_val0;
        cjVec3						m_outTangent0;
        cjVec3						m_a0;
        cjVec3						m_b0;
    };

    struct CubicVec3Fill
    {
        cjUINT32					m_lastIndex;
        cjUINT32					m_keyCount;
        cjAniKeyVec3*				m_keys;
        cjAniKeyVec3::EKEY_TYPE		m_keyType;
        cjBYTE						m_keySiz;
        cjBYTE						m_pad[3]; // to maintain 4 byte alignment
    };
	

	//
	// BSpline-Vec3
	//

    struct BSVec3Seg // 4byte align
    {
        SegHdr						m_hdr;
        cjBSplineBasisF3Dat*		m_bsBasisDat;	// pointer to scratch pad entry
        cjFLOAT						m_src[12];		// 4 cjVec3 control points
    };

    struct BSVec3Fill
    {
        cjBSplineDat::BSOffset		m_cpOffset;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        cjINT32						m_lastMin;
        cjBSplineDat*				m_bsDat;	// pointer to source control point data
    };

    struct BSVec3FillCmpr // half cjFLOAT version of BSVec3Fill
    {
        BSVec3Fill					m_baseDat;
        cjFLOAT						m_halfOffset;
        cjFLOAT						m_halfRange;
    };


	//
	//	Rotation Stuff
	//

		// slerp and squad segement
	struct RotFill	
    {
        cjUINT32					m_lastIndex;
        cjUINT32					m_keyCount;
        cjAniKeyRot*				m_keys;
        cjAniKeyRot::EKEY_TYPE		m_keyType;
        cjBYTE						m_keySiz;
        cjBYTE						m_pad[3]; // keep 4 byte alignment
    };

		// Euler Rotation
    struct EulerRotFill
    {
        cjFLOAT						m_minTime[3];
        cjFLOAT						m_maxTime[3];
        CubicFloatFill				m_floatFill[3];
        CubicFloatSeg				m_floatSeg[3];
    };

		// Slerp Rot
	struct SlerpRotSeg // 4byte align
    {
        SegHdr						m_hdr;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        cjQuat						m_quat0;
        cjQuat						m_quat1;
    };

    struct SquadRotSeg // 4byte align
    {
        SlerpRotSeg					m_baseDat;
        cjQuat						m_outQuat0;
        cjQuat						m_inQuat1;
    };


	//
	// Bezier Spline Rotation Data
	//

    struct BSRotSeg		// 4byte align
    {
        SegHdr						m_hdr;
        cjBSplineBasisF3Dat*		m_bsBasisDat;		// pointer to scratch pad entry

		// quat.w = [0,4,8,12]
		// quat.x = [1,5,9,13]
		// quat.y = [2,6,10,14]
		// quat.z = [3,7,11,15]
        cjFLOAT						m_quatRotCP[16];	// 4 quaternion control points
    };

    struct BSRotFill
    {
        cjBSplineDat::BSOffset		m_cpOffset;			// start index;
        cjFLOAT						m_stTime;
        cjFLOAT						m_invDTime;
        cjINT32						m_lastMin;        
		cjBSplineDat*				m_bsDat;			// 4point control point
    };
		
    struct BSRotCmpr	// cjFLOAT->half(word) compressed BSplineRotDat
    {
        BSRotFill					m_bsRotFill;		
        cjFLOAT						m_halfOffset;		// for convert half cjFLOAT to cjFLOAT
        cjFLOAT						m_halfRange;
    };


	//
	// PathFill
	//

    struct PathFill
    {
        cjFLOAT						m_lastTime;

        cjVec3						m_pos;
        cjQuat						m_rot;

        cjFLOAT						m_pctMinTime;
        cjFLOAT						m_pctMaxTime;
        CubicFloatFill				m_pctFill;
        CubicFloatSeg				m_pctSeg;

		// by percent
        cjFLOAT						m_posMinPct;
        cjFLOAT						m_posMaxPct;
        CubicVec3Fill				m_posFill;
        CubicVec3Seg				m_posSeg;
        cjWORD						m_flag;
        cjSWORD						m_followAxis;
        cjFLOAT						m_maxBankAngle;
        cjFLOAT						m_smoothing;
        cjFLOAT						m_maxCurvature;
        cjINT32						m_bankDir;
        cjFLOAT						m_totalPathLength;
        cjFLOAT*					m_pathLengths;
    };


	//
    // LookAt
	//

    struct LookAtFill
    {
		cjFLOAT						m_lastTime;     // 4

		cjVec3						m_pos;			// 12
		cjFLOAT						m_roll;			// 4
		cjVec3						m_scale;		// 12

		cjWORD						m_flags;		// 2
		cjWORD						m_pad;			// 2

		cjSceneNodeRPtr				m_lookAt;		// 8
		cjSceneNodeRPtr				m_driven;		// 8
		cjAniEvalSPDat				m_evalSPDat[3]; // x
		cjBSplineBasisF3DatRPtr		m_bsBasisDat[3];// 32
    };


public:	
	cjAniSPad(cjAniEvaluator* evaluator);

    void* GetDatBlock(cjAniSPadBlock spadBlk)		const { cjAssert(spadBlk < cjAniSPADB_Max); return m_block[spadBlk]; }
    cjWORD GetBlockDatCount(cjAniSPadBlock spadBlk) const { cjAssert(spadBlk < cjAniSPADB_Max); return m_blockDatCount[spadBlk]; }
    cjWORD GetBlockDatSiz(cjAniSPadBlock spadBlk)	const { cjAssert(spadBlk < cjAniSPADB_Max); cjAssert(spadBlk != cjAniSPADB_FillDat); return m_blockDatSiz[spadBlk]; }

    void Update(cjFLOAT etime, cjFLOAT scaledTime, cjSWORD slod, cjAniPoseBuf* poseBuf);
		
    void InitPooledSPad(cjAniCntrSeq* cntrSeq); // initializer for cjAniSPad in pool
    void ResetAllEvalSpadDat(); // using pool for evalSPDats in m_block

    void SetLastAccumTM(const cjQuatTM& tm) { m_lastAccumTM = tm; }
    void ClearAccumTM()
	{
		m_lastAccumScaledTime = CJ_FTIME_INFINITY;

		m_lastAccumTM.SetScale(cjVec3::UNIT);
		m_lastAccumTM.SetRotate(cjQuat::IDENTITY);
		m_lastAccumTM.SetPos(cjVec3::ZERO);
		
		m_loopAccumTM.SetScale(cjVec3::UNIT);
		m_loopAccumTM.SetRotate(cjQuat::IDENTITY);
		m_loopAccumTM.SetPos(cjVec3::ZERO);
		
		m_startAccumTM.SetScale(cjVec3::UNIT);
		m_startAccumTM.SetRotate(cjQuat::IDENTITY);
		m_startAccumTM.SetPos(cjVec3::ZERO);
		
		m_startRotAccum.SetIdentity();
	}

	void _CommonInit();

protected:
    cjAniSPad();

    void Init(cjAniCntrSeq* cntrSeq);
    void Init(cjAniEvaluator* evaluator);
    void Shutdown();

    void UpdatePoseBufFromSPadSegDat(cjFLOAT scaledTime, cjSWORD sLOD, 
        cjBSplineBasisF3Dat* bsBasisDatBlk, cjAniPoseBuf* poseBuf);

    void UpdateAccumTransformDelta(cjFLOAT etime, cjFLOAT scaledTime, cjSWORD sLOD, cjAniPoseBuf* poseBuf);

protected:    
    cjAniCntrManager*				m_cntrMan;
    
    cjUINT32						m_spadBufSiz;
    cjBYTE*							m_spadBuf;

    cjBYTE*							m_block[cjAniSPADB_Max];
    cjWORD							m_blockDatCount[cjAniSPADB_Max];
    cjWORD							m_blockDatSiz[cjAniSPADB_Max];

    cjWORD							m_pbItemCount;
    cjSWORD							m_lastPBUpdateLOD;
    cjFLOAT							m_lastPBUpdateTime;

    cjAniEvaluator*					m_accumEvaluator;
    cjAniEvalSPDat*					m_accumEvalSPDat[EMAX_ACCUM_CHANS];
    cjFLOAT							m_lastAccumScaledTime;

    cjQuatTM						m_lastAccumTM;
    cjQuatTM						m_loopAccumTM;
    cjQuatTM						m_startAccumTM;
    cjMatrix3						m_startRotAccum;

};

cjRefPtrDefine(cjAniSPad)


#endif

