
#include "cjGamePCH.h"
#include "cjAppGame.h"

CJOBJ_IMPL_CLASS(cjAppGame)

cjAppGame::cjAppGame()
	: cjGame(EGAMEOPER_GAME)
{
	m_engine = NULL;
}

void cjAppGame::OnDestroy()
{
	OnDestroyEngine();	

	cjSuper::OnDestroy();
}

cjBool cjAppGame::Init()
{
	if(!OnCreateEngine())
		return cjFalse;

		
	return cjTrue;
}

cjGame::ERUN cjAppGame::Tick()
{
    cjUpdateEngineTime();

	if(!m_engine || IsForceExiting())
		return ERUN_EXIT;

	// clear statitics
	
	// time update

	// update the world
		
	// engine tick
	int tickRst = 0;

	if(m_engine)
		tickRst = m_engine->Tick(0.001f);

	if(tickRst == cjEngine::ETICKRST_EXIT)
		SetForceExiting(cjTrue);

	// windows tick

	//TickCount++;

	// pump window message
	PumpMessage();

	return ERUN_OK;	
}

void cjAppGame::Draw()
{
	if(!m_engine || IsForceExiting())
		return;
    
    cjViewport* priViewport = cjViewport::GetPrimaryViewport();
    m_engine->Draw(priViewport);
}

void cjAppGame::PumpMessage()
{
#ifdef CJ_PLATFORM_WIN32
	MSG msg;
	
	while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT)
			SetForceExiting(cjTrue);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#else

#endif
}

cjBool cjAppGame::OnCreateEngine()
{
    cjResetEngineTime();

	cjObjCls* clsEngine = cjObjLoadCls<cjEngine>(_CU("cjEngine.cjAppEngine"));
	cjEngine* engine = cjObjNew<cjEngine>(clsEngine);

	if(!engine->Init())
	{
		delete engine;
		return cjFalse;
	}

	m_engine = engine;

	return cjTrue;
}

void cjAppGame::OnDestroyEngine()
{
	CJ_SAFE_DELETE(m_engine);
}

