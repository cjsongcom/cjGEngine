
#ifndef _CJMESH_H_
#define _CJMESH_H_


class cjMeshInstance;
class cjObjCls;
class cjRenInterface;
class cjSceneNode;
class cjSceneMesh;

cjRefPtrForwardDefine(cjSceneMesh)
typedef cjTAry<cjSceneMeshRPtr> cjSceneMeshRPtrAry;


//
// cjMesh
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjMesh, cjGeometry, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE

public:
    cjMesh();
    
    virtual cjObjCls* GetMeshInstanceClass() { return 0; }
    virtual cjMeshInstance* NewMeshInstance() = 0;
    
		// vertex stream
    virtual cjINT32 GetNumVertexStream() const { return 0; }
	virtual cjFVertexStream* GetVertexStream(cjINT32 index) { return 0; }
    virtual cjFVertexStream** GetVertexStreamAry() { return 0; }
	virtual cjINT32 GetVertexStreamIndex(cjFVertexStream* stream) { return -1; }

		// index buffer
	virtual cjFIndexBuffer* GetIndexBuffer(cjINT32 index=0) { return cjNULL; }
	virtual cjINT32 GetIndexBufIndex(cjFIndexBuffer* indexBuf) { return -1; }
	virtual cjINT32 GetNumIndexBuffer() const { return 0; }
	virtual cjINT32 GetNumPrimitive(cjINT32 index=0) const { return 0; }

		// management
    virtual cjFVertexStreamAttribLayout* ResolveVertexStreamAttribLayout(
		const cjFVertexStream::StreamAry streams, cjINT32 streamCount) { return cjNULL; }
	virtual cjFVertexStream* SetVertexStream(cjINT32 index, cjFVertexStream* vertexStream) { return cjNULL; }
	virtual cjFIndexBuffer* SetIndexBuffer(cjINT32 index, cjFIndexBuffer* indexBuffer) { return cjNULL; }
	virtual cjINT32 AddStreamAndIndexBuf() { return -1; }

		// scene
	virtual cjSceneNode* GetSceneRoot() { return cjNULL; }

	//virtual cjINT32 GetGPUSkinningMatrixAryCount() { return 0; }
	//virtual cjVec4* GetGPUSkinningMatrixAry() { return 0; }
	
	//virtual void TestDeform() {}

	virtual void SetStreamDirty(cjBool /*dirty*/) {}
	   
protected:
    virtual void OnDestroy() = 0;
    
protected:
    cjMeshInstance*                     m_defaultInst;	

CJOBJ_DECL_ABSTRACT_END(cjMesh)


//
// cjLODMesh
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjLODMesh, cjMesh, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE
	CJOBJ_DECL_STATIC_REG
  
public:
    virtual cjObjCls* GetMeshInstanceClass();
    virtual cjMeshInstance* NewMeshInstance();
   
		// vertex stream
    virtual cjINT32 GetNumVertexStream() const { return m_numVertexStream; }
	virtual cjFVertexStream* GetVertexStream(cjINT32 index)
	{ 
		if(m_numVertexStream > index) return m_vertexStreams[index];
		return 0; 
	}
	virtual cjFVertexStream** GetVertexStreamAry() { return m_vertexStreams; }
	virtual cjINT32 GetVertexStreamIndex(cjFVertexStream* stream)
	{
		if(!m_vertexStreams || !stream)
			return -1;

		for(cjINT32 i=0; i<m_numVertexStream; i++)
			if(m_vertexStreams[i] == stream)
				return i;

		return -1;
	}

		//  index buffer
	virtual cjFIndexBuffer* GetIndexBuffer(cjINT32 index=0)
	{
		if(m_numIndexBuffers > index) return m_indexBuffers[index];
		return 0;
	}
	virtual cjINT32 GetIndexBufIndex(cjFIndexBuffer* indexBuf)
	{
		if(!m_indexBuffers || !indexBuf)
			return -1;

		for(cjINT32 i=0; i<m_numIndexBuffers; i++)
			if(m_indexBuffers[i] == indexBuf)
				return i;

		return -1;
	}
	virtual cjINT32 GetNumIndexBuffer() const { return m_numIndexBuffers; }
	virtual cjINT32 GetNumPrimitive(cjINT32 index=0) const
	{
		if(m_numIndexBuffers > index) 
			return m_indexBuffers[index]->GetNumPrimitive();

		return cjNULL;
	}

		// management    
    virtual cjFVertexStreamAttribLayout* ResolveVertexStreamAttribLayout(
		const cjFVertexStream::StreamAry streams, cjINT32 streamCount);

	virtual cjFVertexStream* SetVertexStream(cjINT32 index, cjFVertexStream* vertexStream) 
	{ 
		cjFVertexStream* old = m_vertexStreams[index];
		m_vertexStreams[index] = vertexStream;		
		return old;
	}
	virtual cjFIndexBuffer* SetIndexBuffer(cjINT32 index, cjFIndexBuffer* indexBuffer) 
	{
		cjFIndexBuffer* old = m_indexBuffers[index];
		m_indexBuffers[index] = indexBuffer;		
		return old;
	}
	virtual cjINT32 AddStreamAndIndexBuf(); // ret : lastest added stream index in stream array

		// scene
	virtual cjSceneNode* GetSceneRoot() { return m_scene; }
	virtual void SetSceneRoot(cjSceneNode* sceneObj);

	cjSceneMeshRPtrAry& GetSceneMeshes() { return m_sceneMeshs; }

	//	// bone matrix ary & count
	//virtual cjINT32 GetGPUSkinningMatrixAryCount();
	//virtual cjVec4* GetGPUSkinningMatrixAry();
	//virtual void TestDeform();

	virtual void SetStreamDirty(cjBool dirty);

protected:
    virtual void OnDestroy();
    
public:
    cjLODMesh();
    
public:
//cjtemp
//protected:
	// can be shared between meshes
	cjINT32                                         m_numVertexStream;
    cjFVertexStream**                               m_vertexStreams;
	
	cjINT32											m_numIndexBuffers;
	cjFIndexBuffer**								m_indexBuffers;

	cjTAry<cjFVertexStream::Key>					m_streamKey;
    cjTAry<cjFVertexStreamAttribLayout*>			m_streamAttribLayout;

	cjSceneNode*									m_scene;  // scene root

	cjSceneMeshRPtrAry								m_sceneMeshs;

CJOBJ_DECL_CLASS_END(cjLODMesh)


//
// cjTestMesh
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTestMesh, cjLODMesh, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE

public:
    virtual cjObjCls* GetMeshInstanceClass();
    virtual cjMeshInstance* NewMeshInstance();
   
protected:
    virtual void OnDestroy();
    
public:
//	static void CreateTestStuffA(cjTestMesh* mesh);
    static void CreateTestStuffIndex(cjTestMesh* mesh);

public:
    cjTestMesh();

CJOBJ_DECL_CLASS_END(cjTestMesh)


//
// cjMeshInstance
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjMeshInstance, cjGeometry, 0, cjEngine)

public:
    cjMeshInstance();
 
    virtual cjMesh* GetMesh() = 0;
    virtual void SetMesh(cjMesh* mesh) = 0;

        // material
    virtual cjMaterial* GetMaterial(cjINT32 index) { return NULL; }
    virtual cjINT32 GetNumMaterial() { return 0; }
    virtual cjMaterial* GetActiveMaterial() = 0;
    
	virtual void BuildBoundInfo(){}
    
	virtual void SetScale(cjVec3& scale) {}
	virtual const cjMatrix& GetWorldTM() const { return cjMatrix::Identity; }
    
	virtual void Render(cjRenInterface* ri, cjMaterial* material=NULL) { material = material; }
    
protected:
    virtual void OnDestroy() = 0;

CJOBJ_DECL_ABSTRACT_END(cjMeshInstance)


//
// cjLODMeshInstance
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjLODMeshInstance, cjMeshInstance, 0, cjEngine)

public:
    virtual cjMesh* GetMesh() { return m_mesh; }
    virtual void SetMesh(cjMesh* mesh);
    
    virtual cjMaterial* GetActiveMaterial() { return m_activeMaterial; }
	virtual const cjMatrix& GetWorldTM() const { return m_tmWorld; }

	virtual void Render(cjRenInterface* ri, cjMaterial* material=NULL);

protected:
    virtual void OnDestroy();
    
public:
    cjLODMeshInstance();

protected:
    cjLODMesh*                                 m_mesh;
    
    cjMaterial*                                m_activeMaterial;
    cjMatrix                                   m_tmWorld;

CJOBJ_DECL_CLASS_END(cjLODMeshInstance)



//
// cjTestMeshInstance
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTestMeshInstance, cjLODMeshInstance, 0, cjEngine)

public:
    virtual void SetMesh(cjMesh* mesh);
	virtual void Render(cjRenInterface* ri, cjMaterial* material=NULL);

protected:
    virtual void OnDestroy();
    
public:
    cjTestMeshInstance();

protected:

CJOBJ_DECL_CLASS_END(cjTestMeshInstance)


#endif

