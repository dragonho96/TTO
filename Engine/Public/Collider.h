#pragma once

#include "Component.h"
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
		_bool bCC;
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
	virtual _uint	LateUpdate(double deltaTime);
	virtual HRESULT Render();

public:
	virtual HRESULT SetUpDebugLine() PURE;
	virtual void SetUpRigidActor(void* pShapeInfo, RIGIDBODYDESC desc, class CGameObject* obj) PURE;

public:
	RIGIDBODYDESC&	GetRigidBodyDesc() { return m_RigidBodyDesc; }
	PxRigidActor*	GetRigidActor() { return m_pRigidActor; }
	PxController* GetController() { return m_pController; }

public:
	_float3&	GetRelativePos() { return m_vRelativePos; }
	void		SetRelativePos(_float3 center) { m_vRelativePos = center; }

public:
	void ReleaseController();
protected:
	class CEngine*		m_pEngine;

	_float3			m_vRelativePos = { 0.f, 0.f, 0.f };
	RIGIDBODYDESC	m_RigidBodyDesc;

	CComponent*		m_pDebugLine = nullptr;
	CTransform*		m_pObjTransform = nullptr;

	PxRigidActor*	m_pRigidActor = nullptr;
	PxController*	m_pController = nullptr;
	_float4x4		m_pxMat;
	//protected:
	//	PxPhysics*					m_pPhysics;
	//	PxControllerManager*		m_pControllerManager;
};

END