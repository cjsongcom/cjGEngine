
#ifndef _CJWINWND_H_
#define _CJWINWND_H_


//
// Win32 Window & Dialog
//

class CJCORE_API cjWinWnd : public cjMem
{
public:
	cjWinWnd(cjWinWnd* parent=NULL, cjHINSTANCE hInstance=0);
	virtual ~cjWinWnd();

	inline cjBool IsUnicodeWnd() const { return m_isUniWnd; }

	virtual void MoveWindow(cjINT32 left, cjINT32 top, cjINT32 w, cjINT32 h, cjBool repaint);
	virtual void Show(cjBool show=cjTrue);

	static cjBool RegisterWindowClass(const cjChar* className, cjTFLAG style, 
		cjHINSTANCE hinstance, cjINT32 icon=0, cjINT32 iconsm=0);

	static cjBool RegisterWindowClass(const cjUni* className, cjTFLAG style, 
		cjHINSTANCE hinstance, cjINT32 icon=0, cjINT32 iconsm=0);

	static cjHINSTANCE GetDefaultHInstance();
	operator HWND() const { return m_hwnd; }

	cjBool IsValid();

	virtual void GetClientSiz(cjINT32& width, cjINT32& height);
	virtual void GetPos(cjINT32& left, cjINT32& top);

protected:
	//cjugly
	cjBool CreateWndA(cjTFLAG exStyle, const cjChar* clsName, const cjChar* wndName,
		cjTFLAG style, cjINT32 x, cjINT32 y, cjINT32 w, cjINT32 h, HWND wndParent, 
		HMENU menu=NULL, cjHINSTANCE hInstance=GetDefaultHInstance());

	cjBool CreateWndW(cjTFLAG exStyle, const cjUni* clsName, const cjUni* wndName,
		cjTFLAG style, cjINT32 x, cjINT32 y, cjINT32 w, cjINT32 h, HWND wndParent, 
		HMENU menu=NULL, cjHINSTANCE hInstance=GetDefaultHInstance());

//	static LRESULT CALLBACK _WndProcA(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _WndProcW(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, LPARAM lParam);

//	static cjBool RegWndClass(const cjChar* name, cjTFLAG style);

	void ConditionalDestroy();

	virtual cjBool OnQueryClosing() { return cjTrue; }
	virtual cjBool WndProc(HWND hWnd, cjUINT32 uiMsg, WPARAM wParam, LPARAM lParam, LRESULT* rst);
	virtual void OnDestroy();


protected:
	HWND								m_hwnd;
	cjWinWnd*							m_parent;
	cjHINSTANCE							m_hInstance;
	cjBool								m_isUniWnd; // is unicode window

	cjINT32								m_icon;
	cjINT32								m_iconsm;

	cjBool								m_isDestroying;	
	cjBool								m_sendDestroyWnd;

	static cjUINT32						ms_numWindow;
};


//
// cjWinDialog
//

class CJCORE_API cjWinDailog : public cjMem
{
public:
	cjWinDailog(cjHWND parentWnd, cjWORD winResID, cjHINSTANCE hInstance=0);
	virtual ~cjWinDailog();
	void ConditionalDestroy();

	virtual INT_PTR DoModal();

	void MoveWindow(cjINT32 left, cjINT32 top, cjINT32 w, cjINT32 h, cjBool repaint);
	void Show(cjBool show=cjTrue);
	void GetClientSiz(cjINT32& width, cjINT32& height);
	void GetPos(cjINT32& left, cjINT32& top);

	cjHWND GetHWnd() { return m_hwnd; }

protected:
	virtual void OnDestroy();

	virtual cjBool OnQueryClosing() { return cjTrue; }
	virtual cjBool DlgProc(cjHWND /*hwnd*/, cjUINT32 /*msg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) { return cjFalse; }

private:
	static INT_PTR CALLBACK _DlgProc(cjHWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual cjBool InitDialog(cjHWND hwnd);
	virtual cjBool OnCommand(cjHWND hwnd, WPARAM wParam, LPARAM lParam) = 0;

private:
	cjHINSTANCE										m_moduleInstance;
	cjHWND											m_parentWnd;
	cjHWND											m_hwnd;
	cjWORD											m_dlgResID;
	cjBool											m_isDestroying;	
	cjBool											m_sendDestroyWnd;
};


#endif
