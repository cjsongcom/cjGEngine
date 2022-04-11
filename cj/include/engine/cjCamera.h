
#ifndef _CJCAMERA_H_
#define _CJCAMERA_H_


class CJENGINE_API cjCamera : public cjRefObj
{
	CJSER_DECL_CLASS(cjCamera, cjRefObj, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjCamera)

public:
	enum EAxis
	{
		EAXIS_RIGHT,
		EAXIS_UP,
		EAXIS_DIR,
		EAXIS_MAX
	};
	
CJFLAG_DECL_BEGIN
		MASK_DIRTY_VIEW_MAT = 0x00000001,	// 1bit
		BPOS_DIRTY_VIEW_MAT = 0,

		MASK_ATTENU			= 0x00000006,	// 2bit
		BPOS_ATTENU			= 1,
CJFLAG_DECL_END


public:
	cjCamera();

	const cjVec3& GetRight() { return m_axis[EAXIS_RIGHT]; }
	const cjVec3& GetUp()	{ return m_axis[EAXIS_UP]; }
	const cjVec3& GetDir()	{ return m_axis[EAXIS_DIR]; }

	const cjVec3& GetEye()	{ return m_eye; }
	const cjVec3& GetLookAt() { return m_lookAt; }

	void SetDefault();

	// Movement, Rotation
	void SetCamera(const cjVec3& eye, const cjVec3& lookat, const cjVec3* _up=NULL);
	void SetEyePos(const cjVec3& eye);
	void SetLookAt(const cjVec3& lookAt);

		// with axis
	void Rotate(cjFLOAT yaw, cjFLOAT pitch, cjFLOAT roll, cjBool isDelta=cjTrue);
	void Move(cjFLOAT dir, cjFLOAT right, cjFLOAT up, cjBool isDelta=cjTrue);

	// Near, Far, Fov,Aspect
	void SetNearFar(cjFLOAT n, cjFLOAT f);

	void SetFov(cjFLOAT fov, cjFLOAT aspect/* hor:vert */); // vertical fov
	void SetFovY(cjFLOAT fov, cjFLOAT aspect/* hor:vert */); // vertical fov

	// viewport
	void SetViewport(cjINT32 vx, cjINT32 vy, cjINT32 vw, cjINT32 vh, cjINT32 totalW, cjINT32 totalH);

	// utility
	//static void CalcCameraAxis(cjVec3& right, cjVec3& up, cjVec3& dir, 
	//	cjFLOAT pitch, cjFLOAT yaw, cjFLOAT roll, cjBool isDelta, cjBool noInverse=cjFalse);

	static void CalcCameraAxisByEyeLookAt(cjVec3& right, cjVec3& up, cjVec3& dir,
		const cjVec3& eye, const cjVec3& lookAt, const cjVec3* _up=NULL);

	const cjMatrix* GetViewMatrix() { ClearDirty(); return &m_viewMatrix; }

	//
	// Flag
	//
	void SetDirtyView(cjBool dirty=cjTrue) 
	{ 
		_SetFlagAsBool(MASK_DIRTY_VIEW_MAT, BPOS_DIRTY_VIEW_MAT, dirty); 
	}

	cjBool IsDirtyView() 
	{ 
		return _GetFlagAsBool(MASK_DIRTY_VIEW_MAT, BPOS_DIRTY_VIEW_MAT); 
	}

	void ClearDirty();
	
public:
	const static cjVec3				BASE_AXIS[EAXIS_MAX];

private:	
	cjVec3							m_axis[EAXIS_MAX];

	cjVec3							m_eye;
	cjVec3							m_lookAt;

	cjVec3							m_rollUP;


	cjMatrix						m_viewMatrix;
	
	cjFrustum						m_frustum;		// projection area
	cjRect<cjFLOAT>					m_port;			// screen-space viewport

	cjTFLAG							m_flag;

};

cjRefPtrDefine(cjCamera)


#endif
