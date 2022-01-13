#pragma once

#include "Client_Defines.h"
#include "Camera.h"


BEGIN(Client)
class CCamera_Follow final : public CCamera
{
public:
	explicit CCamera_Follow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Follow(const CCamera_Follow& rhs);
	virtual ~CCamera_Follow() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;

private:
	HRESULT SetUp_Components();

public:
	_float GetAngle() { return angle; }
public:
	void Shake_Rifle();
	void Shake_Grenade();
	HRESULT Shake_Grenade_BeginThread();

public:
	CRITICAL_SECTION& Get_CS() { return m_CS; }

private:
	CTransform* m_pPlayerTransform = nullptr;
	CTransform* m_pTargetTransform = nullptr;
	_vector		m_pTargetLook = { 0, 0, 1};

	_float		angle = 0.f;
	_float		targetAngle = 0.f;

	_vector		m_ShakeOffset = { 0.f, 0.f, 0.f };
	_vector		m_CurShakeOffset = { 0.f, 0.f, 0.f };

	_vector		m_GrenadeShakeOffset = { 0.f, 0.f, 0.f };
	_vector		m_CurGrenadeShakeOffset = { 0.f, 0.f, 0.f };
private:
	CRITICAL_SECTION	m_CS;
	HANDLE				m_hThread = 0;
public:
	static CCamera_Follow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END