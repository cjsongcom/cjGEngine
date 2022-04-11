
#include "cjEnginePCH.h"


//
// cjCamera
//

CJSER_IMPL_CLASS(cjCamera, cjSerializable)

CJSER_IMPL_SERIALIZE_BEGIN(cjCamera)

	if(ser.IsLoadOrSaving())
	{
		CJFLAG_SERIALIZE

		for(cjAryOff i=0; i<EAXIS_MAX; i++)
			ser << m_axis[i];

		ser << m_eye;
		ser << m_lookAt;
		ser << m_viewMatrix;
		ser << m_frustum;
		ser << m_port;
	}

CJSER_IMPL_SERIALIZE_END(cjCamera)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjCamera)

	CJFLAG_CLONE

	for(cjAryOff i=0; i<EAXIS_MAX; i++)
		d->m_axis[i] = m_axis[i];

	d->m_eye = m_eye;
	d->m_lookAt = m_lookAt;
	d->m_viewMatrix = m_viewMatrix;
	d->m_frustum = m_frustum;
	d->m_port = m_port;
	
CJSER_IMPL_CLONE_PROPCOPY_END(cjCamera)

CJSER_IMPL_CLONE(cjCamera)


//const cjVec3 cjCamera::BASE_AXIS[] = { cjVec3(0,0,1), cjVec3(0,1,0), cjVec3(1,0,0) };
const cjVec3 cjCamera::BASE_AXIS[] = { cjVec3(1,0,0), cjVec3(0,1,0), cjVec3(0,0,1) };


cjCamera::cjCamera()
{
	CJFLAG_IMPL_INIT

	m_rollUP = BASE_AXIS[EAXIS_UP];
	m_flag = 0;
}

cjCamera::~cjCamera()
{
}

void cjCamera::SetDefault()
{
	cjMemCpy(m_axis, BASE_AXIS, sizeof(BASE_AXIS));
	
	m_eye = cjVec3(0,0,0);
	m_lookAt = cjVec3(0, -100, 150);

	m_rollUP = BASE_AXIS[EAXIS_UP];

	// 45' aspect=1.0f
	m_frustum.SetFov((cjFLOAT)(CJ_PI/4.0f), 1.3f);
	m_frustum.m_near = 0.01f;
	m_frustum.m_far = 1000.0f;
	m_frustum.m_ortho = cjFalse;

	m_port.m_left	= 0.0f;
	m_port.m_right	= 1.0f;
	m_port.m_top	= 1.0f;
	m_port.m_bottom = 0.0f;

	CJFLAG_IMPL_INIT

	SetDirtyView();
	ClearDirty();
}

void cjCamera::SetCamera(const cjVec3& eye, const cjVec3& lookat, const cjVec3* _up)
{	
	m_eye = eye;
	m_lookAt = lookat;

	if(_up)
		m_rollUP = *_up;

	SetDirtyView();
}

void cjCamera::SetEyePos(const cjVec3& eye)
{
	m_eye = eye;
	SetDirtyView();
}

void cjCamera::SetLookAt(const cjVec3& lookAt)
{
	m_lookAt = lookAt;
	SetDirtyView();
}


/*
// http://www.codecolony.de/docs/camera2.htm

The orientation of the camera is described by three vectors: The view direction (like in the first camera), the right vector and the up vector.
Initially the orientation points along the negative z-axis: View Direction (0|0|-1), Right Vector (1|0|0), UpVector (0|1|0).
Let's say, RotateY is called. This does no longer mean that the orientation is rotated around the y-axis. Instead, it is rotated around the up vector.
But what does it mean to rotate the "orientation"? It means to rotate the view direction, the right vector and the up vector

#1 only two vectors must be rotated at each RotateX/Y/Z
#2 all vectors are perpendicular, so one vector always can be computed by the cross product (i.e. we have to rotate only one vector!)
#3 the vector which must be rotated around the straight line is perpendicular to the line's direction
#4 we know the vector which is perpendicular to the vector which must be computed and the line's direction


Let's assume we want to rotate the camera's orientation around the right vector. 
This means, that 
	1. the right vector will not change.(#1) -> up,view vector will be rotated
	2. #2 says, it is enough to rotate the view direction vector.  -> enough to rotate with view vector only
	3. The new up vector can be computed by the cross product 
	   -> cross ( right , new view vector ) = new up vector
	   (we could also rotate the up vector and calculate the view direction vector).
*/

void cjCamera::Rotate(cjFLOAT yaw, cjFLOAT pitch, cjFLOAT roll, cjBool isDelta)
{
	cjWrap2XPI(yaw); cjWrap2XPI(pitch); cjWrap2XPI(roll);
	
	cjBool dirtyView = cjFalse;
	
	if(yaw != 0.0f)
	{
		cjVec3* dir;
		
		// rotate view vec by up vector(yaw)
		dir = cjVec3::RotateByAxis(m_axis[EAXIS_DIR], m_axis[EAXIS_DIR], m_axis[EAXIS_UP], yaw);
		dir->Normalize();

		// new right vector = cross(view, up)
		// updated by ClearDirty();
//		m_axis[EAXIS_RIGHT] = cjVec3::Cross(*dir, m_axis[EAXIS_UP]);
//		m_axis[EAXIS_RIGHT].Normalize();

		dirtyView = cjTrue;
	}

	if(pitch != 0.0f)
	{				
		cjVec3* dir;
		
		// rotate view vec by right vector(pitch)
		dir = cjVec3::RotateByAxis(m_axis[EAXIS_DIR], m_axis[EAXIS_DIR], m_axis[EAXIS_RIGHT], pitch);
		dir->Normalize();

		// new up vector = cross(view, right)
		// updated by ClearDirty();
	//	m_axis[EAXIS_UP] = cjVec3::Cross(*dir, m_axis[EAXIS_RIGHT]);
	//	m_axis[EAXIS_UP].Normalize();
		
		dirtyView = cjTrue;
	}

	if(roll != 0.0f)
	{
		cjVec3* up;

//		cjVec3::RotateByAxis(m_rollUP, m_axis[EAXIS_UP], m_axis[EAXIS_DIR], roll);
		up = cjVec3::RotateByAxis(m_rollUP, m_rollUP, m_axis[EAXIS_DIR], roll);
		up->Normalize();
		
		//// rotate right vec by dir vector(roll)
		//right = cjVec3::RotateByAxis(m_axis[EAXIS_RIGHT], m_axis[EAXIS_RIGHT], m_axis[EAXIS_DIR], roll);
		//right->Normalize();

		//// new up vector = cross(view, right)
		//// updated by ClearDirty();
		//m_axis[EAXIS_UP] = cjVec3::Cross(*right, m_axis[EAXIS_DIR]);
		//m_axis[EAXIS_UP].Normalize();

		dirtyView = cjTrue;
	}
	
	if(dirtyView)
	{
		m_lookAt = m_eye + ( (m_axis[EAXIS_DIR] * cjVec3(m_lookAt - m_eye).Length()) );
		SetDirtyView();
	}

}

void cjCamera::Move(cjFLOAT dir, cjFLOAT right, cjFLOAT up, cjBool isDelta)
{
	//cjVec3 toAdd = cjVec3(0.0f,0.0f,0.0f);

//	cjVec3 toAdd = cjVec3(rightDelta, upDelta, dirDelta);
	cjVec3 f = cjVec3(0,0,0);

	if(isDelta)
	{
		if(dir != 0.0f)	f += m_axis[EAXIS_DIR]	 * dir;
		if(right != 0.0f)	f += m_axis[EAXIS_RIGHT] * right;
		if(up != 0.0f)		f += m_axis[EAXIS_UP]	 * up;
	}
	else
	{
		if(dir != 0.0f)	f += BASE_AXIS[EAXIS_DIR]	* dir;
		if(right != 0.0f)	f += BASE_AXIS[EAXIS_RIGHT] * right;
		if(up != 0.0f)		f += BASE_AXIS[EAXIS_UP]	* up;
	}

	m_eye += f;
	m_lookAt += f;

	SetDirtyView();
}

void cjCamera::SetNearFar(cjFLOAT n, cjFLOAT f)
{
	m_frustum.m_near = n;
	m_frustum.m_far = f;
}

void cjCamera::SetFov(cjFLOAT fov, cjFLOAT aspect)
{
	m_frustum.SetFov(fov, aspect);
}

void cjCamera::SetFovY(cjFLOAT fov, cjFLOAT aspect)
{
	m_frustum.SetFovY(fov, aspect);
}

void cjCamera::SetViewport(cjINT32 vx, cjINT32 vy, cjINT32 vw, cjINT32 vh, 
	cjINT32 totalW, cjINT32 totalH)
{
	cjCalcViewportArea(m_port, vx, vy, vw, vh, totalW, totalH);
}
//
//void cjCamera::CalcCameraAxis(cjVec3& right, cjVec3& up, cjVec3& dir, 
//	cjFLOAT pitch, cjFLOAT yaw, cjFLOAT roll, cjBool isDelta, cjBool noInverse)
//{
////	cjMatrix3 matRotAxis;
////
////	if(isDelta)
////	{
////		up = BASE_AXIS[EAXIS_UP];
////
////		// apply pitch	
////		matRotAxis.MakeAxisRot(pitch, BASE_AXIS[EAXIS_RIGHT]);
////		dir		= matRotAxis * dir;
////
////		// apply yaw
////		matRotAxis.MakeAxisRot(yaw, BASE_AXIS[EAXIS_UP]);
////		dir		= matRotAxis * dir;
////
////		right = dir.Cross(up); right.Normalize();
////		up = right.Cross(dir);
////	}
////	else
////	{		
////		up = BASE_AXIS[EAXIS_UP];
////
////		// apply pitch	
////		matRotAxis.MakeAxisRot(pitch, BASE_AXIS[EAXIS_RIGHT]);
////		dir = matRotAxis * BASE_AXIS[EAXIS_DIR];
////		if(noInverse) up = matRotAxis * BASE_AXIS[EAXIS_UP];
////
////		// apply yaw
////		matRotAxis.MakeAxisRot(yaw, BASE_AXIS[EAXIS_UP]);
////		dir = matRotAxis * dir;
////		if(noInverse) up = matRotAxis * up;
////
////		// build right
////		right = dir.Cross(up); right.Normalize();
////		up = right.Cross(dir);
////	}
////
//}

//void cjCamera::CalcCameraAxisByEyeLookAt(cjVec3& right, cjVec3& up, cjVec3& dir,
//	const cjVec3& eye, const cjVec3& lookAt, const cjVec3* _up)
//{
//	if(!_up) _up = &BASE_AXIS[EAXIS_UP];
//
//	dir	  = lookAt - eye;    dir.Normalize();
//	right = dir.Cross(*_up);  right.Normalize();
//	up	  = right.Cross(dir);
//}

void cjCamera::ClearDirty()
{		
	if(!IsDirtyView())
		return;
		
	cjMatViewLookAtRH(m_viewMatrix, m_eye, m_lookAt, m_rollUP,
		&m_axis[EAXIS_RIGHT], &m_axis[EAXIS_UP], &m_axis[EAXIS_DIR]);

	SetDirtyView(cjFalse);
}

