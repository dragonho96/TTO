#include "stdafx.h"
#include "..\Public\Terrain.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create CTerrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CTerrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	CGameObject::Free();
}

HRESULT CTerrain::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_uint CTerrain::Update(_double TimeDelta)
{
	if (NO_EVENT != __super::Update(TimeDelta))
		return CHANGE;

	return NO_EVENT;
}

_uint CTerrain::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (NO_EVENT != __super::LateUpdate(TimeDelta))
		return -1;

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_PRIORITY, this);

	return NO_EVENT;
}

HRESULT CTerrain::Render()
{
	m_pVIBuffer->Render();
	return S_OK;
}

HRESULT CTerrain::SetUpComponents()
{
	/* For.Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_Terrain", "Com_VIBuffer", (CComponent**)&m_pVIBuffer, (void*)m_pTransform)))
		return E_FAIL;

	return S_OK;
}
