#pragma once

#include "Camera.h"

BEGIN(Tool)
class CCamera_Tool final : public CCamera
{
public:
	explicit CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Tool(const CCamera_Tool& rhs);
	virtual ~CCamera_Tool() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;

private:
	HRESULT SetUp_Components();

public:
	static CCamera_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END