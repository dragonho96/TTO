#pragma once

#include "Component.h"
#include "VIBuffer.h"
#include "VIBuffer_LineSphere.h"
#include "Transform.h"


BEGIN(Engine)

class ENGINE_DLL CSphereCollider final : public CComponent
{
private:
	explicit CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;
public:
	static CSphereCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CSphereCollider* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	void Render();

public:
	HRESULT SetUpDebugLine();
	void SetUpPhysX();

public:
	void SetRadius(float fRadius);
private:
	CEngine*		m_pEngine;
	// Physx info
	// Debug line info
	CComponent*		m_pDebugLineSphere;
	CTransform*		m_pObjTransform;

	float			m_fRadius;
};

END