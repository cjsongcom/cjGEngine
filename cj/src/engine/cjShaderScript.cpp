
#include "cjEnginePCH.h"
#include "cjShaderScript.h"


//
// SSC_Element
//

const cjChar* SSC_Element::ms_token[SSCT__MAX] = 
{
	"",

	"attribute",

	"vec4",		
	"vec3",
	"vec2",
	"vec1",
	
	"mat4",
	"mat3",
	"mat2",
	"mat1",
	
	"varying",
	
	"const",
	"int",
	"uniform",

	"v_color",
	"gl_Position",
	"a_color",
	"a_texcoord0",
	
	"matMVP",

	"sampler2D",

	"v_texcoord0",
	"v_texcoord1",
	"v_texcoord2",
	"v_texcoord3",
	"v_texcoord4",
	"v_texcoord5",
	"v_texcoord6",
	"v_texcoord7",

	"s_texsamp0",
	"s_texsamp1",
	"s_texsamp2",
	"s_texsamp3",
	"s_texsamp4",
	"s_texsamp5",
	"s_texsamp6",
	"s_texsamp7",

};

cjINT32	SSC_Element::ms_totalCount;

SSC_Element::SSC_Element()
{
	m_refCount = 0;
	cjAtomicInc(ms_totalCount);
	
	m_elementType = 0;
	m_pad[0]=m_pad[1]=m_pad[2] = 0;
}

SSC_Element::~SSC_Element()
{
	cjAssert(m_refCount == 0);
	cjAtomicDec(ms_totalCount);	
}

cjINT32 SSC_Element::AddRef(void)
{
	cjAssert(m_refCount > -1);
	cjAtomicInc(m_refCount);

	return m_refCount;
}

cjINT32 SSC_Element::ReleaseRef(void)
{	
	cjAtomicDec(m_refCount);

#if defined(CJ_PLATFORM_WIN32)

	// sizeof(LONG) in win32/win64 is always 32
	if(::InterlockedCompareExchange((LONG*)&m_refCount, (LONG)0, 0) == 0)
	{
		delete this;

		return 0;
	}
	else
	{
		if(::InterlockedCompareExchange((LONG*)&m_refCount, (LONG) -1, -1) == -1)
		{
			cjAssert(!"ERR! - RefCount is under zero..");
		}

		return m_refCount;
	}

#elif defined(CJ_PLATFORM_APPLE)

	if(::OSAtomicCompareAndSwap32Barrier(0, 0, (cjINT32*)&m_refCount))
	{
		delete this;

		return 0;
	}
	else
	{
		if(::OSAtomicCompareAndSwap32Barrier(-1, -1, (cjINT32*)&m_refCount))
		{
			cjAssert(!"ERR! - RefCount is under zero..");
		}

		return m_refCount;
	}

#else
	#error "not implemented"
	return -1;
#endif

}

const cjChar* SSC_Element::ToStr(SSC_TOKEN token)
{
	if(token >= SSCT__MAX)
		return "";

	return ms_token[token];
}

const cjChar* SSC_Element::ToSIWZStr(SWIZBUF buf, cjUINT32 siwzFlag)
{
	// not equal to 'const cjChar s[4] = {..}
	const static cjChar x[4] = { 'x', 'y', 'z', 'w' };
	const static cjChar r[4] = { 'r', 'g', 'b', 'a' };

	const cjChar* s = x;

	if(siwzFlag & SSCV_SIWZ_PF_RGBA)
		s = r;
		
	cjINT32 i=0;
	cjINT32 k=1;
	cjINT32 t=0;

	do 
	{	
		cjBYTE c = siwzFlag & 0xf;
		i = 0;

		do
		{
			if(c & 0x1)
			{
				buf[k] = s[i];
				k++;
				break;
			}

			c = c >> 1;
			i++;

		} while(i < 4);

		siwzFlag = siwzFlag >> 4;
		t++;
		
	} while(t < 4 && (k >= 0));

	if(k == 0)
		return "";

	buf[0] = '.';

	cjAssert(k <= 5);
	buf[k] = cjNULL;

	return buf;
}


//
// SSC_Buf
//

SSC_Buf::SSC_Buf(cjAryOff initSiz)
{
	m_curOff = 0;
	m_lastOff = 0;

	m_bufSiz = initSiz;
	m_buf = cjNULL;

	if(initSiz)
		m_buf = (cjChar*)cjMemAlloc(m_bufSiz);
}

SSC_Buf::~SSC_Buf()
{
	if(m_buf)
	{
		cjMemDeAlloc(m_buf);
		m_buf = cjNULL;
	}
}

void SSC_Buf::AddLine()
{
	m_lastOff = m_curOff;
	Copy("", cjTrue);
}

void SSC_Buf::AddStr(const cjChar* str)
{
	m_lastOff = m_curOff;
	Copy(str);
}

void SSC_Buf::AddStrEx(const cjChar* str, const cjChar* suffix)
{
	m_lastOff = m_curOff;
	CopyEx(str, suffix);
}

void SSC_Buf::AddStrln(const cjChar* str)
{
	m_lastOff = m_curOff;
	Copy(str, cjTrue);
}

void SSC_Buf::Print(const cjChar* fmt, ...)
{	
	m_lastOff = m_curOff;

	va_list arg;
	va_start(arg, fmt);

	cjChar buf[ETMP_BUF_SIZ];
	buf[0] = 0;

	cjvsnprintf(buf, ETMP_BUF_SIZ, fmt, arg);

	va_end(arg);

	Copy(buf);
}

void SSC_Buf::Println(const cjChar* fmt, ...)
{
	m_lastOff = m_curOff;

	va_list arg;
	va_start(arg, fmt);

	cjChar buf[ETMP_BUF_SIZ];
	buf[0] = 0;

	cjvsnprintf(buf, ETMP_BUF_SIZ, fmt, arg);

	va_end(arg);

	Copy(buf, cjTrue);
}

void SSC_Buf::TestPrint() const
{
#ifdef CJ_PLATFORM_WIN32

	OutputDebugStringA(m_buf);

#else

#endif
}

cjBool SSC_Buf::Copy(const cjChar* str, cjBool ln)
{	
	cjINT32 len = cjStrlen(str);
	cjAryOff diff = m_bufSiz - (m_curOff + len + (ln ? 1:0) +1);  // +1 for null

	if(diff < 0)
	{
		if(!ExtendBuf(diff))
			return cjFalse;
	}

	cjChar* c = m_buf+m_curOff;

	cjMemCpy(c, str, len);

	c += len;
	m_curOff += len;

	if(ln)
	{
		*c = '\n';
		c++;

		m_curOff++;
	}
		
	*c = cjNULL;

	cjAssert( (m_buf+m_curOff) == c );

	return cjTrue;
}

cjBool SSC_Buf::CopyEx(const cjChar* str, const cjChar* suffix)
{
	cjINT32 len = cjStrlen(str);
	cjINT32 lenSuffix = suffix ? cjStrlen(suffix) : 0;

	cjAryOff diff = m_bufSiz - (m_curOff + len + lenSuffix +1);  // +1 for null

	if(diff < 0)
	{
		if(!ExtendBuf(diff))
			return cjFalse;
	}

	cjChar* c = m_buf+m_curOff;

	cjMemCpy(c, str, len);

	c += len;
	m_curOff += len;

	if(lenSuffix)
	{
		cjMemCpy(c, suffix, lenSuffix);
		
		c += lenSuffix;
		m_curOff += lenSuffix;
	}
		
	*c = cjNULL;

	cjAssert( (m_buf+m_curOff) == c );

	return cjTrue;
}

cjAryOff SSC_Buf::GetExtendSiz() const
{
	return m_bufSiz + (2 * m_bufSiz / 8) + 16;
}

cjBool SSC_Buf::ExtendBuf(cjAryOff minSiz)
{
	cjAssert(m_bufSiz >= m_curOff);

	cjAryOff newSiz = GetExtendSiz() + cjAbs(minSiz);
	cjChar* newBuf = (cjChar*)cjMemReAlloc(m_buf, newSiz, 0);

	if(!newBuf)
		return cjFalse;

	m_buf = newBuf;
	m_bufSiz = newSiz;

	return cjTrue;
}

const cjChar* SSC_Buf::GetLastStr() const
{
	if(m_lastOff == m_curOff)
		return "";

	return m_buf + m_lastOff;
}

//
// SSC_V
//

void SSC_V::InitCommon(cjSSCV_FLAG flag, SSC_TOKEN type, const cjChar* name, 
	const cjChar* defVal, cjINT32 ary)
{
	m_elementType = EVAR;

	m_varFlag = flag;
	m_type = type;

	if(name)
	{
		cjMemSet(m_varName, 0, sizeof(m_varName));
		// cjStncpy doesn't add null terminator
		cjStrncpy(m_varName, name, SSC_SIZ_PRM_NAME-1); 
	}
	else
	{
		m_varName[0] = cjNULL;
	}

	if(defVal)
		m_defValue = new cjStringA(defVal);
	else
		m_defValue = cjNULL;

	m_aryCount = ary;
	m_script = cjNULL;
}

SSC_V::SSC_V(const cjChar* script)
{
	InitCommon(SSCV_FL_SCRIPT, SSCT_NONE, cjNULL, cjNULL, 0);

	if(script)
		m_script = new cjStringA(script);
}

SSC_V::SSC_V(cjSSCV_FLAG flag, SSC_TOKEN type, const cjChar* name, 
	const cjChar* defVal, cjINT32 ary)
{
	InitCommon(flag, type, name, defVal, ary);
}

SSC_V::~SSC_V()
{
	if(m_defValue)
	{
		delete m_defValue;
		m_defValue = cjNULL;
	}

	if(m_script)
	{
		delete m_script;
		m_script = cjNULL;
	}
}

const cjChar* SSC_V::GetVarName() const 
{


	return m_varName[0] ? m_varName : ""; 
}

void SSC_V::GenerateScript(SSC_Buf* buf) const
{
	if(m_varFlag & (SSCV_FL_PRM))
	{
		const cjChar* qualifer = "";
		
		if((m_varFlag & (SSCV_FL_PRM_IN | SSCV_FL_PRM_OUT)) == (SSCV_FL_PRM_IN | SSCV_FL_PRM_OUT))
		{
			qualifer = "inout";
		}
		else
		{
			if(m_varFlag & SSCV_FL_PRM_IN)
				qualifer = "in";
			else if(m_varFlag & SSCV_FL_PRM_OUT)
				qualifer = "out";
		}

		buf->Print("%s %s %s", qualifer, ToStr(m_type), m_varName);

		if(m_aryCount == -1)
			buf->AddStr("[]");
		else if(m_aryCount > 0)
			buf->Print("[%d]", m_aryCount);
	}	
	else if(m_varFlag & SSCV_FL_SCRIPT)
	{
		buf->Println("%s", m_script ? m_script->GetStr() : "");
	}
	else if(m_varFlag & SSCV_FL_VSH_OUT)
	{
		buf->Print("%s", GetVarName());
	}
	else if(m_varFlag & SSCV_FL_LOCAL)
	{
		buf->Print("%s", GetVarName());
	}
	else if(m_varFlag & SSCV_FL_RET)
	{
		buf->Println("return %s;", m_defValue ? m_defValue->GetStr() : "");
	}
	else
	{
		cjAssert(!"[SSC_V::GenerateScript] unhandled parameter..");
	}
}


//
// SSC_ShVarDecl
//

SSC_ShVarDecl::SSC_ShVarDecl(SSC_TOKEN shVarAttr, SSC_TOKEN varType, 
	const cjChar* name, const cjChar* defVal, cjINT32 ary)
	: m_shVarAttr(shVarAttr), SSC_V(SSCV_FL_SHATTR, varType, name, defVal, ary)
{
	m_elementType = ESHVARDECL;
}

SSC_ShVarDecl::~SSC_ShVarDecl()
{
}

void SSC_ShVarDecl::GenerateScript(SSC_Buf* buf) const
{
	if(m_aryCount == 0)
		buf->Print("%s %s %s", ToStr(m_shVarAttr), ToStr(m_type), GetVarName());
	else
		buf->Print("%s %s %s[%d]", ToStr(m_shVarAttr), ToStr(m_type), GetVarName(), m_aryCount);
}


//
// SSC_Call
//

SSC_Call::SSC_Call(SSC_Fn* fn, ...)
{
	va_list ap;
	SSC_Element* e;

	va_start(ap, fn); // fn is prev param of variant argument

	cjINT32 i = 0;
	cjUINT32 flag = 0;

	for(;;)
	{
		if(i % 2)
		{
			flag = va_arg(ap, cjUINT32);
			m_paramFlags.Push(flag);
		}
		else
		{
			e = va_arg(ap, SSC_Element*);
		
			if(!e)
				break;

			m_params.Push(e);
		}
		
		i++;
	}

	va_end(ap);

	cjAssert(m_params.Count() == m_paramFlags.Count());

	m_fn = fn;
}

SSC_Call::~SSC_Call()
{
	for(cjAryOff i=0; i<m_params.Count(); i++)
		m_params(i) = cjNULL;

	m_params.RemoveAll();
}

void SSC_Call::GenerateScript(SSC_Buf* buf) const
{
	buf->AddStr(m_fn->GetFuncName());
	buf->AddStr("(");

	cjAryOff count = m_params.Count();

	for(cjAryOff i=0; i<count; i++)
	{
		SSC_V* v = m_params(i)->GetAsVar();
		cjUINT32 fl = m_paramFlags(i);

		if(!v)
			continue;

		if(fl == 0)
		{
			if((i+1) == count)
				buf->AddStr(v->GetVarName());
			else
				buf->AddStrEx(v->GetVarName(), ", ");
		}
		else
		{
			SWIZBUF swizBuf;
			buf->AddStrEx(v->GetVarName(), ToSIWZStr(swizBuf, fl));

			if((i+1) != count)
				buf->AddStr(", ");				
		}
	}

	buf->AddStr(")");
}


//
// SSC_Oper
//

SSC_Oper::SSC_Oper(SSC_Oper::EOP op, SSC_V* lhs, SSC_V* rhs)
{
	m_op = op;
	m_lhs = lhs;
	m_rhs = rhs;
}

SSC_Oper::~SSC_Oper()
{
}

void SSC_Oper::GenerateScript(SSC_Buf* buf) const
{
	const cjChar* opStr;

	switch(m_op)
	{
	case EOP_ADD :		opStr = "+"; break;
	case EOP_SUBTRACT : opStr = "-"; break;
	case EOP_MUL :		opStr = "*"; break;
	case EOP_DIV :		opStr = "/"; break;
	default:
		return;
	}

	buf->Print(" %s %s %s ", m_lhs->GetVarName(), opStr, m_rhs->GetVarName());
}


//
// SSC_Let
//

SSC_Let::SSC_Let(SSC_V* lhs, SSC_Element* rhs)
{
	m_lhs = lhs;
	m_rhs = rhs;

	m_lhsSiwz = (SSCV_SIWZ)0;
	m_rhsSiwz = (SSCV_SIWZ)0;

	m_flag = 0;
}

SSC_Let::SSC_Let(SSC_V* lhs, SSCV_SIWZ siwzLeft, SSC_Element* rhs, SSCV_SIWZ siwzRight)
{
	m_lhs = lhs;
	m_rhs = rhs;

	m_lhsSiwz = siwzLeft;
	m_rhsSiwz = siwzRight;

	m_flag = 0;
}

SSC_Let::SSC_Let(cjUINT32 flag, SSC_V* lhs, SSC_Element* rhs)
{
	m_lhs = lhs;
	m_rhs = rhs;

	m_lhsSiwz = (SSCV_SIWZ)0;
	m_rhsSiwz = (SSCV_SIWZ)0;

	m_flag = flag;
}

SSC_Let::~SSC_Let()
{
}

void SSC_Let::GenerateScript(SSC_Buf* buf) const
{
	if(!m_lhs)
		return;

	if(!m_rhs && !(*m_lhs->GetDefaultLet(cjNULL)))
		return;

	if(m_flag & FL_LOCAL_DECL)
	{
		SSC_V* v = m_lhs->GetAsVar();
		buf->AddStr(v->GetTypeName());
	}

	if(m_lhsSiwz == 0)
	{
		buf->Print(" %s = ", m_lhs->GetVarName());
	}
	else
	{
		SWIZBUF swiz;		
		buf->Print(" %s%s = ", m_lhs->GetVarName(), ToSIWZStr(swiz, m_lhsSiwz));
	}

	if(m_rhs)	
	{
		m_rhs->PutEvaluated(buf, m_rhsSiwz);
	}
	else
	{
		m_lhs->GetDefaultLet(buf, m_rhsSiwz);
	}	
}


//
// SSC_Fn
//

SSC_Fn::SSC_Fn(const cjChar* funcName)
{
	m_elementType = EFN;
	m_funcName[0] = cjNULL;

	if(funcName)
		cjStrncpy(m_funcName, funcName, SSC_SIZ_PRM_NAME);

	m_ret = cjNULL;
}

SSC_Fn::~SSC_Fn()
{
	if(!m_params.IsEmpty())
	{
		for(cjAryOff i=0; i<m_params.Count(); i++)
		{
			m_params(0) = cjNULL;
		}

		m_params.RemoveAll();
	}

	if(!m_cmds.IsEmpty())
	{
		for(cjAryOff i=0; i<m_cmds.Count(); i++)
		{
			m_cmds(0) = cjNULL;
		}

		m_cmds.RemoveAll();
	}

	m_ret = cjNULL;
}

void SSC_Fn::SetFuncName(const cjChar* fnName)
{
	m_funcName[0] = cjNULL;

	if(fnName)
		cjStrcpy(m_funcName, fnName);
}

SSC_V* SSC_Fn::AddParam(SSC_V* param)
{
	m_params.Push(param);
	return param;
}

SSC_V* SSC_Fn::SetRetParam(SSC_V* param)
{
	m_ret = param;
	return param;
}

SSC_Element* SSC_Fn::AddCmd(SSC_Element* cmd)
{
	cjAssert(cmd);

	if(cmd->GetAsVar() || cmd->GetAsLet())
	{
		m_cmds.Push(cmd);
	}
	else
	{
		cjAssert(!"[SSC_Fn::AddCmd] invalid cmd!");
	}

	return cmd;
}

void SSC_Fn::GenerateScript(SSC_Buf* buf) const
{
	const cjChar* retType = "void";
	
	if(m_ret)
		retType = m_ret->GetTypeName();
		
	buf->Print("%s %s(", retType, GetFuncName());

	cjAryOff paramCount = m_params.Count();

	for(cjAryOff i=0; i<paramCount; i++)
	{
		SSC_Element* curParam = m_params(i);

		curParam->GenerateScript(buf);

		if(i+1 != paramCount)
			buf->AddStr(", ");
	}

	buf->AddStrln(")");
	buf->AddStrln("{");


	cjAryOff cmdCount = m_cmds.Count();

	for(cjAryOff i=0; i<cmdCount; i++)
	{
		SSC_Element* curCmd = m_cmds(i);

		curCmd->GenerateScript(buf);
		buf->Println(";");
	}

	if(m_ret)
		m_ret->GenerateScript(buf);
		
	buf->AddStrln("}");
}


//
// SSCE_VtxProg
//

SSCE_VtxProg::SSCE_VtxProg()
{
	m_elementType = EVTXPROG;
}

SSCE_VtxProg::~SSCE_VtxProg()
{
	for(cjAryOff i=0; i<m_shVarDecls.Count(); i++)
		m_shVarDecls(i) = cjNULL;

	m_shVarDecls.RemoveAll();

	for(cjAryOff i=0; i<m_funcs.Count(); i++)
		m_funcs(i) = cjNULL;

	m_funcs.RemoveAll();
}

void SSCE_VtxProg::SetName(const cjChar* name)
{
	if(name)
		m_name = name;
	else
		m_name.Clear();
}
	
SSC_V* SSCE_VtxProg::AddShVarDecl(SSC_V* decl)
{	
	if(!decl)
		return cjNULL;

	if(!decl->GetAsShVarDecl())
		return cjNULL;

	m_shVarDecls.Push(decl);

	return decl;
}

SSC_Fn* SSCE_VtxProg::AddShaderFunc(SSC_Fn* fn)
{
	m_funcs.Push(fn);
	return fn;
}

SSC_Fn* SSCE_VtxProg::SetShaderMain(SSC_Fn* fn)
{
	m_main = fn;
	return fn;
}

cjINT32 SSCE_VtxProg::Generate(SSC_Buf* buf)
{
	buf->AddLine();

	for(cjAryOff i=0; i<m_shVarDecls.Count(); i++)
	{
		m_shVarDecls(i)->GenerateScript(buf);
		buf->Println(";");
	}

	buf->AddLine();

	for(cjAryOff i=0; i<m_funcs.Count(); i++)
		m_funcs(i)->GenerateScript(buf);

	buf->AddLine();

	if(m_main)
		m_main->GenerateScript(buf);
	
	buf->AddLine();

	buf->TestPrint();

	return 0;
}


//
// SSCE_PxlProg
//

SSCE_PxlProg::SSCE_PxlProg()
{
	m_elementType = EPXLPROG;
}

SSCE_PxlProg::~SSCE_PxlProg()
{
	for(cjAryOff i=0; i<m_shVarDecls.Count(); i++)
		m_shVarDecls(i) = cjNULL;

	m_shVarDecls.RemoveAll();

	for(cjAryOff i=0; i<m_funcs.Count(); i++)
		m_funcs(i) = cjNULL;

	m_funcs.RemoveAll();
}

void SSCE_PxlProg::SetName(const cjChar* name)
{
	if(name)
		m_name = name;
	else
		m_name.Clear();
}
	
SSC_V* SSCE_PxlProg::AddShVarDecl(SSC_V* decl)
{	
	if(!decl)
		return cjNULL;

	if(!decl->GetAsShVarDecl())
		return cjNULL;

	m_shVarDecls.Push(decl);

	return decl;
}

SSC_Fn* SSCE_PxlProg::AddShaderFunc(SSC_Fn* fn)
{
	m_funcs.Push(fn);
	return fn;
}

SSC_Fn* SSCE_PxlProg::SetShaderMain(SSC_Fn* fn)
{
	m_main = fn;
	return fn;
}

cjINT32 SSCE_PxlProg::Generate(SSC_Buf* buf)
{
	buf->AddLine();

	for(cjAryOff i=0; i<m_shVarDecls.Count(); i++)
	{
		m_shVarDecls(i)->GenerateScript(buf);
		buf->Println(";");
	}

	buf->AddLine();

	for(cjAryOff i=0; i<m_funcs.Count(); i++)
		m_funcs(i)->GenerateScript(buf);

	buf->AddLine();

	if(m_main)
		m_main->GenerateScript(buf);
	
	buf->AddLine();

	buf->TestPrint();

	return 0;
}
