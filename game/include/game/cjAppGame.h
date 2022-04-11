
#ifndef _CJAPPGAME_H_
#define _CJAPPGAME_H_


CJOBJ_DECL_CLASS_BEGIN(CJGAME_API, cjAppGame, cjGame, cjClsFL_Transient, cjGame)

public:
	// cjgame
	virtual cjBool Init();
    
	virtual ERUN Tick();
    virtual void Draw();

	virtual void* GetGameParam() { return NULL; }
	virtual cjEngine* GetEngine() { return m_engine; }

	virtual void PumpMessage();

public:
	cjAppGame();

protected:
	virtual void OnDestroy();

	virtual cjBool OnCreateEngine();
	virtual void OnDestroyEngine();

private:
	cjEngine*							m_engine;

CJOBJ_DECL_CLASS_END(cjAppGame)



#endif
