#include "..\public\Transform.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
	m_TransformDesc.fRotatePerSec = 5.f;
	m_TransformDesc.fSpeedPerSec = 5.f;
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_TransformDesc(rhs.m_TransformDesc)
{

}

_float CTransform::GetScale(STATE eState)
{
	return XMVectorGetX(XMVector3Length(GetState(eState)));	
}

HRESULT CTransform::InitializePrototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	// XMStoreFloat4x4(&m_WorldMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::GoStraight(_double TimeDelta)
{
	_vector		vLook = GetState(CTransform::STATE_LOOK);
	_vector		vPosition = GetState(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoLeft(_double TimeDelta)
{
	_vector		vRight = GetState(CTransform::STATE_RIGHT);
	_vector		vPosition = GetState(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta * -1.0f;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoRight(_double TimeDelta)
{
	_vector		vRight = GetState(CTransform::STATE_RIGHT);
	_vector		vPosition = GetState(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::GoBackWard(_double TimeDelta)
{
	_vector		vLook = GetState(CTransform::STATE_LOOK);
	_vector		vPosition = GetState(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta * -1.0f;

	SetState(CTransform::STATE_POSITION, vPosition);
}

void CTransform::ChaseTarget(CTransform * pTarget)
{
}

void CTransform::ChaseTarget(_fvector vTargetPos, _double TimeDelta)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);

	_vector		vDirection = vTargetPos - vPosition;

	vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	SetState(CTransform::STATE_POSITION, vPosition);

	LookAtForLandObject(vTargetPos);
}

void CTransform::LookAtForLandObject(_fvector vTargetPos)
{
	_vector		vPosition = GetState(CTransform::STATE_POSITION);
	_vector		vUp = GetState(CTransform::STATE_UP);

	_vector		vDirection = vTargetPos - vPosition;

	_vector		vRight = XMVector3Cross(vUp, vDirection);

	vRight = XMVector3Normalize(vRight) * GetScale(CTransform::STATE_RIGHT);
	_vector		vLook = XMVector3Cross(vRight, vUp);
	vLook = XMVector3Normalize(vLook) * GetScale(CTransform::STATE_LOOK);

	SetState(CTransform::STATE_RIGHT, vRight);
	SetState(CTransform::STATE_LOOK, vLook);
}

void CTransform::LookAt(_fvector vTargetPos)
{

}

void CTransform::SetUpRotation(_fvector vAxis, _float fDegree)
{
	_vector		vRight, vUp, vLook;

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * GetScale(CTransform::STATE_RIGHT);
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * GetScale(CTransform::STATE_UP);
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * GetScale(CTransform::STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fDegree));

	SetState(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	SetState(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	SetState(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::RotateAxis(_fvector vAxis, _double TimeDelta)
{
	_vector		vRight, vUp, vLook;

	vRight = GetState(CTransform::STATE_RIGHT);
	vUp = GetState(CTransform::STATE_UP);
	vLook = GetState(CTransform::STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(m_TransformDesc.fRotatePerSec) * TimeDelta);

	SetState(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	SetState(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	SetState(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CTransform");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CTransform");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
