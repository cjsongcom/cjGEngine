
#include "cjEnginePCH.h"
#include "cjSceneCamera.h"


CJSER_IMPL_CLASS(cjSceneCamera, cjSceneNode)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneCamera)

	if(ser.IsQuerySerObjAndStr())
	{
	}
	else if(ser.IsSaving() || ser.IsLoading())
	{
		ser << m_worldToCam;
		ser << m_frustum;
		ser << m_near;
		ser << m_far;
		ser << m_aspect;
		ser << m_viewport;
	}

CJSER_IMPL_SERIALIZE_END(cjSceneCamera)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneCamera)

	d->m_worldToCam = m_worldToCam;
	d->m_frustum	= m_frustum;
	d->m_near		= m_near;
	d->m_far		= m_far;
	d->m_aspect		= m_aspect;
	d->m_viewport	= m_viewport;

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneCamera)

CJSER_IMPL_CLONE(cjSceneCamera)


static cjVec3 g_tempVec;

void cjSceneCamera::UpdateWorldData()
{

}

void cjSceneCamera::UpdateWorldBound()
{

}

cjSceneCamera::~cjSceneCamera()
{

}

const cjVec3& cjSceneCamera::GetWorldLocation() const
{
	
	return g_tempVec;
}

const cjVec3& cjSceneCamera::GetWorldLookAt() const
{
	return g_tempVec;
}

cjVec3 cjSceneCamera::GetWorldDir() const
{
	return g_tempVec;
}

cjVec3 cjSceneCamera::GetWorldUp() const
{
	return g_tempVec;
}

cjVec3 cjSceneCamera::GetWorldRight() const
{
	return g_tempVec;
}

const cjMatrix* cjSceneCamera::GetWorldToCamMatrix() const
{
	return NULL;
}

void cjSceneCamera::SetViewFrustum(const cjFrustum& frustum)
{

}

const cjFrustum& cjSceneCamera::GetViewFrustum() const
{
	static cjFrustum fr;
	return fr;
}

void cjSceneCamera::SetNear(cjFLOAT nearPlane)
{

}

void cjSceneCamera::SetFar(cjFLOAT farPlane)
{

}

void cjSceneCamera::SetAspect(cjFLOAT aspect)
{

}

void cjSceneCamera::SetFov(cjFLOAT fov)
{

}

void cjSceneCamera::SetViewPort(const cjRect<cjFLOAT>& port)
{

}

const cjRect<cjFLOAT>& cjSceneCamera::GetViewPort() const
{
	static cjRect<cjFLOAT> fl;
	return fl;
}

void cjSceneCamera::WorldToCameraMatrix()
{

}

