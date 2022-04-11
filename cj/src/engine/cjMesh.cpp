
#include "cjEnginePCH.h"
#include "cjMesh.h"


//
// cjMesh
//

CJOBJ_IMPL_CLASS(cjMesh)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjMesh)
CJOBJ_IMPL_SERIALIZE_END(cjMesh)

cjMesh::cjMesh()
{
    m_defaultInst = NULL;
}

void cjMesh::OnDestroy()
{
    cjSuper::OnDestroy();
}


//
// cjLODMesh
//

CJOBJ_IMPL_CLASS(cjLODMesh)
CJOBJ_IMPL_SERIALIZE_BEGIN(cjLODMesh)

	if(stream.IsObjOnly())
		return;

	if(stream.IsLoading())
	{
		cjAssert(!m_numVertexStream);
		cjAssert(!m_vertexStreams);
		cjAssert(!m_numIndexBuffers);
		cjAssert(!m_indexBuffers);

		m_vertexStreams = NULL;

		m_streamKey.RemoveAll();
		m_streamAttribLayout.RemoveAll();
		m_scene = NULL;
	}

	stream << cjFStreamCmprI32(m_numVertexStream);
	stream << cjFStreamCmprI32(m_numIndexBuffers);

	if(stream.IsLoading())
	{
		if(m_numVertexStream)
		{
			m_vertexStreams = new cjFVertexStream*[m_numVertexStream];
			cjMemZero(m_vertexStreams, sizeof(cjFVertexStream*)*m_numVertexStream);
		}

		if(m_numIndexBuffers)
		{
			m_indexBuffers = new cjFIndexBuffer*[m_numIndexBuffers];
			cjMemZero(m_indexBuffers, sizeof(cjFIndexBuffer*)*m_numIndexBuffers);
		}		
	}

	for(cjINT32 i=0; i<m_numVertexStream; i++)
		::SerializeStream(stream, (cjFVertexStream**)&m_vertexStreams[i]); // m_vertexStream is dynamically created

	for(cjINT32 i=0; i<m_numIndexBuffers; i++)
		::SerializeStream(stream, (cjFIndexBuffer**)&m_indexBuffers[i]);

	{
		cjBYTE hasSceneRoot = 0;
		cjSceneNode* sceneRoot = GetSceneRoot();

		if(stream.IsSaving())
			hasSceneRoot = sceneRoot ? 1 : 0;

		stream << hasSceneRoot;
				
		if(stream.IsLoading())
		{
			cjAssert(!m_scene);

			if(hasSceneRoot)
			{
				cjSerObjStreamBroker broker(&stream);
				//broker.Serialize((cjNode**)&sceneRoot);
				broker.Serialize(&sceneRoot);
				
				m_scene = sceneRoot; // ref=1

				cjAssert(m_sceneMeshs.IsEmpty());
								
				cjNodeFindAll<cjSceneMesh>(sceneRoot, m_sceneMeshs);
				//do not use SetSceneRoot(sceneRoot);
			}
		}
		else if(stream.IsSaving())
		{
			if(hasSceneRoot)
			{
				cjSerObjStreamBroker broker(&stream);
				broker.Serialize(&sceneRoot);
			}
		}
	}
	
CJOBJ_IMPL_SERIALIZE_END(cjLODMesh)

CJOBJ_IMPL_STATIC_REG_BEGIN(cjLODMesh)

CJOBJ_IMPL_STATIC_REG_END(cjLODMesh)


cjLODMesh::cjLODMesh()
{
	m_scene = 0;

	m_numVertexStream = 0;
	m_vertexStreams = NULL;

	m_numIndexBuffers = 0;
	m_indexBuffers = NULL;
}

void cjLODMesh::OnDestroy()
{
	SetSceneRoot(NULL);

    if(!m_streamAttribLayout.IsEmpty())
    {
        for(cjAryOff i=0; i<m_streamAttribLayout.Num(); i++)
            delete m_streamAttribLayout(i);
    
        m_streamAttribLayout.RemoveAll();
    }

    if(m_vertexStreams)
	{
		for(cjINT32 i=0; i<m_numVertexStream; i++)
			delete m_vertexStreams[i];
    
		delete [] m_vertexStreams;
    
		m_vertexStreams = NULL;
		m_numVertexStream = 0;
	}

	if(m_indexBuffers)
	{
		for(cjINT32 i=0; i<m_numIndexBuffers; i++)
			delete m_indexBuffers[i];

		delete [] m_indexBuffers;
		
		m_indexBuffers = NULL;
		m_numIndexBuffers = 0;
	}

    cjSuper::OnDestroy();
}

cjObjCls* cjLODMesh::GetMeshInstanceClass()
{
    return cjLODMeshInstance::cjObjGetStaticCls();
}

cjMeshInstance* cjLODMesh::NewMeshInstance()
{
    cjLODMeshInstance* meshInst = cjObjNew<cjLODMeshInstance>(GetMeshInstanceClass());
    meshInst->SetMesh(this);
    
    return meshInst;
}

cjFVertexStreamAttribLayout* cjLODMesh::ResolveVertexStreamAttribLayout(
	const cjFVertexStream::StreamAry streams, cjINT32 streamCount)
{
	cjFVertexStreamAttribLayout* layout = cjNULL;
	cjFVertexStream::Key streamKey(streams, streamCount);

    for(cjAryOff i=0; i<m_streamKey.Num(); i++)
    {
        if(m_streamKey(i) == streamKey)
			return m_streamAttribLayout(i);
    }
    
	// not found , create new layout
    layout = new cjFVertexStreamAttribLayout(streams, streamCount);
    
    m_streamAttribLayout.Push(layout);
    m_streamKey.Push(streamKey);
    
    return layout;
}

cjINT32 cjLODMesh::AddStreamAndIndexBuf()
{
	if(m_numVertexStream == 0)
	{
		cjAssert(!m_vertexStreams);
		cjAssert(!m_indexBuffers);

		m_vertexStreams = new cjFVertexStream*[1];
		m_vertexStreams[0] = NULL;
		m_indexBuffers = new cjFIndexBuffer*[1];
		m_indexBuffers[0] = NULL;

		m_numVertexStream++;
		m_numIndexBuffers++;
	}
	else
	{
		cjFVertexStream** aryVtxStream = new cjFVertexStream*[m_numVertexStream+1];
		cjMemCpy(aryVtxStream, m_vertexStreams, sizeof(cjFVertexStream*)*m_numVertexStream);
		aryVtxStream[m_numVertexStream] = NULL;

		cjFIndexBuffer** aryIndexBuffer = new cjFIndexBuffer*[m_numIndexBuffers+1];
		cjMemCpy(aryIndexBuffer, m_indexBuffers, sizeof(cjFIndexBuffer*)*m_numIndexBuffers);
		aryIndexBuffer[m_numIndexBuffers] = NULL;
		
		CJ_SAFE_DELETE_ARY(m_vertexStreams);
		CJ_SAFE_DELETE_ARY(m_indexBuffers);

		m_vertexStreams = aryVtxStream;
		m_indexBuffers = aryIndexBuffer;

		m_numVertexStream++;
		m_numIndexBuffers++;
	}

	cjAssert(m_numVertexStream == m_numIndexBuffers);

	return m_numVertexStream - 1;
}

void cjLODMesh::SetSceneRoot(cjSceneNode* sceneObj)
{
	CJ_REF_SAFE_RELEASE(m_scene);
	m_scene = sceneObj;
	CJ_REF_SAFE_ADD(m_scene);
}

//int cjLODMesh::GetGPUSkinningMatrixAryCount()
//{
//	cjSceneMesh* sceneMesh = m_sceneMeshs(0);
//	cjSceneSkinMeshMod* skinMod = sceneMesh->GetSkinningModifier();
//	skinMod->_test_meshNode = sceneMesh;
//
//	return skinMod->GetGPUSkinningMatrixCount();
//}
//
//cjVec4* cjLODMesh::GetGPUSkinningMatrixAry()
//{
//	cjSceneMesh* sceneMesh = m_sceneMeshs(0);
//	cjSceneSkinMeshMod* skinMod = sceneMesh->GetSkinningModifier();
//	skinMod->_test_meshNode = sceneMesh;
//
//	return skinMod->GetGPUSkinningMatrixAry();
//}

//void cjLODMesh::TestDeform()
//{
//	cjSceneMesh* sceneMesh = m_sceneMeshs(0);
//	cjSceneSkinMeshMod* skinMod = sceneMesh->GetSkinningModifier();
//	skinMod->_test_meshNode = sceneMesh;
//
//	skinMod->GetGPUSkinningMatrixAry();
//
//	cjFVertexStream* outVtxStream = 0;
////	skinMod->DeformVertex(outVtxStream, sceneMesh->GetVertexStream());
//}

void cjLODMesh::SetStreamDirty(cjBool dirty)
{
	for(cjINT32 i=0; i<m_numVertexStream; i++)
	{
		if(m_vertexStreams[i])
			m_vertexStreams[i]->IncCacheRev();
	}

	m_streamKey.RemoveAll();
	m_streamAttribLayout.RemoveAll();
}

//

// cjTestMesh
//

CJOBJ_IMPL_CLASS(cjTestMesh)
CJOBJ_IMPL_SERIALIZE_BEGIN(cjTestMesh)

CJOBJ_IMPL_SERIALIZE_END(cjTestMesh)


cjTestMesh::cjTestMesh()
{
//    CreateTestStuffIndex();
}

void cjTestMesh::OnDestroy()
{
    if(!m_streamAttribLayout.IsEmpty())
    {
        for(cjAryOff i=0; i<m_streamAttribLayout.Num(); i++)
            delete m_streamAttribLayout(i);
    
        m_streamAttribLayout.RemoveAll();
    }
    
//cjtemp    m_streamKey.RemoveAll();
    cjSuper::OnDestroy();
}

cjObjCls* cjTestMesh::GetMeshInstanceClass()
{
    return cjTestMeshInstance::cjObjGetStaticCls();
}

cjMeshInstance* cjTestMesh::NewMeshInstance()
{
    cjMeshInstance* meshInst = cjObjNew<cjTestMeshInstance>(GetMeshInstanceClass());
    meshInst->SetMesh(this);
    
    return meshInst;
}

//void cjTestMesh::CreateTestStuffA(cjTestMesh* mesh)
//{
//	mesh->m_numVertexStream = 1;
//
//	mesh->m_vertexStreams = new cjFVertexStream*[mesh->m_numVertexStream];
//	mesh->m_vertexStreams[0] = NULL;
//
//	cjFVertexDecl vtxDecl;
//	vtxDecl.MakeDecl(3,
//		cjVtxComp_Position,     cjVtxCompSiz_FLOAT3,
//		cjVtxComp_Normal,       cjVtxCompSiz_FLOAT3,
//		cjVtxComp_TexCoord0,    cjVtxCompSiz_FLOAT2
//		);
//
//	static const cjINT32 numVtx = 36;
//	static const cjFLOAT rawVtxSrc[] =
//	{
//		// Data layout for each line below is:
//		// positionX, positionY, positionZ,     normalX, normalY, normalZ,   texU, texV
//		0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
//		0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
//		0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
//
//		0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
//		0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
//		0.5f, 0.5f, 0.5f,          1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
//
//		0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
//		-0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
//		0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
//		0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
//		-0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
//		-0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
//
//		-0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//		-0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//		-0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
//		-0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
//
//		-0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
//		0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
//		-0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
//		-0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
//		0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
//		0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
//
//		0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
//		-0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
//		0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
//		0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
//		-0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
//		-0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
//
//		0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
//		0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
//		0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
//		-0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f,   1.0f, 1.0f
//	};
//
//	cjINT32 stride = vtxDecl.GetStride();
//
//	mesh->m_vertexStreams[0] = new cjFStaticVertexStream(numVtx, &vtxDecl, (void*)rawVtxSrc);
//}

void cjTestMesh::CreateTestStuffIndex(cjTestMesh* mesh)
{
 //   mesh->m_numVertexStream = 1;
 //   
 //   mesh->m_vertexStreams = new cjFVertexStream*[mesh->m_numVertexStream];
 //   mesh->m_vertexStreams[0] = NULL;
 //   
 //   cjFVertexDecl vtxDecl;
 //   vtxDecl.MakeDecl(3,
 //       cjVtxComp_Position,     cjVtxCompSiz_FLOAT3,
 //       cjVtxComp_Normal,       cjVtxCompSiz_FLOAT3,
 //       cjVtxComp_TexCoord0,    cjVtxCompSiz_FLOAT2
 //       );
 //   
 //   static const cjINT32 numVtx = 36;
 //   static const cjFLOAT rawVtxSrc[] =
 //   {
 //       // Data layout for each line below is:
 //       // positionX, positionY, positionZ,     normalX, normalY, normalZ,   texU, texV
 //       0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
 //       0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
 //       0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,    1.0f, 0.0f,            
 //       0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
 //       0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
 //       0.5f, 0.5f, 0.5f,          1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
 //       
 //       0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
 //       -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
 //       0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
 //       0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
 //       -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
 //       -0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
 //       
 //       -0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
 //       -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
 //       -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
 //       -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
 //       -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
 //       -0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
 //       
 //       -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
 //       0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
 //       -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
 //       -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
 //       0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
 //       0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
 //       
 //       0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
 //       -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
 //       0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
 //       0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
 //       -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
 //       -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
 //       
 //       0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
 //       -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
 //       0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
 //       0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
 //       -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
 //       -0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f,   1.0f, 1.0f
 //   };
 //   
 //   cjINT32 stride = vtxDecl.GetStride();
 //   mesh->m_vertexStreams[0] = new cjFStaticVertexStream(numVtx, &vtxDecl, (void*)rawVtxSrc);

	//mesh->m_numIndexBuffers = 1;
	//mesh->m_indexBuffers = new cjFIndexBuffer*[mesh->m_numIndexBuffers];

	//mesh->m_indexBuffers[0] = new cjFIndexBuffer;
	//cjFIndexBuffer* indexBuffer = mesh->m_indexBuffers[0];

	//const static cjWORD indexBuf[] = 
	//{
	//	0,1,2,3,4,5,
	//	6,7,8,9,10,11,
	//	12,13,14,15,16,17,

	//	18,19,20,21,22,23,
	//	24,25,26,27,28,29,
	//	30,31,32,33,34,35
	//};

	//cjINT32 numIndex = sizeof(indexBuf) / sizeof(indexBuf[0]);
	//indexBuffer->SetIndexDat(numIndex, (void*)indexBuf);
}




//
// cjMeshInstance
//

CJOBJ_IMPL_CLASS(cjMeshInstance)

cjMeshInstance::cjMeshInstance()
{
}

void cjMeshInstance::OnDestroy()
{
    cjSuper::OnDestroy();
}


//
// cjLODMeshInstance
//

CJOBJ_IMPL_CLASS(cjLODMeshInstance)

cjLODMeshInstance::cjLODMeshInstance()
{
}

void cjLODMeshInstance::OnDestroy()
{
    cjSuper::OnDestroy();
}

void cjLODMeshInstance::SetMesh(cjMesh* mesh)
{
    if(!mesh || !mesh->cjObjHasKindOfCls(cjLODMesh::cjObjGetStaticCls()))
    {
        cjError("ERR! - given mesh is not LODmesh");
        return;
    }
    
    m_mesh = (cjLODMesh*)mesh;
}


void cjLODMeshInstance::Render(cjRenInterface* ri, cjMaterial* material)
{
    cjMesh* mesh = GetMesh();

	//
	// setup common
	//
	
	class VSALResolver : public cjRenInterface::cjVertexStreamAttribLayoutResolver
	{
	public:
		virtual cjFVertexStreamAttribLayout* OnResolveVertexStreamAttribLayout(
			const cjFVertexStream::StreamAry streams, cjINT32 streamCount, cjRenDev* renDev) const
		{
			return m_meshInst->GetMesh()->ResolveVertexStreamAttribLayout(streams, streamCount);
		}
        
		VSALResolver(cjMeshInstance*  meshInst) { m_meshInst = meshInst; }
		cjMeshInstance* m_meshInst;
        
	} const valayoutResolver(this);

	cjINT32 numStream = mesh->GetNumVertexStream();
	ri->SetVertexStreams(mesh->GetVertexStreamAry(), numStream);

	//
	// setup for each cjSceneMesh
	//

	cjEngineTime_t t = cjGetEngineTimeSec();

	cjSceneMeshRPtrAry& sceneMeshes = m_mesh->GetSceneMeshes();
	cjAryOff sceneMeshCount = sceneMeshes.Count();

	for(cjAryOff i=0; i<sceneMeshCount; i++)
	{
		// cjstemp
		static int ix=1;
		static int iz=0;

		if(ix == 0)
		{
			if(i== iz)
				continue;
		}


		cjSceneMesh* sceneMesh = sceneMeshes(i);
		
		cjINT32 numPri = sceneMesh->GetPrimitiveCount();

		cjINT32 streamCount;
		cjFVertexStream** vtxStream = sceneMesh->GetVertexStream(&streamCount);
		
		ri->SetVertexStreams(vtxStream, streamCount);

		
		cjSceneSkinMeshMod* skinMod = sceneMesh->GetSkinningModifier();

		if(skinMod)
		{
			ri->SetWorld();
			skinMod->Update(t);
			ri->SetSkinBoneMatrixs(skinMod->GetBoneMatrixAsVec4Ary(), skinMod->GetBoneMatrixCount()*4);
		}
		else
		{
			//if(g_skinMod)
			//{
				cjSceneNode* parSceneNode = sceneMesh->GetParentSceneNode();
			//	
			//	cjTransform worldToSkinTM = meshTM * m_invSkinTM;
			//	
			//	cjMatrix boneMatrices = worldToSkinTM * boneWorldTM * skinToBoneTM;
			//}

//			cjTransform ltm = sceneMesh->GetLocalTM();
//			cjTransform meshTM = sceneMesh->GetLocalTM() * parSceneNode->GetWorldTM();
//			cjTransform meshTM = parSceneNode->GetWorldTM() * sceneMesh->GetLocalTM(); 
//			cjMatrix m = meshTM;

				// parent world x local vs  local x parent world
			cjMatrix world;
			
			world.SetIdentity();
			
			cjTransform wtm = sceneMesh->GetWorldTM();
			//cjMatrix z = wtm;

			//world.SetRow(3, wtm.m_pos.x, wtm.m_pos.y, wtm.m_pos.z, 1.0 );

			//const cjTransform& ltm = sceneMesh->GetLocalTM();

			//cjTransform x;

			//x.m_scale = cjVec3(1.3749998, 1.3749998, 1.3749998);
			
			//x.m_rotate.SetRow(0, cjVec3(0.93288368, -0.062544197, -0.35470581));
			//x.m_rotate.SetRow(1, cjVec3(0.00000000,  0.98480773,  -0.17364816));
			//x.m_rotate.SetRow(2, cjVec3(0.36017773,  0.16199353,   0.91871113));

			//x.m_rotate.SetRow(0, cjVec3(0.93288368, -0.062544197, -0.35470581));
			//x.m_rotate.SetRow(1, cjVec3(0.00000000,  0.98480773,  -0.17364816));
			//x.m_rotate.SetRow(2, cjVec3(0.36017773,  0.16199353,   0.91871113));

			//x.m_rotate = x.m_rotate.Transpose(); // transpose해야함, opengl이라서, rotation방향이 틀림

			//x.m_pos   = cjVec3(-3.4015560, -4.7010841, 39.696575);

			//wtm.m_rotate.SetIdentity();

			// transform -> matrix에서 변환시 오류가능성

			//cjMatrix k = x;

			// wtm.m_rotate.MakeFromEulerAngleXYZ(0,-3.14159/8.0f, 0);
			// shader code
			// void main()
			//{
			//	v_texcoord0 = a_texcoord0;
			//	vec4 pos = a_position;
			//	pos = u_m4World * pos ;  <<<<<   u_m4World is assumed column major matrix, but sceneMesh->GetWorldTM().m_rotate is row-major,  so transpose is need
			//	gl_Position = u_m4MVP * pos ;
			//}
			//
			//
			wtm.m_rotate = wtm.m_rotate.Transpose();
			world = wtm;

			// 각 부모본에 쓰레기 rotaion 값들어감 대부분 1,0,0 이 되어야

			ri->SetWorld(&world);
			ri->SetSkinBoneMatrixs(cjNULL, 0);
		}

		cjFIndexBuffer* indexBuf = sceneMesh->GetIndexBuffer();

		cjINT32 baseVertexIndex = 0;
		ri->SetIndexBuffer(indexBuf, baseVertexIndex);
					
		cjMaterial* mtl = sceneMesh->GetActiveMtl();

		ri->SetScenePropList(&sceneMesh->GetScenePropList());

		ri->SetMaterial(mtl);

		ri->ResolveShader(&valayoutResolver);
		
		//cjtemp
		//mesh->TestDeform();
		//mesh->SetStreamDirty(cjTrue);
		//cjtemp
		//cjAssert(numStream < 2);
		// cjtemp
		//	mesh->GetSceneRoot()->Update(0.0f);
		//cjtemp
		//{
		//	cjMLOG(_CT("[index start] ") );
		//	for(cjUINT32 xr=0; xr<indexBuf->GetNumIndex(); xr++)
		//	{
		//		cjMLOG(_CT("[index list] [%d] = %d "), xr, indexBuf->GetIndex(xr));
		//	}
		//	cjMLOG(_CT("[index end] ") );
		//}
	  
		//cjtemp
		//static int _numPri = numPri = 12;
		cjINT32 baseIndex = 0; 
		cjINT32 numMin = 0; cjINT32 numMax = 0;

		cjPrimitiveType ptType = sceneMesh->GetPrimitiveType();
		ri->DrawPrimitive(ptType, baseIndex, numPri, numMin, numMax);				
	}

}


//
// cjTestMeshInstance
//

CJOBJ_IMPL_CLASS(cjTestMeshInstance)


cjTestMeshInstance::cjTestMeshInstance()
{
}

void cjTestMeshInstance::OnDestroy()
{
    cjSuper::OnDestroy();
}

void cjTestMeshInstance::SetMesh(cjMesh* mesh)
{
    if(!mesh || !mesh->cjObjHasKindOfCls(cjTestMesh::cjObjGetStaticCls()))
    {
        cjError("ERR! - given mesh is not testmesh");
        return;
    }
    
    m_mesh = (cjTestMesh*)mesh;
}


void cjTestMeshInstance::Render(cjRenInterface* ri, cjMaterial* material)
{
	cjSuper::Render(ri, material); 
}

