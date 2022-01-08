#include "stdafx.h"
#include "..\Public\Camera_Follow.h"
#include "Engine.h"

USING(Client)
CCamera_Follow::CCamera_Follow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Follow::CCamera_Follow(const CCamera_Follow & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Follow::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Follow::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (list.size() <= 0)
		return E_FAIL;

	CGameObject* m_pGameObject = list.front();
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameObject->GetComponent("Com_Transform"));

	SetUpComponents(0, "Prototype_Transform", "Com_Transform_Target", (CComponent**)&m_pTargetTransform);
	return S_OK;
}

_uint CCamera_Follow::Update(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	m_pTargetTransform->SetState(CTransform::STATE_POSITION, m_pPlayerTransform->GetState(CTransform::STATE_POSITION));

	// Q || E 누르면 m_pTargetLook을 회전시긴다
	if (GetActiveWindow() == g_hWnd && m_bRolling)
	{

		float speed = 8.f;
		if (m_pEngine->IsKeyPressed('Q'))
			targetAngle -= XMConvertToRadians(45.f) * TimeDelta * speed;
		else if (m_pEngine->IsKeyPressed('E'))
			targetAngle += XMConvertToRadians(45.f) * TimeDelta * speed;

		_float	fFollowSpeed = TimeDelta * 15.f;

		angle = Lerp(angle, targetAngle, fFollowSpeed);

		_vector vTargetPos = m_pTargetTransform->GetState(CTransform::STATE_POSITION);

		_vector vInvTargetLook = _vector{ 0, 0, 1 } * -15.f;

		// Right-Axis Rotation
		XMMATRIX matRot;
		matRot = XMMatrixRotationAxis(m_pTargetTransform->GetState(CTransform::STATE_RIGHT), XMConvertToRadians(60.f));
		vInvTargetLook = XMVector3TransformNormal(ToVec3(vInvTargetLook), matRot);

		// Up-Axis Rotation
		matRot = XMMatrixRotationAxis(m_pTargetTransform->GetState(CTransform::STATE_UP), angle);
		vInvTargetLook = XMVector3TransformNormal(ToVec3(vInvTargetLook), matRot);

		XMVECTOR vector = vTargetPos + vInvTargetLook;
		memcpy(&m_CameraDesc.vEye, &vector, sizeof(_float3));
		memcpy(&m_CameraDesc.vAt, &vTargetPos, sizeof(_float3));

		_vector	vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
		m_pTransformCom->SetState(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

		_vector	vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
		m_pTransformCom->SetState(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));

		_vector	vUp = XMVector3Cross(vLook, vRight);
		m_pTransformCom->SetState(CTransform::STATE_UP, XMVector3Normalize(vUp));

		m_pTransformCom->SetState(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	}

	return __super::Update(TimeDelta);
}

_uint CCamera_Follow::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	return _uint();
}

HRESULT CCamera_Follow::SetUp_Components()
{

	return S_OK;
}

CCamera_Follow * CCamera_Follow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Follow*		pInstance = new CCamera_Follow(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CCamera_Follow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Follow::Clone(void * pArg)
{
	CCamera_Follow*		pInstance = new CCamera_Follow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CCamera_Follow");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCamera_Follow::Free()
{
	__super::Free();

}
