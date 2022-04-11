
#ifndef _CJSHADERSCRIPT_H_
#define _CJSHADERSCRIPT_H_


class SSC_Buf;
class SSC_Fn; 
class SSC_V;
class SSC_ShVarDecl;
class SSC_Oper;
class SSC_Let;



enum SSC_TOKEN
{
	SSCT_NONE,

	SSCT_attribute,				// 'attribute'
	
	SSCT_vec4,					// 'vec4'
	SSCT_vec3,
	SSCT_vec2,
	SSCT_vec1,

	SSCT_mat4,
	SSCT_mat3,
	SSCT_mat2,
	SSCT_mat1,

	SSCT_varying,				// 'varying'

	SSCT_const,
	SSCT_int,
	SSCT_uniform,

	SSCT_v_color,	
	SSCT_gl_Position,
	SSCT_a_color,
	SSCT_a_texcoord0,

	SSCT_matMVP,

	SSCT_sampler2D,
	
	SSCT_v_texcoord0,
	SSCT_v_texcoord1,
	SSCT_v_texcoord2,
	SSCT_v_texcoord3,
	SSCT_v_texcoord4,
	SSCT_v_texcoord5,
	SSCT_v_texcoord6,
	SSCT_v_texcoord7,

	SSCT_s_texsamp0,
	SSCT_s_texsamp1,
	SSCT_s_texsamp2,
	SSCT_s_texsamp3,
	SSCT_s_texsamp4,
	SSCT_s_texsamp5,
	SSCT_s_texsamp6,
	SSCT_s_texsamp7,
	
	SSCT__MAX
};

enum SSCV_FLAG
{
	SSCV_FL_RET					= 0x00000001,
	SSCV_FL_SCRIPT				= 0x00000002,

	SSCV_FL_PRM_IN				= 0x00000010,
	SSCV_FL_PRM_OUT				= 0x00000020,
	SSCV_FL_PRM					= SSCV_FL_PRM_IN | SSCV_FL_PRM_OUT,

	SSCV_FL_SHATTR				= 0x00000100,

	SSCV_FL_LOCAL				= 0x00001000,

	SSCV_FL_VSH_OUT				= 0x00010000,
	SSCV_FL_PSH_OUT				= 0x00020000,

	SSCV_FL_UINT32				= 0xffffffff,
};


enum SSCV_SIWZ
{
	SSCV_SIWZ_NONE			= 0x00000000, // xyzw

	// print as rgba
	SSCV_SIWZ_PF_RGBA		= 0x10000000, // xyzw -> rgba

	// vec4.000x
	SSCV_SIWZ_x0			= 0x00000001,
	SSCV_SIWZ_y0			= 0x00000002,
	SSCV_SIWZ_z0			= 0x00000004,
	SSCV_SIWZ_w0			= 0x00000008,
	SSCV_SIWZ_F0 = SSCV_SIWZ_x0 | SSCV_SIWZ_y0 | SSCV_SIWZ_z0 | SSCV_SIWZ_w0,

	// vec4.00x0
	SSCV_SIWZ_x1			= 0x00000010,
	SSCV_SIWZ_y1			= 0x00000020,
	SSCV_SIWZ_z1			= 0x00000040,
	SSCV_SIWZ_w1			= 0x00000080,
	SSCV_SIWZ_F1 = SSCV_SIWZ_x1 | SSCV_SIWZ_y1 | SSCV_SIWZ_z1 | SSCV_SIWZ_w1,

	// vec4.0x00
	SSCV_SIWZ_x2			= 0x00000100,
	SSCV_SIWZ_y2			= 0x00000200,
	SSCV_SIWZ_z2			= 0x00000400,
	SSCV_SIWZ_w2			= 0x00000800,
	SSCV_SIWZ_F2 = SSCV_SIWZ_x2 | SSCV_SIWZ_y2 | SSCV_SIWZ_z2 | SSCV_SIWZ_w2,

	// vec4.x000
	SSCV_SIWZ_x3			= 0x00001000,
	SSCV_SIWZ_y3			= 0x00002000,
	SSCV_SIWZ_z3			= 0x00004000,
	SSCV_SIWZ_w3			= 0x00008000,
	SSCV_SIWZ_F3 = SSCV_SIWZ_x3 | SSCV_SIWZ_y3 | SSCV_SIWZ_z3 | SSCV_SIWZ_w3,
		
	// vec4.xyz =>  SSCV_SIWZ_x0 | SSCV_SIWZ_y1 | SSCV_SIWZ_z2
	// vec4.xyy =>  SSCV_SIWZ_x0 | SSCV_SIWZ_y1 | SSCV_SIWZ_y2
	// vec4.xy  =>  SSCV_SIWZ_x0 | SSCV_SIWZ_y1
	
	
	// vec4.000x
	SSCV_SIWZ_r0			= 0x00000001,
	SSCV_SIWZ_g0			= 0x00000002,
	SSCV_SIWZ_b0			= 0x00000004,
	SSCV_SIWZ_a0			= 0x00000008,
	SSCV_SIWZ_CF0 = SSCV_SIWZ_r0 | SSCV_SIWZ_g0 | SSCV_SIWZ_b0 | SSCV_SIWZ_a0,

	// vec4.00x0
	SSCV_SIWZ_r1			= 0x00000010,
	SSCV_SIWZ_g1			= 0x00000020,
	SSCV_SIWZ_b1			= 0x00000040,
	SSCV_SIWZ_a1			= 0x00000080,
	SSCV_SIWZ_CF1 = SSCV_SIWZ_r1 | SSCV_SIWZ_g1 | SSCV_SIWZ_b1 | SSCV_SIWZ_a1,

	// vec4.0x00
	SSCV_SIWZ_r2			= 0x00000100,
	SSCV_SIWZ_g2			= 0x00000200,
	SSCV_SIWZ_b2			= 0x00000400,
	SSCV_SIWZ_a2			= 0x00000800,
	SSCV_SIWZ_CF2 = SSCV_SIWZ_r2 | SSCV_SIWZ_g2 | SSCV_SIWZ_b2 | SSCV_SIWZ_a2,

	// vec4.x000
	SSCV_SIWZ_r3			= 0x00001000,
	SSCV_SIWZ_g3			= 0x00002000,
	SSCV_SIWZ_b3			= 0x00004000,
	SSCV_SIWZ_a3			= 0x00008000,
	SSCV_SIWZ_CF3 = SSCV_SIWZ_r3 | SSCV_SIWZ_g3 | SSCV_SIWZ_b3 | SSCV_SIWZ_a3,

	SSCV_SIWZ_rgb			= SSCV_SIWZ_PF_RGBA | SSCV_SIWZ_r0 | SSCV_SIWZ_g1 | SSCV_SIWZ_b2,
	SSCV_SIWZ_a				= SSCV_SIWZ_PF_RGBA | SSCV_SIWZ_a0,

};

typedef cjChar SWIZBUF[6];


enum SSC_SIZ
{
	SSC_SIZ_VAR			= 32,
	SSC_SIZ_PRM_NAME	= 32,
	SSC_SIZ_FUNCNAME	= 32,
};

typedef cjWORD cjSSCToken;
typedef cjUINT32 cjSSCV_FLAG;

#define SSCE_RELEASE(x)		if(x) { x->ReleaseRef(); x=cjNULL; }


template <class T> 
class cjSSCRPtr : public cjMem
{
public:
	typedef T	Type;

public:
	inline cjSSCRPtr() { _m_refPtr = cjNULL; }
    inline cjSSCRPtr(T* inRef);
    inline cjSSCRPtr(const cjSSCRPtr& refPtr);
    
	inline ~cjSSCRPtr()
	{
		if(_m_refPtr)
			_m_refPtr->ReleaseRef();
	}
	
    inline operator T*() const;
    inline T& operator*() const;
    inline T* operator->() const;
	inline T* _Ptr() const;

    inline cjSSCRPtr& operator=(const cjSSCRPtr& refPtr);
    inline cjSSCRPtr& operator=(T* refPtr);

    inline cjBool operator==(T* refPtr) const;
    inline cjBool operator!=(T* refPtr) const;
    inline cjBool operator==(const cjSSCRPtr& refPtr) const;
    inline cjBool operator!=(const cjSSCRPtr& refPtr) const;

private:
    T* _m_refPtr;
};

typedef cjSSCRPtr<SSC_Fn> SSC_FnRPtr;



#define cjSSCRPtrDefine(CLS_NAME) \
    class CLS_NAME; \
    typedef cjSSCRPtr<CLS_NAME> CLS_NAME##RPtr; \
	CJTYPEINFO_SUPPORT_IMPL(cjSSCRPtr<CLS_NAME>) 

#define cjSSCRPtrForwardDefine(CLS_NAME) \
    class CLS_NAME; \
    typedef cjSSCRPtr<CLS_NAME> CLS_NAME##RPtr;


template <class T>
inline cjSSCRPtr<T>::cjSSCRPtr(T* refPtr)
{
	_m_refPtr = refPtr;
	if(_m_refPtr)
		_m_refPtr->AddRef();
}

template <class T>
inline cjSSCRPtr<T>::cjSSCRPtr(const cjSSCRPtr& refPtr)
{
    _m_refPtr = refPtr._m_refPtr;
    if(_m_refPtr)
        _m_refPtr->AddRef();
}

template <class T>
inline cjSSCRPtr<T>::operator T*() const
{
    return _m_refPtr;
}

template <class T>
inline T& cjSSCRPtr<T>::operator*() const
{
    return *_m_refPtr;
}

template <class T>
inline T* cjSSCRPtr<T>::operator->() const
{
    return _m_refPtr;
}

template <class T>
inline T* cjSSCRPtr<T>::_Ptr() const
{
    return _m_refPtr;
}

template <class T>
inline cjSSCRPtr<T>& cjSSCRPtr<T>::operator=(const cjSSCRPtr& refPtr)
{
	if(_m_refPtr != refPtr._m_refPtr)
	{
		if(_m_refPtr)
			_m_refPtr->ReleaseRef();

		_m_refPtr = refPtr._m_refPtr;

		if(_m_refPtr)
			_m_refPtr->AddRef();
	}

	return *this;
}

template <class T>
inline cjSSCRPtr<T>& cjSSCRPtr<T>::operator=(T* refPtr)
{
    if(_m_refPtr != refPtr)
    {
        if(_m_refPtr)
            _m_refPtr->ReleaseRef();

        _m_refPtr = refPtr;

        if(_m_refPtr)
            _m_refPtr->AddRef();
    }

    return *this;
}

template <class T>
inline cjBool cjSSCRPtr<T>::operator==(T* refPtr) const
{
    return (_m_refPtr == refPtr);
}

template <class T>
inline cjBool cjSSCRPtr<T>::operator!=(T* refPtr) const
{
    return (_m_refPtr != refPtr);
}

template <class T>
inline cjBool cjSSCRPtr<T>::operator==(const cjSSCRPtr& refPtr) const
{
    return (_m_refPtr == refPtr._m_refPtr);
}

template <class T>
inline cjBool cjSSCRPtr<T>::operator!=(const cjSSCRPtr& refPtr) const
{
    return (_m_refPtr != refPtr._m_refPtr);
}


//
// SSC_Buf
//

class CJENGINE_API SSC_Buf: public cjMem
{
public:
	enum { ETMP_BUF_SIZ = 1024 };

	SSC_Buf(cjAryOff initSiz=512);
	~SSC_Buf();

	void AddLine();

	void AddStr(const cjChar* str);
	void AddStrEx(const cjChar* str, const cjChar* suffix);
	void AddStrln(const cjChar* str);

	void Print(const cjChar* fmt, ...);
	void Println(const cjChar* fmt, ...);

	void TestPrint() const;

	cjBool Copy(const cjChar* str, cjBool ln=cjFalse);
	cjBool CopyEx(const cjChar* str, const cjChar* suffix);

	cjAryOff GetExtendSiz() const;

	const cjChar* GetBuf() const { return m_buf; }
	cjAryOff GetSiz() const { return m_bufSiz; }

	const cjChar* GetLastStr() const;

protected:
	cjBool ExtendBuf(cjAryOff minSiz);
		
protected:
	cjAryOff						m_curOff;
	cjAryOff						m_lastOff;
	
	cjAryOff						m_bufSiz;
	cjChar*							m_buf;
};


//
// SSC_Element
//

class CJENGINE_API SSC_Element : public cjMem
{
public:
	enum ElemType
	{
		EVAR,
		ESHVARDECL,
		EFN,
		ELET,
		EVTXPROG,
		EPXLPROG,
	};

	SSC_Element();
	virtual ~SSC_Element();

	cjINT32 AddRef(void);
	cjINT32 ReleaseRef(void);
	cjINT32 GetRef(void) const { return m_refCount; }

	static cjINT32 GetTotalCount() { return ms_totalCount; }
	static const cjChar* ToStr(SSC_TOKEN token);
	static const cjChar* ToSIWZStr(cjChar buf[6], cjUINT32 siwzFlag);

	cjBool IsType(ElemType type) const
	{
		return (m_elementType == type) ? cjTrue : cjFalse;
	}

public:
	virtual SSC_Fn* GetAsFn() { return cjNULL; }
	virtual SSC_V* GetAsVar() { return cjNULL; }
	virtual SSC_ShVarDecl* GetAsShVarDecl() { return cjNULL; }
	virtual SSC_Oper* GetAsOper() { return cjNULL; }
	virtual SSC_Let* GetAsLet() { return cjNULL; }

	virtual const cjChar* GetVarName() const { return ""; }
	virtual const cjChar* PutEvaluated(SSC_Buf* buf, SSCV_SIWZ swiz=SSCV_SIWZ_NONE) const { return ""; }	
	virtual const cjChar* GetDefaultLet(SSC_Buf* buf, SSCV_SIWZ swiz=SSCV_SIWZ_NONE) const { return ""; }

	virtual void GenerateScript(SSC_Buf* buf) const = 0;

protected:
	virtual cjBool _AddResult(SSC_Buf* dst, SSC_Element* lhs) { return cjFalse; }
			
protected:
	cjINT32							m_refCount;

	cjBYTE							m_elementType;
	cjBYTE							m_pad[3];			// 0: in,

private:
	static const cjChar*			ms_token[SSCT__MAX];
	static const cjChar*			ms_qualifer[];

public:
	static cjINT32					ms_totalCount;

};

cjSSCRPtrDefine(SSC_Element)
typedef cjTAry< cjSSCRPtr<SSC_Element> > SSC_ElementRPtrAry;


#define SSC_ELEM_PUTEVALUATED_SUFFIX \
	const cjChar* rst = buf->GetLastStr(); \
	SWIZBUF swizBuf; \
	if(swiz != SSCV_SIWZ_NONE) \
		buf->AddStr(ToSIWZStr(swizBuf, swiz)); \
	\
	return rst; 


//
// SSC_V
//

class CJENGINE_API SSC_V : public SSC_Element
{
public:
	virtual SSC_V* GetAsVar() { return this; }
	virtual const cjChar* GetVarName() const;

	virtual const cjChar* GetDefaultLet(SSC_Buf* buf, SSCV_SIWZ swiz=SSCV_SIWZ_NONE) const 
	{ 
		if(buf)
		{
			buf->AddStr(GetDefValue());
			SSC_ELEM_PUTEVALUATED_SUFFIX
		}
		else
		{
			return GetDefValue();
		}
	}

	virtual const cjChar* PutEvaluated(SSC_Buf* buf, SSCV_SIWZ swiz) const 
	{ 
		if(buf)
		{
			buf->AddStr(GetVarName());
			SSC_ELEM_PUTEVALUATED_SUFFIX
		}
		else
		{
			return GetVarName();
		}
	}

	virtual void GenerateScript(SSC_Buf* buf) const;	
	
public:
	SSC_V(const cjChar* script);
	SSC_V(cjSSCV_FLAG flag, SSC_TOKEN type, const cjChar* name=cjNULL,
		const cjChar* defVal=cjNULL, cjINT32 ary=0);
	virtual ~SSC_V();

	const cjChar* GetTypeName() const { return ToStr(m_type); }	
	const cjChar* GetDefValue() const 
	{ 
		if(m_defValue)
			return m_defValue->GetStr();
		else
			return "";
	}
	
private:
	void InitCommon(cjSSCV_FLAG flag, SSC_TOKEN type, const cjChar* name, 
		const cjChar* defVal, cjINT32 ary);

protected:
	SSC_V() {}
	
protected:
	cjSSCV_FLAG						m_varFlag;
	SSC_TOKEN						m_type;

	cjChar							m_varName[SSC_SIZ_PRM_NAME];

	cjStringA*						m_defValue;
	cjINT32							m_aryCount; // 0:none, -1: [] unknown

	cjStringA*						m_script;
};

cjSSCRPtrDefine(SSC_V)
typedef cjTAry< cjSSCRPtr<SSC_V> > SSC_VRPtrAry;


//
// SSCE_ShVarDecl
//

class CJENGINE_API SSC_ShVarDecl : public SSC_V
{
public:
	virtual SSC_ShVarDecl* GetAsShVarDecl() { return this; }	
	virtual void GenerateScript(SSC_Buf* buf) const;	

public:
	SSC_ShVarDecl(SSC_TOKEN shVarAttr, SSC_TOKEN varType, 
		const cjChar* name=cjNULL, const cjChar* defVal=cjNULL, cjINT32 ary=0);
	virtual ~SSC_ShVarDecl();

protected:
	SSC_ShVarDecl() {}
	
private:
	SSC_TOKEN						m_shVarAttr;
};

cjSSCRPtrDefine(SSC_ShVarDecl)
typedef cjTAry< cjSSCRPtr<SSC_ShVarDecl> > SSC_ShVarDeclRPtrAry;


//
// SSC_Call
//

class CJENGINE_API SSC_Call : public SSC_Element
{
public:
	virtual SSC_Call* GetAsSSC_Call() { return this; }

	virtual const cjChar* PutEvaluated(SSC_Buf* buf, SSCV_SIWZ swiz) const 
	{ 
		GenerateScript(buf);

		if(buf)
		{
			SSC_ELEM_PUTEVALUATED_SUFFIX
		}
		else
		{
			return "";
		}
	}

	virtual void GenerateScript(SSC_Buf* buf) const;

public:
	SSC_Call(SSC_Fn* fn, ...); // parameter must be end with cjNULL
	virtual ~SSC_Call();

protected:
	SSC_Call() {}

protected:
	SSC_FnRPtr						m_fn;
	cjTAry<SSC_ElementRPtr>			m_params;
	cjTAry<cjUINT32>				m_paramFlags;
	
};

cjSSCRPtrDefine(SSC_Call)


//
// SSC_Oper
//

class CJENGINE_API SSC_Oper : public SSC_Element
{
public:
	virtual SSC_Oper* GetAsOper() { return this; }

	virtual const cjChar* PutEvaluated(SSC_Buf* buf, SSCV_SIWZ swiz) const 
	{ 
		GenerateScript(buf);

		if(buf)
		{
			SSC_ELEM_PUTEVALUATED_SUFFIX
		}
		else
		{
			return "";
		}
	}

	virtual void GenerateScript(SSC_Buf* buf) const;

public:
	enum EOP
	{ 
		EOP_NONE,
		EOP_ADD,
		EOP_SUBTRACT,
		EOP_MUL,
		EOP_DIV,
		EOP_MAX
	};

public:
	SSC_Oper(EOP op, SSC_V* lhs, SSC_V* rhs);
	virtual ~SSC_Oper();

protected:
	SSC_Oper() {}
	
private:
	EOP								m_op;
	SSC_ElementRPtr					m_lhs;
	SSC_ElementRPtr					m_rhs;
};

cjSSCRPtrDefine(SSC_Oper)


//
// SSC_Let
//

class CJENGINE_API SSC_Let : public SSC_Element
{
public:
	virtual SSC_Let* GetAsLet() { return this; }
	virtual void GenerateScript(SSC_Buf* buf) const;

public:
	enum 
	{
		FL_LOCAL_DECL = 0x00001,
	};
	
	SSC_Let(SSC_V* lhs, SSC_Element* rhs);
	SSC_Let(SSC_V* lhs, SSCV_SIWZ siwzLeft, SSC_Element* rhs, SSCV_SIWZ siwzRight);

	SSC_Let(cjUINT32 flag, SSC_V* lhs, SSC_Element* rhs);

	virtual ~SSC_Let();

protected:
	SSC_Let() {}

private:
	SSC_ElementRPtr					m_lhs;
	SSC_ElementRPtr					m_rhs;

	SSCV_SIWZ						m_lhsSiwz;
	SSCV_SIWZ						m_rhsSiwz;

	cjUINT32						m_flag;
};

cjSSCRPtrDefine(SSC_Let)
typedef cjTAry< cjSSCRPtr<SSC_Let> > SSC_LetRPtrAry;


//
// SSC_Fn
//

class CJENGINE_API SSC_Fn : public SSC_Element
{
public:
	virtual SSC_Fn* GetAsFn() { return this; }
	virtual void GenerateScript(SSC_Buf* buf) const;	

public:
	SSC_Fn(const cjChar* funcName);
	virtual ~SSC_Fn();

	const cjChar* GetFuncName() const { return m_funcName; }
	void SetFuncName(const cjChar* fnName);

	SSC_V* AddParam(SSC_V* param);
	SSC_V* SetRetParam(SSC_V* param);

	// SSC_Let
	// SSC_V
	SSC_Element* AddCmd(SSC_Element* cmd);

private:
	cjChar							m_funcName[SSC_SIZ_FUNCNAME];

	SSC_VRPtrAry					m_params;
	SSC_VRPtr						m_ret;
	SSC_ElementRPtrAry				m_cmds;
};

cjSSCRPtrDefine(SSC_Fn)
typedef cjTAry< cjSSCRPtr<SSC_Fn> > SSC_FnRPtrAry;


//
// SSCE_VtxProg
//

class CJENGINE_API SSCE_VtxProg : public SSC_Element
{
public:
	virtual void GenerateScript(SSC_Buf* buf) const { }

public:
	SSCE_VtxProg();
	virtual ~SSCE_VtxProg();

	void SetName(const cjChar* name);
	
	SSC_V* AddShVarDecl(SSC_V* decl);
	SSC_Fn* AddShaderFunc(SSC_Fn* fn);
	SSC_Fn* SetShaderMain(SSC_Fn* fn);

	cjINT32 Generate(SSC_Buf* buf);

private:
	cjStringA						m_name;

	SSC_VRPtrAry					m_shVarDecls;
	SSC_FnRPtrAry					m_funcs;
	SSC_FnRPtr						m_main;
};

cjSSCRPtrDefine(SSCE_VtxProg)


//
// SSCE_PxlProg
//

class CJENGINE_API SSCE_PxlProg : public SSC_Element
{
public:
	virtual void GenerateScript(SSC_Buf* buf) const {}

public:
	SSCE_PxlProg();
	virtual ~SSCE_PxlProg();

	void SetName(const cjChar* name);
	
	SSC_V* AddShVarDecl(SSC_V* decl);
	SSC_Fn* AddShaderFunc(SSC_Fn* fn);
	SSC_Fn* SetShaderMain(SSC_Fn* fn);

	cjINT32 Generate(SSC_Buf* buf);

private:
	cjStringA						m_name;

	SSC_VRPtrAry					m_shVarDecls;
	SSC_FnRPtrAry					m_funcs;
	SSC_FnRPtr						m_main;
};

cjSSCRPtrDefine(SSCE_PxlProg)


#endif

