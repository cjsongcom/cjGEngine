
#ifndef _CJIOSVIEWPORT_H_
#define _CJIOSVIEWPORT_H_

class cjEAGLAdapter;


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjIOSViewport, cjViewport, cjClsFL_Transient, cjEngine)

	friend class cjGLRenInterface;
    
public:
	virtual cjBool Init(const cjNameChar* title, cjINT32 , cjINT32 , cjINT32 , cjINT32 , void* prm=NULL);
	virtual cjBool Init(void* givenWnd, const cjNameChar* title, void* prm);

	virtual void Tick();
    
	// input
	virtual void ReadInput(cjFLOAT elapsedTime=0.0f);
	virtual void UpdateInputData(cjBool reset, cjFLOAT elapsedTime=0.0f);
    
	virtual cjBool IsMouseCapture() { return cjFalse; }
	virtual void SetMouseCapture(cjBool doCapture, cjBool clipMousePos,
                                 cjBool focusOnly=cjFalse) { };
   
	// for processing viewport console command
	virtual cjBool Exec(const cjNameChar* cmd) { return cjFalse; }
	virtual cjBool SetDrag(cjBool newDragging) { return cjFalse; }
    
	virtual cjBool IsFullScreen() { return cjTrue; } // always
	virtual cjBool ToogleFullScreen() { return cjFalse; }
	virtual void EndFullScreen() {}
    
	virtual cjBool ResizeViewport(cjBool /*fullScreen*/ , cjINT32 width, cjINT32 height);
	virtual void SetPendingFrame(cjBool pending);
    
	// rendering
	virtual void Paint();
    
	virtual cjBool RenderLock();
	virtual void RenderUnLock();
    
	virtual void Present();
   
	virtual cjHWND GetWnd();
	virtual cjBool AttachRenderDevice(cjRenDev* renDev);
    virtual void DetachRenderDevice();
	virtual void GetClientSiz(cjINT32& width, cjINT32& height);
    
public:
	cjIOSViewport();
    
protected:
    virtual void OnDestroy();
    
protected:
    cjEAGLAdapter*                          m_eaglAdapter;
    void*                                           m_iosEAGLView; // cjIOSEAGLView
};


#endif
