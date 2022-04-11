
#ifndef _CJSCENECAMERA_H_
#define _CJSCENECAMERA_H_


/*
	default camera axis

	direction = 1,0,0
	up        = 0,1,0
	right     = 0,0,1
*/


class CJENGINE_API cjSceneCamera : public cjSceneNode
{
	CJSER_DECL_CLASS(cjSceneCamera, cjSceneNode, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneCamera)

protected:
    virtual void UpdateWorldData();
    virtual void UpdateWorldBound();

public:
    const cjVec3& GetWorldLocation() const;
	const cjVec3& GetWorldLookAt() const;
    cjVec3 GetWorldDir() const;
    cjVec3 GetWorldUp() const;
    cjVec3 GetWorldRight() const;

	// CameraMatrix
    const cjMatrix* GetWorldToCamMatrix() const;

    void SetViewFrustum(const cjFrustum& frustum);
    const cjFrustum& GetViewFrustum() const;

	void SetNear(cjFLOAT nearPlane);
	void SetFar(cjFLOAT farPlane);
	void SetAspect(cjFLOAT aspect);
	void SetFov(cjFLOAT fov);

    void SetViewPort(const cjRect<cjFLOAT>& port);
    const cjRect<cjFLOAT>& GetViewPort() const;

protected:
    void WorldToCameraMatrix();

protected:
	cjMatrix							m_worldToCam;
    cjFrustum							m_frustum;
	cjFLOAT								m_near;
	cjFLOAT								m_far;
	cjFLOAT								m_aspect;
	cjRect<cjFLOAT>						m_viewport;
};


cjRefPtrDefine(cjSceneCamera)


#endif
