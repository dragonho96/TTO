#include "..\public\LightManager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLightManager)

CLightManager::CLightManager()
{
}

const LIGHTDESC * CLightManager::GetLightDesc(_uint iIndex) const
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->GetLightDesc();
}

HRESULT CLightManager::AddLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

void CLightManager::Free()
{
	for (auto& pLight : m_Lights)
		SafeRelease(pLight);

	m_Lights.clear();
}