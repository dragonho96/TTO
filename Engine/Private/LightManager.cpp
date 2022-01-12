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
	CLight*		pLight = CLight::Create(pDevice, pDeviceContext, LightDesc, nullptr);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLightManager::AddLight(CLight * pLight)
{
	//if (pLight->GetLightDesc()->eType == LIGHTDESC::TYPE::LIGHT_DIRECTION)
	//	m_Lights.push_back(pLight);

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLightManager::SortLight()
{
	m_RenderLights.clear();

	for (auto& light : m_Lights)
	{
		if (light->GetLightDesc()->eType == LIGHTDESC::TYPE::LIGHT_DIRECTION)
			m_RenderLights.push_front(light);
		else
			m_RenderLights.push_back(light);
	}

	return S_OK;
}

HRESULT CLightManager::Render_Lights()
{
	for (auto& pLight : m_Lights)
		pLight->Render_Light();

	return S_OK;
}

HRESULT CLightManager::Render_DebugBuffer()
{
	for (auto& iter : m_RenderLights)
		iter->Render_DebugBuffer();

	return S_OK;
}

_matrix CLightManager::GetViewMatrix(_uint index)
{
	auto iter = m_RenderLights.begin();
	advance(iter, index);

	return (*iter)->GetViewMatrix();
}

_matrix CLightManager::GetProjMatrix(_uint index)
{
	auto iter = m_RenderLights.begin();
	advance(iter, index);

	return (*iter)->GetProjMatrix();
}

_vector CLightManager::GetPosition(_uint index)
{
	auto iter = m_RenderLights.begin();
	advance(iter, index);

	return (*iter)->GetPosition();
}

ID3D11ShaderResourceView * CLightManager::GetShaderResourceView(_uint index)
{
	auto iter = m_RenderLights.begin();
	advance(iter, index);

	return (*iter)->GetShaderResourceView();
}

string CLightManager::GetTargetName(_uint index)
{
	auto iter = m_RenderLights.begin();
	advance(iter, index);

	return (*iter)->GetTargetName();
}

void CLightManager::Free()
{
	for (auto& pLight : m_Lights)
		SafeRelease(pLight);

	m_Lights.clear();
}