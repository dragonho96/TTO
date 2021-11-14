#include "..\public\Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CRenderer::CRenderer(const CRenderer & rhs)
	: CComponent(rhs)
{

}

HRESULT CRenderer::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::AddRenderGroup(RENDER eRenderID, CGameObject * pRenderObject)
{
	if (eRenderID >= RENDER_END ||
		nullptr == pRenderObject)
		return E_FAIL;

	m_RenderGroups[eRenderID].push_back(pRenderObject);

	SafeAddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::DrawRenderGroup()
{
	if (FAILED(RenderPriority()))
		return E_FAIL;

	if (FAILED(RenderNonAlpha()))
		return E_FAIL;

	if (FAILED(RenderAlpha()))
		return E_FAIL;

	if (FAILED(RenderUI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderPriority()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::RenderNonAlpha()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_NONALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::RenderAlpha()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUI()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_UI].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CRenderer");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{

	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();
}