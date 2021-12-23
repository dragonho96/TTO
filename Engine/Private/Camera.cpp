#include "..\public\Camera.h"
#include "Pipeline.h"
#include "Transform.h"
#include <dsound.h>
CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	: CGameObject(pDevice, pDevice_Context)
	, m_pPipeline(CPipeline::GetInstance())
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeline(rhs.m_pPipeline)
{
}

HRESULT CCamera::InitializePrototype()
{
	__super::InitializePrototype();

	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	//AddComponent(0, "Prototype_Transform", "Com_Transform");

	//m_pTransformCom = dynamic_cast<CTransform*>(GetComponent("Com_Transform"));

	SetUpComponents(0, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom);

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	_vector	vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
	m_pTransformCom->SetState(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	_vector	vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	m_pTransformCom->SetState(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));

	_vector	vUp = XMVector3Cross(vLook, vRight);
	m_pTransformCom->SetState(CTransform::STATE_UP, XMVector3Normalize(vUp));

	m_pTransformCom->SetState(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));

	return S_OK;
}

_uint CCamera::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	m_pPipeline->Set_Transform(CPipeline::D3DTS_VIEW, m_pTransformCom->GetWorldMatrixInverse());
	m_pPipeline->Set_Transform(CPipeline::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XM_PIDIV2, _float(1280) / 720.0f, 0.2f, 300.f));

	return _uint();
}

_uint CCamera::LateUpdate(_double TimeDelta)
{
	__super::LateUpdate(TimeDelta);

	return _uint();
}


void CCamera::Free()
{
	__super::Free();
}
