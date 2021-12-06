#include "stdafx.h"
#include "..\public\Camera_Fly.h"
#include "Engine.h"

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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera_Fly::Update(_double TimeDelta)
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
	}

	string str = "";
	_vector		vPosition = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_float4 pos;
	XMStoreFloat4(&pos, vPosition);
	str = "" + to_string(pos.x) + ", " + to_string(pos.y) + ", " + to_string(pos.z);
	ADDLOG(str.c_str());


	
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
