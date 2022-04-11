
#ifndef _CJCLIENT_H_
#define _CJCLIENT_H_


CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjClient, cjObj, 0, cjEngine)

public:	
	cjClient();

	virtual cjBool Init(cjEngine* engine) = 0;
	virtual void Tick() = 0;
    virtual void Draw() = 0;
	virtual cjBool Exec(const cjNameChar* cmd) = 0;
	virtual void FlushCache() = 0;

	virtual cjViewport* CreateViewport(cjTFLAG vpFlag, const cjNameChar* name,
		cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, void* prm=NULL) = 0;

	virtual void OnDestroyViewport(cjViewport* viewport) = 0;
	inline cjEngine* GetEngine() { return m_engine; }

	cjINT32 GetFullscreenViewportWidth() { return m_fullScreenW; }
	cjINT32 GetFullscreenViewportHeight() { return m_fullScreenH; }
	
protected:
	virtual void OnDestroy();

protected:
	cjEngine*							m_engine;
	cjTAry<cjViewport*>					m_viewports;

	cjBitField							m_noLight			:1;
	cjBitField							m_fullScreen		:1;
	
	cjFLOAT								m_gamma;
	cjFLOAT								m_hudScale;
	cjViewport*							m_currentViewport;
	cjFLOAT								m_drawDistLOD;

public:
	cjINT32								m_windowedViewportW;
	cjINT32								m_windowedViewportH;
	cjINT32								m_fullScreenW;
	cjINT32								m_fullScreenH;
    
    //cj test
    cjFile*                             m_cjtestFile;
	
CJOBJ_DECL_ABSTRACT_END(cjClient)

#endif
