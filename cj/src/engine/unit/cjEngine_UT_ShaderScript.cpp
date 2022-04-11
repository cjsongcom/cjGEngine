
#include "cjEnginePCH.h"
#include "cjShaderScript.h"


CJUT_DECL(CJENGINE_API, cjEngine, SHADER_SCRIPT_TEST)



void GEN_TEST_SHADER(cjStringA& shader)
{
	SSCE_VtxProgRPtr vtxProg = new SSCE_VtxProg;

	vtxProg->SetName("00000-00000");

	SSC_V* a_apos	= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_position");
	SSC_V* a_nrm	= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec3, "a_normal");
	SSC_V* a_color	= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_color");
	SSC_V* a_tex0	= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec2, "a_texcoord0");

	SSC_V* a_bw		= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_blendweight");
	SSC_V* a_bi		= new SSC_ShVarDecl(SSCT_attribute, SSCT_vec4, "a_blendindices");

	SSC_V* v_color	= new SSC_ShVarDecl(SSCT_varying, SSCT_vec4, "v_color");
	SSC_V* v_tex0	= new SSC_ShVarDecl(SSCT_varying, SSCT_vec2, "v_texcoord");

	SSC_V* u_bonePals = new SSC_ShVarDecl(SSCT_uniform, SSCT_vec4, "u_bonePals", cjNULL, 35*4);
	SSC_V* u_m4MVP	= new SSC_ShVarDecl(SSCT_uniform, SSCT_mat4, "u_m4MVP");
	
	vtxProg->AddShVarDecl(a_apos);
	vtxProg->AddShVarDecl(a_nrm);
	vtxProg->AddShVarDecl(a_color);
	vtxProg->AddShVarDecl(a_tex0);
	vtxProg->AddShVarDecl(a_bw);
	vtxProg->AddShVarDecl(a_bi);
	vtxProg->AddShVarDecl(v_color);
	vtxProg->AddShVarDecl(v_tex0);
	vtxProg->AddShVarDecl(u_bonePals);
	vtxProg->AddShVarDecl(u_m4MVP);
	
	// func
	SSC_FnRPtr fnSkin4W = new SSC_Fn("cjSkin4w");	

	{
		SSC_V* lpos		= fnSkin4W->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec3, "lpos"));
		SSC_V* bonePals = fnSkin4W->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec4, "bonePals", cjNULL, -1));
		SSC_V* bi		= fnSkin4W->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec4, "bi"));
		SSC_V* bw		= fnSkin4W->AddParam(new SSC_V(SSCV_FL_PRM_IN, SSCT_vec4, "bw"));

		fnSkin4W->AddCmd(new SSC_V(
			"mat3 sr; \n"
			"vec3 t; \n"
			"vec3 p = vec3(0,0,0); \n"
			" \n"
			"for(int i=0; i<4; i++) \n"
			"{ \n"
			"	int index = int(bi[i]) * 4; \n"
			" \n"
			"	sr = mat3(bonePals[index].xyz, \n"
			"			  bonePals[index+1].xyz,  \n"
			"			  bonePals[index+2].xyz ); \n"
			" \n"
			"	t = bonePals[index+3].xyz; \n"
			" \n"
			"	p = p + ( ((lpos * sr) + t) * bw[i] ); \n"
			"}"));

		fnSkin4W->SetRetParam(new SSC_V(SSCV_FL_RET, SSCT_vec4, cjNULL, "vec4(p, 1.0)"));
	}

	vtxProg->AddShaderFunc(fnSkin4W);

	
	// main	
	SSC_FnRPtr fnMain = new SSC_Fn("main");

	{
		SSC_V*		vtxout_pos = new SSC_V(SSCV_FL_VSH_OUT, SSCT_vec4, "gl_Position");
		SSC_V*		pos = new SSC_V(SSCV_FL_LOCAL, SSCT_vec4, "sp");
		SSC_Call*	callSkinning = new SSC_Call(fnSkin4W, a_apos, SSCV_SIWZ_x0 | SSCV_SIWZ_y1 | SSCV_SIWZ_z2, 
														  u_bonePals, 0, a_bi, 0, a_bw, 0, cjNULL);
//		SSC_Call*	callSkinning = new SSC_Call(fnSkin4W, "a_position.xyz, blendMatrixAry, a_blendindices, a_blendweight");
		SSC_Oper*	opMulMVPxPos = new SSC_Oper(SSC_Oper::EOP_MUL, u_m4MVP, pos);

		fnMain->AddCmd(new SSC_Let(v_color, a_color));
		fnMain->AddCmd(new SSC_Let(v_tex0, a_tex0));
		fnMain->AddCmd(new SSC_Let(SSC_Let::FL_LOCAL_DECL, pos, callSkinning));
		fnMain->AddCmd(new SSC_Let(vtxout_pos, opMulMVPxPos));
	}
		
	vtxProg->SetShaderMain(fnMain);

	SSC_Buf buf;	
	vtxProg->Generate(&buf);

	shader = buf.GetBuf();
}

//
//static void TestBuf()
//{
//	SSC_Buf bufTest(512);
//
//	bufTest.Print("abcd %s", "hijk");
//	bufTest.Println("aaaabbbccc %s", "ddd");
//	bufTest.AddLine();
//	bufTest.Println("end of line");
//
//	const cjChar* b = bufTest.GetBuf();
//
//	cjAryOff s = bufTest.GetSiz();
//}
//
//static void Test()
//{
//	cjStringA rst;
//
//	SSCE_VtxProg*  vtxProg = new SSCE_VtxProg;
//	vtxProg->AddRef();
//
//
//	
//	
//
//	//
//	// func
//	//
//
//	
//
//
//	//
//	// main
//	//
//
//	SSC_Fn* fnMain = new SSC_Fn();
//
//	{
//		fnMain->SetFuncName("main");
//
//		SSC_V*		vtxout_pos = new SSC_V(SSCV_FL_VSH_OUT, SSCT_vec4, "gl_Position");
//		SSC_V*		pos = new SSC_V(SSCV_FL_LOCAL, SSCT_vec4, "sp");
//		SSC_Call*	callSkinning = new SSC_Call(fnSkin4W, a_apos, SSCV_SIWZ_x0 | SSCV_SIWZ_y1 | SSCV_SIWZ_z2, 
//														  u_bonePals, 0, a_bi, 0, a_bw, 0, cjNULL);
////		SSC_Call*	callSkinning = new SSC_Call(fnSkin4W, "a_position.xyz, blendMatrixAry, a_blendindices, a_blendweight");
//		SSC_Oper*	opMulMVPxPos = new SSC_Oper(SSC_Oper::EOP_MUL, u_m4MVP, pos);
//
//		fnMain->AddCmd(new SSC_Let(v_color, a_color));
//		fnMain->AddCmd(new SSC_Let(v_tex0, a_tex0));
//		fnMain->AddCmd(new SSC_Let(SSC_Let::FL_LOCAL_DECL, pos, callSkinning));
//		fnMain->AddCmd(new SSC_Let(vtxout_pos, opMulMVPxPos));
//		
//		//SSC_V* vcolor = new SSC_V(SSCV_FL_LOCAL, SSCT_vec4 "v_color", )
//		//fnMain->AddLet(new SSC_Let(SSCT_v_color, SSCT_a_color));
//		//fnMain->AddLet(new SSC_Let(SSCT_v_texcoord, SSCT_a_texcoord0));
//		//	
//		
//		//fnMain->AddLet(new SSC_Let(pos, callSkinning));
//		//
//		//SSCE_OperMul* mulMVPxPos = new SSCE_OperMul("modelViewProjectionMatrix", p);
//		//fnMain->AddLet(new SSC_Let(SSCT_gl_Position, mulMVPxPos))
//
//		//fnMain->AddScript(new SSC_V(SSCQ_VAR_SCRIPT, 
//		//	"v_color = a_color; \n"
//		//	"v_texcoord = a_texcoord0; \n"
//		//	"vec4 p = cjSkin4W(a_position.xyz, blendMatrixAry, a_blendindices, a_blendweight); \n"
//		//	"gl_Position = modelViewProjectionMatrix * p; \n"
//		//	));
//	}
//		
//	vtxProg->SetShaderMain(fnMain);
//
//
//	//
//	// generate
//	//
//
//	SSC_Buf buf;
//	
//	vtxProg->Generate(&buf);
//	
//	vtxProg->ReleaseRef();
//}


CJUT_IMPL_BEGIN(CJENGINE_API, cjEngine, SHADER_SCRIPT_TEST)

	cjStringA shader;
	GEN_TEST_SHADER(shader);

CJUT_IMPL_END(CJENGINE_API, cjEngine, SHADER_SCRIPT_TEST)

