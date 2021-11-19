#pragma once

#include "Collider.h"
#include "VIBuffer.h"
#include "VIBuffer_LineBox.h"


BEGIN(Engine)

class ENGINE_DLL CBoxCollider final : public CCollider
{
private:
	explicit CBoxCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider() = default;
public:
	static CBoxCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual HRESULT SetUpDebugLine() override;
	virtual void SetUpPhysX() override;
public:
	void SetSize(_float3 vSize);

private:
	CEngine*		m_pEngine;
	_float3			m_vSize;
};

END