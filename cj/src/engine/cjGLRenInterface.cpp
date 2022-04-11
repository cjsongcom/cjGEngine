
#include "cjEnginePCH.h"
#include "cjGLRenInterface.h"

#include "cjGLRenDev.h"


/*
    * VAOs and GL_ARRAY_BUFFER binding  : https://www.opengl.org/discussion_boards/showthread.php/176790-VAOs-and-GL_ARRAY_BUFFER-binding

	#0. VAO state is only affected by
	    - glEnable/DisableVertexAttribArray
		- glVertexAttribPointer calls.

	#1. The fact that glVertexAttribPointer happens to get its buffer from what is bound to GL_ARRAY_BUFFER does not mean that GL_ARRAY_BUFFER is VAO state.
	#2. Changing the GL_ARRAY_BUFFER binding does not affect VAO state. 
	#3. GL_ELEMENT_ARRAY_BUFFER is tied to the VAO state, So no, you do not need to unbind it
	#4. Additionally, you probably want to pass NULL as the last parameter to glDrawElements in your example, since the address is relative to the bound element array buffer

		
	// This is different from the GL_ELEMENT_ARRAY_BUFFER binding, which is directly part of VAO state. 
	// Think of GL_ARRAY_BUFFER as a global GLuint that holds a buffer object. So 'glBindBuffer' is implemented as:
		
		GLuint g_arrayBufferBinding = 0;

		void glBindBuffer(GLenum target, GLuint buffer)
		{
		  switch(target)
		  {
		  case GL_ARRAY_BUFFER:
			g_arrayBufferBinding = buffer;
		  }
		}
		// That's all it does. Only calling 'glBindBuffer' will not affect anything in the VAO.
		What affects the VAO is exactly what the wiki page states: calling glVertexAttribPointer. 
		This function takes whatever is currently bound to GL_ARRAY_BUFFER and puts it in the VAO for that attribute index. 
		So it is the combination of 'glBindBuffer' and 'glVertexAttribPointer' that sets the buffer for an attribute in a VAO.
		The specification is correct as well; you're just misunderstanding it. 
		The spec is talking about the buffer objects as set by 'glVertexAttribPointer', not the GL_ARRAY_BUFFER binding. 

		
		glGenVertexArrays(1, p_vao);
		glBindVertexArray(p_vao);
		{
			glBindBuffer(GL_ARRAY_BUFFER, n_vertex_buffer_object);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), p_OffsetInVBO(0));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), p_OffsetInVBO(3 * sizeof(float)));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_index_buffer_object_list[0]); // red
		}
		glBindVertexArray(0);


		# code snippet #2

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer1);

glVertexAttribPointer(ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(vertexStruct), (void*)offsetof(vertexStruct,position));
glEnableVertexAttribArray(ATTRIB_POSITION);
glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertexStruct), (void*)offsetof(vertexStruct,color);
glEnableVertexAttribArray(ATTRIB_COLOR);

glBindBuffer(GL_ARRAY_BUFFER, 0);

glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices)/sizeof(GLubyte), GL_UNSIGNED_BYTE, (void*)0);

Your code would work just fine. 
What all of the gl*Pointer calls do is look at what is stored in GL_ARRAY_BUFFER at the time the function is called, 
and create an association between that buffer and that attribute. 
So the only way to change what buffer gets used for an attribute is to call gl*Pointer.


The function glVertexAttribPointer specifies the format and source buffer (ignoring the deprecated usage of client arrays) of a vertex attribute that is used when rendering something (i.e. the next glDraw... call).

Now there are two scenarios. You either use vertex array objects (VAOs) or you don't (though not using VAOs is deprecated and discouraged/prohibited in modern OpenGL). If you're not using VAOs, then you would usually call glVertexAttribPointer (and the corresponding glEnableVertexAttribArray) right before rendering to setup the state properly. If using VAOs though, you actually call it (and the enable function) inside the VAO creation code (which is usually part of some initialization or object creation), since its settings are stored inside the VAO and all you need to do when rendering is bind the VAO and call a draw function.

But no matter when you call glVertexAttribPointer, you should bind the corresponding buffer right before (no matter when that was actually created and filled), since the glVertexAttribPointer function sets the currently bound GL_ARRAY_BUFFER as source buffer for this attribute (and stores this setting, so afterwards you can freely bind another VBO).

So in modern OpenGL using VAOs (which is recommended), it's usually similar to this workflow


//initialization
glGenVertexArrays
glBindVertexArray

glGenBuffers
glBindBuffer
glBufferData

glVertexAttribPointer
glEnableVertexAttribArray

glBindVertexArray(0)

glDeleteBuffers //you can already delete it after the VAO is unbound, since the
                //VAO still references it, keeping it alive (see comments below).

...

//rendering
glBindVertexArray
glDrawWhatever










*/


//
// cjGLMtlModifyAccum
//

cjGLMtlModifyAccum::cjGLMtlModifyAccum()
{
	m_commitTexTransforms               = cjFalse;
	m_commitFramebufferBlending         = cjFalse;
	m_commitColor                       = cjFalse;

    m_texMatrix                         = cjMatrix::Identity;
    m_texCoordSource                    = cjTexCoordSrc0;
    m_texCoordCount                     = cjTexCoord2D;
    m_texCoordProjected                 = cjFalse;
    
    m_frameBufferBlend                  = cjRFB_Overwrite;
    m_zWrite                            = cjTrue;
    m_zTest                             = cjTrue;
    m_alphaTest                         = cjFalse;
    m_alphaRef                          = 0;
    m_twoSided                          = cjFalse;
    
    m_alphaBlend                        = cjFalse;
    m_texFactorColor                    = cjColor::BLACK;
    
    m_fallback                          = NULL;
}


//
// cjGLTextureStage
//

cjGLTextureStage::cjGLTextureStage()
{
    m_texture = NULL;
    
    m_texCoordCount = cjTexCoord2D;
    m_texCoordSource = cjTexCoordSrc0;
	m_texTypeForShader = (cjBYTE)-1;
	m_pad0 = 0;

    m_texTransformEnable = cjFalse; // mod
    m_texTransformMatrix = cjMatrix::Identity;
    m_texCoordGenProjected = cjFalse;

    m_useTexMatrixAsTexCoordGen = cjFalse;
    m_texCoordGenMode = cjTexCoordSrc0;
    
    m_texAddrU = GL_CLAMP_TO_EDGE;
    m_texAddrV = GL_CLAMP_TO_EDGE;
    m_texAddrW = GL_CLAMP_TO_EDGE;
    
    m_texMipLODBias = 0;
}


//
// cjGLRenderPass
//

cjGLRenderPass::cjGLRenderPass()
{
    m_numRef                    = 0;
    m_shaderProg                = NULL;
    
    // Setup By CommitFinalBlendModify
    m_srcBlend                  = GL_ONE;
    m_dstBlend                  = GL_ONE_MINUS_SRC_ALPHA;
    m_alphaBlend                = cjFalse;
    m_zWrite                    = cjTrue;
    m_zTest                     = cjTrue;
    m_alphaTest                 = cjFalse;
    m_alphaRef                  = 0;
    
    m_cullMode                  = cjCM_CW;
    m_fillMode                  = cjFM_Solid;
    
    m_numTextureStage           = 0;
    m_vertexStreamAttribLayout  = NULL;
}


//
// cjGLRenInterface::cjGLSavedState
//

cjGLRenInterface::cjGLSavedState::cjGLSavedState()
{
	cjMemSet(this, 0, sizeof(cjGLRenInterface::cjGLSavedState));
}

void cjGLRenInterface::cjGLSavedState::SetDefault()
{
	cjMemSet(this, 0, sizeof(cjGLRenInterface::cjGLSavedState));

	//m_renderTargetGrp = NULL;
	//m_vpX = m_vpY = 0;
	//m_vpW = m_vpH = 0;

	//m_camR = cjVec3(0.0f, 0.0f, 1.0f);
	//m_camU = cjVec3(0.0f, 1.0f, 0.0f);
	//m_camD = cjVec3(1.0f, 0.0f, 0.0f);

//	m_camEye = cjVec3(0.0f, 0.0f, 0.0f);

	//m_camFrustum.m_ortho = false;

	//// default aspect <-> 
	//// fovY 45
	//m_camFrustum.m_left		= -0.5f;
	//m_camFrustum.m_right	=  0.5f;
	//m_camFrustum.m_top		=  0.5;	
	//m_camFrustum.m_bottom	= -0.5f;

	//m_camFrustum.m_near		= ms_projDefNear;
	//m_camFrustum.m_far		= ms_projDefFar;

	//cjMemZero(&m_camPort, sizeof(m_camPort));

	m_world.SetIdentity();
	m_view.SetIdentity();

	cjGLRenInterface::GetDefaultProj(m_proj);

	m_zbias = 0.0f;

//	m_culler = NULL;
}



//
// cjGLRenInterface
//

cjMatrix cjGLRenInterface::ms_defaultProj;

cjFLOAT	cjGLRenInterface::ms_projDefFovY;
cjFLOAT	cjGLRenInterface::ms_projDefAspect;
cjFLOAT	cjGLRenInterface::ms_projDefNear;
cjFLOAT	cjGLRenInterface::ms_projDefFar;

cjGLRenInterface::cjGLRenInterface(cjGLRenDev* renDev)
{
	SetDefaultProj();
	
	m_renDev = renDev;
	m_viewport = NULL;

//	cjMemSet(m_savedState, 0, sizeof(m_savedState));
	m_savedStateIndex = 0;

	m_curState = &m_savedState[m_savedStateIndex];
	m_curState->SetDefault();
}

//cjGLRenInterface::~cjGLRenInterface()
//{
//}


//
// implements cjRenInterface
//

cjBool cjGLRenInterface::Begin(cjViewport* viewport)
{
	if(m_viewport)
		return cjFalse;

	// store current render state
	m_viewport = viewport;

	// init first state
	cjAssert(m_savedStateIndex==0);
	cjAssert(m_curState == &m_savedState[m_savedStateIndex]);
	cjAssert(m_curState->m_renderTargetGrp);


	cjFRenTargetGrp* curRTG = m_renDev->GetCurRenTargetGrp();
	cjAssert(m_curState->m_renderTargetGrp == curRTG);

//	if(m_curState->m_renderTargetGrp)
//		GetCameraDataToState(m_curState); // failed on no rendertargetgroup is activated

	cjINT32 vpClientW, vpClientH;
	m_viewport->GetClientSiz(vpClientW, vpClientH);

	m_curState->m_vpX = 0;
	m_curState->m_vpY = 0;
	m_curState->m_vpW = curRTG->GetWidth(0);
	m_curState->m_vpH = curRTG->GetHeight(0);

	cjAssert(m_curState->m_vpW == vpClientW);
	cjAssert(m_curState->m_vpH == vpClientH);

	m_savedStateIndex = 0;

	PushState();

	if(!m_curState->m_renderTargetGrp)
	{
		if(!m_renDev->BeginUsingDefaultRenTargetGrp(cjRenInterface::ECLEAR_NONE))
		{
			PopState();
			return cjFalse;
		}

		m_curState->m_renderTargetGrp = m_renDev->GetCurRenTargetGrp();
		cjAssert(m_curState->m_renderTargetGrp);
	}

//	cjCamera* vpCam = m_viewport->m_camera;

	// setup camera data of viewport
//	m_curState->m_camEye = vpCam->GetEye();

//	ConvertToGBViewport(m_curState->m_camPort, m_curState->m_vpX, m_curState->m_vpY, 
//		m_curState->m_vpW, m_curState->m_vpH);

	// update cam rud, viewport
//	SetCameraRot(vpCam->GetRight(), vpCam->GetUp(), vpCam->GetDir());
//	CommitCameraData();

	return cjTrue;
}

void cjGLRenInterface::End()
{
	if(!m_viewport)
		return;

	PopState();

	cjAssert(m_savedStateIndex == 0);
	m_viewport = NULL;
}

void cjGLRenInterface::PushState()
{
	m_curState = &m_savedState[m_savedStateIndex+1];

	// copy previous state
	cjMemCpy(m_curState, &m_savedState[m_savedStateIndex], sizeof(cjGLSavedState));
	m_savedStateIndex++;
    
    // Add reference for RenderPass
   	for(cjINT32 renPass=0; renPass<m_curState->m_numRenderPass; renPass++)
        m_curState->m_renderPass[renPass]->m_numRef++;
}

void cjGLRenInterface::PopState()
{
	if(m_savedStateIndex == 0)
	{
		cjError(_CT("No state is there to pop.."));
		return;
	}

	cjGLSavedState& oldState = m_savedState[m_savedStateIndex--];
	cjGLSavedState& popState = m_savedState[m_savedStateIndex];

	m_curState = &m_savedState[m_savedStateIndex];

	// restore render target surfaces & depthstencil
	cjFRenTargetGrp* curRTG = m_renDev->GetCurRenTargetGrp();

	if(curRTG != popState.m_renderTargetGrp)
	{
		m_renDev->EndUsingRenTargetGrp();

		if(popState.m_renderTargetGrp)
		{
			if(!m_renDev->BeginUsingRenTargetGrp(popState.m_renderTargetGrp,
				cjRenInterface::ECLEAR_NONE))
			{
				cjError(_CT("Failed to set renderTargetGrp.."));
				popState.m_renderTargetGrp = NULL;
			}
		}
	}

	if(popState.m_renderTargetGrp)
	{
		// failed under no renderTarget group is activated
		// restore camera & viewport
//		CommitCameraData(&popState);
	}

	// adjust render pass reference
	for(cjINT32 renPass=0; oldState.m_renderPass[renPass]; renPass++)
	{
		if(--oldState.m_renderPass[renPass]->m_numRef == 0)
			m_renderPassPool.Free(oldState.m_renderPass[renPass]);
        
		oldState.m_renderPass[renPass] = NULL;
	}
}

cjBool cjGLRenInterface::SetRenTargetGrp(cjFRenTargetGrp* renderTargetGrp, cjTFLAG opt)
{
	cjFRenTargetGrp* curRTG = m_renDev->GetCurRenTargetGrp();
	cjAssert(curRTG == m_curState->m_renderTargetGrp);

	if(curRTG != renderTargetGrp)
	{
		m_curState->m_renderTargetGrp = renderTargetGrp;

		m_renDev->EndUsingRenTargetGrp();
		m_renDev->BeginUsingRenTargetGrp(renderTargetGrp, opt);
	}

	return cjTrue;
}

void cjGLRenInterface::SetViewport(cjINT32 x, cjINT32 y, cjINT32 width, cjINT32 height)
{
	m_curState->m_vpX = x;
	m_curState->m_vpY = y;
	m_curState->m_vpW = width;
	m_curState->m_vpH = height;
}

cjBool cjGLRenInterface::Clear(cjTFLAG clearFlag, cjColor color, cjFLOAT depth, cjTFLAG stencil)
{
	if(clearFlag & cjRenInterface::ECLEAR_COLOR)
		m_renDev->SetBackgroundColor(color);

	if(clearFlag & cjRenInterface::ECLEAR_DEPTH)
		m_renDev->SetDepthClear(depth);

	if(clearFlag & cjRenInterface::ECLEAR_STENCIL)
		m_renDev->SetStencilClear(stencil);

	m_renDev->ClearBuffer(clearFlag);

	return cjTrue;
}

void cjGLRenInterface::DrawTest()
{
	m_renDev->TestDraw();	
}

//
// cjGLRenInterface
//

cjINT32 cjGLRenInterface::SetMaterial(cjMaterial* material)
{
    // clear render pass
	for(cjINT32 i=0; m_curState->m_renderPass[i] != NULL; i++)
	{
        m_curState->m_renderPass[i]->m_numRef--;
        
		if(m_curState->m_renderPass[i]->m_numRef == 0)
			m_renderPassPool.Free(m_curState->m_renderPass[i]);

		m_curState->m_renderPass[i] = NULL;
	}
    
    if(!material)
	{
        material = cjRenDev::GetDefaultMtl();
	}

	// init default state.
	m_curState->m_renderPass[0] = m_renderPassPool.Alloc(&m_defRenderPass);
	m_curState->m_numRenderPass = 1;

    ///////////////////////////////////////////////////////    
    cjBool useFallbacks = cjFalse;
    cjMaterial** errMat = NULL;
    cjString* errStr = NULL;
    
    for(;;)
    {
        cjGLMtlModifyAccum  modifyAccum;

		cjBitmapMaterial*   bitmapMtl = cjNULL;
        cjShaderMtl*        shaderMtl = cjNULL;        
		cjMaterial*         nonModifier = cjNULL;
        
        cjBool rst = cjFalse;
        cjEngineTime_ft timeSec = 0;
        
		material->PreSetupMaterial(timeSec);

		if((nonModifier = bitmapMtl = cjGLCheckMaterial<cjBitmapMaterial, cjMT_Bitmap>(
			this, material, &modifyAccum, useFallbacks)))
		{
			// cjTexture
			rst = SetBitmapMaterial(bitmapMtl, modifyAccum); // modifyAccum is set in cjGLCheckMaterial
		}
		else if((nonModifier = shaderMtl = cjGLCheckMaterial<cjShaderMtl, cjMT_Shader>(
			this, material, &modifyAccum, useFallbacks)))
		{
			rst = SetShaderMaterial(shaderMtl, modifyAccum);
		}
        else
            break;


		//
		// handle fall back
		//
        
        if(!useFallbacks)
        {
            if(!rst)
            {
                for(cjINT32 passIdx=0; m_curState->m_renderPass[passIdx]; passIdx++)
                {
                    m_renderPassPool.Free(m_curState->m_renderPass[passIdx]);
                    m_curState->m_renderPass[passIdx] = NULL;
                }
                
                m_curState->m_renderPass[0] = m_renderPassPool.Alloc(&m_defRenderPass);
                m_curState->m_numRenderPass = 1;
            }

            break;
        }

        if(rst)
            break;

        if(nonModifier && nonModifier->HasFallback())
        {
            nonModifier->m_useFallbackMaterial = 1;

            if(errMat)
                *errMat = NULL;
            
            if(errStr)
                *errStr = cjString(_CT(""));
        }
        else
        {
            // use default texture
            material = cjObjCast<cjMaterial>(cjMaterial::cjObjGetStaticCls()->cjObjGetDefaultObj())->m_defaultMaterial;
        }
                
        //
        // Rollback RenderPass
        //
        for(cjINT32 passIdx=0; m_curState->m_renderPass[passIdx]; passIdx++)
        {
            m_renderPassPool.Free(m_curState->m_renderPass[passIdx]);
            m_curState->m_renderPass[passIdx] = NULL;
        }
        
        m_curState->m_renderPass[0] = m_renderPassPool.Alloc(&m_defRenderPass);
        m_curState->m_numRenderPass = 1;
        
    } // end of for(;;)
    
    
    //
    // lookup shader
    //
    
//    for(cjINT32 i=0; i<material->GetNumRenderPass(); i++)
//    {
//        m_curState->m_renderPass[0] = m_renderPassPool.Alloc(&m_defRenderPass);
//        cjGLRenderPass* rp = m_curState->m_renderPass[0];
//        
//        cjGLShaderProgram* shaderProgGL
//            = (cjGLShaderProgram*)cjTResolveGLRenRes(m_renDev, material->GetShaderProg(i));
//        
//        rp->m_shaderProg = shaderProgGL;
//        m_curState->m_numRenderPass++;
//    }
    

	//cjdefer
//    for(cjINT32 i=0; i<m_curState->m_numRenderPass; i++)
//    {
//        cjGLRenderPass* rp = m_curState->m_renderPass[i];
//        //cjGLShaderProgram* shaderProgGL = (cjGLShaderProgram*)m_renDev->ResolveGLShader(rp);
////            = (cjGLShaderProgram*)cjTResolveGLRenRes(m_renDev, m_renDev->ResolveGLShader(rp));
////        rp->m_shaderProg = shaderProgGL;
//    }

    m_curState->m_curRenderPass = NULL;
    SetDirty(EDF_RENPASS);
    
    return m_curState->m_numRenderPass;
}

void cjGLRenInterface::SetCullMode(cjCullMode cullMode)
{
    m_curState->m_cullMode = cullMode;
    
	switch(cullMode)
	{
	case cjCM_CW:
        {
            m_renDev->glEnable(GL_CULL_FACE);
            m_renDev->glCullFace(GL_BACK);
        }
		break;
        
	case cjCM_CCW:
        {
            m_renDev->glEnable(GL_CULL_FACE);
            m_renDev->glCullFace(GL_FRONT);
        }
		break;
        
	case cjCM_None:
		m_renDev->glDisable(GL_CULL_FACE); // do not culling
		break;
	}
}

void cjGLRenInterface::SetWorld(const cjMatrix* world)
{
	if(world)
		m_curState->m_world = *world;
	else
		m_curState->m_world.SetIdentity();
}

void cjGLRenInterface::SetView(const cjMatrix* view)
{
	if(view)
		m_curState->m_view = *view;
	else
		m_curState->m_view.SetIdentity();
}

void cjGLRenInterface::SetProj(const cjMatrix* proj)
{
	if(proj)
		m_curState->m_proj = *proj;
	else
		m_curState->m_proj = ms_defaultProj;
}

cjINT32 cjGLRenInterface::SetVertexStreams(cjFVertexStream** stream, cjINT32 numStream)
{
    // clear unused stream
    for(cjINT32 i=numStream; i<m_curState->m_numStream; i++)
        m_curState->m_stream[i] = NULL;
    
//    cjFShaderStreamDecl shaderStreamDecl;
//    shaderStreamDecl.m_numStream = numStream;

    // generate shader stream declration
//    for(cjINT32 i=0; i<m_curState->m_numStream; i++)
//        shaderStreamDecl.m_decl[i] = cjVertexStreamDecl(stream[i]);
    
    cjINT32 siz=0, totalStreamSiz = 0;
    
    for(cjINT32 i=0; i<numStream; i++)
	{
	   stream[i]->Validate();
       totalStreamSiz += stream[i]->GetByteSiz();
	}

    for(cjINT32 i=0; i<numStream; i++)
    {
        siz += stream[i]->GetByteSiz();		
        cjGLVertexStream* glStreamCache = cjTResolveGLRenRes<cjGLVertexStream>(m_renDev, stream[i]);

//        cjResCacheID cacheID = stream[i]->GetCacheID();
//        cjGLVertexStream* glStreamCache = (cjGLVertexStream*)m_renDev->FindResCache(cacheID);
//
//        if(!glStreamCache)
//        {
////            glStreamCache = cjGLRenDev::GenerateVertexStream(m_renDev, cacheID);
//        }
//
//        if(glStreamCache->GetCacheRev() != stream[i]->GetCacheRev())
//        {
//            siz += stream[i]->GetSiz();
//            glStreamCache->Cache(stream[i]);
//        }

        cjINT32 stride = stream[i]->GetStride();

		m_curState->m_fstream[i] = stream[i];
        m_curState->m_stream[i] = glStreamCache;
        m_curState->m_streamStride[i] = stride;
    }
	
	m_curState->m_numStream = numStream;

    SetDirty(EDF_STREAM_CHANGED);
        
    return siz;
}

void cjGLRenInterface::SetSkinBoneMatrixs(const cjVec4* boneMatrixs, cjINT32 boneMatrixCount)
{
	if(boneMatrixCount == 0)
		boneMatrixs = cjNULL;

	m_curState->m_skinBoneMatrixs = boneMatrixs;
	m_curState->m_skinBoneMatrixCount = boneMatrixCount;
}

void cjGLRenInterface::SetScenePropList(const cjScenePropRPtrList* scenePropList)
{
	m_curState->m_scenePropList = scenePropList;
}

void cjGLRenInterface::ResolveShader(const cjVertexStreamAttribLayoutResolver* resolver)
{
	for(cjINT32 i=0; m_curState->m_renderPass[i]; i++)
	{
		cjFShaderProgram* shaderProg = m_curState->m_renderPass[i]->m_shaderProg;

		if(!shaderProg)
			continue;
		
		cjFVertexStreamAttribLayout* layout = resolver->OnResolveVertexStreamAttribLayout(
			*((const cjFVertexStream::StreamAry*)m_curState->m_fstream), m_curState->m_numStream, m_renDev);

		if(!layout)
			continue;

		cjGLVertexStreamAttribLayout* vsaLayoutGLCache
			= cjTResolveGLRenRes<cjGLVertexStreamAttribLayout>(m_renDev, layout);
		
		m_curState->m_renderPass[i]->m_vertexStreamAttribLayout = vsaLayoutGLCache;
	}
}

void cjGLRenInterface::SetIndexBuffer(cjFIndexBuffer* indexBuf, cjINT32 baseVertexIndex)
{
	if(indexBuf)
	{
		cjGLIndexBuffer* cachedGLIndexBuffer 
			= cjTResolveGLRenRes<cjGLIndexBuffer>(m_renDev, indexBuf);

		cachedGLIndexBuffer->SetLastFrameUsed(m_renDev->GetFrameCounter());

		// Set the index buffer.
		m_curState->m_indexBuf = cachedGLIndexBuffer;
		m_curState->m_indexBufBase = baseVertexIndex;
	}
	else
	{
		m_curState->m_indexBuf = NULL;
		m_curState->m_indexBufBase = 0;
	}
}

cjBool cjGLRenInterface::SetupRenderPass(cjGLRenderPass* renderPass)
{
    if(!renderPass)
    {
        m_curState->m_curRenderPass = NULL;
        return cjFalse;
    }
    
    if(m_curState->m_curRenderPass == renderPass)
		return cjTrue;
    
    // setup shader program
    if(!renderPass->m_shaderProg || !renderPass->m_vertexStreamAttribLayout)
        return cjFalse;
    

	cjGLShaderProgram* shaderProg 
		= cjTResolveGLRenRes<cjGLShaderProgram>(m_renDev, renderPass->m_shaderProg);

	if(!shaderProg)
		return cjFalse;

    GLuint shaderProgHandle = shaderProg->GetHandle();
    
    if(!shaderProgHandle)
        return cjFalse;

	GLuint vao = renderPass->m_vertexStreamAttribLayout->GetVAO();
	m_renDev->glBindVertexArray(vao); // vao has vertex stream buffer

	//cjstemp
	if(m_dirtyFlag & EDF_STREAM_CHANGED)
    {
		// vertex, texture stream
	//	_CommitStreams(firstIndex, pass);
		_CommitStreams(0, 0);
    }

    
    if(!cjGLVertexStreamAttribLayout::SetupVertexArrayForShaderProgram(
        m_renDev, renderPass->m_vertexStreamAttribLayout->GetVAO(),
        shaderProg, renderPass->m_vertexStreamAttribLayout->GetVtxDecl()))	
	{
		m_renDev->glBindVertexArray(0);
		m_renDev->glBindBuffer(GL_ARRAY_BUFFER, 0);

        return cjFalse;
	}
    
//    renderPass->m_vertexStreamAttribLayout->SetupVertexArrayForShaderProgram( shaderProg);
//    m_renDev->glBindVertexArray(renderPass->m_vertexStreamAttribLayout->GetVAO());

    m_renDev->glUseProgram(shaderProgHandle);
    
    // shader constants - vertex shader, fragmentshader (texture, sampler)
    SetupShaderUniforms(shaderProg);
    
    // setup fillmode
    GLenum fillMode = GL_FILL;

    if(1 || renderPass->m_fillMode == cjFM_Wireframe)
		fillMode = GL_LINE;
    
    m_renDev->glPolygonMode(GL_FRONT_AND_BACK, fillMode);
    
	// setup alpha test
// glAlphaFunc/glEnable(GL_ALPHA_TEST) must be implemented in fragment shader
//	m_renDev->glAlphaFunc(GL_GREATER, renderPass->m_alphaRef / 255.0f);
//	if(renderPass->m_alphaTest)
//		m_renDev->glEnable(GL_ALPHA_TEST);
//	else
//		m_renDev->glDisable(GL_ALPHA_TEST);

	// setup depth buffer.
	m_renDev->glDepthFunc(renderPass->m_zTest ? GL_LESS : GL_ALWAYS );
//	m_renDev->glDepthFunc(renderPass->m_zTest ? GL_LEQUAL : GL_ALWAYS );
	m_renDev->glDepthMask(renderPass->m_zWrite ? GL_TRUE : GL_FALSE );

	// for restore render state.
	m_curState->m_alphaRef	= renderPass->m_alphaRef;
	m_curState->m_alphaTest	= renderPass->m_alphaTest;
	m_curState->m_zTest		= renderPass->m_zTest;
    m_curState->m_zWrite    = renderPass->m_zWrite;

	SetCullMode(renderPass->m_cullMode ? cjCM_None : m_curState->m_cullMode);

    // setup alphablending
    m_curState->m_alphaBlending = renderPass->m_alphaBlend;
    if(m_curState->m_alphaBlending)
    {
        m_renDev->glEnable(GL_BLEND);
    	m_renDev->glBlendFunc(renderPass->m_srcBlend, renderPass->m_dstBlend);
    }
    else
        m_renDev->glDisable(GL_BLEND);
    
    // setup textures & factors

    for(cjINT32 i=0; i<renderPass->m_numTextureStage; i++)
    {
        cjGLTextureStage* ts = &renderPass->m_textureStage[i];
        cjGLTexture* texture = ts->m_texture;
		    
			// select texture unit
		m_renDev->glActiveTexture(GL_TEXTURE0 + i);

        if(texture)
        {
            //setup texture on shader            
            m_renDev->glBindTexture(GL_TEXTURE_2D, texture->m_textureID);
        }
		else
		{
            m_renDev->glBindTexture(GL_TEXTURE_2D, 0);
		}
    }
		
	m_renDev->glActiveTexture(GL_TEXTURE0);
 
    // commit material constant
 //   cjShaderUniformValSet* uvalset = renderPass->m_shaderProg->GetShaderUniformValSet(i);
 //
 //   uvalset->SetMatrix(...)
 //   uvalSet->CommitValue();
 
    
    m_curState->m_curRenderPass = renderPass;

    return cjTrue;
}

void cjGLRenInterface::SetupShaderUniforms(cjGLShaderProgram* shaderProg)
{
    if(!shaderProg)
        return;
    
    cjGLShaderUniformValSet* uniformValSet = shaderProg->GetUniformValSet();
    
    if(!uniformValSet)
        return;

	cjMatrix vp = m_curState->m_view * m_curState->m_proj;
    
	uniformValSet->SetValue("u_m4World",	&m_curState->m_world);
    uniformValSet->SetValue("u_m4MVP",		&vp);
    
	//if(m_matBlendAryCount > 0)
	//{
	//	struct xz
	//	{
	//		cjFLOAT x,y,z,w;
	//	};

	//	//xz v[16] =
	//	//{
	//	//	{ 0.000139,		-1.000000,	-0.000005,	0.000000			},
	//	//	{ 0.000000,		0.000005,	-1.000000,	0.000000			},
	//	//	{ 1.000000,		0.000139,	0.000000,	0.000000			},
	//	//	{ -0.020413,	0.000000,	-0.081652,	1.000000			},
	//	//	
	//	//	{ 1.000000,		-0.000597,		0.000000,	0.000000		},
	//	//	{ 0.000597,		1.000000,		-0.000005,	0.000000		},
	//	//	{ -0.000000,	0.000005,		1.000000,	0.000000		},
	//	//	{ 6.859258,		-0.000001,		0.000000,	1.000000		},
	//	//	
	//	//	{ 1.000000,		-0.000000,	-0.000000,	0.000000			},
	//	//	{ 0.000000,		1.000000,	0.000000,	0.000000			},
	//	//	{ -0.000000,	-0.000000,	1.000000,	0.000000			},
	//	//	{ 6.205688,		-0.000000,	0.000001,	1.000000			},
	//	//	
	//	//	{ 1.000000, 0.000000, 0.000000, 0.000000					},
	//	//	{ 0.000000, 1.000000, 0.000000, 0.000000					},
	//	//	{ 0.000000, 0.000000, 1.000000, 0.000000					},
	//	//	{ 0.000000, 0.000000, 0.000000, 1.000000					}
	//	//};


	//	xz v[16] =
	//	{
	//		{ 1.000000, 0.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 1.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 1.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 0.000000, 1.000000					},
	//		
	//		{ 1.000000, 0.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 1.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 1.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 0.000000, 1.000000					},
	//		
	//		{ 1.000000, 0.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 1.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 1.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 0.000000, 1.000000					},
	//		
	//		{ 1.000000, 0.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 1.000000, 0.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 1.000000, 0.000000					},
	//		{ 0.000000, 0.000000, 0.000000, 1.000000					}
	//	};

		// bone palette
		if(m_curState->m_skinBoneMatrixCount)
		{
			uniformValSet->SetValue("u_bonePals", (cjVec4*)m_curState->m_skinBoneMatrixs, 
				m_curState->m_skinBoneMatrixCount);
		}

		//for(cjINT32 i=0; i<m_matBlendAryCount; i++)
		//{			
		//	cjFLOAT (*t)[4] = m_matBlendAry[i].m;

		//	cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[0][0], t[0][1], t[0][2], t[0][3]);
		//	cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[1][0], t[1][1], t[1][2], t[1][3]);
		//	cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[2][0], t[2][1], t[2][2], t[2][3]);
		//	cjLOG("[bone matrix] %d , %f %f %f %f ", i, t[3][0], t[3][1], t[3][2], t[3][3]);
		//}

		//	uniformValSet->SetValue("blendMatrixAry", (cjMatrix*)v, 16);
	//}

    uniformValSet->CommitToShader(m_renDev);
    
//    shaderProg->SetupShaderConstants(constantsCallback);
//    
//    m_renDev->glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0,
//        (const GLfloat*)g_modelViewProjectionMatrix.m);
//    
//    m_renDev->glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0,
//        (const GLfloat*)g_normalMatrix.m);
}

void cjGLRenInterface::DrawPrimitive(cjPrimitiveType primitiveType, cjINT32 firstIndex,
	cjINT32 numPrimitive, cjINT32 minIndex, cjINT32 maxIndex)
{
    if(!m_curState->m_renderTargetGrp || !m_curState->m_renderTargetGrp->GetRenTarget(0))
        return;

    if(m_dirtyFlag & EDF_MATRIX)
        _CommitMatrices();
    
    cjINT32 pass = 0;
    
	for(cjINT32 pass=0; pass<m_curState->m_numRenderPass; pass++)
	{
		if(!SetupRenderPass(m_curState->m_renderPass[pass]))
            continue;

        cjGLIndexBuffer* indexBuffer = m_curState->m_indexBuf;
        GLenum drawType = GL_TRIANGLES;
        GLsizei drawCount = numPrimitive;
    
        if(indexBuffer)
        {
			// cjstemp
			indexBuffer->BindIndexBufferToGL(m_renDev);			
        }
        else
        {
            m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    
   //     if(m_dirtyFlag & EDF_STREAM_CHANGED)
   //     {
   //         // vertex, texture stream
			//_CommitStreams(firstIndex, pass);
   //     }
 
		switch(primitiveType)
		{
		case cjPT_TriangleList:     drawType = GL_TRIANGLES;          drawCount *= 3; break;
		case cjPT_TriangleStrip:    drawType = GL_TRIANGLE_STRIP;     drawCount += 2; break;
		case cjPT_TriangleFan:		drawType = GL_TRIANGLE_FAN;       drawCount	+= 2; break;
		case cjPT_PointList:		drawType = GL_POINTS;             break;
		case cjPT_LineList:         drawType = GL_LINES;              drawCount *= 2; break;
		default:                    drawType = GL_TRIANGLES;          drawCount = 0; break;
		}
	
		if(indexBuffer)
        {
            GLvoid* indexOffset = (GLvoid*)0;
            GLsizei numIndices = drawCount; // indexBuffer->GetIndexCount();
            GLenum indexType = GL_UNSIGNED_SHORT;

			m_renDev->glDrawElements(drawType, numIndices, indexType, (GLvoid*)indexOffset);

			// cjstemp
			// off
//#2			indexBuffer->UnBindIndexBufferToGL(m_renDev);
			
			// cjstemp


			// m_renDev->glVertexAttribPointer(0, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(1, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(2, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(3, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(4, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(5, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(6, 0, 0, GL_FALSE, 0, 0);
			//m_renDev->glVertexAttribPointer(7, 0, 0, GL_FALSE, 0, 0);

            // eagl3.0, glDrawRangeElements
			/*
				Use glDrawRangeElements Instead of glDrawElements
				Using range elements is more efficient for two reasons:
				* If the specified range can fit into a 16-bit integer, the driver can optimize the
					format of indices to pass to the GPU. It can turn a 32-bit integer format into a
					16-bit integer format. In this case, there is a gain of 2¡¿.
				* The range is precious information for the VBO manager, which can use it to
				  optimize its internal memory configuration.
			*/
        }
        else
		{
            m_renDev->glDrawArrays(drawType, 0, drawCount);
		}

		// cjstemp
		// off
//#3		m_renDev->glBindVertexArray(0); // SetupRenderPass call glBindeVertexArray

		// cjtemp
		m_renDev->glBindVertexArray(0);
		m_renDev->glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_renDev->glUseProgram(0);

	} // end of pass


	// end pass
	
//#4	m_renDev->glBindBuffer(GL_ARRAY_BUFFER, 0);  <<<
	m_renDev->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_renDev->glBindTexture(GL_TEXTURE_2D, 0);
	m_renDev->glUseProgram(0);

	// cjstemp		m_renDev->glUseProgram(0);
	/*
		m_renDev->glDisableVertexAttribArray(0);
		m_renDev->glDisableVertexAttribArray(1);
		m_renDev->glDisableVertexAttribArray(2);
		m_renDev->glDisableVertexAttribArray(3);
		m_renDev->glDisableVertexAttribArray(4);
		m_renDev->glDisableVertexAttribArray(5);
		*/

    
}

void cjGLRenInterface::SetDirty(cjTFLAG flag)
{
    m_dirtyFlag |= flag;
}

void cjGLRenInterface::RemoveDirtyFlag(cjTFLAG flag)
{

}


void cjGLRenInterface::_CommitStreams(cjINT32 firstIndex, cjINT32 pass)
{
	/*
	https://www.opengl.org/discussion_boards/showthread.php/181516-glDrawElements-trouble

	Are you using a VAO? You don't need to bind the VBO and IBO while rendering. Just bind the VAO and call glDrawElements.
 Also, try using unsigned short instead of unsigned byte for the indices */

//    cjBool useIndexBuffer = m_curState->m_indexBuffer == NULL ? cjTrue : cjFalse;
//    cjINT32 indexBufferBase = m_m_curState->m_indexBufferBase;
	
//    if(!useIndexBuffer)
//		indexBufferBase += firstIndex;

//    cjBool addedAdditionalStreamToStream0 = cjFalse;

	for(cjINT32 streamIndex = 0; streamIndex < m_curState->m_numStream; streamIndex++)
	{
        cjGLVertexStream* curStream = m_curState->m_stream[streamIndex];

        if(streamIndex == 0)
        {
        //	m_renDev->glBindVertexArray(curStream->GetVAO());
        //}
        //else
        //{
  //          addedAdditionalStreamToStream0 = cjTrue;
            
            // attach into vertex array object at stream 0

			// cjstemp
			// on
            m_renDev->glBindBuffer(GL_ARRAY_BUFFER, curStream->GetVtxBufHandle(0));

         //   cjGLVertexDeclService::PerformVertexAttribFromVertexDecl(m_renDev, curStream->GetVertexDecl());
        }
    }
    
    RemoveDirtyFlag(EDF_STREAM_CHANGED);
}

void cjGLRenInterface::_CommitMatrices()
{

}

cjBool cjGLRenInterface::SetBitmapMaterial(cjBitmapMaterial* bitmapMtl, cjGLMtlModifyAccum modifyAccum)
{
    if(!bitmapMtl)
        return cjFalse;
    
    cjINT32 numUsedPass = 0;
    cjINT32 numUsedStage = 0;
    
    cjGLTextureStage& texStage
        = m_curState->m_renderPass[numUsedPass]->m_textureStage[numUsedStage];
    
    SetTextureStageBitmap(texStage, bitmapMtl);
	CommitTexStageModify(texStage, &modifyAccum); // commit texture stage info
    texStage.m_texCoordSource = 1;
    numUsedStage++; // texture stage used
    
    m_curState->m_renderPass[numUsedPass]->m_numTextureStage = numUsedStage;
    
    
    numUsedPass++;
    m_curState->m_numRenderPass = numUsedPass;
    
    return cjTrue;
}

void cjGLRenInterface::SetTextureStageBitmap(cjGLTextureStage& stage, cjBitmapMaterial* bitmapMtl)
{
	if(bitmapMtl)
	{
        cjViewport* viewport = GetViewport();
        bitmapMtl = bitmapMtl->Get(viewport->GetCurTime(), viewport);
        cjFBaseTexture* srcBaseTexture = bitmapMtl->GetTexInterface();
        
		stage.m_texture = CacheTexture(srcBaseTexture);

		{
			switch(bitmapMtl->GetTexClampModeU())
			{
			case cjTexClamp_Wrap:	stage.m_texAddrU = GL_REPEAT; break;
			case cjTexClamp_Clamp:	stage.m_texAddrU = GL_CLAMP_TO_EDGE; break;
			}
            
			switch(bitmapMtl->GetTexClampModeV())
			{
			case cjTexClamp_Wrap:	stage.m_texAddrV = GL_REPEAT; break;
			case cjTexClamp_Clamp:	stage.m_texAddrV = GL_CLAMP_TO_EDGE; break;
			}
		}
	}
	else
		stage.m_texture = NULL;
}

void cjGLRenInterface::CommitTexStageModify(cjGLTextureStage& stage, cjGLMtlModifyAccum* modifyAccum)
{
	if(modifyAccum->m_commitTexTransforms)
	{
		// Apply texture coordinate transforms
		stage.m_texCoordCount = modifyAccum->m_texCoordCount;
        
		if(modifyAccum->m_texCoordProjected)
			stage.m_texCoordGenProjected = cjTrue;

		switch(modifyAccum->m_texCoordSource)
		{
		case cjTexCoordSrcPassThru:
            {
                stage.m_texCoordSource = cjTexCoordSrcPassThru;
                stage.m_texTransformEnable = cjTrue;
            }
			break;
            
		case cjTexCoordSrc0: case cjTexCoordSrc1: case cjTexCoordSrc2: case cjTexCoordSrc3:
		case cjTexCoordSrc4: case cjTexCoordSrc5: case cjTexCoordSrc6: case cjTexCoordSrc7:
			{
				stage.m_texCoordSource = modifyAccum->m_texCoordSource;
				if(modifyAccum->m_texCoordProjected || !(modifyAccum->m_texMatrix == cjMatrix::Identity))
				{
					stage.m_texTransformMatrix = modifyAccum->m_texMatrix;
					stage.m_texTransformEnable = cjTrue;
				}
				else
                {
					stage.m_texTransformEnable = cjFalse;
                }
			}
			break;
            
		case cjTexCoordSrcWorld:
		case cjTexCoordSrcCamera:
		case cjTexCoordSrcEnvMapWorld:
		case cjTexCoordSrcEnvMapCamera:
			{
				stage.m_texCoordGenMode = modifyAccum->m_texCoordSource;
				if(!(modifyAccum->m_texMatrix == cjMatrix::Identity))
				{
					stage.m_useTexMatrixAsTexCoordGen = cjTrue;
					stage.m_texTransformMatrix = modifyAccum->m_texMatrix;
				}
			}
			break;
            
		default:
            {
                cjAssert(!"ERR! - Bad texcoordSource..");
                break;
            }
		}
	}
}

cjINT32 cjGLRenInterface::GetFVertexStream(cjFVertexStream*** vtxStreamAry)
{
	if(vtxStreamAry)
		*vtxStreamAry = m_curState->m_fstream;

	return m_curState->m_numStream;
}

const cjScenePropRPtrList* cjGLRenInterface::GetScenePropList()
{
	return m_curState->m_scenePropList;
}

cjBool cjGLRenInterface::SetShaderMaterial(const cjShaderMtl* shaderMtl, cjGLMtlModifyAccum modifyAccum)
{
	cjShaderMtlDesc mtlDesc;

	cjFVertexStream** streamAry = cjNULL;
	cjINT32 streamCount = GetFVertexStream(&streamAry);

	if(!streamCount)
		return cjFalse;

	const cjScenePropRPtrList* scenePropList =GetScenePropList();
	
	// apply vertex stream info on mtlDesc	
	// cjopti - get rid of here
	cjShader* shader = shaderMtl->ResolveShader(mtlDesc, (const cjFVertexStream**)streamAry, 
		streamCount, scenePropList);
		
	//// apply cjScenePropRPtr info on mtlDesc				
	//cjShader* shader = cjShaderManager::Get()->ResolveShader(mtlDesc, (const cjFVertexStream**)streamAry, streamCount);

	if(!shader)
		return cjFalse;

	cjAryOff usedPassCount = 0;
	cjINT32 usedTexStageCount = 0;

	cjAryOff renPassCount = shader->GetRenPassCount();

	const cjSceneRenPropTextures* texProp = cjNULL;
	cjBool setupTexture = cjFalse;

	if(scenePropList)
		texProp = cjScenePropUtil_Find<cjSceneRenPropTextures>(*scenePropList);

	for( ; usedPassCount<renPassCount; usedPassCount++)
	{
		if(texProp)
		{
			const cjSceneRenPropTextures::TexAry& texAry = texProp->GetTexAry();

			//cjopti need to loop with actual texture count
			for(cjAryOff t=0; t<texAry.Count(); t++)
			{
				cjSceneRenPropTextures::Tex* curTex = texAry(t);

				if(!curTex)
					continue;

				cjTexture* texObj = curTex->GetTexObj();

				if(!texObj)
					continue;

				cjGLTextureStage& texStage = m_curState->m_renderPass[usedPassCount]->m_textureStage[usedTexStageCount];
				texStage.m_texTypeForShader = cjSceneRenPropTextures::Tex::ConvertToShaderTexType(t);

				SetTextureStageBitmap(texStage, texObj);
				CommitTexStageModify(texStage, &modifyAccum);

				texStage.m_texCoordSource = 0;
				usedTexStageCount++;
			}
			
			// total used texture
			m_curState->m_renderPass[usedPassCount]->m_numTextureStage = usedTexStageCount;
		}

			// setup shader program
		cjFShaderProgram* shaderProg = shader->GetShaderProgram(usedPassCount);		
		m_curState->m_renderPass[usedPassCount]->m_shaderProg = shaderProg;
			
		usedTexStageCount = 0;
	}

	m_curState->m_numRenderPass = usedPassCount;
		
//	cjINT32 maxTextureStages = m_renDev->GetMaxTextureStages();
//	cjINT32 numStagesUsed = 0;
//	cjINT32 numPassesUsed = 0;
//
//	cjMaterial* diffuse = shader->m_diffuse;
//	cjBool nolighting = cjFalse;
//    
//	cjBool needsLightmap = m_curState->m_lightmap ? 1 : 0;
//	cjBool needsDiffuseLight = m_curState->m_useStaticLighting || m_curState->m_useDynamicLighting;
//	
////	if(shader->m_selfIllumination)
////	{
////		diffuse	= shader->m_selfIllumination;
////		nolighting = 1;
////		needsDiffuseLight = 0;
////		needsLightmap = 0;
////	}
//
//	//
//	// 1st - Diffuse, opacity and lightmap
//	//
//	cjMaterial* diffuse = shader->m_diffuse;
//
//	cjBool nolighting           = cjFalse;
//	cjBool needsLightmap        = m_curState->m_lightmap ? cjTrue : cjFalse;
//	cjBool needsDiffuseLight    = m_curState->m_useStaticLighting || m_curState->m_useDynamicLighting;
//	
//	// Check for an unlit material
//	if( InShader->SelfIllumination && !InShader->SelfIlluminationMask )
//	{
//		InDiffuse			= InShader->SelfIllumination;
//		Unlit				= 1;
//		NeedsDiffuseLight	= 0;
//		NeedsLightmap		= 0;
//	}
//
//	// Check for an opacity override
//	UMaterial*	InOpacity				= InShader->Opacity;
//	UBOOL		OpacityUseBaseModifier	= 1;
//	
//	if( BaseModifierInfo.ModifyOpacity )
//	{
//		InOpacity						= BaseModifierInfo.Opacity;
//		OpacityUseBaseModifier			= !BaseModifierInfo.OpacityOverrideTexModifier;
//	}
//
//
//
//	// Simple case: no opacity!
//	if( !InOpacity )
//	{
//		// Process diffuse channel
//		if( !HandleCombinedMaterial( InDiffuse, PassesUsed, StagesUsed, BaseModifierInfo, 0, ErrorString, ErrorMaterial ) )
//			return 0;
//
//		if( StagesUsed == 1 )
//		{
//			// patch in diffuse to stage[0]
//			if( NeedsDiffuseLight )
//				HandleDiffuse_Patch( m_curState->m_renderPass[numUsedPasses]->m_textureStage[0] );
//
//			// add a lightmap
//			if( NeedsLightmap )
//			{
//				HandleLightmap_SP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], m_curState->Lightmap );
//				StagesUsed++;
//			}
//		}
//		else
//		if( StagesUsed<MaxTextureStages && NeedsLightmap && !NeedsDiffuseLight )
//		{
//			HandleLightmap_SP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], m_curState->Lightmap );
//			StagesUsed++;
//		}
//		else
//		if( StagesUsed<MaxTextureStages && !NeedsLightmap && NeedsDiffuseLight )
//		{
//			if( StagesUsed )
//			{
//				HandleDiffuse_Stage( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], InShader->ModulateStaticLighting2X );
//				StagesUsed++;
//			}
//		}
//		else
//		{
//			// Perform lighting in a seperate pass
//			if( NeedsDiffuseLight || NeedsLightmap )
//			{
//				NEWPASS();
//				HandleLighting_MP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], m_curState->Lightmap, NeedsDiffuseLight );
//				StagesUsed++;
//
//				// Set lighting framebuffer blending
//				if( NeedsDiffuseLight && !NeedsLightmap && !InShader->ModulateStaticLighting2X )
//				{
//					m_curState->m_renderPass[numUsedPasses]->SrcBlend				= GL_DST_COLOR;
//					m_curState->m_renderPass[numUsedPasses]->DestBlend				= GL_ZERO;
//					m_curState->m_renderPass[numUsedPasses]->OverriddenFogColor	= FColor( 255, 255, 255, 0 );
//					m_curState->m_renderPass[numUsedPasses]->OverrideFogColor		= 1;
//				}
//				else
//				{
//					m_curState->m_renderPass[numUsedPasses]->SrcBlend				= GL_DST_COLOR;
//					m_curState->m_renderPass[numUsedPasses]->DestBlend				= GL_SRC_COLOR;
//					m_curState->m_renderPass[numUsedPasses]->OverriddenFogColor	= FColor( 127, 127, 127, 0 );
//					m_curState->m_renderPass[numUsedPasses]->OverrideFogColor		= 1;
//				}
//
//				m_curState->m_renderPass[numUsedPasses]->TwoSided					= InShader->TwoSided;
//				m_curState->m_renderPass[numUsedPasses]->AlphaBlending				= 1;
//			}
//		}
//	
//
//		//
//		// 2. Self-illumination
//		//
//		UBOOL SinglePassSelfIllum=0;
//		if( !Unlit && InShader->SelfIllumination )
//		{
//			FOpenGLModifierInfo SelfIlluminationModifierInfo		= BaseModifierInfo;
//			FOpenGLModifierInfo SelfIlluminationMaskModifierInfo	= BaseModifierInfo;
//
//			UBitmapMaterial* SelfIlluminationBitmap		= CheckMaterial<UBitmapMaterial,MT_BitmapMaterial>(this, InShader->SelfIllumination, &SelfIlluminationModifierInfo );
//			UBitmapMaterial* SelfIlluminationMaskBitmap = CheckMaterial<UBitmapMaterial,MT_BitmapMaterial>(this, InShader->SelfIlluminationMask, &SelfIlluminationMaskModifierInfo );
//
//			if( PassesUsed==0 && StagesUsed<MaxTextureStages-1 && SelfIlluminationBitmap && SelfIlluminationMaskBitmap )
//			{
//				// we have room to add the self-illumination in the single pass
//				HandleOpacityBitmap( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SelfIlluminationMaskBitmap );
//				ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed],&SelfIlluminationMaskModifierInfo );
//				StagesUsed++;
//
//				HandleSelfIllumination_SP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SelfIlluminationBitmap);
//				ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed],&SelfIlluminationModifierInfo );
//				StagesUsed++;
//				SinglePassSelfIllum=1;
//			}
//			else
//			{
//				// we need to do the self-illumination in its own pass
//				NEWPASS();
//				m_curState->m_renderPass[numUsedPasses]->TwoSided = InShader->TwoSided;
//
//				// Load SelfIllumination
//				if( !HandleCombinedMaterial( InShader->SelfIllumination, PassesUsed, StagesUsed, BaseModifierInfo, 0, ErrorString, ErrorMaterial ) )
//					return 0;
//	
//				if( InShader->SelfIlluminationMask == InShader->SelfIllumination )
//				{
//					// Self-illumination alpha channel is in SelfIllumination.  Just blend it to the framebuffer.
//					m_curState->m_renderPass[numUsedPasses]->SrcBlend			= GL_SRC_ALPHA;
//					m_curState->m_renderPass[numUsedPasses]->DestBlend			= GL_ONE_MINUS_SRC_ALPHA;
//					m_curState->m_renderPass[numUsedPasses]->AlphaBlending		= 1;
//				}
//				else
//				if( SelfIlluminationMaskBitmap )
//				{
//					if( StagesUsed < MaxTextureStages )
//					{
//						HandleOpacityBitmap( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SelfIlluminationMaskBitmap );
//						ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed],&SelfIlluminationMaskModifierInfo );
//						StagesUsed++;
//					}
//					else
//					{
//						if( ErrorString ) *ErrorString = FString::Printf(TEXT("No room for SelfIllimunationMask in multipass SelfIllumination (StagesUsed was %d)"), StagesUsed);
//						if( ErrorMaterial ) *ErrorMaterial = InShader;
//						return 0;
//					}
//
//					// Add SelfIllumination to the framebuffer
//					m_curState->m_renderPass[numUsedPasses]->SrcBlend			= GL_SRC_ALPHA;
//					m_curState->m_renderPass[numUsedPasses]->DestBlend			= GL_ONE_MINUS_SRC_ALPHA;
//					m_curState->m_renderPass[numUsedPasses]->AlphaBlending		= 1;
//				}
//				else
//				{
//					if( ErrorString ) *ErrorString = TEXT("SelfIllimunationMask must be a simple bitmap");
//					if( ErrorMaterial ) *ErrorMaterial = InShader;
//					return 0;
//				}				
//			}
//		}
//
//		//
//		// 3. Detail Texture
//		//
//		if( RenDev->DetailTextures && m_curState->UseDetailTexturing )
//		{
//			UMaterial*	DetailMaterial	= NULL;
//			FLOAT		DetailScale		= 8.f;
//			if( InShader->Detail )
//			{
//				DetailMaterial	= InShader->Detail;
//				DetailScale		= InShader->DetailScale;
//			}
//			else
//			{
//				UTexture* DiffuseTex = CheckMaterial<UTexture,MT_Texture>(this, InDiffuse);
//				if( DiffuseTex )
//				{
//					DetailMaterial	= DiffuseTex->Detail;
//					DetailScale		= DiffuseTex->DetailScale;
//				}
//			}
//
//			if( DetailMaterial )
//			{
//				FOpenGLModifierInfo DetailModifierInfo = BaseModifierInfo;
//				DetailModifierInfo.SetDetailTextureScale( DetailScale );
//				UBitmapMaterial* DetailBitmap = CheckMaterial<UBitmapMaterial,MT_BitmapMaterial>(this, DetailMaterial, &DetailModifierInfo);
//				if( DetailBitmap )
//					HandleDetail( DetailBitmap, PassesUsed, StagesUsed, DetailModifierInfo );
//			}
//		}
//
//		//
//		// 4. Process specular and mask
//		//
//		if( InShader->Specular )
//		{
//			FOpenGLModifierInfo SpecularModifierInfo		= BaseModifierInfo;
//			FOpenGLModifierInfo SpecularityMaskModifierInfo = BaseModifierInfo;
//
//			UBitmapMaterial*	SpecularBitmap				= CheckMaterial<UBitmapMaterial,MT_BitmapMaterial>(this, InShader->Specular, &SpecularModifierInfo );
//			UBitmapMaterial*	SpecularityMaskBitmap		= CheckMaterial<UBitmapMaterial,MT_BitmapMaterial>(this, InShader->SpecularityMask, &SpecularityMaskModifierInfo );
//			UConstantMaterial*	SpecularityMaskConstant		= CheckMaterial<UConstantMaterial,MT_ConstantMaterial>(this, InShader->SpecularityMask, &SpecularityMaskModifierInfo );
//
//			if(	PassesUsed==0 && StagesUsed<MaxTextureStages && SpecularBitmap && ((!InShader->SpecularityMask)||(InShader->SpecularityMask==InShader->Diffuse&&!SinglePassSelfIllum)||SpecularityMaskConstant) )
//			{
//				// We can fit single-pass specular
//				if( SpecularityMaskConstant )
//				{
//					BYTE SpecularAlpha = SpecularityMaskConstant->GetColor(Viewport->Actor->Level->TimeSeconds).A;
//					m_curState->m_renderPass[numUsedPasses]->TFactorColor = FColor(SpecularAlpha,SpecularAlpha,SpecularAlpha,SpecularAlpha);
//				}
//				HandleSpecular_SP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SpecularBitmap, InShader->SpecularityMask != NULL, SpecularityMaskConstant != NULL, InShader->ModulateSpecular2X ); // sjs
//				ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], &SpecularModifierInfo );
//				StagesUsed++;
//			}
//			else
//			if(	PassesUsed==0 && StagesUsed<MaxTextureStages-1 && SpecularBitmap && SpecularityMaskBitmap )
//			{
//				// We have room for the specular and mask in the same pass pass as diffuse
//				HandleOpacityBitmap( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SpecularityMaskBitmap );
//				ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], &SpecularityMaskModifierInfo );
//				StagesUsed++;
//				HandleSpecular_SP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SpecularBitmap, 1, 0, InShader->ModulateSpecular2X ); // sjs
//				ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], &SpecularModifierInfo );
//				StagesUsed++;
//			}
//			else
//			{
//				// We need to perform specular in its own pass
//				NEWPASS();
//				m_curState->m_renderPass[numUsedPasses]->TwoSided = InShader->TwoSided;
//
//				// Load specular
//				if( !HandleCombinedMaterial( InShader->Specular, PassesUsed, StagesUsed, BaseModifierInfo, 0, ErrorString, ErrorMaterial ) )
//					return 0;
//	
//				if( !InShader->SpecularityMask )
//				{
//					// Add specular to the framebuffer
//					m_curState->m_renderPass[numUsedPasses]->SrcBlend				= GL_ONE;
//					m_curState->m_renderPass[numUsedPasses]->DestBlend				= GL_ONE;
//					m_curState->m_renderPass[numUsedPasses]->AlphaBlending			= 1;
//					m_curState->m_renderPass[numUsedPasses]->OverrideFogColor		= 1;
//					m_curState->m_renderPass[numUsedPasses]->OverriddenFogColor	= FColor( 0, 0, 0, 0 );
//				}
//				else
//				if( SpecularityMaskConstant )
//				{
//					if( ErrorString ) *ErrorString = TEXT("TODO: constant SpecularityMask with multipass specular");
//					if( ErrorMaterial ) *ErrorMaterial = InShader;
//					return 0;
//				}
//				else
//				if( SpecularityMaskBitmap )
//				{
//					if( StagesUsed < MaxTextureStages )
//					{
//						HandleOpacityBitmap( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], SpecularityMaskBitmap );
//						ApplyTexModifier( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], &SpecularityMaskModifierInfo );
//						StagesUsed++;
//					}
//					else
//					{
//						if( ErrorString ) *ErrorString = FString::Printf(TEXT("No room for SpecularityMask in multipass specular (StagesUsed was %d)"), StagesUsed);
//						if( ErrorMaterial ) *ErrorMaterial = InShader;
//						return 0;
//					}
//
//					// Add specular to the framebuffer
//					m_curState->m_renderPass[numUsedPasses]->SrcBlend				= GL_SRC_ALPHA;
//					m_curState->m_renderPass[numUsedPasses]->DestBlend				= GL_ONE;
//					m_curState->m_renderPass[numUsedPasses]->AlphaBlending			= 1;
//					m_curState->m_renderPass[numUsedPasses]->OverrideFogColor		= 1;
//					m_curState->m_renderPass[numUsedPasses]->OverriddenFogColor	= FColor( 0, 0, 0, 0 );
//				}
//				else
//				{
//					if( ErrorString ) *ErrorString = TEXT("SpecularityMask must be a simple bitmap");
//					if( ErrorMaterial ) *ErrorMaterial = InShader;
//					return 0;
//				}				
//			}
//		}
//
//	}
//
//	//
//	// Framebuffer blending
//	//
//	if( BaseModifierInfo.ModifyFramebufferBlending )
//	{
//		ApplyFinalBlend( &BaseModifierInfo );
//	}
//	else
//	{
//		switch(shader->m_outputBlend)
//		{
//		case OB_Normal :
//        
//			if( InOpacity )
//			{   // 5,6, blend, test1
//				m_curState->m_renderPass[0]->SrcBlend       = GL_SRC_ALPHA;
//				m_curState->m_renderPass[0]->DestBlend		= GL_ONE_MINUS_SRC_ALPHA;
//				m_curState->m_renderPass[0]->AlphaBlending	= 1;
//				m_curState->m_renderPass[0]->AlphaTest		= 1;
//				m_curState->m_renderPass[0]->ZWrite			= 0;
//			}
//			else
//			{   // 1,0, opaque, zw
//				m_curState->m_renderPass[0]->SrcBlend       = GL_ONE;
//				m_curState->m_renderPass[0]->DestBlend		= GL_ZERO;
//				m_curState->m_renderPass[0]->AlphaBlending	= 0;
//				m_curState->m_renderPass[0]->AlphaTest		= 0;
//				m_curState->m_renderPass[0]->ZWrite			= 1;
//			}
//			break;
//
//		case OB_Masked:
//            {   // 1,0, opaque, test127, zw
//                m_curState->m_renderPass[0]->SrcBlend			= GL_ONE;
//                m_curState->m_renderPass[0]->DestBlend			= GL_ZERO;
//                m_curState->m_renderPass[0]->AlphaBlending		= 0;
//                m_curState->m_renderPass[0]->AlphaTest			= 1;
//                m_curState->m_renderPass[0]->AlphaRef			= 127;
//                m_curState->m_renderPass[0]->ZWrite				= 1;
//            }
//			break;
//            
//		case OB_Translucent:
//            { // 1,1, blend, no-zw, effect
//                m_curState->m_renderPass[0]->SrcBlend			= GL_ONE;
//                m_curState->m_renderPass[0]->DestBlend			= GL_ONE;
//                m_curState->m_renderPass[0]->AlphaBlending		= 1;
//                m_curState->m_renderPass[0]->ZWrite				= 0;
//            }
//			break;
//            
//		case OB_Invisible:
//            { // 0,1, blend, no-zw
//                m_curState->m_renderPass[0]->SrcBlend			= GL_ZERO;
//                m_curState->m_renderPass[0]->DestBlend			= GL_ONE;
//                m_curState->m_renderPass[0]->AlphaBlending		= 1;
//                m_curState->m_renderPass[0]->ZWrite             = 0;
//            }
//			break;
//		}
//        
//		m_curState->m_renderPass[0]->TwoSided = InShader->TwoSided;
//	}
//
//	// Fillmode.
//	m_curState->m_renderPass[0]->m_fillMode = shader->m_wireframe ? cjFM_Wireframe : cjFM_Solid;
//
//	// Color modifier
//	if( BaseModifierInfo.ModifyColor )
//	{
//		if(	BaseModifierInfo.AlphaBlend								&&
//			(m_curState->m_renderPass[0]->SrcBlend  == GL_ONE)	&&
//			(m_curState->m_renderPass[0]->DestBlend == GL_ZERO)
//		)
//		{
//				m_curState->m_renderPass[0]->SrcBlend	= GL_SRC_ALPHA;
//				m_curState->m_renderPass[0]->DestBlend	= GL_ONE_MINUS_SRC_ALPHA;
//		}
//		m_curState->m_renderPass[0]->TFactorColor		 = BaseModifierInfo.TFactorColor;
//		m_curState->m_renderPass[0]->AlphaBlending		|= BaseModifierInfo.AlphaBlend;
//		m_curState->m_renderPass[0]->TwoSided			|= BaseModifierInfo.TwoSided;
//
//		if( StagesUsed
//		&&	m_curState->m_renderPass[0]->Stages[StagesUsed-1].AlphaOp == GL_REPLACE
//		&&	m_curState->m_renderPass[0]->Stages[StagesUsed-1].ColorOp == GL_REPLACE
//		)
//		{
//			HandleTFactor_Patch(m_curState->m_renderPass[0]->Stages[StagesUsed-1]);
//		}
//		else if( StagesUsed < MaxTextureStages )
//		{
//			HandleTFactor_SP(m_curState->m_renderPass[0]->Stages[StagesUsed++]);
//		}
//		else
//		{
//			if( ErrorString ) *ErrorString = TEXT("No stages left for constant color modifier.");
//			if( ErrorMaterial ) *ErrorMaterial = InShader;
//			return 0;				
//		}
//	}
//
//	// Done.
//	m_curState->m_renderPass[PassesUsed++]->StagesUsed	= StagesUsed;
//	m_curState->NumMaterialPasses			= PassesUsed;
	
    return cjTrue;
}

cjBool cjGLRenInterface::SetSimpleMaterial(cjMaterial* material,
    cjGLMtlModifyAccum modifyAccum, cjString* errStr, cjMaterial** errMaterial)
{
	cjTexture* texture;
	cjMaterial* detailMtl = NULL;
    cjFLOAT detailScale = 8.f;
 	cjINT32 numUsedStages = 0; // texture stage
	cjINT32 numUsedPasses = 0;
	cjINT32 maxTextureStages = m_renDev->GetNumMaxTextureStages();

    //
    // 1st - Handle texture material (mask, alpha texture)
    //
    
    // blending is same to previous and given material is texture
	if(!modifyAccum.m_commitFramebufferBlending)
	{
		texture = cjMaterialCast<cjTexture, cjMT_Texture>(material);
		if(texture)
		{
			// mask texture
			if(texture->m_maskTexture)
			{
				modifyAccum.m_commitFramebufferBlending = cjTrue;
            
				if(modifyAccum.m_commitColor && modifyAccum.m_alphaBlend)
					modifyAccum.m_frameBufferBlend = cjRFB_AlphaBlend;
				else
					modifyAccum.m_frameBufferBlend = cjRFB_Overwrite;
            
				modifyAccum.m_zWrite	= cjTrue;
				modifyAccum.m_zTest		= cjTrue;
				modifyAccum.m_alphaTest	= cjTrue;
				modifyAccum.m_alphaRef	= 127;
            
				if(!modifyAccum.m_commitColor)
					modifyAccum.m_twoSided = texture->m_twoSided;
			}
			// alpha texture
			else if(texture->m_alphaTexture)
			{
				modifyAccum.m_commitFramebufferBlending	= cjTrue;
				modifyAccum.m_frameBufferBlend = cjRFB_AlphaBlend;
				modifyAccum.m_zWrite    = cjTrue;
				modifyAccum.m_zTest     = cjTrue;
				modifyAccum.m_alphaTest = cjTrue;
				modifyAccum.m_alphaRef  = 0;
            
				if(!modifyAccum.m_commitColor)
					modifyAccum.m_twoSided = texture->m_twoSided;
			}
			// non-alpha texture
			else
			{
				if(!modifyAccum.m_commitColor)
					modifyAccum.m_twoSided = texture->m_twoSided;
			}

	//		m_detailMaterial = texture->m_detail;
	//		m_detailScale = texture->m_detailScale;
		}
	}
//
//    
//    //
//    // 2nd - Handle Combined
//    //
//
////	if(!HandleCombinedMaterial(material, &numUsedPasses, &numUsedStages, &modifyAccum,
////        0, errStr, errMaterial))
////    {
////		return cjFalse;
////    }
//
//
//    //
//    // 3rd - lighting (static, dynamic)
//    //
//
////	if(m_curState->m_useStaticLighting || m_curState->m_useDynamicLighting)
////	{
////		if(m_curState->m_renderPass[numUsedPasses]->PatchLighting )
////		{		
////			HandleDiffuse_Patch( m_curState->m_renderPass[numUsedPasses]->m_textureStage[0] );
////		}
////		else if( StagesUsed < MaxTextureStages )
////		{
////			HandleDiffuse_Stage(m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed++] );
////		}
////		else
////		{
////			NEWPASS();
////			HandleLighting_MP(m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], NULL, 1 );
////			StagesUsed++;
////
////			m_curState->m_renderPass[numUsedPasses]->m_srcBlend				= GL_DST_COLOR;
////			m_curState->m_renderPass[numUsedPasses]->m_dstBlend				= GL_SRC_COLOR;
////			m_curState->m_renderPass[numUsedPasses]->m_OverriddenFogColor	= FColor( 127, 127, 127, 0 );
////			m_curState->m_renderPass[numUsedPasses]->m_OverrideFogColor     = 1;
////			
////			m_curState->m_renderPass[numUsedPasses]->m_twoSided				= modifyAccum.TwoSided;
////			m_curState->m_renderPass[numUsedPasses]->m_alphaBlending		= 1;
////		}
////	}
//
//
//    //
//    // 4th - lightmap
//    //
////	if(m_curState->m_lightMapTexture)
////	{
////		if(numUsedStages < maxTextureStages)
////        {
////			HandleLightmapSinglePass(m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed++], m_curState->Lightmap );
////        }
////		else
////		{
////			NEWPASS();
////			HandleLighting_MP( m_curState->m_renderPass[numUsedPasses]->m_textureStage[StagesUsed], m_curState->Lightmap, m_curState->UseStaticLighting );
////			numUsedStages++;
////
////			m_curState->m_renderPass[numUsedPasses]->m_srcBlend				= GL_DST_COLOR;
////			m_curState->m_renderPass[numUsedPasses]->m_dstBlend				= GL_SRC_COLOR;
////			m_curState->m_renderPass[numUsedPasses]->m_overriddenFogColor	= cjColor(127, 127, 127, 0);
////			m_curState->m_renderPass[numUsedPasses]->m_overrideFogColor		= 1;
////			
////			m_curState->m_renderPass[numUsedPasses]->m_alphaBlending		= 1;
////			m_curState->m_renderPass[numUsedPasses]->m_twoSided				= modifyAccum.m_twoSided;
////		}
////	}
//
//
    //
    // commit framebuffer blending
    //
	if(modifyAccum.m_commitFramebufferBlending)
	{
		CommitFinalBlendModify(&modifyAccum); // apply on curState->m_renderPass[0]
	}
//
//
//    //
//	// commit color modify
//    //
//	if(modifyAccum.m_commitColor)
//	{
//		if(numUsedStages < maxTextureStages)
//		{
//            // alphablend && overwrite
//			if(modifyAccum.m_alphaBlend &&
//			   (m_curState->m_renderPass[numUsedPasses]->m_srcBlend == GL_ONE)	&&
//			   (m_curState->m_renderPass[numUsedPasses]->m_dstBlend == GL_ZERO)
//		    )
//		    {
//                // change to blend-type 5,6
//			    m_curState->m_renderPass[numUsedPasses]->m_srcBlend	= GL_SRC_ALPHA;
//			    m_curState->m_renderPass[numUsedPasses]->m_dstBlend	= GL_ONE_MINUS_SRC_ALPHA;
//		    }
//            
//            // usedStages > 1
////			if(numUsedStages
////			&&	m_curState->m_renderPass[0]->m_textureStage[numUsedStages-1].AlphaOp == GL_REPLACE
////			&&	m_curState->m_renderPass[0]->m_textureStage[numUsedStages-1].ColorOp == GL_REPLACE
////			)
////            {
////				HandleTFactor_Patch(m_curState->m_renderPass[0]->m_textureStage[numUsedStages-1]);
////            }
////			else
////            {
////				HandleTFactor_SinglePass(m_curState->m_renderPass[0]->m_textureStage[numUsedStages++]);
////            }
//            
//		    m_curState->m_renderPass[numUsedPasses]->m_alphaBlend |= modifyAccum.m_alphaBlend;
//		    m_curState->m_renderPass[numUsedPasses]->m_twoSided   |= modifyAccum.m_twoSided;
//		}
//		else
//		{
//            // number of texture stages is exceed maxTextureStage
//            if(errStr)
//            {
//                errStr = "Warn! - exceed maximum texture stages..";
//                errMaterial = material;
//            }
//            
//            return cjFalse;
//		}
//	}
//
//
//    //
//	// Handle detail textures.
//    //
////	if(m_renDev->m_detailTextures && m_curState->m_useDetailTexturing && detailMaterial)
////	{
////		cjGLMtlModifyAccum detailModifyAccum = modifyAccum;
////		DetailModifierInfo.SetDetailTextureScale(detailScale);
////		cjBitmapMaterial* detailBitmap = cjCheckMaterial<cjBitmapMaterial, cjMT_BitmapMaterial>(
////            this, detailMaterial, &detailModifyAccum);
////        
////		if(detailBitmap)
////			HandleDetailMtl(detailBitmap, numUsedPasses, numUsedStages, detailModifyAccum);
////	}
//
//    //
//    // update number of used stages and pass at curState
//    //
//    m_curState->m_renderPass[numUsedPasses]->m_numTextureStage = numUsedStages;
//    
//    numUsedStages = 0;
//    numUsedPasses++;
//    m_curState->m_numMaterialPasses = numUsedPasses;

    return cjTrue;
}

cjBool cjGLRenInterface::HandleCombinedMaterial(cjMaterial* material, cjINT32& numUsedPasses, cjINT32& numUsedStages, cjGLMtlModifyAccum modifyAccum, cjBool invertOutputAlpha, cjString* errStr,
cjMaterial** errMaterial)
{
    return cjFalse;
}

//void cjGLRenInterface::SetShaderBitmap(cjGLTextureState& state, cjBitmapMaterial* bitmapMaterial)
//{
//    
//}

cjGLTexture* cjGLRenInterface::CacheTexture(cjFBaseTexture* srcBaseTexture)
{
	if(!srcBaseTexture)
		return 0;
    
    cjResCacheID cacheID = srcBaseTexture->GetCacheID();
    cjGLTexture* cachedGLTex = (cjGLTexture*)m_renDev->FindResCache(cacheID);

	if(!cachedGLTex)
	{
		cachedGLTex = cjGLTexture::NewInst();
		cachedGLTex->Init(m_renDev, cacheID);
	}

	if(cachedGLTex->GetCacheRev() != srcBaseTexture->GetCacheRev())
		cachedGLTex->Cache(srcBaseTexture);

	cachedGLTex->SetLastFrameUsed(m_renDev->GetFrameCounter());
	
	return cachedGLTex;
}

void cjGLRenInterface::CommitFinalBlendModify(cjGLMtlModifyAccum* modifyAccum)
{
//	cjColor	fogColor = cjColor(0,0,0,1);
//	switch(modifyAccum->m_frameBufferBlend)
//	{
//	case cjRFB_Overwrite :
//		m_curState->m_renderPass[0]->m_srcBlend			= GL_ONE;
//		m_curState->m_renderPass[0]->m_dstBlend			= GL_ZERO;
//		m_curState->m_renderPass[0]->m_alphaBlending	= 0;
//		break;
//        
//	case cjRFB_AlphaBlend:
//		m_curState->m_renderPass[0]->m_srcBlend			= GL_SRC_ALPHA;
//		m_curState->m_renderPass[0]->m_dstBlend			= GL_ONE_MINUS_SRC_ALPHA;
//		m_curState->m_renderPass[0]->m_alphaBlending	= 1;
//		break;
//        
//	case cjRFB_Modulate:
//		m_curState->m_renderPass[0]->m_srcBlend			= GL_DST_COLOR;
//		m_curState->m_renderPass[0]->m_dstBlend			= GL_SRC_COLOR;
//		m_curState->m_renderPass[0]->m_alphaBlending	= 1;
//		break;
//        
//    default:
//        return;
//	}
//
//	m_curState->m_renderPass[0]->m_zWrite		= modifyAccum->m_ZWrite;
//	m_curState->m_renderPass[0]->m_zTest		= modifyAccum->m_ZTest;
//	m_curState->m_renderPass[0]->m_twoSided     = modifyAccum->m_twoSided;
//	m_curState->m_renderPass[0]->m_alphaTest	= modifyAccum->m_alphaTest;
//	m_curState->m_renderPass[0]->m_alphaRef     = modifyAccum->m_alphaRef;
}

void cjGLRenInterface::CommitTexStageModify(cjGLTextureStage* stage,
    cjGLMtlModifyAccum* modifyAccum)
{
//	if(modifyAccum->m_commitTexTransforms)
//	{
//		stage->m_texCoordCount = modifyAccum->m_texCoordCount;
//        
//		if(modifyAccum->m_texCoordProjected)
//			stage->m_texGenProjected = cjTrue;
//
//		switch(modifyAccum->m_texCoordSource)
//		{
//		case cjTexCoordSrc0: case cjTexCoordSrc1: case cjTexCoordSrc2: case cjTexCoordSrc3:
//		case cjTexCoordSrc4: case cjTexCoordSrc5: case cjTexCoordSrc6: case cjTexCoordSrc7:
//			{
//				stage->m_texCoordSource = modifyAccum->m_texCoordSource;
//                
//				if( modifyAccum->m_texCoordProjected || !cjMatrix4Compare(&modifyAccum->m_texMatrix, &cjMatrix::IDENTITY))
//				{
//					stage->m_textureTransformsEnabled = cjTrue;
//					stage->m_textureTransformMatrix	= modifyAccum->m_texMatrix;
//				}
//				else
//					stage->m_textureTransformsEnabled = cjFalse;
//			}
//			break;
//            
//		case cjTexCoordSrcWorld:            case cjTexCoordSrcCamera:
//		case cjTexCoordSrcCameraReflect:    case cjTexCoordSrcEnvMapWorld:
//        case cjTexCoordSrcEnvMapCamera:     case cjTexCoordSrcSphereMap:
//			{
//				stage->m_texCoordGenMode = modifyAccum->m_texCoordSource;
//				if(!cjMatrix4Compare(&modifyAccum->m_texMatrix, &cjMatrix::IDENTITY))
//				{
//					stage->m_useTexMatrixAsTexCoordGen = cjTrue;
//					stage->m_texTransformMatrix = modifyAccum->m_texMatrix;
//				}
//			}
//			break;
//            
//      	case cjTexCoordSrcSameToPrevious:
//			stage->m_texCoordSource = cjTexCoordSrcSameToPrevious;
//			stage->m_texTransformEnable = cjFalse;
//			break;
//      		
//		default:
//            cjAssert(!"ERR! - Unknown TexCoordSource..");
//            break;
//		}
//	}
}


//
// cjGLRenInterface
//

//void cjGLRenInterface::SetCuller(NiCullingProcess* /*culler*/)
//{
//
//}
//
//void cjGLRenInterface::Draw(NiNode* node)
//{
//	if(!node || !m_viewport)
//		return;
//
//	DrawRecursive(node);
//}
//
//cjBool cjGLRenInterface::DrawRecursive(NiObject* obj)
//{
//	if(!obj)
//		return fnTrue;
//
//	if(NiIsKindOf(NiRenderObject, obj))
//	{
//		NiRenderObject* renObj = (NiRenderObject*)obj;
//		renObj->GetEffectState()->AddEffect(m_testLight);
//		renObj->RenderImmediate(m_renDev->m_renderer);
//		renObj->GetEffectState()->RemoveEffect(m_testLight);
//		return fnTrue;
//	}
//	
//	if(NiIsKindOf(NiNode, obj))
//	{
//		NiNode* node = (NiNode*)obj;
//
//		for(UINT ui=0; ui<node->GetChildCount(); ui++)
//		{
//			if(!DrawRecursive(node->GetAt(ui)))
//				return fnTrue;
//		}
//
//		return fnTrue;
//	}
//
//	return fnFalse;
//}
//

//
////
//// Camera functions
////
//
//void cjGLRenInterface::SetCamera(cjVec3& camEye, cjVec3& camLookAt, cjVec3* camUp)
//{
//	cjVec3 r,u,d;
//	cjCamera::CalcCameraAxisByEyeLookAt(r, u, d, camEye, camLookAt, camUp);
//
//	m_curState->m_camR = r;
//	m_curState->m_camU = u;
//	m_curState->m_camD = d;
//
//	m_curState->m_camEye = camEye;
//}
//
//void cjGLRenInterface::SetCameraRot(const cjVec3& right, const cjVec3& up, const cjVec3& dir)
//{
//	m_curState->m_camR = right;
//	m_curState->m_camU = up;
//	m_curState->m_camD = dir;
//}
//
//void cjGLRenInterface::SetCameraRot(cjFLOAT pitch, cjFLOAT yaw, cjFLOAT roll, cjBool isDelta)
//{
//	cjVec3 r,u,d;
//	cjCamera::CalcCameraAxis(r,u,d, pitch, yaw, roll, isDelta);
//
//	m_curState->m_camR = r;
//	m_curState->m_camU = u;
//	m_curState->m_camD = d;
//}
//
//void cjGLRenInterface::CommitCameraData(cjGLSavedState* state)
//{
//	if(!state)
//		state = m_curState;
//
//	m_renDev->SetCameraData(state->m_camEye,
//		state->m_camD, state->m_camU, state->m_camR,
//		state->m_camFrustum, state->m_camPort);
//}
//
//void cjGLRenInterface::GetCameraDataToState(cjGLSavedState* state)
//{
//	m_renDev->GetCameraData(state->m_camEye, 
//		state->m_camD, state->m_camU, state->m_camR,
//		state->m_camFrustum, state->m_camPort);
//}
//
//void cjGLRenInterface::SetCameraMove(const cjVec3& newPos, cjBool isDelta, 
//	cjBool isLocal)
//{
//	if(isDelta)
//		m_curState->m_camEye += newPos;
//	else
//		m_curState->m_camEye = newPos;
//}

void cjGLRenInterface::SetDefaultProj(cjFLOAT fovYRad, cjFLOAT aspect, cjFLOAT projNear, cjFLOAT projFar)
{
	ms_projDefFovY		= fovYRad;
	ms_projDefAspect	= aspect;
	ms_projDefNear		= projNear;
	ms_projDefFar		= projFar;

	cjMatPerspectiveFovRH(ms_defaultProj, ms_projDefFovY, ms_projDefAspect, ms_projDefNear, ms_projDefFar);
}
