#include "stdafx.h"
#include "..\public\Camera_Tool.h"
#include "Engine.h"
#include "Transform.h"
#include "Log.h"

USING(Tool)
CCamera_Tool::CCamera_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Tool::CCamera_Tool(const CCamera_Tool & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Tool::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Tool::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

_uint CCamera_Tool::Update(_double TimeDelta)
{
	if (nullptr == m_pTransformCom)
		return -1;


	if (GetActiveWindow() == g_hWnd)
	{
		if (m_pEngine->IsKeyPressed('W'))
		{
			m_pTransformCom->GoStraight(TimeDelta * 2.f);
		}

		if (m_pEngine->IsKeyPressed('S'))
		{
			m_pTransformCom->GoBackWard(TimeDelta * 2.f);
		}

		if (m_pEngine->IsKeyPressed('A'))
		{
			m_pTransformCom->GoLeft(TimeDelta * 2.f);
		}

		if (m_pEngine->IsKeyPressed('D'))
		{
			m_pTransformCom->GoRight(TimeDelta * 2.f);
		}

		if (m_pEngine->GetMouseMoveValue().z > 0)
			m_pTransformCom->GoStraight(TimeDelta * 3.f);
		if (m_pEngine->GetMouseMoveValue().z < 0)
			m_pTransformCom->GoBackWard(TimeDelta * 3.f);

		_long			MouseMove = 0;

		if (m_pEngine->IsMousePressed(1))
		{
			if (MouseMove = m_pEngine->GetMouseMoveValue().x)
				m_pTransformCom->RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * TimeDelta * 3.f);

			if (MouseMove = m_pEngine->GetMouseMoveValue().y)
				m_pTransformCom->RotateAxis(m_pTransformCom->GetState(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 3.f);
		}

		if (m_pEngine->GetMouseMoveValue().z > 0)
			m_pTransformCom->GoStraight(TimeDelta * 3.f);
		if (m_pEngine->GetMouseMoveValue().z < 0)
			m_pTransformCom->GoBackWard(TimeDelta * 3.f);
	}

	return __super::Update(TimeDelta);
}

_uint CCamera_Tool::LateUpdate(_double TimeDelta)
{
	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	return _uint();
}

HRESULT CCamera_Tool::SetUp_Components()
{



	return S_OK;
}

CCamera_Tool * CCamera_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Tool*		pInstance = new CCamera_Tool(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CCamera_Tool");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Tool::Clone(void * pArg)
{
	CCamera_Tool*		pInstance = new CCamera_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CCamera_Tool");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CCamera_Tool::Free()
{
	__super::Free();

}
