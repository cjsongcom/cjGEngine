
#include "cjEnginePCH.h"
#include "cjAppEngine.h"

#include "cjClient.h"

#include "cjWinViewport.h"


CJOBJ_IMPL_CLASS(cjAppEngine)

cjAppEngine* cjAppEngine::ms_appEngine = NULL;

//cjtemp
cjDouble g_cjtmp_time = 0.0;
cjDouble cj_tmp_GetTime() { return g_cjtmp_time; }

cjAppEngine::cjAppEngine()
{
	m_init = 0;
	m_client = NULL;
	m_renDev = NULL;

	m_cjTestMesh = NULL;
	m_cjTestMeshInst = NULL;

	m_axisMesh = cjNULL;
	
	cjAssert(!ms_appEngine);

	m_backGroundColor = cjColor(128, 128, 128);
}

cjEngine::ETICKRST cjAppEngine::Tick(cjFLOAT elapsedTime)
{
	if(GetExit())
		return ETICKRST_EXIT;

	if(!cjViewport::GetPrimaryViewport())
		return ETICKRST_EXIT;

	if(elapsedTime < 0.0f)
		return ETICKRST_FATAL_EXIT_BAD_TIME;
	
	// GlobalLevel Tick

	// Tick TCP Link

	// Player's Level Tick - if exist

	// Pending Level Tick

	// Client Tick
	m_client->Tick();
    
	return ETICKRST_OK;
}

void cjAppEngine::Draw(cjViewport* viewport)
{
	// present last frame
    if(viewport->m_pendingPresentFrame)
		viewport->Present();

	if(viewport->RenderLock())
	{
		//static cjMatrix3 g_normalMatrix;
        static cjFLOAT g_rotation = 0.0f;
        //static cjMatrix g_matModelView;
        static cjFLOAT g_timeSinceLastUpdate = 0.0f;
        //static cjMatrix g_modelViewProjectionMatrix;


		cjRenInterface* ri = (cjRenInterface*)viewport->m_ri;
		ri->Clear(cjRenInterface::ECLEAR_ALL, m_backGroundColor);

//		ri->DrawTest();
        
        cjFLOAT width = (cjFLOAT)viewport->GetWidth();
		cjFLOAT height = (cjFLOAT)viewport->GetHeight();

		cjFLOAT aspect = 4.0f / 3.0f; // cjAbs(width / height);


		cjMatrix matProj;
		cjMatrix matView;
		cjMatrix matWorld;

		
		//cjVec3 camPos		= cjVec3(0,0, 100);
		//cjVec3 camLookAt	= cjVec3(0,0,0);
		//cjVec3 camUp		= cjVec3(0,1,0);
				
		//cjVec3 camPos		= cjVec3(0,-250,20);
		
		cjVec3 camPos		= cjVec3(0,0, 100);
		cjVec3 camLookAt	= cjVec3(0,0,0);		
		cjVec3 camUp		= cjVec3(0,1,0);            // GLCoord:   right: x+    up:y+   dir: z+  ,   so,  cam dir (z-)  cam up(y+)
		
		//cjVec3 camPos		= cjVec3(-1900, 0, 0);
		//cjVec3 camLookAt	= cjVec3(0,0,0);
		//cjVec3 camUp		= cjVec3(0,0,1);


		//cjMatrix matProjRHMe;
		//cjMatPerspectiveFovRH(matProjRHMe, cjDegree2Rad(45.0f), 1.0f, 1.0f, 100.0f );

		//cjMatrix matProjRHMe2;
		//cjMatPerspectiveFovRH(matProjRHMe2, cjDegree2Rad(45.0f), 1.0f, 1.0f, 100.0f );


//		cjMatPerspectiveFovRH(matProj, cjDegree2Rad(45.0f), aspect, 0.01f, 10000.0f);
		cjMatPerspectiveFovRH(matProj, cjDegree2Rad(45.0f), 1.0f, 0.1f, 10000.0f );
		
		matWorld.MakeRotation(g_rotation, 0, 0, 1.0f);
		matWorld.SetIdentity();

		//camPos = matWorld * camPos;
				
		//cjtemp
//		cjMatViewLookAtRH(matView, cjVec3(0,0,-50), cjVec3(0,0,0), cjVec3(0,1,0));
		//cjMatViewLookAtRH(matView, cjVec3(0,-100, 150), cjVec3(0,0,0), cjVec3(0,1,0));

//		cjMatViewLookAtRH(matView, cjVec3(0,-2310, 1150), cjVec3(0,0,1130), cjVec3(0,1,0));

		const cjMatrix* viewMat = viewport->GetViewMatrix();


		//cjMatViewLookAtRH(matView, camPos, camLookAt, camUp);
						
		//matWorld.MakeScale(1, 2, 1);

  //         
  //      cjMatrix matProj;
  //      cjGLMatrixPerspective(&matProj, cjDegree2Rad(45.0f), aspect, 0.1f, 1000.0f);
  //  
  //      cjMatrix matTrans; matTrans.MakeTranslation(0.0f, 0.0f, 0.0f);
  //      cjMatrix baseModelViewMatrix;

		//baseModelViewMatrix.MakeRotation(g_rotation, 0.0f, 1.0f, 0.0f);
		//baseModelViewMatrix = baseModelViewMatrix * matTrans;
  //  
  //      cjMatrix matModelView, matModelRot;

		//matModelView.MakeTranslation(0,0,-20);
		//matModelRot.MakeRotation(g_rotation, 1.0f, 1.0f, 1.0f);
		//matModelView = matModelView * baseModelViewMatrix;

		//g_matModelView = matModelView;

  //      cjMatrix invMat; matModelView.Inverse(invMat);
		//invMat.Transpose();
		//invMat.CopyMatrix3(g_normalMatrix);

		//g_modelViewProjectionMatrix = matModelView * matProj;
        
		ri->SetWorld();
		ri->SetView(viewMat);
		ri->SetProj(&matProj);

/*		ri->m_matWorld.SetIdentity();
        ri->m_matProj = matView * matProj;
        ri->m_matNormal = cjMatrix3::Identity;
*/

    
        cjFLOAT fv = cjGetEngineTimeElapsedSec();
        g_rotation += fv;
    
        if(g_rotation > CJ_PI *2.0)
        {
            while(g_rotation > (CJ_PI * 2.0))
                g_rotation -= (CJ_PI*2.0);
        }
    
        g_timeSinceLastUpdate += fv;
        
        //g_rotation = -CJ_PI/2.0f;
        
        
        //-------------------------------------------------
        
        static cjFLOAT vtmp = 0.0f;
        vtmp += cjGetEngineTimeElapsedSec();
    
        if(vtmp > 1.0f)
            vtmp = vtmp - ((cjUINT32)vtmp);
 
		//if(gs_nif)
		//	gs_nif->Draw(ri);

		//if(gs_axis)
		//{
		//	foWinViewport* fwv = (foWinViewport*)viewport;

		//	gs_axis->SetAxis(0, fwv->m_camera->GetRight()); //R
		//	gs_axis->SetAxis(1, fwv->m_camera->GetUp()); //U
		//	gs_axis->SetAxis(2, fwv->m_camera->GetDir()); //D

		//	//static fnVec3 old = fwv->m_camRot;
		//	//
		//	//if(old != fwv->m_camRot)
		//	//{
		//	//	fwv->m_camRot.x, fwv->m_camRot.y, fwv->m_camRot.z,
		//	//	fwv->m_camAxis[0].x,fwv->m_camAxis[0].y,fwv->m_camAxis[0].z,
		//	//	fwv->m_camAxis[1].x,fwv->m_camAxis[1].y,fwv->m_camAxis[1].z,
		//	//	fwv->m_camAxis[2].x,fwv->m_camAxis[2].y,fwv->m_camAxis[2].z);

		//	//	old = fwv->m_camRot;
		//	//}

		//	gs_axis->Draw(ri, 50, 50);
		//}

		//cjtemp

		cjFLOAT ti = 0.0f;// g_timeSinceLastUpdate;
		ti = g_timeSinceLastUpdate;

		m_cjTestAnimator->Update(ti);

		cjLODMesh* lodMesh = (cjLODMesh*)m_cjTestMeshInst->GetMesh();
		cjSceneNode* sceneRoot = lodMesh->GetSceneRoot();

		sceneRoot->Update(ti);
//		sceneRoot->Update(g_cjtmp_time);
//		sceneRoot->UpdateBySelectFlag(g_timeSinceLastUpdate);
//		sceneRoot->UpdateBySelectFlag(g_cjtmp_time);
		
		m_cjTestMeshInst->Render(ri);

		static cjMeshInstance* axisMeshInst = cjNULL;

		if(!axisMeshInst)
			axisMeshInst = m_axisMesh->NewMeshInstance();
				
		axisMeshInst->Render(ri);

		viewport->RenderUnLock();
		viewport->m_pendingPresentFrame = 1;
	}
}

cjBool cjAppEngine::Init()
{
	cjSuper::Init();

	m_init = 1;
    
    cjInitParam initParam;
    cjGetInitParam(initParam);

	// coinit
	// dxinput with hinstance
	// m_client = new(this) cjAppClient;
	cjObjCls* clsClient = cjObjLoadCls<cjClient>(_CU("cjEngine.cjAppClient"));
	m_client = cjObjNew<cjClient>(clsClient);
	m_client->Init(this);

	cjViewport* viewport = m_client->CreateViewport(cjViewport::EVF_PrimaryView, 
		_CU("cjAppEngine 1.0.100 - cjsong 2013") ,
#if defined(CJ_PLATFORM_WIN32)
//             0, 0, 1144, 667,  NULL);  // actually 1136x640 (iphone 5s screen size)
             0, 0, 800, 600,  NULL);  // actually 1136x640 (iphone 5s screen size)
#elif defined(CJ_PLATFORM_APPLE)
            0, 0, -1, -1, initParam.m_eaglView);
#else
             0, 0, -1, -1, NULL);
#endif

	if(viewport)
	{
		cjViewport::SetRenDevFocusWnd(viewport->GetWnd());

		cjRenDevParam renDevPrm;

		renDevPrm.m_focusWnd = cjViewport::GetRenDevFocusWnd();
		renDevPrm.m_renderWnd = viewport->GetWnd();

#if defined(CJ_PLATFORM_WIN32)
		renDevPrm.m_renderWndWidth = 1024;
		renDevPrm.m_renderWndHeight = 768;
        
#elif defined(CJ_PLATFORM_APPLE)
		renDevPrm.m_renderWndWidth = 0;
		renDevPrm.m_renderWndHeight = 0;
        renDevPrm.m_eaglAdapter = (cjEAGLAdapter*)initParam.m_eaglAdapter;
#else
		renDevPrm.m_renderWndWidth = 1024;
		renDevPrm.m_renderWndHeight = 768;
#endif

		cjBool rstRenDev = cjFalse;
		rstRenDev = CreateRenDev(&renDevPrm);

		// init audio
		//viewport->m_input->Init();

		if(rstRenDev)
			viewport->AttachRenderDevice(GetRenDev());
	}
	else
	{
		return cjFalse;
	}       

	//cjtemp
	//if(1)
	//{
	
	cjResChr* chr = (cjResChr*)cjObj::cjObjFind(cjResChr::cjObjGetStaticCls(), 0, _CT("test_female.female00"));
//	cjResChr* chr = (cjResChr*)cjObj::cjObjFind(cjResChr::cjObjGetStaticCls(), 0, _CT("TestSol.TestSol"));
//	cjResChr* chr = (cjResChr*)cjObj::cjObjFind(cjResChr::cjObjGetStaticCls(), 0, _CT("female.testaaa"));
//	cjResChr* chr = (cjResChr*)cjObj::cjObjFind(cjResChr::cjObjGetStaticCls(), 0, _CT("TestResChar.Cat"));
//	cjResChr* chr = (cjResChr*)cjObj::cjObjFind(cjResChr::cjObjGetStaticCls(), 0, _CT("TestArmy.Soldier"));

	m_cjTestMesh		= chr->m_model;
	m_cjTestMeshInst	= m_cjTestMesh->NewMeshInstance();
	m_cjTestAnimator	= chr->m_animator;

	m_axisMesh			= BuildAxisMesh();


	//cjtemp settarget
	m_cjTestAnimator->SetTargetSeqID(1);

	cjSceneNode* _sn = m_cjTestAnimator->GetSceneRoot()->GetSceneNode();

	_sn = _sn->GetFirChild()->GetSceneNode();

//	_sn->SetPos(0, 3.0597, -0.7536);

//	_sn = _sn->GetSibilSceneNode();

	cjTransform _ltm;

//	_ltm.m_pos = cjVec3(0, 0, 0);
//	_ltm.m_scale = cjVec3(1, 1, 1);

	//float _zz(*)[3] = _ltm.m_rotate.m;

	//_ltm.m_rotate.SetRow(0, cjVec3(0.431771, 0.847398, -0.309017));
	//_ltm.m_rotate.SetRow(1, cjVec3(0.140291, 0.275336, 0.951057 ));
	//_ltm.m_rotate.SetRow(2, cjVec3(0.891007, -0.453991, -0.000000 ));

	//_sn->SetLocalTM(_ltm);

	//}
	
	return cjTrue;
}

cjBool cjAppEngine::CreateRenDev(cjRenDevParam* renDevPrm)
{
	if(!renDevPrm || m_renDev)
		return cjFalse;

	cjObjCls* clsGLRenDev = NULL;

	clsGLRenDev = cjObjLoadCls<cjRenDev>(_CU("cjEngine.cjGLRenDev"));
	cjAssert(clsGLRenDev);

	m_renDev = cjObjNew<cjRenDev>(clsGLRenDev);

	if(!m_renDev->Init(renDevPrm))
	{
		delete m_renDev;
		m_renDev = NULL;

		return cjFalse;
	}

	return cjTrue;
}

cjBool cjAppEngine::CreateAudioDev()
{
	return cjTrue;
}

void cjAppEngine::OnDestroy()
{
	m_cjTestAnimator = cjNULL;

	//cjtemp

	if(m_cjTestMeshInst)
	{
		delete m_cjTestMeshInst;
		m_cjTestMeshInst = NULL;
	}

	if(m_cjTestMesh)
	{
		delete m_cjTestMesh;
		m_cjTestMesh = NULL;
	}

	if(m_client)
	{
		delete m_client;
		m_client = NULL;
	}

	if(m_renDev)
	{
		delete m_renDev;
		m_renDev = NULL;
	}

	if(m_renDevParam)
	{
		delete m_renDevParam;
		m_renDevParam = NULL;
	}

	ms_appEngine = NULL;

	cjSuper::OnDestroy();
}


cjBool cjAppEngine::OnCharKey(cjViewport* viewport, cjInputKey key)
{
//	cjWinViewport* fwv = (cjWinViewport*)viewport;
//
//	switch(key)
//	{
//	case CJIK_Escape:
//		{
//			viewport->ToggleMouseShow();
////			else
////				SetExit(fnTrue);
//		}
//		break;
//
//	case CJIK_W:  
//		{
//			fwv->DoCameraMoveDelta(0.1f);
//		//	fwv->DoCameraRotateDelta(0.01f, 0,0); 			
//		}
//		break;
//
//		//cjtemp
//	case CJIK_F4:
//	case CJIK_S:
//		{
//			g_cjtmp_time = 0.0;
//			fwv->DoCameraMoveDelta(-0.1f);
//		//	fwv->DoCameraRotateDelta(-0.01f, 0,0); 
//		}
//		break;
//
//	case CJIK_NumPad1:
//	case CJIK_A:
//		{
//			//cjtemp
//			g_cjtmp_time = g_cjtmp_time - 0.01;
//			fwv->DoCameraMoveDelta(0, 0.1f);
//
//		//	fwv->DoCameraRotateDelta(0, 0.01f, 0); 
//		}
//		break;
//
//	case CJIK_NumPad4:
//	case CJIK_D:
//		{
//			//cjtemp
//			g_cjtmp_time = g_cjtmp_time + 0.01;
//			fwv->DoCameraMoveDelta(0, -0.1f);
//			
//		// fwv->DoCameraRotateDelta(0,-0.01f, 0); 
//		}
//		break;
//
//	//case CJIK_Q:  fwv->Exec(_FT("ResetTestCam")); break;
//
//	default:
//		break;
//	}

	return cjFalse;
}

cjBool cjAppEngine::GenerateInputEvent(cjViewport* viewport, cjINT32 key, 
	cjInputAction action, cjFLOAT delta) 
{ 
	cjWinViewport* fwv = (cjWinViewport*)viewport;

	if(key == CJIK_Ctrl)
		return cjFalse;

	switch(key)
	{
	case CJIK_Escape:
		{
			viewport->ToggleMouseShow();
//			else
//				SetExit(fnTrue);
		}
		break;

	case CJIK_F4:
		{
			fwv->DoCameraReset();
		}
		break;

	case CJIK_T: // Forward
		{
			fwv->DoCameraMove(10.0f, 0.0f, 0.0f);
		}
		break;

	case CJIK_G: // Backward
		{
			fwv->DoCameraMove(-10.0f, 0.0f, 0.0f);
		}
		break;


	case CJIK_F: // Left
		{
			fwv->DoCameraMove(0,10.0f,0.0f);
		}
		break;

	case CJIK_H: // Right
		{
			fwv->DoCameraMove(0,-10.0f,0.0f);
		}
		break;


	case CJIK_R: // Up+
		{
			fwv->DoCameraMove(0,0,10.0f);
		}
		break;


	case CJIK_Y: // Up-
		{
			fwv->DoCameraMove(0,0,-10.0f);
		}
		break;

	case CJIK_W: // pitch+
		{
			fwv->DoCameraRotate(0, 0.01f, 0);
		}
		break;

	case CJIK_S: // pitch-
		{
			fwv->DoCameraRotate(0, -0.01f, 0);
		}
		break;

	case CJIK_Q:  // roll+
		{
			fwv->DoCameraRotate(0, 0, 0.01f);
		}
		break;

	case CJIK_E: // roll-
		{
			g_cjtmp_time = 0.0;
			fwv->DoCameraRotate(0, 0, -0.01f); 
		}
		break;

	case CJIK_A: // yaw+
		{
			fwv->DoCameraRotate(0.01f, 0, 0); 
		}
		break;

	case CJIK_D:  // yaw-
		{
			fwv->DoCameraRotate(-0.01f, 0, 0);
		}
		break;

	//case CJIK_Q:  fwv->Exec(_FT("ResetTestCam")); break;

	default:
		break;
	}

	return cjFalse;
}

const cjNameChar* cjAppEngine::GetAxisMeshName(cjBool nameOnly)
{
	if(nameOnly)
		return _CT("DefaultAxisMesh");
	else
		return _CT("cjEngine.DefaultAxisMesh");
}

cjMesh* cjAppEngine::BuildAxisMesh()
{
	cjObjPackage* pkgEngine = cjObj::cjObjCreatePkg(cjNULL, cjEngine_PkgName);
	cjLODMesh* axisMesh = (cjLODMesh*)cjObjFind(cjLODMesh::cjObjGetStaticCls(), pkgEngine, GetAxisMeshName());

	if(!axisMesh)
	{
		axisMesh = cjObjNew<cjLODMesh>(cjLODMesh::cjObjGetStaticCls(), pkgEngine, GetAxisMeshName(cjTrue));

		cjFVertexDecl vtxDecl;
		vtxDecl.MakeDecl(2, 
						 0, cjVtxComp_Position, cjVtxCompSiz_FLOAT3,	
						 0, cjVtxComp_Diffuse,  cjVtxCompSiz_FLOAT4);
		
		cjINT32 stride = vtxDecl.GetStride();


		static const cjINT32 numVtx = 6;

		struct vtx
		{
			cjFLOAT  x,y,z;
			cjFLOAT  r,g,b,a;
		};

		static const vtx rawVtxSrc[numVtx] =
		{
			{ 0,0,0,				0.0f, 0.0f, 0.0f, 1.0f },
			{ 100,0,0,				1.0f, 0.0f, 0.0f, 1.0f },

			{ 0,0,0,				0.0f, 0.0f, 0.0f, 1.0f },
			{ 0,100, 0,				0.0f, 1.0f, 0.0f, 1.0f },

			{ 0,0,0,				0.0f, 0.0f, 0.0f, 1.0f },
			{ 0,0,100,				0.0f, 0.0f, 1.0f, 1.0f },
		};

		axisMesh->m_numVertexStream = 1;
		axisMesh->m_vertexStreams = new cjFVertexStream*[axisMesh->m_numVertexStream];
		axisMesh->m_vertexStreams[0] = new cjFStaticVertexStream(numVtx, &vtxDecl, (void*)rawVtxSrc);
				
		axisMesh->m_numIndexBuffers = 1;
		axisMesh->m_indexBuffers = new cjFIndexBuffer*[axisMesh->m_numIndexBuffers];
		axisMesh->m_indexBuffers[0] = new cjFIndexBuffer;
		cjFIndexBuffer* indexBuffer = axisMesh->m_indexBuffers[0];
		indexBuffer->SetIndexInterval(2);
				
		const static cjWORD indexBuf[] = 
		{
			0,1,
			2,3,
			4,5
		};

		cjINT32 numIndex = sizeof(indexBuf) / sizeof(indexBuf[0]);
		indexBuffer->SetIndexDat(numIndex, (void*)indexBuf);

		cjSceneMesh* sceneMesh = new cjSceneMesh();

		sceneMesh->SetOwnerMesh(axisMesh);
		sceneMesh->SetVertexStream(axisMesh->m_vertexStreams[0]);
		sceneMesh->SetIndexBuffer(indexBuffer);		
		sceneMesh->SetPrimitiveType(cjPT_LineList);
				
		axisMesh->m_sceneMeshs.Push(sceneMesh);
	}

	return axisMesh;
}