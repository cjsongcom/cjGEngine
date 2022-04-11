
#include "cjEnginePCH.h"

#include "cjGLShaderManager.h"
#include "cjShaderMtlDesc.h"
#include "cjShaderProgDesc.h"

#include "cjShaderScript.h"

#include "cjGLShaderProgSnippet_ShaderMtlDef.h"


//
// cjGLShaderProgSnippet_ShaderMtlDefault
//


cjGLShaderProgSnippet_ShaderMtlDefault::cjGLShaderProgSnippet_ShaderMtlDefault()
{
}

cjGLShaderProgSnippet_ShaderMtlDefault::~cjGLShaderProgSnippet_ShaderMtlDefault()
{
}

cjFVertexShader* cjGLShaderProgSnippet_ShaderMtlDefault::ResolveVertexShader(
	const cjShaderProgramDesc* vtxDesc)
{
	if(vtxDesc->GetShaderMtlClsName() != cjName_cjShaderMtlDefault)
		return cjNULL;

	VtxDesc2Prog::Pair* pair = m_vtxDesc2Prog.Find(vtxDesc->GetHashKey());
	VertexProg* vtxProg = cjNULL;

	if(pair)
	{
		vtxProg = pair->m_val;
	}
	else
	{
		 cjFVertexShader* vtxShader = GenerateVtxProgram((const cjDefaultShaderVtxProgDesc*)vtxDesc);

		if(vtxShader)
		{
			vtxProg = new VertexProg();

			vtxProg->m_progDesc = *vtxDesc;
			vtxProg->m_vertexShader = vtxShader;

			m_vtxDesc2Prog.Insert(vtxDesc->GetHashKey(), vtxProg);
		}
	}

	if(!vtxProg)
		return cjNULL;

	return vtxProg->m_vertexShader;
}

cjFPixelShader* cjGLShaderProgSnippet_ShaderMtlDefault::ResolvePixelShader(
	const cjShaderProgramDesc* pxlDesc)
{
	if(pxlDesc->GetShaderMtlClsName() != cjName_cjShaderMtlDefault)
		return cjNULL;

	PxlDesc2Prog::Pair* pair = m_pxlDesc2Prog.Find(pxlDesc->GetHashKey());
	PixelProg* pxlProg = cjNULL;

	if(pair)
	{
		pxlProg = pair->m_val;
	}
	else
	{
		cjFPixelShader* pxlShader = GeneratePxlProgram((const cjDefaultShaderPxlProgDesc*)pxlDesc);

		if(pxlShader)
		{
			pxlProg = new PixelProg();

			pxlProg->m_progDesc = *pxlDesc;
			pxlProg->m_pixelShader = pxlShader;

			m_pxlDesc2Prog.Insert(pxlDesc->GetHashKey(), pxlProg);
		}
	}

	if(!pxlProg)
		return cjNULL;

	return pxlProg->m_pixelShader;
}

cjFVertexShader* cjGLShaderProgSnippet_ShaderMtlDefault::GenerateVtxProgram(const cjDefaultShaderVtxProgDesc* vtxDesc)
{
	cjAssert(SSC_Element::ms_totalCount == 0);

	cjLOG("Begin Generate Vtx Program");
	cjFVertexShader* vtxShader = _GenerateVtxProgram(vtxDesc);
	cjLOG("End Generate Vtx Program");

	cjAssert(SSC_Element::ms_totalCount == 0);

	return vtxShader;
}

cjFVertexShader* cjGLShaderProgSnippet_ShaderMtlDefault::_GenerateVtxProgram(const cjDefaultShaderVtxProgDesc* vtxDesc)
{	
	cjFVertexShader* rstVtxShader = cjNULL;

	// input
	cjBool	skinned = cjFalse; 
	cjBool	useBW = cjFalse; 
	cjBool	useBI = cjFalse;

	cjBool hasVtxNormal = cjFalse;
	cjBool hasVtxColor = cjFalse;
//	cjBool hasVtxTexUV0 = cjFalse;

	cjBool outVtxColor = cjFalse;
	cjBool outVtxNormal = cjFalse;
	cjBool outTex0 = cjFalse;
		
	cjBool	vtxLighting = cjFalse;
		
	{
		cjShaderMtlDefaultDesc::_ETM tm = (cjShaderMtlDefaultDesc::_ETM)vtxDesc->GetTM();

		switch(tm)
		{
		case cjShaderMtlDefaultDesc::TM_DEFAULT: break;
		case cjShaderMtlDefaultDesc::TM_SKINNED: useBW = cjTrue; useBI = cjTrue; skinned = cjTrue; break;
		default:
			return cjNULL;
		}

		if(vtxDesc->GetOUT_NORMAL() != 0)
		{
			hasVtxNormal = cjTrue;

			if(vtxDesc->GetOUT_NORMAL())
				outVtxNormal = cjTrue;
			else if(vtxDesc->GetOUT_WORLD_NBT())
				outVtxNormal = cjTrue;
		}

		if(vtxDesc->GetVTXCOLOR_FROM() != 0)
			hasVtxColor = cjTrue;
		
		if(vtxDesc->GetOUT_UV_COUNT())
			outTex0 = cjTrue;
	}


	SSCE_VtxProgRPtr vtxProg = new SSCE_VtxProg;
	

	//
	// shader input param
	//
	
		// input
	SSC_VRPtr a_apos	= cjNULL;
	SSC_VRPtr a_nrm		= cjNULL;
	SSC_VRPtr a_color	= cjNULL;
	SSC_VRPtr a_tex0	= cjNULL;
	SSC_VRPtr a_bw		= cjNULL;
	SSC_VRPtr a_bi		= cjNULL;

		// output
	SSC_VRPtr v_color	= cjNULL;
	SSC_VRPtr v_tex0	= cjNULL;

		// uniform shader constant
	SSC_VRPtr u_bonePals = cjNULL;
	SSC_VRPtr u_m4MVP	= cjNULL;
	SSC_VRPtr u_m4World = cjNULL;


	a_apos = new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_position");

	if(hasVtxNormal)
		a_nrm = new SSC_ShVarDecl(SSCT_attribute, SSCT_vec3, "a_normal");

	if(hasVtxColor)
	{
		a_color = new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_color");
		outVtxColor = cjTrue;
	}

	if(outTex0)
		a_tex0 = new SSC_ShVarDecl(SSCT_attribute, SSCT_vec2, "a_texcoord0");

	if(skinned)
	{
		a_bw		= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_blendweight");
		a_bi		= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_blendindices");
	}

	if(outVtxColor)
		v_color	= new SSC_ShVarDecl(SSCT_varying, SSCT_vec4, "v_color");

	if(outTex0)
		v_tex0	= new SSC_ShVarDecl(SSCT_varying, SSCT_vec2, "v_texcoord0");

	if(skinned)
		u_bonePals	= new SSC_ShVarDecl(SSCT_uniform, SSCT_vec4, "u_bonePals", cjNULL, 35*4);
		
	u_m4World	= new SSC_ShVarDecl(SSCT_uniform, SSCT_mat4, "u_m4World"); // column major
	u_m4MVP		= new SSC_ShVarDecl(SSCT_uniform, SSCT_mat4, "u_m4MVP"); // column major
	
	vtxProg->AddShVarDecl(a_apos);
	vtxProg->AddShVarDecl(a_nrm);
	vtxProg->AddShVarDecl(a_color);
	vtxProg->AddShVarDecl(a_tex0);
	vtxProg->AddShVarDecl(a_bw);
	vtxProg->AddShVarDecl(a_bi);
	vtxProg->AddShVarDecl(v_color);
	vtxProg->AddShVarDecl(v_tex0);
	vtxProg->AddShVarDecl(u_bonePals);
	vtxProg->AddShVarDecl(u_m4World);	
	vtxProg->AddShVarDecl(u_m4MVP);	


	//
	// shader function
	//

	SSC_FnRPtr fnTransformPos = new SSC_Fn("cjSkin4wPos");	

	SSC_VRPtr lpos;		
	SSC_VRPtr bonePals;	
	SSC_VRPtr bi;		
	SSC_VRPtr bw;		

	if(skinned)
	{
		lpos		= fnTransformPos->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec3, "lpos"));
		bonePals	= fnTransformPos->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec4, "bonePals", cjNULL, -1));
		bi			= fnTransformPos->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec4, "bi"));
		bw			= fnTransformPos->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec4, "bw"));

		fnTransformPos->AddCmd(new SSC_V(
			"mat3 srMat3; \n"
			"vec3 t; \n"
			"vec3 p = vec3(0,0,0); \n"
			" \n"
			"for(int i=0; i<4; i++) \n"
			"{ \n"
			"	int index = int(bi[i]) * 4; \n"
			"   \n"
			"   \n"			
			"	srMat3 = mat3(bonePals[index].xyz, \n"
			"			  bonePals[index+1].xyz,  \n"
			"			  bonePals[index+2].xyz ); \n"
			" \n"
			"	t = bonePals[index+3].xyz; \n"
			" \n"
			"  \n"
			"   // srMat3 is column major matrix by mat3(..), so use vec x mat w \n"
			"	p = p + ( ((srMat3 * lpos) + t) * bw[i] ); \n"
			"}"));

		// OpenGL uses column-major matrices
		/*
			matrix bonePal[4][4]; // row-major
			// feeded as rom-major matrix
			vec4 bonePals[0] = AXx, AXy, AXz, AXw
			vec4 bonePals[1] = AYx, AYy, AYz, AYw
			vec4 bonePals[2] = AZx, AZy, AZz, AZw
			vec4 bonePals[3] = Tx,   Ty, Tz,  1

			// in glsl, mat3, set columns of matrix
			mat4(bonePals[0], bonePals[1], bonePals[2], bonePals[3])
			=>  AXx  AYx AZx Tx       vX
			    AXy  AYy AZy Ty   x   vY
				AXz  AYz AZz Tz       vZ  
				AXw  AYw AZw 1        1
					
			    column major matrix  X   column vector 

		*/
		
		fnTransformPos->SetRetParam(new SSC_V(SSCV_FL_RET, SSCT_vec4, cjNULL, "vec4(p, 1.0)"));

		vtxProg->AddShaderFunc(fnTransformPos);
	}
	

	//
	// main
	//

	SSC_FnRPtr fnMain = new SSC_Fn("main");
	SSC_VRPtr vtxout_pos;	
	SSC_VRPtr pos;			
	
	SSC_CallRPtr callTMPos;
	SSC_OperRPtr opMulWorldxPos	;
	SSC_OperRPtr opMulMVPxPos	;

	{
		vtxout_pos		= new SSC_V(SSCV_FL_VSH_OUT, SSCT_vec4, "gl_Position");
		pos				= new SSC_V(SSCV_FL_LOCAL, SSCT_vec4, "pos");

		


		/*
			Prog												      Shader
			ColumnMajor(GL) -> glUniformMatrix4vf(noTranspose) ->   ColumnMajor
			
			
			RowMajor(DX)    -> glUniformMatrix4vf(noTranspose) ->   RowMajor

			Column Marix  X  Colum Vector
			(Row Major)   X  Colum Vector

	     -> Row Vector    X  RowMajor Matrix

		 -> Colum Vector  X  

			u_m4World x pos
			:  
			  > 1. u_m4World = row major
			  > 2. in shader
			      > matrix x vector
				  > row major matrix             x row vector
				     > row 0 :  x, y, z, 0
					 > row 1 :  x, y, z, 0
					 > row 2 :  x, y, z, 0
					 > row 3 :  t0,t1,t2,0


			pos x u_m4World
			:
			  > 1. u_m4World = row major
			  > 2. in shader
			      > vector x matrix
				  > column x row
				     > row 0 :  x, x, x, t1
					 > row 1 :  y, y, y, t2
					 > row 2 :  z, z, z, t3

		*/


		// u_m4World, u_m4MVP is treated as row major (vector x matrix)
//		SSC_OperRPtr opMulWorldxPos = new SSC_Oper(SSC_Oper::EOP_MUL, pos, u_m4World);
//		SSC_OperRPtr opMulMVPxPos = new SSC_Oper(SSC_Oper::EOP_MUL, pos, u_m4MVP);

		opMulWorldxPos		= new SSC_Oper(SSC_Oper::EOP_MUL, u_m4World, pos);
		opMulMVPxPos		= new SSC_Oper(SSC_Oper::EOP_MUL, u_m4MVP, pos);


		if(v_color)
			fnMain->AddCmd(new SSC_Let(v_color, a_color));

		if(v_tex0)
			fnMain->AddCmd(new SSC_Let(v_tex0, a_tex0));

		if(skinned)
		{
			// vec4 sp = cjTMPos(..)
			callTMPos = new SSC_Call(fnTransformPos, a_apos, SSCV_SIWZ_x0 | SSCV_SIWZ_y1 | SSCV_SIWZ_z2, u_bonePals, 0, a_bi, 0, a_bw, 0, cjNULL);
			fnMain->AddCmd(new SSC_Let(SSC_Let::FL_LOCAL_DECL, pos, callTMPos));
		}
		else
		{
			fnMain->AddCmd(new SSC_Let(SSC_Let::FL_LOCAL_DECL, pos, a_apos));
		}

		// pos = pos * u_m4World;
		fnMain->AddCmd(new SSC_Let(pos, opMulWorldxPos));

		// gl_Position = pos * u_m4MVP;
		fnMain->AddCmd(new SSC_Let(vtxout_pos, opMulMVPxPos));
	}
		
	vtxProg->SetShaderMain(fnMain);
	
	SSC_Buf buf;	
	vtxProg->Generate(&buf);

	cjStringA vtxShaderName;
	vtxDesc->GetFlagStringA(vtxShaderName);
		
	rstVtxShader = new cjFVertexShader(cjFShader::ESHADER_FROM_STR, buf.GetBuf(), *vtxShaderName);

	return rstVtxShader;
}

static void SetupSSCVSamplerTexCoord(SSC_VRPtr texcoords[], cjINT32 index)
{
	if(index <= 0)
		return;

	index--; // 0:none, 1:texcoord0 ..

	if(index < cjDefaultShaderPxlProgDesc::MAX_TEXCOORD_COUNT)
	{
		if(!texcoords[index])
		{
			texcoords[index] = new SSC_ShVarDecl(SSCT_varying, SSCT_vec4, 
					SSC_Element::ToStr((SSC_TOKEN)((cjINT32)SSCT_v_texcoord0+index)));
		}
	}
}

static void SetupSSCVSampler(SSC_VRPtr texsamplers[], cjINT32 index)
{
	if(index <= 0)
		return;

	index--; // 0:none, 1:texcoord0 ..

	if(index < cjDefaultShaderPxlProgDesc::MAX_TEXCOORD_COUNT)
	{
		if(!texsamplers[index])
		{
			texsamplers[index] = new SSC_ShVarDecl(SSCT_uniform, SSCT_sampler2D, 
					SSC_Element::ToStr((SSC_TOKEN)((cjINT32)SSCT_s_texsamp0+index)));
		}
	}
}

cjFPixelShader* cjGLShaderProgSnippet_ShaderMtlDefault::GeneratePxlProgram(const cjDefaultShaderPxlProgDesc* pxlDesc)
{
	cjAssert(SSC_V::ms_totalCount == 0);

	cjLOG("Begin Generate Pxl Program");

	cjFPixelShader* pxlShader = _GeneratePxlProgram(pxlDesc);

	cjLOG("End Generate Pxl Program");

	cjAssert(SSC_V::ms_totalCount == 0);

	return pxlShader;
}

cjFPixelShader* cjGLShaderProgSnippet_ShaderMtlDefault::_GeneratePxlProgram(const cjDefaultShaderPxlProgDesc* pxlDesc)
{
	SSCE_PxlProgRPtr pxlProg = new SSCE_PxlProg;

		// varying input
	SSC_V* v_pos		= cjNULL;
	SSC_V* v_nrm		= cjNULL;
	SSC_V* v_color		= cjNULL;
	SSC_VRPtr v_texcoords[cjDefaultShaderPxlProgDesc::MAX_TEXCOORD_COUNT];
	SSC_VRPtr s_texsamplers[cjDefaultShaderPxlProgDesc::MAX_SAMPLER_COUNT];
	
	// color
	if(pxlDesc->GetIN_VTX_COLOR())
	{
		v_color = new SSC_ShVarDecl(SSCT_varying, SSCT_vec4, "v_color");
	}
	
	// texcoord
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP0_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP1_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP2_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP3_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP4_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP5_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP6_TEXCOORD());
	SetupSSCVSamplerTexCoord(v_texcoords, pxlDesc->GetSAMP7_TEXCOORD());

	for(cjINT32 i=0; i<cjDefaultShaderPxlProgDesc::MAX_TEXCOORD_COUNT; i++)
		pxlProg->AddShVarDecl(v_texcoords[i]);
	
	// sampler, s_texsamp0:TEX_DIFFUSE
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_DIFFUSE_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_ENV_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_DETAIL_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_NORMAL_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_SPECULAR_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_GLOW_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_EXT0_CNT());
	SetupSSCVSampler(s_texsamplers, pxlDesc->GetTEX_EXT1_CNT());

	for(cjINT32 i=0; i<cjDefaultShaderPxlProgDesc::MAX_SAMPLER_COUNT; i++)
		pxlProg->AddShVarDecl(s_texsamplers[i]);

//	pxlProg->AddShVarDecl(v_pos);
//	pxlProg->AddShVarDecl(v_nrm);
	pxlProg->AddShVarDecl(v_color);

	//
	// main
	//

	SSC_FnRPtr fnMain = new SSC_Fn("main");

	cjBool useDefaultPxlOutCode = cjFalse;

	SSC_VRPtr pxl_color = new SSC_V(SSCV_FL_VSH_OUT, SSCT_vec4, "gl_FragColor");

	{		
		if(s_texsamplers[0] && v_texcoords[0])
		{
			SSC_VRPtr ltexColor0 = new SSC_V(SSCV_FL_LOCAL, SSCT_vec4, "ltexColor0");

			// vec4 ltexColor0 = texture2D(s_texsamp0, v_texcoord0);
			fnMain->AddCmd(new SSC_Let(SSC_Let::FL_LOCAL_DECL, ltexColor0, 
							new SSC_Call(new SSC_Fn("texture2D"), s_texsamplers[0], 0, v_texcoords[0], 0, cjNULL)));
		
			// gl_FragColor.rgb = ltexColor0.rgb;
			fnMain->AddCmd(new SSC_Let(pxl_color, SSCV_SIWZ_rgb, ltexColor0, SSCV_SIWZ_rgb));
		
			// gl_FragColor.a = ltexColor0.a;
			fnMain->AddCmd(new SSC_Let(pxl_color, SSCV_SIWZ_a, ltexColor0, SSCV_SIWZ_a));
		}
		else
		{
			// no texture is there
			useDefaultPxlOutCode = cjTrue;
		}
	}

	if(useDefaultPxlOutCode)
	{
		SSC_VRPtr defColor = new SSC_V(SSCV_FL_LOCAL, SSCT_vec4, "defColor", "vector(1,1,1,1)");
		fnMain->AddCmd(new SSC_Let(SSC_Let::FL_LOCAL_DECL, defColor, v_color));
		fnMain->AddCmd(new SSC_Let(pxl_color, defColor));
	}

	pxlProg->SetShaderMain(fnMain);
	
	SSC_Buf buf;	
	pxlProg->Generate(&buf);

	cjStringA pxlShaderName;
	pxlDesc->GetFlagStringA(pxlShaderName);

	return new cjFPixelShader(cjFShader::ESHADER_FROM_STR, buf.GetBuf(), *pxlShaderName);
}

