
#include "cjEnginePCH.h"



//
// cjSceneMeshMod
//

CJSER_IMPL_CLASS(cjSceneMeshMod, cjRefObj)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneMeshMod)
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneMeshMod)


//
// cjSceneSkinMeshMod
//

CJSER_IMPL_CLASS(cjSceneSkinMeshMod, cjSceneMeshMod)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneSkinMeshMod)

	if(ser.IsQuerySerObjAndStr())
	{
		cjSceneNode::SerializeAry(ser, &m_bones, &m_numBones);
		*m_topNodeForRootBone >> ser;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjINT32 serCount;
		cjSerID serID;

		if(ser.IsSaving())
			serCount = m_numBones;

		ser << cjFStreamCmprI32(serCount);

		if(ser.IsLoading())
		{
			cjSLOG("Load NumBone=%d \n", serCount);

			cjSceneNode::SerializeAry(ser, &m_bones, &serCount);
			cjTransform::SerializeRAry(ser, &m_skinToBoneTMAry, &serCount);
			
			cjSerID_Serialize(ser, serID);
			m_topNodeForRootBone = ser.GetSerObj<cjSceneNode>(serID);

			ser << m_invSkinTM;

			m_numBones = serCount;
			m_lastUpdateTime = 0.0f;
		}
		else if(ser.IsSaving())
		{	
			cjSLOG("Save NumBone=%d \n", m_numBones);

			cjSceneNode::SerializeAry(ser, &m_bones, &m_numBones);
			cjTransform::SerializeRAry(ser, &m_skinToBoneTMAry, &m_numBones);

			serID = ser.GetSerObjID(m_topNodeForRootBone);
			cjSerID_Serialize(ser, serID);

			ser << m_invSkinTM;
			
//			cjMatrix34::SerializeFAry(ser, m_boneMatrices, &m_numBones);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjSceneSkinMeshMod)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjSceneSkinMeshMod)

	if(ser.IsLoading())
	{
		cjSceneNode::PostSerializeAry(ser, m_bones);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjSceneSkinMeshMod)


cjSceneSkinMeshMod::cjSceneSkinMeshMod()
{
	m_numBones = 0;

	m_bones = cjNULL;
	m_skinToBoneTMAry = cjNULL;

	m_topNodeForRootBone = cjNULL;
	m_lastUpdateTime =  CJ_FTIME_INVALID;

	m_boneMatrices = cjNULL;
	m_boneWorldTMAry = cjNULL;

	m_attachNode = cjNULL;
}

cjSceneSkinMeshMod::cjSceneSkinMeshMod(cjUINT32 boneCount)
{
	m_numBones = boneCount;

	if(boneCount > 0)
	{
		m_bones = new cjSceneNode*[boneCount];
		m_skinToBoneTMAry = new cjTransform[boneCount];
	}
	else
	{
		m_bones = cjNULL;
		m_skinToBoneTMAry = cjNULL;
	}

	m_topNodeForRootBone = cjNULL;
	m_lastUpdateTime =  CJ_FTIME_INVALID;

	m_boneMatrices = cjNULL;
	m_boneWorldTMAry = cjNULL;

	m_attachNode = cjNULL;
}

cjSceneSkinMeshMod::~cjSceneSkinMeshMod()
{
	CJ_SAFE_DELETE_ARY(m_bones);
	CJ_SAFE_DELETE_ARY(m_skinToBoneTMAry);

	CJ_SAFE_DEALLOC(m_boneMatrices);
	CJ_SAFE_DELETE_ARY(m_boneWorldTMAry);
}
	
cjUINT32 cjSceneSkinMeshMod::GetBoneCount() const
{
	return m_numBones;
}

cjBool cjSceneSkinMeshMod::Attach(cjSceneMesh* sceneMesh)
{
	if(m_attachNode)
		return cjFalse;

	if(!m_topNodeForRootBone)
		return cjFalse;

    for(cjINT32 i=0; i<m_numBones; i++)
    {
        if(!m_bones[i])
            return cjFalse;
    }

	cjAssert(m_boneMatrices == cjNULL);
	cjAssert(m_boneWorldTMAry == cjNULL);

	if(m_numBones > 0)
	{
		m_boneMatrices = new cjMatrix[m_numBones];
		m_boneWorldTMAry = new cjTransform*[m_numBones];

		for(cjINT32 k=0; k<m_numBones; k++)
			m_boneWorldTMAry[k] = (cjTransform*)&m_bones[k]->GetWorldTM();
	}

	m_lastUpdateTime = CJ_FTIME_INVALID;
	m_attachNode = sceneMesh;

	return cjTrue;
}

cjBool cjSceneSkinMeshMod::Detach(cjSceneMesh* sceneMesh)
{
	if(m_boneMatrices)
	{
		delete [] m_boneMatrices;
		m_boneMatrices = 0;
	}

	if(m_boneWorldTMAry)
	{
		delete [] m_boneWorldTMAry;
		m_boneWorldTMAry = 0;
	}

	m_lastUpdateTime = CJ_FTIME_INVALID;
	m_attachNode = cjNULL;
   
    return cjTrue;
}


cjFLOAT cjSceneSkinMeshMod::Update(cjEngineTime_t t, cjBool force)
{
	if(m_lastUpdateTime == t && !force)
		return m_lastUpdateTime;

	m_lastUpdateTime = t;

	if(!m_numBones)
		return cjNULL;
	
	for(cjINT32 i=0; i<m_numBones; i++)
	{
		cjTransform boneWorldTM = m_bones[i]->GetWorldTM();
		cjTransform skinToBoneTM = m_skinToBoneTMAry[i];

		cjTransform meshTM;

		if(m_attachNode)
		{
			cjSceneNode* parSceneNode = m_attachNode->GetParentSceneNode();

			if(parSceneNode)
			{
				meshTM = parSceneNode->GetWorldTM() * m_attachNode->GetLocalTM();
			}
			else
				meshTM = m_attachNode->GetLocalTM();
		}

		cjTransform worldToSkinTM = meshTM * m_invSkinTM;
		cjTransform immBM = worldToSkinTM * boneWorldTM * skinToBoneTM;		
		immBM.m_rotate = immBM.m_rotate.Transpose(); // m_rotate is column major
		m_boneMatrices[i] = immBM;
	}

	return m_lastUpdateTime;
}
//
//cjINT32 cjSceneSkinMeshMod::GetGPUSkinningMatrixCount()
//{
//	if(GetGPUSkinningMatrixAry())
//		return m_numBones;
//
//	return 0;
//}
//
//cjVec4* cjSceneSkinMeshMod::GetGPUSkinningMatrixAry()
//{
//	if(m_numBones)
//	{
//		if(!m_boneMatrices)
//			m_boneMatrices = (cjMatrix*)cjMemAlloc(sizeof(cjMatrix) * m_numBones);
//
//		for(cjINT32 i=0; i<m_numBones; i++)
//		{
//			//cjMatrix matInvTopNodeForRootBone;
//			//cjTransform invTopNodeForRootBoneTM;
//			//(m_topNodeForRootBone->GetWorldTM()).Invert(invTopNodeForRootBoneTM);
//			//matInvTopNodeForRootBone = invTopNodeForRootBoneTM;
//
//			//m_invSkinTM;
//			cjTransform boneWorldTM = m_bones[i]->GetWorldTM();
//			cjTransform skinToBoneTM = m_skinToBoneTMAry[i];
//
//			//cjLOG("[BWTM] p , %f %f %f ",	boneWorldTM.m_pos.x, boneWorldTM.m_pos.y, boneWorldTM.m_pos.z);
//			//			
//			//cjLOG("[BWTM] 1 , %f %f %f  1r=%f %f %f  2r=%f %f %f", 
//			//	boneWorldTM.m_rotate.m[0][0],
//			//	boneWorldTM.m_rotate.m[0][1],
//			//	boneWorldTM.m_rotate.m[0][2],
//
//			//	boneWorldTM.m_rotate.m[1][0],
//			//	boneWorldTM.m_rotate.m[1][1],
//			//	boneWorldTM.m_rotate.m[1][2],
//
//			//	boneWorldTM.m_rotate.m[2][0],
//			//	boneWorldTM.m_rotate.m[2][1],
//			//	boneWorldTM.m_rotate.m[2][2]);
//
//						
////			m_boneMatrices[i] = matInvSkin * matInvTopNodeForRootBone * matWorldTMBone * matSkinToBone;
////			m_boneMatrices[i] = matSkinToBone *  matInvSkin;
////			m_boneMatrices[i] = matSkinToBone * matWorldTMBone * matInvTopNodeForRootBone * matInvSkin;
//
//			//cjMatrix matWorldToSkin = cjMatrix(
//			//	cjVec4(1, 0, 0, 0), 
//			//	cjVec4(0, 0.99999994,     -5.9604645e-008, 0),
//			//	cjVec4(0, 5.9604645e-008, 0.99999994,      0),
//			//	cjVec4(0, 5.9604645e-008, 5.9604645e-008,  1));
//
//			cjTransform meshTM;
//
//			if( m_bones[i]->GetName() == cjName(_CT("BoxBone-head")))
//				{
//					int xzz=0;
//					xzz=1;
//				}
//
//			if(_test_meshNode)
//			{
//				
//
//				cjSceneNode* parSceneNode = _test_meshNode->GetParentSceneNode();
//
//				if(parSceneNode)
//					meshTM = parSceneNode->GetWorldTM() * _test_meshNode->GetLocalTM();
//				else
//					meshTM = _test_meshNode->GetLocalTM();
//			}
//
//			cjTransform worldToSkinTM = meshTM * m_invSkinTM;
//
//			//cjtemp
//	//		worldToSkinTM.m_pos = cjVec3(4.7409441e-005, -3.0597296, 0.75363022);
//
//			//worldToSkinTM.m_rotate = cjMatrix3(
//			//	cjVec3(1, 0, 0),
//			//	cjVec3(0, 0.99999994,     -5.9604645e-008),
//			//	cjVec3(0, 5.9604645e-008, 0.99999994));
//			//worldToSkinTM.m_pos = cjVec3(0, 5.9604645e-008, 5.9604645e-008);
//			//worldToSkinTM.m_scale = 1;
//
//			//m_boneMatrices[i] = 
//			
////			cjTransform WB = worldToSkinTM * boneWorldTM;
////			cjTransform TB = WB * skinToBoneTM;
////			cjTransform XB = skinToBoneTM * WB;
////			m_boneMatrices[i] = matSkinToBone * matInvTopNodeForRootBone * matInvSkin;
//			m_boneMatrices[i] = worldToSkinTM * boneWorldTM * skinToBoneTM;
//
//			cjFLOAT (*t)[4] = m_boneMatrices[i].m;
//
//
//			////cjtemp
//			//if(0) //if(i==0)
//			//{
//
//
//			//	t[0][0]=1, t[0][1]=0.000001, t[0][2]=0, t[0][3]=0;
//			//	t[1][0]=-0, t[1][1]=1, t[1][2]=-0, t[1][3] = 0;
//			//	t[2][0]=-0, t[2][1]=0, t[2][2]=1, t[2][3] = 0;
//			//	t[3][0]=0.000011, t[3][1]=-0.000002, t[3][2]=0.000002, t[3][3]=1.0;
//			//}
//
//			//if(i == 3 || i == 4 || i== 7)
//			{
//
//				//{				
//				//	cjFLOAT (*_p) = &worldToSkinTM.m_pos.x;
//				//	cjFLOAT (*_r)[3] = worldToSkinTM.m_rotate.m;
//
//				//	cjLOG("[worldToSkinTM-begin] %d", i);
//				//	cjLOG("[POS] %f %f %f ", _p[0], _p[1], _p[2]);
//				//	cjLOG("[R0]  %f %f %f ", _r[0][0], _r[0][1], _r[0][2], _r[0][3]);
//				//	cjLOG("[R1]  %f %f %f ", _r[1][0], _r[1][1], _r[1][2], _r[1][3]);
//				//	cjLOG("[R2]  %f %f %f ", _r[2][0], _r[2][1], _r[2][2], _r[2][3]);
//				//	cjLOG("[worldToSkinTM-end] %d", i);
//				//}
//
//
//				/*{				
//					cjFLOAT (*_p) = &m_invSkinTM.m_pos.x;
//					cjFLOAT (*_r)[3] = m_invSkinTM.m_rotate.m;
//
//					cjLOG("[InvSkinTM] %d", i);
//					cjLOG("[POS] %f %f %f ", _p[0], _p[1], _p[2]);
//					cjLOG("[R0]  %f %f %f ", _r[0][0], _r[0][1], _r[0][2], _r[0][3]);
//					cjLOG("[R1]  %f %f %f ", _r[1][0], _r[1][1], _r[1][2], _r[1][3]);
//					cjLOG("[R2]  %f %f %f ", _r[2][0], _r[2][1], _r[2][2], _r[2][3]);
//					cjLOG("[InvSkinTM] %d", i);
//				}
//						
//
//				{				
//					cjFLOAT (*_p) = &boneWorldTM.m_pos.x;
//					cjFLOAT (*_r)[3] = boneWorldTM.m_rotate.m;
//
//					cjLOG(_CT("[boneWorldTM-begin] %d  %s "), i, *m_bones[i]->GetName());
//					cjLOG("[R0]  %f %f %f ", _r[0][0], _r[0][1], _r[0][2], _r[0][3]);
//					cjLOG("[R1]  %f %f %f ", _r[1][0], _r[1][1], _r[1][2], _r[1][3]);
//					cjLOG("[R2]  %f %f %f ", _r[2][0], _r[2][1], _r[2][2], _r[2][3]);
//					cjLOG("[POS] %f %f %f ", _p[0], _p[1], _p[2]);
//					cjLOG("[boneWorldTM-end] %d", i);
//				}*/
//
//				/*{				
//					cjTransform boneLocalTM = m_bones[i]->GetLocalTM();
//
//					cjFLOAT (*_p) = &boneLocalTM.m_pos.x;
//					cjFLOAT (*_r)[3] = boneLocalTM.m_rotate.m;
//
//					cjLOG(_CT("[boneLocalTM-begin] %d %s "), i, *m_bones[i]->GetName());
//					cjLOG("[POS] %f %f %f ", _p[0], _p[1], _p[2]);
//					cjLOG("[R0]  %f %f %f ", _r[0][0], _r[0][1], _r[0][2], _r[0][3]);
//					cjLOG("[R1]  %f %f %f ", _r[1][0], _r[1][1], _r[1][2], _r[1][3]);
//					cjLOG("[R2]  %f %f %f ", _r[2][0], _r[2][1], _r[2][2], _r[2][3]);
//					cjLOG("[boneLocalTM-end] %d", i);
//				}*/
//
//				/*{				
//					cjFLOAT (*_p) = &skinToBoneTM.m_pos.x;
//					cjFLOAT (*_r)[3] = skinToBoneTM.m_rotate.m;
//
//					cjLOG("[skinToBoneTM-begin] %d", i);					
//					cjLOG("[R0]  %f %f %f ", _r[0][0], _r[0][1], _r[0][2], _r[0][3]);
//					cjLOG("[R1]  %f %f %f ", _r[1][0], _r[1][1], _r[1][2], _r[1][3]);
//					cjLOG("[R2]  %f %f %f ", _r[2][0], _r[2][1], _r[2][2], _r[2][3]);
//					cjLOG("[POS] %f %f %f ", _p[0], _p[1], _p[2]);
//					cjLOG("[skinToBoneTM-end] %d", i);
//				}*/
//
//			}
//
//			extern cjDouble cj_tmp_GetTime();
//
//
//			/*cjLOG(" time =%f                          ",cj_tmp_GetTime());
//			cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[0][0], t[0][1], t[0][2], t[0][3]);
//			cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[1][0], t[1][1], t[1][2], t[1][3]);
//			cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[2][0], t[2][1], t[2][2], t[2][3]);
//			cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[3][0], t[3][1], t[3][2], t[3][3]);
//			cjLOG("                               ");*/
//
//			//DWORD axxx=0;
//			//axxx=1;
//		}
//	}
//
//	//cjVec4* tx = (cjVec4*)m_boneMatrices;
//
//	//for(cjINT32 k=0; k<16;k++)
//	//{
//	//	cjLOG("[bone vec row=%d] %f  %f   %f   %f \n", k, tx->x, tx->y, tx->z, tx->w);
//	//	tx++;
//	//}
//	
//	return (cjVec4*)m_boneMatrices;
//}

//void cjSceneSkinMeshMod::DeformVertex(cjFVertexStream* outVtxStream, cjFVertexStream* srcVtxStream)
//{
////	cjAssert(0);
////	return;
////
////	cjVtxCompStream* srcPos				= srcVtxStream->GetVtxCompStream(cjVtxComp_Position);
////	cjVtxCompStream* srcBlendIndex		= srcVtxStream->GetVtxCompStream(cjVtxComp_BL4Index);
////	cjVtxCompStream* srcBlendWeight		= srcVtxStream->GetVtxCompStream(cjVtxComp_BL4Weight);
////
////	cjINT32 vtxCount = srcVtxStream->GetVertexCount();
////
//////	cjVtxCompStream* outPosStream = new cjVtxCompStream();	
//////	outPosStream->SetCompInfo(cjVtxComp_Position, cjVtxCompSiz_FLOAT3);
//////	outPosStream->SetVertexCount(vtxCount);	
//////	cjTStrideRandomAccessIterator<cjVec3, 1>	outPosIter(outPosStream->Lock());
////
////	cjTStrideRandomAccessIterator<cjVec3, 1>	posIter(srcPos->Lock());
////	cjTStrideRandomAccessIterator<cjBYTE, 4>	blendIdxIter(srcBlendIndex->Lock());
////	cjTStrideRandomAccessIterator<cjVec4, 1>	blendWeightIter(srcBlendWeight->Lock());
////
////
////	//cjtemp
////	//if(!m_test_vtx)
////	//{
////	//	cjSiz_t siz = posIter.GetElemSiz() * vtxCount;
////	//	
////	//	m_test_vtx = new cjBYTE[siz];		
////	//	cjMemCpy(m_test_vtx, posIter.GetSrc(), siz);
////	//}
////	//else
////	//{
////	//	cjSiz_t siz = posIter.GetElemSiz() * vtxCount;
////	//	cjMemCpy(srcPos->Lock(), m_test_vtx, siz);
////	//}
////
////	//static cjINT32 doVtxCount = vtxCount;
////
////	for(cjINT32 i=0; i<vtxCount; i++)
////	{
////		cjVec3& pos = *posIter;
////		cjBYTE* boneAry = &(*blendIdxIter);
////		cjVec4& boneW = *blendWeightIter;
////				
////		{
////		//	cjVec3& outPos = *outPosIter;
////			cjVec3 outPos;
////
////			cjVec4 p = cjVec4(0,0,0,0);
////			cjVec4 vtx = cjVec4(pos.x, pos.y, pos.z, 1.0f);
////
////			cjMatrix* matB0 = &m_boneMatrices[boneAry[0]];
////			cjMatrix* matB1 = &m_boneMatrices[boneAry[1]];
////			cjMatrix* matB2 = &m_boneMatrices[boneAry[2]];
////			cjMatrix* matB3 = &m_boneMatrices[boneAry[3]];
////
////			//cjMatrix   fin  = (*matB0) * boneW.x;
////			//           fin += (*matB1) * boneW.y;
////			//		   fin += (*matB2) * boneW.z;
////			//		   fin += (*matB3) * boneW.w;
////
////			//cjVec3 ww = fin * pos;
////					  			
////			/*cjVec4 p0 = (*matB0) * vtx;
////			cjVec4 p1 = (*matB1) * vtx;
////			cjVec4 p2 = (*matB2) * vtx;
////			cjVec4 p3 = (*matB3) * vtx;
////
////			p0 = p0 * boneW.x;
////			p1 = p1 * boneW.y;
////			p2 = p2 * boneW.z;
////			p3 = p3 * boneW.w;
////
////			p = p0 + p1 + p2 + p3;
////
////			outPos = cjVec3(p.x, p.y, p.z);*/
////
////
//////			cjVec4 px0 = (*matB0) * vtx;
////		
////			cjVec3 p0 = (*matB0) * pos;
////			cjVec3 p1 = (*matB1) * pos;
////			cjVec3 p2 = (*matB2) * pos;
////			cjVec3 p3 = (*matB3) * pos;
////			
////			p0 = p0 * boneW.x;
////			p1 = p1 * boneW.y;
////			p2 = p2 * boneW.z;
////			p3 = p3 * boneW.w;
////			
////			outPos = p0 + p1 + p2 + p3;
////			
////			//outPos = cjVec3(p.x, p.y, p.z);
////
////
////
////
////
////	//		outPos = ww;
////
////		/*	cjLOG("[VTX] %d ,  %f   %f   %f   b=%d %d %d %d , w=%f %f %f %f", i, p.x, p.y, p.z, 
////				boneAry[0],boneAry[1],boneAry[2],boneAry[3],
////				boneW.x,boneW.y,boneW.z,boneW.w);*/
////
////
////			/*cjLOG("[VTX] %d ,  %f   %f   %f   b=%d %d %d %d , w=%f %f %f %f", i, pos.x, pos.y, pos.z, 
////				boneAry[0],boneAry[1],boneAry[2],boneAry[3],
////				boneW.x,boneW.y,boneW.z,boneW.w);*/
////
////			static int d = vtxCount;
////
////			if(i < d)
////			  *posIter = outPos;
////
////			//if(i==0)
////			//	*posIter = cjVec3(-0, 0, 0);
////			//else if(i==1)
////			//	*posIter = cjVec3(0, 0, 0);
////			//else if(i==2)
////			//	*posIter = cjVec3(0, 0, 0);
////			//else if(i==3)
////			//	*posIter = cjVec3(0, 0, 0);
////			//else if(i==4)
////			//	*posIter = cjVec3(0, 0, 0);
////			//else if(i==5)
////			//	*posIter = cjVec3(0, 0, 0);
////
////		}
////		
//////		++outPosIter;
////		++posIter;
////		++blendIdxIter;
////		++blendWeightIter;
////	}
//
//}

