#include "stdafx.h"
#include "..\public\Camera_Fly.h"
#include "Engine.h"
#include "Transform.h"

USING(Client)
CCamera_Fly::CCamera_Fly(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Fly::CCamera_Fly(const CCamera_Fly & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Fly::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Fly::Initialize(void * pArg)
{
	//if (FAILED(__super::Initialize(pArg)))
	//	return E_FAIL;
	__super::Initialize(pArg);

	return S_OK;
}

_uint CCamera_Fly::Update(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;

	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->GoStraight(TimeDelta);
	}

	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->GoBackWard(TimeDelta);
	}

	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->GoLeft(TimeDelta);
	}

	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->GoRight(TimeDelta);
	}

	return __super::Update(TimeDelta);
}

_uint CCamera_Fly::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	return _uint();
}

HRESULT CCamera_Fly::SetUp_Components()
{



	return S_OK;
}

CCamera_Fly * CCamera_Fly::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Fly*		pInstance = new CCamera_Fly(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CCamera_Fly");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Fly::Clone(void * pArg)
{
	CCamera_Fly*		pInstance = new CCamera_Fly(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CCamera_Fly");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCamera_Fly::Free()
{
	__super::Free();

}
