#include "..\public\Light.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	SafeAddRef(m_pDeviceContext);
	SafeAddRef(m_pDevice);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed To Creating CLight");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	SafeRelease(m_pDeviceContext);
	SafeRelease(m_pDevice);
}