#pragma once

#include "Component.h"
#include "Engine.h"
#include "Transform.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	typedef struct tagRigidBody
	{
		_bool bEnabled;
		_bool bGravity;
		_bool bKinematic;
	}RIGIDBODYDESC;

public:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

public:
	virtual HRESULT SetUpDebugLine() PURE;
	virtual void SetUpRigidActor(void* pShapeInfo, RIGIDBODYDESC desc) PURE;

public:
	RIGIDBODYDESC&	GetRigidBodyDesc() { return m_RigidBodyDesc; }

protected:
	CComponent*		m_pDebugLine;
	CTransform*		m_pObjTransform;
	PxRigidActor*	m_pRigidActor;
	RIGIDBODYDESC	m_RigidBodyDesc;
	//protected:
	//	PxPhysics*					m_pPhysics;
	//	PxControllerManager*		m_pControllerManager;
};

END