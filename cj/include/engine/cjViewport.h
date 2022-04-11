
#ifndef _CJVIEWPORT_H_
#define _CJVIEWPORT_H_


class cjRenDev;
cjRefPtrForwardDefine(cjCamera)


CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjViewport, cjObj, cjClsFL_Transient, cjEngine)

	enum EViewportFlag
	{
		EVF_Opened				= 0x10000000,
		EVF_PrimaryView			= 0x00000001,
		EVF_SwapChain			= 0x00000002,

		EVF_ToolViewport		= 0x00000100,
		EVF_Managed				= 0x00000200,
	};

	cjViewport();

	virtual cjBool Init(const cjNameChar* title, cjINT32 left, cjINT32 top, 
		cjINT32 width, cjINT32 height, void* prm=NULL) = 0;
	virtual cjBool Init(void* givenWnd, const cjNameChar* title, void* prm) = 0;

	virtual void Tick() = 0;

	// input
	virtual void ReadInput(cjFLOAT elapsedTime=0.0f);
	virtual void UpdateInputData(cjBool reset, cjFLOAT elapsedTime=0.0f) = 0;
	virtual void UpdateMousePosition() {}

	virtual cjBool IsMouseCapture() = 0;
	virtual void SetMouseCapture(cjBool doCapture, cjBool clipMousePos, 
		cjBool focusOnly=cjFalse) = 0;

	// for processing viewport console command
	virtual cjBool Exec(const cjNameChar* cmd) { return cjFalse; }

	virtual void ToggleMouseShow() {}

	virtual cjBool SetDrag(cjBool newDragging) { return cjFalse; }

	virtual cjBool IsFullScreen() = 0;
	virtual cjBool ToogleFullScreen() { return cjFalse; }
	virtual void EndFullScreen() {}

	virtual cjBool ResizeViewport(cjBool fullscreen, cjINT32 width, cjINT32 height) = 0;
	virtual void SetPendingFrame(cjBool pending) = 0;

	// rendering
	virtual void Paint() = 0;

	virtual cjBool RenderLock();
	virtual void RenderUnLock();

	virtual void Present();
	static inline cjViewport* GetPrimaryViewport() 
		{ return ms_primaryViewport; }

	inline cjTFLAG GetViewportFlag() { return m_viewportFlag; }
	inline void SetViewportFlag(cjTFLAG flag, cjBool absolute=0) 
		{ absolute ? m_viewportFlag = flag : m_viewportFlag |= flag; }

//	virtual void ToggleFullScreen();
	virtual cjHWND GetWnd() = 0;

	inline cjINT32 GetLeft() { return m_left; }
	inline cjINT32 GetTop() { return m_top; }
	inline cjINT32 GetWidth() { return m_width; } 
	inline cjINT32 GetHeight() { return m_height; }

	static void* GetRenDevFocusWnd() { return ms_renDevFocusWnd; }
	static void SetRenDevFocusWnd(void* focusWnd) { ms_renDevFocusWnd = focusWnd; }

	inline void* GetParam() { return m_prm; }

	static cjViewport* CreateInstance(cjTFLAG flag, cjObj* outer);

	virtual cjBool AttachRenderDevice(cjRenDev* renDev) = 0;

//	virtual void SetOutputRenderTargetGrp(cjRenderTargetGrp* output);
//	virtual cjRenderTargetGrp* GetOutputRenderTargetGrp();

	virtual void GetClientSiz(cjINT32& width, cjINT32& height) = 0;

	// callling from cjWnd
	virtual cjBool OnQueryWndClosing() { return cjTrue; }
	virtual void OnWndClose() {}

	inline void SetClient(cjClient* client) { m_client = client; }
	inline cjClient* GetClient() { return m_client; }

	// camera test
	cjCamera* GetCamera() { return m_camera; }

	void DoCameraRotate(cjFLOAT yaw=0.0f, cjFLOAT pitch=0.0f, cjFLOAT roll=0.0f, cjBool isDelta=cjTrue);
	void DoCameraMove(cjFLOAT dir=0.0f, cjFLOAT right=0.0f, cjFLOAT up=0.0f, cjBool isDelta=cjTrue);
	void DoCameraDist(cjFLOAT distDelta);
	void DoCameraReset();
    
    virtual cjEngineTime_ft GetCurTime() { return cjEngineTime_ft(0); }

	const cjMatrix* GetViewMatrix();

protected:
	virtual void OnDestroy();

	static inline void SetPrimaryViewport(cjViewport* viewport)
		{ ms_primaryViewport = viewport; }

protected:
	static cjUINT32						ms_numViewports;
	static cjViewport*					ms_primaryViewport; // associated with renderer
	static void*						ms_renDevFocusWnd;
	
	cjTFLAG								m_viewportFlag;

	cjBool								m_mouseShow;
	cjBool								m_mouseDragging;

	cjPoint<cjINT32>					m_mouseScreenPos;
	cjPoint<cjINT32>					m_mouseClientPos;	

	cjBool								m_isDestroying;
	cjBool								m_wasFullscreen;

public:
	// system caps
	cjBool								m_support32bitColor;	// 16bit/32bitcolor
	cjBool								m_fullscreenOnly;

	cjINT32								m_left;
	cjINT32								m_top;
	cjINT32								m_width;
	cjINT32								m_height;

	void*								m_prm;					// can be null

	class cjInput*						m_input;	// input processor
	class cjRenDev*						m_renDev;	// ref
	class cjClient*						m_client;   // ref

	cjBool								m_useOpenGL;

	// Begin/EndRender stuff(assigned or updated when called BeginRender)
	cjBool								m_pendingPresentFrame;
	cjUINT32							m_totalRenFrameCnt;		// frame
	cjDOUBLE							m_beginRenderTime;
	class cjRenInterface*				m_ri;		// ref
	cjDOUBLE							m_lastPresentTime;

//	class cjSceneNode*					m_scene;

	// output
//	class cjRenderTargetGrp*			m_output;   // ref

private:
	cjCameraRPtr						m_camera;

	cjVec3								m_camDefaultEye;
	cjVec3								m_camDefaultLookAt;
	cjFLOAT								m_camDefaultFovY;
	cjFLOAT								m_camDefaultAspect;


CJOBJ_DECL_ABSTRACT_END(cjViewport)


#endif
