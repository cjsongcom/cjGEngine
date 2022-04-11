
#include "cjEnginePCH.h"
#include "cjRenDev.h"


//
// cjRenDevParam
//

cjRenDevParam::cjRenDevParam()
{
	SetDefault();

	m_focusWnd					= NULL;

	m_renderWnd					= NULL;
	m_renderWndWidth		= 0;
	m_renderWndHeight		= 0;

	m_fullScreen				= cjFalse;	
	m_useStencil				= cjFalse;
    
#if defined(CJ_PLATFORM_APPLE)
    m_eaglAdapter           = NULL;
#endif
}

cjRenDevParam::cjRenDevParam(const cjRenDevParam& rhs)
{
	*this = rhs;
}

cjRenDevParam& cjRenDevParam::operator= (const cjRenDevParam& rhs)
{
	if(this != &rhs)
		memcpy(this, &rhs, sizeof(cjRenDevParam));

	return *this;
}

void cjRenDevParam::SetDefault()
{
	m_format		= cjRSurf_RGBA8;
	m_depthStencil	= cjRSurf_D24X8;
	m_swapEffect	= cjRFB_SWAPEFF_DISCARD;

	m_interval		= cjRPRESENT_INTERVAL_ONE;
	m_refreshRate	= 0;
}


//
// cjRenDev
//

CJOBJ_IMPL_CLASS(cjRenDev)

cjRenDev::cjRenDev()
{
    m_numMaxTextureStages = 2;
}

void cjRenDev::OnDestroy()
{
	cjSuper::OnDestroy();
}

cjBool cjRenDev::Init(const cjRenDevParam* renDevPrm)
{
	m_defaultShaderMtl = GenerateDefaultShaderMtl();
	m_defaultMtl = GenerateDefaultMtl();
	
	return cjTrue;
}

void cjRenDev::Shutdown()
{
	if(m_defaultShaderMtl)
	{
		delete m_defaultShaderMtl;
		m_defaultShaderMtl = cjNULL;
	}

	if(m_defaultMtl)
	{
		delete m_defaultMtl;
		m_defaultMtl = cjNULL;
	}
}

// camera
void cjRenDev::SetCameraData(const cjCamera* camera)
{

}

void cjRenDev::SetCameraData(const cjVec3& eye, const cjVec3& dir, const cjVec3& up, 
	const cjVec3& right, const cjFrustum& frustum, const cjRect<cjFLOAT>& port)
{

}

void cjRenDev::SetScreenSpaceCameraData(const cjRect<cjFLOAT>* port)
{

}

void cjRenDev::GetCameraData(cjCamera& camera)
{

}

void cjRenDev::GetCameraData(cjVec3& eye, cjVec3& dir, cjVec3& up, cjVec3& right, 
	cjFrustum& frustum, cjRect<cjFLOAT>& port)
{

}

cjShaderMtl* cjRenDev::GetDefaultShaderMtl()
{
	if(!m_defaultShaderMtl)
		m_defaultShaderMtl = GenerateDefaultShaderMtl();

	return m_defaultShaderMtl;
}

const cjNameChar* cjRenDev::GetDefaultShaderMtlName(cjBool nameOnly)
{
	if(nameOnly)
		return _CT("DefaultShaderMtl");
	else
		return _CT("cjEngine.DefaultShaderMtl");
}

cjShaderMtl* cjRenDev::GenerateDefaultShaderMtl()
{
	cjObjPackage* pkgEngine = cjObj::cjObjCreatePkg(cjNULL, cjEngine_PkgName);

	cjShaderMtl* shaderMtl = (cjShaderMtl*)cjObjFind(cjShaderMtl::cjObjGetStaticCls(), pkgEngine, 
		GetDefaultShaderMtlName());

	if(shaderMtl)
		return shaderMtl;

	shaderMtl = cjObjNew<cjShaderMtl>(cjShaderMtl::cjObjGetStaticCls(), 
		pkgEngine, GetDefaultShaderMtlName(cjTrue));
	
	return shaderMtl;
}

cjMaterial* cjRenDev::GetDefaultMtl()
{
	cjRenDev* renDev = cjEngine::GetEngine()->GetRenDev();

	if(!renDev->m_defaultMtl)
		renDev->m_defaultMtl = renDev->GenerateDefaultMtl();		

	return renDev->m_defaultMtl;
}

const cjNameChar* cjRenDev::GetDefaultMtlName(cjBool nameOnly)
{
	if(nameOnly)
		return _CT("DefaultMtl");
	else
		return _CT("cjEngine.DefaultMtl");
}

cjMaterial* cjRenDev::GenerateDefaultMtl()
{
	cjObjPackage* pkgEngine = cjObj::cjObjCreatePkg(cjNULL, cjEngine_PkgName);


#if 0
	cjTexture* defaultMtl = (cjTexture*)cjObjFind(cjTexture::cjObjGetStaticCls(), pkgEngine, 
		GetDefaultMtlName());

	if(!defaultMtl)
	{
		defaultMtl = cjObjNew<cjTexture>(cjTexture::cjObjGetStaticCls(), pkgEngine, 
			GetDefaultMtlName(cjTrue));
	}
#else
	cjShaderMtlDefault* defaultMtl = (cjShaderMtlDefault*)cjObjFind(cjShaderMtlDefault::cjObjGetStaticCls(), 
		pkgEngine, GetDefaultMtlName());

	if(!defaultMtl)
	{
		defaultMtl = cjObjNew<cjShaderMtlDefault>(cjShaderMtlDefault::cjObjGetStaticCls(), pkgEngine, 
			GetDefaultMtlName(cjTrue));

		//cjtemp
		cjTexture* tex = cjNULL; //cjObjLoadPkgObj<cjTexture>(NULL, _CT("TestResChar.Cat_TexBody_D"));
		// cjtemp
		cjTexture* tex2 = cjNULL; //cjObjLoadPkgObj<cjTexture>(NULL, _CT("TestResChar.Cat_TexEye_D"));

		//"TestResChar.Cat_TexBody_D"

		defaultMtl->SetTexture(cjSceneRenPropTextures::TEX_DIFFUSE, tex);
		defaultMtl->SetTexture(cjSceneRenPropTextures::TEX_ENV, tex2);

	}

#endif
	
	return defaultMtl;
}

