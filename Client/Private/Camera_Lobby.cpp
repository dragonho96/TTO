#include "stdafx.h"
#include "..\public\Camera_Lobby.h"
#include "Engine.h"

USING(Client)
CCamera_Lobby::CCamera_Lobby(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Lobby::CCamera_Lobby(const CCamera_Lobby & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Lobby::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Lobby::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vCurPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	m_vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	m_vCurTargetPos = { 0.f, };
	m_vTargetPos = { 0.f, };



	return S_OK;
}

_uint CCamera_Lobby::Update(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	_float fSpeed = TimeDelta * 3.f;

	m_vCurPos = XMVectorLerp(m_vCurPos, m_vPos, fSpeed);
	m_vCurTargetPos = XMVectorLerp(m_vCurTargetPos, m_vTargetPos, fSpeed);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vCurPos);
	m_pTransformCom->LookAtForLandObject(m_vCurTargetPos);

	_matrix rotation = XMMatrixRotationY(XMConvertToRadians(-15.f));
	m_pTransformCom->SetMatrix(rotation * m_pTransformCom->GetWorldMatrix());

	_vector pos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	ADDLOG((to_string(XMVectorGetX(pos)) + ",  " + to_string(XMVectorGetX(pos))).c_str());

	__super::Update(TimeDelta);
	return _uint();
}

_uint CCamera_Lobby::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	return _uint();
}

HRESULT CCamera_Lobby::SetUp_Components()
{

	return S_OK;
}

void CCamera_Lobby::SetTarget(_vector vPos, _vector vTargetPos)
{
	m_vPos = vPos;
	m_vTargetPos = vTargetPos;
}

CCamera_Lobby * CCamera_Lobby::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Lobby*		pInstance = new CCamera_Lobby(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CCamera_Lobby");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Lobby::Clone(void * pArg)
{
	CCamera_Lobby*		pInstance = new CCamera_Lobby(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CCamera_Lobby");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCamera_Lobby::Free()
{
	__super::Free();

}
