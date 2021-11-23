#include "..\public\Component.h"


CComponent::CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_isCloned(false)
{
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_isCloned(true)
{
}

HRESULT CComponent::InitializePrototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void * pArg)
{
	return S_OK;
}

void CComponent::Free()
{

}