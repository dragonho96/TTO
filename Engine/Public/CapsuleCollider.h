#pragma once

#include "Collider.h"
#include "VIBuffer.h"
#include "VIBuffer_LineCapsule.h"


BEGIN(Engine)

class ENGINE_DLL CCapsuleCollider final : public CCollider
{
private:
	explicit CCapsuleCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCapsuleCollider(const CCapsuleCollider& rhs);
	virtual ~CCapsuleCollider() = default;
public:
	static CCapsuleCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual HRESULT SetUpDebugLine() override;
	virtual void SetUpPhysX() override;
public:
	void SetSize(float fRadius, float fHeight);

private:
	CEngine*		m_pEngine;
	float			m_fRadius;
	float			m_fHeight;
};

END