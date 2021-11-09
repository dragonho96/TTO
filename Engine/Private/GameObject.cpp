#include "..\public\GameObject.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	SafeAddRef(m_pDeviceContext);
	SafeAddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	SafeAddRef(m_pDeviceContext);
	SafeAddRef(m_pDevice);
}

HRESULT CGameObject::InitializePrototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

_uint CGameObject::Update(_double TimeDelta)
{
	return _uint();
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Free()
{
	SafeRelease(m_pDeviceContext);
	SafeRelease(m_pDevice);
}
