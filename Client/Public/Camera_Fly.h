#pragma once

#include "Camera.h"

BEGIN(Client)
class CCamera_Fly final : public CCamera
{
public:
	explicit CCamera_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Fly(const CCamera_Fly& rhs);
	virtual ~CCamera_Fly() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;

private:
	HRESULT SetUp_Components();

public:
	static CCamera_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END