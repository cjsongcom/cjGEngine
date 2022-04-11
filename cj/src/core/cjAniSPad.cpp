
#include "cjCorePCH.h"
#include "cjAniSPad.h"
 

//
// cjAniSPad
//

CJSER_IMPL_CLASS(cjAniSPad, cjRef);

//CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniSPad)
// no need property copy
//CJSER_IMPL_CLONE_PROPCOPY_END(cjAniSPad)

 
void cjAniSPad::_CommonInit()
{
    m_cntrMan				= cjNULL;
	m_spadBufSiz			= 0;
    m_spadBuf				= cjNULL;
	m_pbItemCount			= 0;
    m_lastPBUpdateLOD		= -2;
    m_lastPBUpdateTime		= CJ_FTIME_INVALID;
    m_accumEvaluator		= cjNULL;
	m_lastAccumScaledTime	= CJ_FTIME_INVALID;

    m_blockDatSiz[cjAniSPADB_FillDat]			= 0;
    m_blockDatSiz[cjAniSPADB_EvaluatorDat]		= sizeof(cjAniEvalSPDat);

    m_blockDatSiz[cjAniSPADB_BSBasisDat]		= sizeof(cjBSplineBasisF3Dat);

    m_blockDatSiz[cjAniSPADB_LinearColorSeg]	= sizeof(LColorSeg);
    m_blockDatSiz[cjAniSPADB_BSColorSeg]		= sizeof(BSColorSeg);

    m_blockDatSiz[cjAniSPADB_CBoolSeg]			= sizeof(CBoolSeg);

    m_blockDatSiz[cjAniSPADB_CubicFloatSeg]	= sizeof(CubicFloatSeg);
    m_blockDatSiz[cjAniSPADB_BSFloatSeg]		= sizeof(BSFloatSeg);

    m_blockDatSiz[cjAniSPADB_CubicVec3SSeg]	= sizeof(CubicVec3Seg);
    m_blockDatSiz[cjAniSPADB_BSVec3SSeg]		= sizeof(BSVec3Seg);

    m_blockDatSiz[cjAniSPADB_CubicVec3PSeg]	= sizeof(CubicVec3Seg);
    m_blockDatSiz[cjAniSPADB_BSVec3PSeg]		= sizeof(BSVec3Seg);

    m_blockDatSiz[cjAniSPADB_SlerpRotSeg]		= sizeof(SlerpRotSeg);
    m_blockDatSiz[cjAniSPADB_SquadRotSeg]		= sizeof(SquadRotSeg);
    m_blockDatSiz[cjAniSPADB_BSRotSeg]			= sizeof(BSRotSeg);

    cjAssert(cjAniSPADB_BSRotSeg == cjAniSPADB_Max - 1);

    for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
    {
        m_block[i] = cjNULL;
        m_blockDatCount[i] = 0;

        cjAssert(m_blockDatSiz[i] % 4 == 0); // keep 4 byte alignment
        //cjAssert(i == cjAniSPADB_FillDat || m_blockDatSiz[i] > 0);
    }

    m_accumEvalSPDat[EPOS]		= cjNULL;
    m_accumEvalSPDat[EROT]		= cjNULL;
    m_accumEvalSPDat[ESCALE]	= cjNULL;
}

cjAniSPad::cjAniSPad(cjAniCntrSeq* seq)
{
	_CommonInit();
    Init(seq);
}

cjAniSPad::cjAniSPad()
{
	_CommonInit();
}

cjAniSPad::cjAniSPad(cjAniEvaluator* evaluator)
{
	_CommonInit();
    Init(evaluator);
}

cjAniSPad::~cjAniSPad()
{
    Shutdown();
}

void cjAniSPad::Update(cjFLOAT etime, cjFLOAT scaledTime, cjSWORD slod, cjAniPoseBuf* poseBuf)
{
    cjAssert(poseBuf);

    cjAniEvalSPDat* curEvalSPDat = (cjAniEvalSPDat*)m_block[cjAniSPADB_EvaluatorDat];
    cjAniEvalSPDat* endEvalSPDat = curEvalSPDat + m_blockDatCount[cjAniSPADB_EvaluatorDat];

    if(etime != m_lastPBUpdateTime || slod != m_lastPBUpdateLOD)
    {
        while(curEvalSPDat < endEvalSPDat)
        {
            if(slod <= curEvalSPDat->GetLOD())
            {
                cjAssert(curEvalSPDat->GetEvaluator());

                if(  scaledTime > curEvalSPDat->GetSegMaxTime() || 
					(scaledTime == curEvalSPDat->GetSegMaxTime() && (curEvalSPDat->IsStepFunction())) || 
					scaledTime < curEvalSPDat->GetSegMinTime() )
                {
                    cjAniEvalSPadFillFunc pfnSPadFillFunc = curEvalSPDat->GetFillFunc();
                    cjAssert(pfnSPadFillFunc);
					
                    (*pfnSPadFillFunc)(scaledTime, curEvalSPDat);
                }
            }

            curEvalSPDat++;

        } // end while

		// update bsplineBasisF3Dat
        cjBSplineBasisF3Dat* bsBasisDatBlk = (cjBSplineBasisF3Dat*)m_block[cjAniSPADB_BSBasisDat];
        UpdatePoseBufFromSPadSegDat(scaledTime, slod, bsBasisDatBlk, poseBuf);

        if(m_accumEvaluator)
            UpdateAccumTransformDelta(etime, scaledTime, slod, poseBuf);
    }
    else
    {
		// Handle MultiTargetPoseHdr
        cjAssert(m_cntrMan);

        cjAniMTargetPoseHandler* mtPoseHandler = m_cntrMan->GetMTPoseHandler();
        if(etime != mtPoseHandler->GetLastUpdateTime())
        {
            while(curEvalSPDat < endEvalSPDat)
            {
				// always update marked only
                if(slod <= curEvalSPDat->GetLOD() && curEvalSPDat->IsAlwaysUpdate())
                {
                    cjAniEvaluator* evaluator = curEvalSPDat->GetEvaluator();
                    cjAssert(evaluator);

                    cjAniEvalChannel channel = curEvalSPDat->GetEvalChannel();
                    cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItem(channel);

                    SegHdr* segHdr = (SegHdr*)curEvalSPDat->GetSegDat();
                    cjAssert(segHdr);

                    cjAniPoseBufHandle pbHandle(pbChanItem, segHdr->m_outputPBIndex);

#ifdef CJDEBUG
                    const cjAniEvaluatorTag& evalTag = evaluator->GetTag();
                    cjAniPoseBufHandle pbHandleDebug;
                    cjSWORD slodDebug;

					cjAssert(
						poseBuf->GetHandle(evalTag.GetID(), 
						evalTag.GetPropRttiName(), 
                        evalTag.GetCntrRttiName(), 
						evalTag.GetCntrID(),
						evalTag.GetEvaluatorID(),
						pbChanItem, 
                        pbHandleDebug, 
						slodDebug)
						);

                    cjAssert(pbHandleDebug == pbHandle);
#endif

                    cjFLOAT* pbDatRPtr = poseBuf->GetDat(pbHandle);
                    cjAssert(pbDatRPtr);

                    evaluator->UpdateChannel(scaledTime, channel, curEvalSPDat, pbDatRPtr);
                }

                curEvalSPDat++;
            }

            if(m_accumEvaluator)
                UpdateAccumTransformDelta(etime, scaledTime, slod, poseBuf);
        }
    }

    m_lastPBUpdateTime = etime;
    m_lastPBUpdateLOD = slod;
}

void cjAniSPad::InitPooledSPad(cjAniCntrSeq* cntrSeq)
{
    m_pbItemCount				= 0;
    
	m_lastPBUpdateLOD			= cjAniCntrSeq::LODSKIPEVALUATOR;
    m_lastPBUpdateTime			= CJ_FTIME_INVALID;

    m_accumEvaluator			= cjNULL;
    m_accumEvalSPDat[EPOS]		= cjNULL;
    m_accumEvalSPDat[EROT]		= cjNULL;
    m_accumEvalSPDat[ESCALE]	= cjNULL;

    m_lastAccumScaledTime		= CJ_FTIME_INVALID;
    m_lastAccumTM.MakeInvalid();

    Init(cntrSeq);
}

void cjAniSPad::ResetAllEvalSpadDat()
{
    cjAniEvalSPDat* evalSPDatAry = (cjAniEvalSPDat*)m_block[cjAniSPADB_EvaluatorDat];

    for(cjUINT32 i=0; i<m_blockDatCount[cjAniSPADB_EvaluatorDat]; i++)
    {
        cjAniEvaluator* evaluator = evalSPDatAry[i].GetEvaluator();

        if(evaluator && evaluator->IsShutdownRequired())
            evaluator->DisposeSPadDat(evalSPDatAry[i].GetEvalChannel(), &evalSPDatAry[i]);

		// call destructor explicitly 
        evalSPDatAry[i].~cjAniEvalSPDat();
    }

    // Delete BSplineBasisDat
	cjBSplineBasisF3Dat* bsBasisDatBlk = (cjBSplineBasisF3Dat*)m_block[cjAniSPADB_BSBasisDat];
    for(cjUINT32 i=0; i<m_blockDatCount[cjAniSPADB_BSBasisDat]; i++)
    {
        cjAssert(bsBasisDatBlk[i].GetRef() == 1); // must be 1		

		// call destructor explicitly 
        cjBSplineBasisF3Dat::_PDel(&bsBasisDatBlk[i]); // bsBasisDatBlk[i].~cjBSplineBasisF3Dat(); 
    }

    for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
    {
        m_block[i] = cjNULL;
        m_blockDatCount[i] = 0;
    }

    m_accumEvaluator	= cjNULL;
    m_cntrMan			= cjNULL;
}

void cjAniSPad::Init(cjAniCntrSeq* cntrSeq)
{
    cjAssert(cntrSeq);

    m_cntrMan = cntrSeq->GetOwner();
    cjAssert(m_cntrMan);
	
	cjRefObjNameHash* sceneNodesHash = m_cntrMan->GetNodesHash();
    cjAssert(sceneNodesHash);

    const cjName& accumRootName = m_cntrMan->GetAccumRootName();

    cjAniPoseBind* poseBind = m_cntrMan->GetPoseBind();
    cjAssert(poseBind);

    cjAniPoseBuf* poseBuf = cntrSeq->GetPoseBuf();

    cjAssert(poseBuf);
    cjAssert(poseBuf->GetPoseBind() == poseBind);
    cjAssert(poseBuf->GetTotalDatCount() == poseBind->GetTotalBindCount());

    cjUINT32 maxSPadSiz = m_cntrMan->GetMaxSPadBufSiz();

	//
	// realloc size
	//
    if(m_spadBufSiz < maxSPadSiz)
    {
        if(m_spadBuf)
			cjMemDeAlloc(m_spadBuf);

		m_spadBuf = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * maxSPadSiz);
        m_spadBufSiz = maxSPadSiz;
    }
	
	//
	// basisData
	//	
    cjUINT32 maxBasisDats = m_cntrMan->GetMaxUniqueBasisDats() + 1;
    cjBSplineBasisF3Dat** basisDats = (cjBSplineBasisF3Dat**)cjMemAlloc(sizeof(cjBSplineBasisF3Dat*) * maxBasisDats);

    cjUINT32 basisDatCount = 0;
    cjUINT32 totalFillSiz = 0;
	cjWORD maxDats[cjAniSPADB_Max];
    
	for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
        maxDats[i] = 0;

    cjAniSeq* aniSeq = cntrSeq->GetSeqDat();
    cjAssert(aniSeq);

    cjUINT32 evaluatorCount = aniSeq->GetEvaluatorCount();
    for(cjUINT32 i=0; i<evaluatorCount; i++)
    {
        cjAniEvaluator* evaluator = aniSeq->GetEvaluator(i);
        cjAssert(evaluator);

        cjBool forceAlwaysUpdate = cjFalse;
		cjUINT32 maxEvalChanCount = evaluator->GetMaxChannels();

		// 1:pos , 3:transform(pos,rot,scale)
        cjAssert(maxEvalChanCount == 1 || (maxEvalChanCount == 3 && evaluator->IsTransformEvaluator()));

		// transform evaluator and accumRootName exist
        if(maxEvalChanCount == 3 && accumRootName.IsValid() && accumRootName == evaluator->GetID())
        {
            cjAssert(!evaluator->IsReferenced());

            m_accumEvaluator = evaluator;
            forceAlwaysUpdate = cjTrue;
        }
		        
        cjINT32 sharedFillSiz = 0;
        for(cjUINT32 chanIdx=0; chanIdx<maxEvalChanCount; chanIdx++)
        {
            cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(chanIdx);
            if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjUINT32				curChanFillSiz;
                cjBool					curChanIsSharedFill;
                cjAniSPadBlock			curChanSpadBlk;
                cjBSplineBasisF3Dat*	curChanBasisDat;

                if(evaluator->GetSPadInfo(chanIdx, forceAlwaysUpdate, sceneNodesHash, curChanFillSiz, 
					curChanIsSharedFill, curChanSpadBlk, curChanBasisDat))
                {
                    cjAssert(curChanFillSiz % 4 == 0); // keep 4byte align
                    cjAssert(
						curChanSpadBlk != cjAniSPADB_EvaluatorDat && 
						curChanSpadBlk != cjAniSPADB_FillDat && 
						curChanSpadBlk != cjAniSPADB_BSBasisDat && 
						curChanSpadBlk != cjAniSPADB_Max);

                    if(curChanIsSharedFill)
                    {                        
                        cjAssert(sharedFillSiz >= 0);
                       
						if(sharedFillSiz == 0)
						{
                            sharedFillSiz = curChanFillSiz;
						}
                        else
                        {                            
                            cjAssert((cjINT32)curChanFillSiz == sharedFillSiz);
                            curChanFillSiz = 0;
                        }
                    }
                    else
                    {                        
                        cjAssert(sharedFillSiz <= 0);	// No channels can share the fill data.
                        sharedFillSiz = -1;				// -1: not shared.
                    }

                    if(curChanFillSiz > 0)
                    {
                        maxDats[cjAniSPADB_FillDat]++;
                        totalFillSiz += curChanFillSiz;
                    }

                    maxDats[cjAniSPADB_EvaluatorDat]++;
                    maxDats[curChanSpadBlk]++;
					                    
                    if(curChanBasisDat)
                    {
                        cjBool found = cjFalse;
                        for(cjUINT32 m=0; m<basisDatCount; m++)
                        {
                            if(curChanBasisDat == basisDats[m])
                            {
                                found = cjTrue;
                                break;
                            }
                        }

                        if(!found)
                        {                            
                            cjAssert(basisDatCount < maxBasisDats);
                            basisDats[basisDatCount] = curChanBasisDat;
                            basisDatCount++;
                        }
                    }
                }
            }
        }
    }

    maxDats[cjAniSPADB_BSBasisDat] = (cjWORD)basisDatCount;
	
#ifdef CJDEBUG
    cjUINT32 totalBufSiz = totalFillSiz;

    for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
        totalBufSiz += maxDats[i] * m_blockDatSiz[i];

	cjAssert(totalBufSiz <= m_spadBufSiz);

    cjUINT32 uniqueBasisDatCount;
    cjAssert(totalBufSiz == aniSeq->GetScratchPadBufSiz(m_cntrMan->IsCumulativeAni(), sceneNodesHash, uniqueBasisDatCount));
	cjAssert(basisDatCount == uniqueBasisDatCount);

#endif

    m_block[cjAniSPADB_FillDat]			= m_spadBuf;
    m_block[cjAniSPADB_EvaluatorDat]	= m_block[cjAniSPADB_FillDat] + totalFillSiz;

    cjAssert(cjAniSPADB_BSRotSeg == cjAniSPADB_Max - 1);

    for(cjUINT32 i=cjAniSPADB_EvaluatorDat; i<cjAniSPADB_BSRotSeg; i++)
        m_block[i + 1] = m_block[i] + maxDats[i] * m_blockDatSiz[i];

	cjAssert(m_block[cjAniSPADB_BSRotSeg] + maxDats[cjAniSPADB_BSRotSeg] * m_blockDatSiz[cjAniSPADB_BSRotSeg] == m_spadBuf + totalBufSiz);

    cjAniEvalSPDat* curEvalSPDat = (cjAniEvalSPDat*)m_block[cjAniSPADB_EvaluatorDat];
    cjAniEvalSPDat* endEvalSPDat = curEvalSPDat + maxDats[cjAniSPADB_EvaluatorDat];

    while(curEvalSPDat < endEvalSPDat)
    {
        new(curEvalSPDat) cjAniEvalSPDat(); // no reference count instance
        curEvalSPDat++;
    }
	

	//
    // bspline basis data
	// 1. copy cjBSplineBasisF3Data from
	//

	cjBSplineBasisF3Dat* bsBasisDatBlk = (cjBSplineBasisF3Dat*)m_block[cjAniSPADB_BSBasisDat];
    cjBSplineBasisF3Dat* newBSBasisDat = bsBasisDatBlk;

    for(cjUINT32 i=0; i<basisDatCount; i++, newBSBasisDat++)
    {
        cjBSplineBasisF3Dat::PNew(newBSBasisDat); // new(newBSBasisDat) cjBSplineBasisF3Dat();  
        newBSBasisDat->AddRef(); // ref+1, prevent deletion
        newBSBasisDat->SetBasis(&basisDats[i]->GetBasis());
    }

    m_blockDatCount[cjAniSPADB_BSBasisDat] = (cjWORD)basisDatCount;
    curEvalSPDat = (cjAniEvalSPDat*)m_block[cjAniSPADB_EvaluatorDat];
    cjBYTE* spadBlkIterFillDat = (cjBYTE*)m_block[cjAniSPADB_FillDat];

    for(cjUINT32 evaluatorIdx=0; evaluatorIdx<evaluatorCount; evaluatorIdx++)
    {
        cjAniEvaluator* evaluator = aniSeq->GetEvaluator(evaluatorIdx);
        cjAssert(evaluator);

        const cjAniEvaluatorTag& evalTag = evaluator->GetTag();
        const cjAniPoseBindInfo* bindInfo = poseBind->GetBindInfo(evalTag);

        if(!bindInfo)
        {
            cjAssert(cjFalse);
            continue;
        }

        cjBYTE* curSPadBlkSharedFill = cjNULL;
        cjBool isSharedDatInited = cjTrue;
        cjUINT32 maxEvalChanCount = evaluator->GetMaxChannels();

        for(cjUINT32 chanIdx=0; chanIdx<maxEvalChanCount; chanIdx++)
        {
            cjAniPoseBufChanItem pbChanItem;
            cjBool isPosed;

            evaluator->GetPBChannelItemFromChanItem(chanIdx, pbChanItem, isPosed);
			if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjAniPoseBufHandle pbHandle = bindInfo->GetPBHandle(pbChanItem);

                if(pbHandle.IsValid())
                {
                    poseBuf->SetValid(pbHandle, cjTrue);

                    // need to be update always ?
                    cjBYTE spadDatflags = 0;
                    if(evaluator->IsAlwaysUpdate() || evaluator == m_accumEvaluator)
                    {
                        spadDatflags = cjAniEvalSPDat::EFL_ALWAYS_UPDATE;
                        isPosed = cjFalse;
                    }

                    // Does the evaluator need the scratch pad?
                    cjUINT32					curChanFillSiz;
                    cjBool						curChanIsSharedFill;
                    cjAniSPadBlock				curChanSPadBlk;
                    cjBSplineBasisF3Dat*		curChanBasisDat;

                    cjBool isSPadRequired = evaluator->GetSPadInfo(chanIdx, (evaluator == m_accumEvaluator), 
						sceneNodesHash, curChanFillSiz, curChanIsSharedFill, curChanSPadBlk, curChanBasisDat);

                    if(isPosed)
                    {
                        cjFLOAT* pbDat = poseBuf->GetDat(pbHandle);
                        cjAssert(pbDat);

                        if(evaluator->IsReferenced())
                        {
                            cjAniPoseBuf::RefDat* refDat = (cjAniPoseBuf::RefDat*)pbDat;

                            refDat->m_cntrSeq				= cntrSeq;
                            refDat->m_refEvaluator			= evaluator;
                            refDat->m_evalSPDat[chanIdx]	= isSPadRequired ? curEvalSPDat : cjNULL;
                        }
                        else
                        {
                            evaluator->GetChannelPosedValue(chanIdx, pbDat);
                        }
                    }

					// do not create constant item (one-keyframe), (pos, scale, rot ..)
                    if(!isSPadRequired)
                        continue;
					                    
                    if(evaluator == m_accumEvaluator)
                        m_accumEvalSPDat[chanIdx] = curEvalSPDat;
					                    
                    curEvalSPDat->SetEvaluator(evaluator);

                    if(pbChanItem == cjAniPoseBufChanItem_Ref)
                        curEvalSPDat->SetLOD(cjAniCntrSeq::LODSKIPEVALUATOR);
                    else
                        curEvalSPDat->SetLOD(bindInfo->m_lod);

					curEvalSPDat->SetEvalChannel((cjAniEvalChannel)chanIdx);
                    curEvalSPDat->SetFlag(spadDatflags);

                    cjAssert(curEvalSPDat->GetSegMinTime() == CJ_FTIME_INFINITY);
                    cjAssert(curEvalSPDat->GetSegMaxTime() == CJ_FTIME_INVALID);
                    cjAssert(curEvalSPDat->GetFillFunc() == cjNULL);

                    if(curChanFillSiz > 0)
                    {
                        if(curSPadBlkSharedFill)
                        {
                            curEvalSPDat->SetFillDat(curSPadBlkSharedFill);
                            isSharedDatInited = cjFalse;
                        }
                        else
                        {
                            if(curChanIsSharedFill)
                                curSPadBlkSharedFill = spadBlkIterFillDat;

							curEvalSPDat->SetFillDat(spadBlkIterFillDat);
                            spadBlkIterFillDat += curChanFillSiz;

                            cjAssert(cjAniSPADB_FillDat + 1 == cjAniSPADB_EvaluatorDat);
                            cjAssert(spadBlkIterFillDat <= m_block[cjAniSPADB_EvaluatorDat]);

                            m_blockDatCount[cjAniSPADB_FillDat]++;
                            cjAssert(m_blockDatCount[cjAniSPADB_FillDat] <= maxDats[cjAniSPADB_FillDat]);
                        }
                    }

                    cjBYTE* spadBlkSeg = m_block[curChanSPadBlk] + m_blockDatCount[curChanSPadBlk] * m_blockDatSiz[curChanSPadBlk];
                    curEvalSPDat->SetSegDat(spadBlkSeg);

                    cjAssert((curChanSPadBlk == cjAniSPADB_Max-1) || (spadBlkSeg + m_blockDatSiz[curChanSPadBlk] <= m_block[curChanSPadBlk + 1]));
                    cjAssert((curChanSPadBlk  < cjAniSPADB_Max-1) || (spadBlkSeg + m_blockDatSiz[curChanSPadBlk] <= m_spadBuf + m_spadBufSiz));

                    m_blockDatCount[curChanSPadBlk]++;
                    cjAssert(m_blockDatCount[curChanSPadBlk] <= maxDats[curChanSPadBlk]);

                    m_blockDatCount[cjAniSPADB_EvaluatorDat]++;
                    cjAssert(m_blockDatCount[cjAniSPADB_EvaluatorDat] <= maxDats[cjAniSPADB_EvaluatorDat]);

                    cjUINT32 bsBasisDatIndex=0;
                    if(curChanBasisDat)
                    {
                        for(cjUINT32 m=0; m<basisDatCount; m++)
                        {
                            if(curChanBasisDat == basisDats[m])
                            {                                
                                bsBasisDatIndex = m + 1; // +1 for error check
                                break;
                            }
                        }

                        cjAssert(bsBasisDatIndex != 0);
                        bsBasisDatIndex--;
                    }
					                    
                    evaluator->InitSPadDat(chanIdx, curEvalSPDat, bsBasisDatBlk + bsBasisDatIndex, 
						isSharedDatInited, sceneNodesHash, pbHandle);

                    curEvalSPDat++;
                }
                else
                {
                    cjAssert(cjFalse);
                }
            }
        } // end of for(chanIdx)
    }


	//
	// handle accumEvaluator
	//
	
	// 1. check accum evaluator spad data (pos, rot, scale)

    if(m_accumEvaluator)
    {
        poseBuf->ValidateAccumDelta();

        cjQuatTM stTM;
        cjQuatTM enTM;
        cjFLOAT duration = cntrSeq->GetDuration();

        if(m_accumEvalSPDat[EPOS])
        {
            m_accumEvaluator->UpdateChannel(duration,	EPOS, m_accumEvalSPDat[EPOS], enTM.GetPosPtr());
            m_accumEvaluator->UpdateChannel(0.0f,		EPOS, m_accumEvalSPDat[EPOS], stTM.GetPosPtr());
        }
				
        if(!stTM.IsPosValid()) // no pos
        {
            stTM.SetPos(cjVec3::ZERO);
            cjAssert(!enTM.IsPosValid());
            enTM.SetPos(cjVec3::ZERO);
        }

        if(m_accumEvalSPDat[EROT])
        {
            m_accumEvaluator->UpdateChannel(duration,	EROT, m_accumEvalSPDat[EROT], enTM.GetRotatePtr());
            m_accumEvaluator->UpdateChannel(0.0f	,	EROT, m_accumEvalSPDat[EROT], stTM.GetRotatePtr());
        }

        if(!stTM.IsRotateValid())
        {
            stTM.SetRotate(cjQuat::IDENTITY);
            cjAssert(!enTM.IsRotateValid());
            enTM.SetRotate(cjQuat::IDENTITY);
        }

        if(m_accumEvalSPDat[ESCALE])
        {
            m_accumEvaluator->UpdateChannel(duration,	ESCALE, m_accumEvalSPDat[ESCALE], enTM.GetScalePtr());
            m_accumEvaluator->UpdateChannel(0.0f,		ESCALE, m_accumEvalSPDat[ESCALE], stTM.GetScalePtr());
        }

        if(!stTM.IsScaleValid())
        {
            stTM.SetScale(cjVec3::UNIT);
            cjAssert(!enTM.IsScaleValid());
            enTM.SetScale(cjVec3::UNIT);
        }

        cjQuatTM invStTM;
        stTM.InvertEachSRT(invStTM);
        cjMatrix3 refRot;

        invStTM.GetRotate().ToRotateMatrix(refRot);
        invStTM.SetPos(refRot * invStTM.GetPos());

        enTM.GetRotate().ToRotateMatrix(refRot);
        invStTM.SetPos(refRot * invStTM.GetPos());

        m_loopAccumTM = enTM * invStTM;
    }

	cjMemDeAlloc(basisDats);
}

void cjAniSPad::Init(cjAniEvaluator* evaluator)
{
    cjAssert(evaluator);

    cjUINT32 maxEvalChanCount = evaluator->GetMaxChannels();
    cjAssert(maxEvalChanCount == 1 || (maxEvalChanCount == 3 && evaluator->IsTransformEvaluator()));

    cjUINT32 maxBasisDatCount = maxEvalChanCount + 1;
    cjBSplineBasisF3Dat** bsBasisDat 
		= (cjBSplineBasisF3Dat**)cjMemAlloc(sizeof(cjBSplineBasisF3Dat*) * maxBasisDatCount);

    cjUINT32 basisDatCount = 0;
    cjUINT32 totalFillSiz = 0;
    cjWORD maxDats[cjAniSPADB_Max];

    for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
        maxDats[i] = 0;

    cjINT32 sharedFillSiz = 0;
    for(cjUINT32 chanIdx=0; chanIdx < maxEvalChanCount; chanIdx++)
    {
        cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(chanIdx);

        if(pbChanItem != cjAniPoseBufChanItem_Invalid)
        {
            cjUINT32				curChanFillSiz;
            cjBool					curChanIsSharedFill;
            cjAniSPadBlock			curChanSPadBlk;
            cjBSplineBasisF3Dat*	curChanBSBasisDat;

            evaluator->GetSPadInfo(chanIdx, cjTrue, cjNULL, curChanFillSiz, curChanIsSharedFill, 
				curChanSPadBlk, curChanBSBasisDat);

            cjAssert(curChanFillSiz % 4 == 0); // keep 4byte align
            cjAssert(curChanSPadBlk != cjAniSPADB_EvaluatorDat && curChanSPadBlk != cjAniSPADB_FillDat &&
                curChanSPadBlk != cjAniSPADB_BSBasisDat && curChanSPadBlk != cjAniSPADB_Max);

            if(curChanIsSharedFill)
            {
                cjAssert(sharedFillSiz >= 0);

                if(sharedFillSiz == 0)
                {
                    sharedFillSiz = curChanFillSiz;
                }
                else
                {                    
                    cjAssert((cjINT32)curChanFillSiz == sharedFillSiz);
                    curChanFillSiz = 0;
                }
            }
            else
            {
                cjAssert(sharedFillSiz <= 0);
                sharedFillSiz = -1; // -1: no shared item is there
            }

            if(curChanFillSiz > 0)
            {
                maxDats[cjAniSPADB_FillDat]++;
                totalFillSiz += curChanFillSiz;
            }

            maxDats[cjAniSPADB_EvaluatorDat]++;
            maxDats[curChanSPadBlk]++;
			            
            if(curChanBSBasisDat)
            {
                cjBool bFound = cjFalse;                
				for(cjUINT32 m=0; m<basisDatCount; m++)
                {
                    if(curChanBSBasisDat == bsBasisDat[m])
                    {
                        bFound = cjTrue;
                        break;
                    }
                }

                if(!bFound)
                {                    
                    cjAssert(basisDatCount < maxBasisDatCount);
                    bsBasisDat[basisDatCount] = curChanBSBasisDat;
                    basisDatCount++;
                }
            }
        }
    }

    maxDats[cjAniSPADB_BSBasisDat] = (cjWORD)basisDatCount;
    cjUINT32 totalBufSiz = totalFillSiz;

	for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
        totalBufSiz += maxDats[i] * m_blockDatSiz[i];

    m_spadBuf = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * totalBufSiz);
    m_spadBufSiz = totalBufSiz;

    m_block[cjAniSPADB_FillDat]			= m_spadBuf;
    m_block[cjAniSPADB_EvaluatorDat]	= m_block[cjAniSPADB_FillDat] + totalFillSiz;

	cjAssert(cjAniSPADB_BSRotSeg == cjAniSPADB_Max - 1);

    for(cjUINT32 i=cjAniSPADB_EvaluatorDat; i<cjAniSPADB_BSRotSeg; i++)
        m_block[i+1] = m_block[i] + maxDats[i] * m_blockDatSiz[i];

	cjAssert(m_block[cjAniSPADB_BSRotSeg] + maxDats[cjAniSPADB_BSRotSeg] * 
        m_blockDatSiz[cjAniSPADB_BSRotSeg] == m_spadBuf + totalBufSiz);

    cjAniEvalSPDat* curEvalSPDat = (cjAniEvalSPDat*)m_block[cjAniSPADB_EvaluatorDat];
    cjAniEvalSPDat* endEvalSPDat = curEvalSPDat + maxDats[cjAniSPADB_EvaluatorDat];

    while(curEvalSPDat < endEvalSPDat)
    {
        new(curEvalSPDat) cjAniEvalSPDat();
        curEvalSPDat++;
    }

    cjBSplineBasisF3Dat* bsBasisDatBlk = (cjBSplineBasisF3Dat*)m_block[cjAniSPADB_BSBasisDat];
    cjBSplineBasisF3Dat* newBasisData = bsBasisDatBlk;

    for(cjUINT32 i=0; i<basisDatCount; i++, newBasisData++)
    {
		cjBSplineBasisF3Dat::PNew(newBasisData); // new(newBasisData) cjBSplineBasisF3Dat();
        newBasisData->AddRef(); // ref+1 for prevent deletion
        newBasisData->SetBasis(&bsBasisDat[i]->GetBasis());
    }

    m_blockDatCount[cjAniSPADB_BSBasisDat] = (cjWORD)basisDatCount;

    curEvalSPDat = (cjAniEvalSPDat*)m_block[cjAniSPADB_EvaluatorDat];
    cjBYTE* spadBlkFill = (cjBYTE*)m_block[cjAniSPADB_FillDat];
    cjBYTE* shareFillDat = cjNULL;
    cjBool isSharedDatInited = cjTrue;

    for(cjUINT32 chanIdx=0; chanIdx<maxEvalChanCount; chanIdx++)
    {
        cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(chanIdx);

        if(pbChanItem != cjAniPoseBufChanItem_Invalid)
        {
            cjUINT32				curChanFillSiz;
            cjBool					curChanIsSharedFill;
            cjAniSPadBlock			curChanSPadBlk;
            cjBSplineBasisF3Dat*	curChanBSBasisDat;

            evaluator->GetSPadInfo(chanIdx, cjTrue, cjNULL, curChanFillSiz, curChanIsSharedFill, 
				curChanSPadBlk, curChanBSBasisDat);

            curEvalSPDat->SetEvaluator(evaluator);
            curEvalSPDat->SetLOD(0);
            curEvalSPDat->SetEvalChannel((cjAniEvalChannel)chanIdx);
            curEvalSPDat->SetFlag(cjAniEvalSPDat::EFL_ALWAYS_UPDATE);

            cjAssert(curEvalSPDat->GetSegMinTime() == CJ_FTIME_INFINITY);
            cjAssert(curEvalSPDat->GetSegMaxTime() == CJ_FTIME_INVALID);
            cjAssert(curEvalSPDat->GetFillFunc() == cjNULL);

            if(curChanFillSiz > 0)
            {
                if(shareFillDat)
                {
                    curEvalSPDat->SetFillDat(shareFillDat);
                    isSharedDatInited = cjFalse;
                }
                else
                {
                    if(curChanIsSharedFill)
                        shareFillDat = spadBlkFill;

					curEvalSPDat->SetFillDat(spadBlkFill);
                    spadBlkFill += curChanFillSiz;
                    
					cjAssert(cjAniSPADB_FillDat + 1 == cjAniSPADB_EvaluatorDat);
                    cjAssert(spadBlkFill <= m_block[cjAniSPADB_EvaluatorDat]);

                    m_blockDatCount[cjAniSPADB_FillDat]++;
                    cjAssert(m_blockDatCount[cjAniSPADB_FillDat] <= maxDats[cjAniSPADB_FillDat]);
                }
            }

            cjBYTE* curChanSegDat = m_block[curChanSPadBlk] + m_blockDatCount[curChanSPadBlk] * m_blockDatSiz[curChanSPadBlk];
            curEvalSPDat->SetSegDat(curChanSegDat);

            cjAssert((curChanSPadBlk == cjAniSPADB_Max - 1) || (curChanSegDat + m_blockDatSiz[curChanSPadBlk] <= m_block[curChanSPadBlk + 1]));
            cjAssert((curChanSPadBlk < cjAniSPADB_Max - 1) ||  (curChanSegDat + m_blockDatSiz[curChanSPadBlk] <= m_spadBuf + m_spadBufSiz));

            m_blockDatCount[curChanSPadBlk]++;
            cjAssert(m_blockDatCount[curChanSPadBlk] <= maxDats[curChanSPadBlk]);

            m_blockDatCount[cjAniSPADB_EvaluatorDat]++;
            cjAssert(m_blockDatCount[cjAniSPADB_EvaluatorDat] <= maxDats[cjAniSPADB_EvaluatorDat]);

            cjUINT32 bsBasisDatIndex = 0;
            if(curChanBSBasisDat)
            {
                for(cjUINT32 m=0; m<basisDatCount; m++)
                {
                    if(curChanBSBasisDat == bsBasisDat[m])
                    {                        
                        bsBasisDatIndex = m + 1;
                        break;
                    }
                }

                cjAssert(bsBasisDatIndex != 0);
                bsBasisDatIndex--;
            }

            evaluator->InitSPadDat(chanIdx, curEvalSPDat, bsBasisDatBlk + bsBasisDatIndex, 
                isSharedDatInited, cjNULL, cjAniPoseBufHandle(pbChanItem, EINVALID_OUTPUT_INDEX));

            curEvalSPDat++;
        }
    }

    cjMemDeAlloc(bsBasisDat);
}

void cjAniSPad::Shutdown()
{
    ResetAllEvalSpadDat();

	cjMemDeAlloc(m_spadBuf);

    m_spadBuf = cjNULL;
    m_spadBufSiz = 0;
}

void cjAniSPad::UpdatePoseBufFromSPadSegDat(cjFLOAT scaledTime, cjSWORD slod, cjBSplineBasisF3Dat*, cjAniPoseBuf* poseBuf)
{
    cjAssert(poseBuf);

    poseBuf->AddNewDatsFromPoseBind(cjTrue, cjTrue);

    if(m_pbItemCount != poseBuf->GetTotalDatCount())
        m_pbItemCount = (cjWORD)poseBuf->GetTotalDatCount();
	
	//
	// linear / b-spline color interpolation
	//

    cjColorF* colorDatBlk = (cjColorF*)poseBuf->GetColorDats();

		// linear color interpolation
    LColorSeg* curLinearColor = (LColorSeg*)m_block[cjAniSPADB_LinearColorSeg];
    LColorSeg* endLinearColor = curLinearColor + m_blockDatCount[cjAniSPADB_LinearColorSeg];

    while(curLinearColor < endLinearColor)
    {
        if(slod <= curLinearColor->m_hdr.m_lod)
        {
            cjFLOAT ratio = (scaledTime - curLinearColor->m_stTime) * curLinearColor->m_invDTime;
			 
            cjAssert(curLinearColor->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);
            colorDatBlk[curLinearColor->m_hdr.m_outputPBIndex] 
				= curLinearColor->m_value0 + curLinearColor->m_outTangent0 * ratio;
        }

        curLinearColor++;
    }

		// b-spline color interpolation
    BSColorSeg* curBSColor = (BSColorSeg*)m_block[cjAniSPADB_BSColorSeg];
    BSColorSeg* endBSColor = curBSColor + m_blockDatCount[cjAniSPADB_BSColorSeg];

    while(curBSColor < endBSColor)
    {
        if(slod <= curBSColor->m_hdr.m_lod)
        {
            cjAssert(curBSColor->m_bsBasisDat);

            cjBSplineBasisF3& basis = curBSColor->m_bsBasisDat->GetBasis();

            cjFLOAT b0 = basis.Get(0); 
			cjFLOAT b1 = basis.Get(1); 
			cjFLOAT b2 = basis.Get(2); 
			cjFLOAT b3 = basis.Get(3);

            cjAssert(curBSColor->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            colorDatBlk[curBSColor->m_hdr.m_outputPBIndex].r = 
                b0 * curBSColor->m_src[0] + 
                b1 * curBSColor->m_src[4] + 
                b2 * curBSColor->m_src[8] + 
                b3 * curBSColor->m_src[12];

            colorDatBlk[curBSColor->m_hdr.m_outputPBIndex].g = 
                b0 * curBSColor->m_src[1] + 
                b1 * curBSColor->m_src[5] + 
                b2 * curBSColor->m_src[9] + 
                b3 * curBSColor->m_src[13];

            colorDatBlk[curBSColor->m_hdr.m_outputPBIndex].b = 
                b0 * curBSColor->m_src[2] + 
                b1 * curBSColor->m_src[6] + 
                b2 * curBSColor->m_src[10] + 
                b3 * curBSColor->m_src[14];

            colorDatBlk[curBSColor->m_hdr.m_outputPBIndex].a = 
                b0 * curBSColor->m_src[3] + 
                b1 * curBSColor->m_src[7] + 
                b2 * curBSColor->m_src[11] + 
                b3 * curBSColor->m_src[15];
        }

        curBSColor++;
    }


	//
    // Constant cjBool
	//

    cjFLOAT* pbBoolBlock = (cjFLOAT*)poseBuf->GetBoolDats();

    // Copy the constant cjBool values.
    CBoolSeg* curSPadBlkCBool = (CBoolSeg*)m_block[cjAniSPADB_CBoolSeg];
    CBoolSeg* endSPadBlkCBool = curSPadBlkCBool + m_blockDatCount[cjAniSPADB_CBoolSeg];

    while(curSPadBlkCBool < endSPadBlkCBool)
    {
        if(slod <= curSPadBlkCBool->m_hdr.m_lod)
        {
            cjAssert(curSPadBlkCBool->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);
            pbBoolBlock[curSPadBlkCBool->m_hdr.m_outputPBIndex] = curSPadBlkCBool->m_val0;
        }

        curSPadBlkCBool++;
    }


	//
	// cjFLOAT
	//

    cjFLOAT* pbBlkFloat = (cjFLOAT*)poseBuf->GetFloatDat();

    // cubic cjFLOAT interpolation.
    CubicFloatSeg* curSPadBlkCubicFloat = (CubicFloatSeg*)m_block[cjAniSPADB_CubicFloatSeg];
    CubicFloatSeg* endSPadBlkCubicFloat = curSPadBlkCubicFloat + m_blockDatCount[cjAniSPADB_CubicFloatSeg];

    while(curSPadBlkCubicFloat < endSPadBlkCubicFloat)
    {
        if(slod <= curSPadBlkCubicFloat->m_hdr.m_lod)
        {
            cjFLOAT t = (scaledTime - curSPadBlkCubicFloat->m_stTime) * curSPadBlkCubicFloat->m_invDTime;
            cjAssert(curSPadBlkCubicFloat->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbBlkFloat[curSPadBlkCubicFloat->m_hdr.m_outputPBIndex] 
				= curSPadBlkCubicFloat->m_value0 + (curSPadBlkCubicFloat->m_outTangent0 + 
					(curSPadBlkCubicFloat->m_a0 + curSPadBlkCubicFloat->m_b0 * t) * t) * t;
        }

        curSPadBlkCubicFloat++;
    }
	

	// b-spline cjFLOAT interpolation.
    BSFloatSeg* curBSFloat = (BSFloatSeg*)m_block[cjAniSPADB_BSFloatSeg];
    BSFloatSeg* endBSFloat = curBSFloat + m_blockDatCount[cjAniSPADB_BSFloatSeg];

    while(curBSFloat < endBSFloat)
    {
        if(slod <= curBSFloat->m_hdr.m_lod)
        {
            cjAssert(curBSFloat->m_bsBasisDat);

            cjBSplineBasisF3& basis = curBSFloat->m_bsBasisDat->GetBasis();
            cjFLOAT b0 = basis.Get(0);
            cjFLOAT b1 = basis.Get(1);
            cjFLOAT b2 = basis.Get(2);
            cjFLOAT b3 = basis.Get(3);

            cjAssert(curBSFloat->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbBlkFloat[curBSFloat->m_hdr.m_outputPBIndex] = 
                b0 * curBSFloat->m_src[0] + 
                b1 * curBSFloat->m_src[1] + 
                b2 * curBSFloat->m_src[2] + 
                b3 * curBSFloat->m_src[3];
        }

        curBSFloat++;
    }


	//
    // Vec3S (scale)
    //

    cjVec3* pbVec3SBlk = (cjVec3*)poseBuf->GetVec3SDat();

		// cubic vec3 interpolation.
    CubicVec3Seg* cubicVec3S	= (CubicVec3Seg*)m_block[cjAniSPADB_CubicVec3SSeg];
    CubicVec3Seg* endCubicVec3S	= cubicVec3S + m_blockDatCount[cjAniSPADB_CubicVec3SSeg];

    while(cubicVec3S < endCubicVec3S)
    {
        if(slod <= cubicVec3S->m_hdr.m_lod)
        {
            cjFLOAT t = (scaledTime - cubicVec3S->m_stTime) * cubicVec3S->m_invDTime;
            cjAssert(cubicVec3S->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbVec3SBlk[cubicVec3S->m_hdr.m_outputPBIndex] = cubicVec3S->m_val0 + (cubicVec3S->m_outTangent0 + 
                (cubicVec3S->m_a0 + cubicVec3S->m_b0 * t) * t) * t;
        }

        cubicVec3S++;
    }

		// b-spline vec3 interpolation.
    BSVec3Seg* bsVec3S		= (BSVec3Seg*)m_block[cjAniSPADB_BSVec3SSeg];
    BSVec3Seg* endBSVec3S	= bsVec3S + m_blockDatCount[cjAniSPADB_BSVec3SSeg];

    while(bsVec3S < endBSVec3S)
    {
        if(slod <= bsVec3S->m_hdr.m_lod)
        {
            cjAssert(bsVec3S->m_bsBasisDat);
            cjBSplineBasisF3& basis = bsVec3S->m_bsBasisDat->GetBasis();
            cjFLOAT b0 = basis.Get(0);
            cjFLOAT b1 = basis.Get(1);
            cjFLOAT b2 = basis.Get(2);
            cjFLOAT b3 = basis.Get(3);

            cjAssert(bsVec3S->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbVec3SBlk[bsVec3S->m_hdr.m_outputPBIndex].x = 
                b0 * bsVec3S->m_src[0] + b1 * bsVec3S->m_src[3] + b2 * bsVec3S->m_src[6] + b3 * bsVec3S->m_src[9];
            pbVec3SBlk[bsVec3S->m_hdr.m_outputPBIndex].y = 
                b0 * bsVec3S->m_src[1] + b1 * bsVec3S->m_src[4] + b2 * bsVec3S->m_src[7] + b3 * bsVec3S->m_src[10];
            pbVec3SBlk[bsVec3S->m_hdr.m_outputPBIndex].z = 
                b0 * bsVec3S->m_src[2] + b1 * bsVec3S->m_src[5] + b2 * bsVec3S->m_src[8] + b3 * bsVec3S->m_src[11];
        }

        bsVec3S++;
    }


	//
    // Vec3P (pos)
    //

    cjVec3* pbVec3PBlk = (cjVec3*)poseBuf->GetVec3PDat();

		// cubic vec3 interpolation.
    CubicVec3Seg* cubicVec3P	= (CubicVec3Seg*)m_block[cjAniSPADB_CubicVec3PSeg];
    CubicVec3Seg* endCubicVec3P	= cubicVec3P + m_blockDatCount[cjAniSPADB_CubicVec3PSeg];

    while(cubicVec3P < endCubicVec3P)
    {
        if(slod <= cubicVec3P->m_hdr.m_lod)
        {
            cjFLOAT t = (scaledTime - cubicVec3P->m_stTime) * cubicVec3P->m_invDTime;
            cjAssert(cubicVec3P->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbVec3PBlk[cubicVec3P->m_hdr.m_outputPBIndex] = cubicVec3P->m_val0 + (cubicVec3P->m_outTangent0 + 
                (cubicVec3P->m_a0 + cubicVec3P->m_b0 * t) * t) * t;
        }

        cubicVec3P++;
    }

		// b-spline vec3 interpolation.
    BSVec3Seg* bsVec3P		= (BSVec3Seg*)m_block[cjAniSPADB_BSVec3PSeg];
    BSVec3Seg* endBSVec3P	= bsVec3P + m_blockDatCount[cjAniSPADB_BSVec3PSeg];

    while(bsVec3P < endBSVec3P)
    {
        if(slod <= bsVec3P->m_hdr.m_lod)
        {
            cjAssert(bsVec3P->m_bsBasisDat);
            cjBSplineBasisF3& basis = bsVec3P->m_bsBasisDat->GetBasis();
            cjFLOAT b0 = basis.Get(0);
            cjFLOAT b1 = basis.Get(1);
            cjFLOAT b2 = basis.Get(2);
            cjFLOAT b3 = basis.Get(3);

            cjAssert(bsVec3P->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbVec3PBlk[bsVec3P->m_hdr.m_outputPBIndex].x = 
                b0 * bsVec3P->m_src[0] + b1 * bsVec3P->m_src[3] + b2 * bsVec3P->m_src[6] + b3 * bsVec3P->m_src[9];
            pbVec3PBlk[bsVec3P->m_hdr.m_outputPBIndex].y = 
                b0 * bsVec3P->m_src[1] + b1 * bsVec3P->m_src[4] + b2 * bsVec3P->m_src[7] + b3 * bsVec3P->m_src[10];
            pbVec3PBlk[bsVec3P->m_hdr.m_outputPBIndex].z = 
                b0 * bsVec3P->m_src[2] + b1 * bsVec3P->m_src[5] + b2 * bsVec3P->m_src[8] + b3 * bsVec3P->m_src[11];
        }

        bsVec3P++;
    }


	//
    // Rot
    //

	cjQuat* pbRotBlk = (cjQuat*)poseBuf->GetRotDats();

		// slerp rotation interpolation.
    SlerpRotSeg* slerpRot		= (SlerpRotSeg*)m_block[cjAniSPADB_SlerpRotSeg];
    SlerpRotSeg* endSlerpRot	= slerpRot + m_blockDatCount[cjAniSPADB_SlerpRotSeg];

    while(slerpRot < endSlerpRot)
    {
        if(slod <= slerpRot->m_hdr.m_lod)
        {
            cjFLOAT t = (scaledTime - slerpRot->m_stTime) * slerpRot->m_invDTime;
            cjAssert(slerpRot->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            if(t == 0.0f)
                pbRotBlk[slerpRot->m_hdr.m_outputPBIndex] = slerpRot->m_quat0;
            else
                cjQuat::Slerp(t, slerpRot->m_quat0, slerpRot->m_quat1, &pbRotBlk[slerpRot->m_hdr.m_outputPBIndex]);
        }

        slerpRot++;
    }

		// squad rotation interpolation.
    SquadRotSeg* squadRot = (SquadRotSeg*)m_block[cjAniSPADB_SquadRotSeg];
    SquadRotSeg* endSquadRot = squadRot + m_blockDatCount[cjAniSPADB_SquadRotSeg];

    while(squadRot < endSquadRot)
    {
        cjAniSPad::SlerpRotSeg* baseSeg = &squadRot->m_baseDat;

        if(slod <= baseSeg->m_hdr.m_lod)
        {
            cjFLOAT t = (scaledTime - baseSeg->m_stTime) * baseSeg->m_invDTime;
            cjAssert(baseSeg->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);

            pbRotBlk[baseSeg->m_hdr.m_outputPBIndex] = cjQuat::Squad(t, baseSeg->m_quat0, 
				squadRot->m_outQuat0, squadRot->m_inQuat1, baseSeg->m_quat1);
        }

        squadRot++;
    }

		// b-spline rotation interpolation.
    BSRotSeg* bsRot		= (BSRotSeg*)m_block[cjAniSPADB_BSRotSeg];
    BSRotSeg* endBSRot	= bsRot + m_blockDatCount[cjAniSPADB_BSRotSeg];

    while(bsRot < endBSRot)
    {
        if(slod <= bsRot->m_hdr.m_lod)
        {
            cjAssert(bsRot->m_bsBasisDat);
            
			cjBSplineBasisF3& basis = bsRot->m_bsBasisDat->GetBasis();
            cjFLOAT b0 = basis.Get(0); cjFLOAT b1 = basis.Get(1); cjFLOAT b2 = basis.Get(2); cjFLOAT b3 = basis.Get(3);

            cjQuat q;
            q.w = b0 * bsRot->m_quatRotCP[0] + b1 * bsRot->m_quatRotCP[4] + b2 * bsRot->m_quatRotCP[8] + b3 * bsRot->m_quatRotCP[12];
            q.x = b0 * bsRot->m_quatRotCP[1] + b1 * bsRot->m_quatRotCP[5] + b2 * bsRot->m_quatRotCP[9] + b3 * bsRot->m_quatRotCP[13];
            q.y = b0 * bsRot->m_quatRotCP[2] + b1 * bsRot->m_quatRotCP[6] + b2 * bsRot->m_quatRotCP[10] + b3 * bsRot->m_quatRotCP[14];
            q.z = b0 * bsRot->m_quatRotCP[3] + b1 * bsRot->m_quatRotCP[7] + b2 * bsRot->m_quatRotCP[11] + b3 * bsRot->m_quatRotCP[15];
            
			q.Normalize();

            cjAssert(bsRot->m_hdr.m_outputPBIndex != cjAniSPad::EINVALID_OUTPUT_INDEX);
            pbRotBlk[bsRot->m_hdr.m_outputPBIndex] = q;
        }

        bsRot++;
    }
	
}

void cjAniSPad::UpdateAccumTransformDelta(cjFLOAT etime, cjFLOAT scaledTime, cjSWORD slod, cjAniPoseBuf* poseBuf)
{
    cjAssert(poseBuf);
	    
    cjQuatTM quatTM;
    cjQuat* rot;
	cjVec3* pos;
    cjVec3* scale;


	//
	// pos
	//

    if(m_accumEvalSPDat[EPOS] && slod <= m_accumEvalSPDat[EPOS]->GetLOD() && poseBuf->IsVec3PValid(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3P, 0)))
    {
        pos = &poseBuf->GetVec3PDat()->m_vec3;
        cjAssert(pos->x != CJ_FTIME_INVALID);
        quatTM.SetPos(*pos);
    }
    else
    {
        pos = cjNULL;
        quatTM.SetPos(cjVec3::ZERO);
    }


	//
	// rot
	//
	
    if(m_accumEvalSPDat[EROT] && slod <= m_accumEvalSPDat[EROT]->GetLOD() && poseBuf->IsRotValid(cjAniPoseBufHandle(cjAniPoseBufChanItem_Rot, 0)))
    {
        rot = &poseBuf->GetRotDats()->m_rotQuat;
        cjAssert(rot->x != CJ_FTIME_INVALID);
        quatTM.SetRotate(*rot);
    }
    else
    {
        rot = cjNULL;
        quatTM.SetRotate(cjQuat::IDENTITY);
    }


	//
	// scale
	//

    if(m_accumEvalSPDat[ESCALE] && slod <= m_accumEvalSPDat[ESCALE]->GetLOD() && poseBuf->IsFloatValid(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3S, 0)))
    {
        scale = &poseBuf->GetVec3SDat()->m_vec3;
        cjAssert(*scale != cjVec3::INVALID);

        quatTM.SetScale(*scale);
    }
    else
    {
        scale = cjNULL;
        quatTM.SetScale(cjVec3::UNIT);
    }
	
    if(m_lastAccumScaledTime == CJ_FTIME_INVALID)
    {
        cjQuatTM invQTM;
        quatTM.InvertEachSRT(invQTM);

        invQTM.GetRotate().ToRotateMatrix(m_startRotAccum);
        invQTM.SetPos(m_startRotAccum * invQTM.GetPos());

        m_startAccumTM = invQTM;
    }

	// handle looping
    if(scaledTime < m_lastAccumScaledTime)
    {
        cjQuatTM loopTM(m_loopAccumTM);
        loopTM.SetPos(m_startRotAccum * loopTM.GetPos());

        m_startAccumTM = m_startAccumTM * loopTM;
        m_startAccumTM.GetRotate().ToRotateMatrix(m_startRotAccum);
    }

    quatTM.SetPos(m_startRotAccum * quatTM.GetPos());
    quatTM = m_startAccumTM * quatTM;

#ifdef CJDEBUG
    if(m_lastAccumScaledTime == CJ_FTIME_INVALID)
    {
        if(quatTM.IsPosValid())
        {
            cjAssert(cjAbs(quatTM.GetPos().x) < 0.00001f);
            cjAssert(cjAbs(quatTM.GetPos().y) < 0.00001f);
            cjAssert(cjAbs(quatTM.GetPos().z) < 0.00001f);
        }

        if(quatTM.IsRotateValid())
        {
            cjAssert(cjAbs(quatTM.GetRotate().w - 1.0f) < 0.00001f);
            cjAssert(cjAbs(quatTM.GetRotate().x) < 0.00001f);
            cjAssert(cjAbs(quatTM.GetRotate().y) < 0.00001f);
            cjAssert(cjAbs(quatTM.GetRotate().z) < 0.00001f);
        }

        if(quatTM.IsScaleValid())
        {
            cjAssert(quatTM.GetScale() == cjVec3::UNIT);
        }
    }
#endif

    if(m_lastAccumTM.IsPosValid())
        m_lastAccumTM = quatTM;

    cjQuatTM lastFrameInvQuatTM;
    m_lastAccumTM.InvertEachSRT(lastFrameInvQuatTM);

    if(lastFrameInvQuatTM.IsRotateValid())
    {
        cjMatrix3 refRot;
        lastFrameInvQuatTM.GetRotate().ToRotateMatrix(refRot);

        lastFrameInvQuatTM = lastFrameInvQuatTM * quatTM;
        lastFrameInvQuatTM.SetPos(refRot * lastFrameInvQuatTM.GetPos());
    }
    else
    {        
        lastFrameInvQuatTM = lastFrameInvQuatTM * quatTM; 
    }
	
	// Accum Position Delta
	if(pos)
    {
        *pos = quatTM.GetPos();

        cjAssert(lastFrameInvQuatTM.IsPosValid());
        cjAssert((pos + 1) == (cjVec3*)poseBuf->GetDat(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3P, 1)));

        if(etime == m_lastPBUpdateTime)
            lastFrameInvQuatTM.SetPos(*(pos + 1) + lastFrameInvQuatTM.GetPos());

        *(pos + 1) = lastFrameInvQuatTM.GetPos();
    }
	
	// Accum Rotation Delta
    if(rot)
    {
        *rot = quatTM.GetRotate();

        cjAssert(lastFrameInvQuatTM.IsRotateValid());
        cjAssert((rot + 1) == (cjQuat*)poseBuf->GetDat(cjAniPoseBufHandle(cjAniPoseBufChanItem_Rot, 1)));

        if(etime == m_lastPBUpdateTime)
            lastFrameInvQuatTM.SetRotate(*(rot + 1) * lastFrameInvQuatTM.GetRotate());

		*(rot + 1) = lastFrameInvQuatTM.GetRotate();
    }
	
	// Accum Scale Delta
    if(scale)
    {
        *scale = quatTM.GetScale();

        cjAssert(lastFrameInvQuatTM.IsScaleValid());
        cjAssert((scale + 1) == (cjVec3*)poseBuf->GetDat(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3S, 1)));

        if(etime == m_lastPBUpdateTime)
            lastFrameInvQuatTM.SetScale(*(scale + 1) * lastFrameInvQuatTM.GetScale()); 

		*(scale + 1) = lastFrameInvQuatTM.GetScale();
    }

    // update lastAccumTM
    m_lastAccumTM = quatTM;
    m_lastAccumScaledTime = scaledTime;
}




