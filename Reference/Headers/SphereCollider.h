#pragma once

#include "Collider.h"
#include "VIBuffer.h"
#include "VIBuffer_LineSphere.h"


BEGIN(Engine)

class ENGINE_DLL CSphereCollider final : public CCollider
{
private:
	explicit CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;
public:
	static CSphereCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual HRESULT SetUpDebugLine() override;
	virtual void SetUpPhysX() override;
public:
	void SetSize(float fRadius);
	float GetSize() { return m_fRadius; }

private:
	CEngine*		m_pEngine;
	float			m_fRadius;
};

END