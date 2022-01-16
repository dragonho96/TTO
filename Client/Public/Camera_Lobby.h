#pragma once

#include "Client_Defines.h"
#include "Camera.h"


BEGIN(Client)
class CCamera_Lobby final : public CCamera
{
public:
	explicit CCamera_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Lobby(const CCamera_Lobby& rhs);
	virtual ~CCamera_Lobby() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;

private:
	HRESULT SetUp_Components();

public:
	void SetTarget(_vector vPos, _vector vTargetPos);

private:
	_vector		m_vCurPos;
	_vector		m_vPos;
	_vector		m_vCurTargetPos;
	_vector		m_vTargetPos;

public:
	static CCamera_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END