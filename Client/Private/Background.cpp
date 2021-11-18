#include "stdafx.h"
#include "..\public\Background.h"
#include "Engine.h"
#include "VIBuffer.h"

CBackground::CBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBackground::CBackground(const CBackground & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackground::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackground::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_uint CBackground::Update(_double TimeDelta)
{
	if (NO_EVENT != __super::Update(TimeDelta))
		return CHANGE;

	return NO_EVENT;
}

_uint CBackground::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (NO_EVENT != __super::LateUpdate(TimeDelta))
		return -1;

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_PRIORITY, this);

	return NO_EVENT;
}

HRESULT CBackground::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->Render();
	return S_OK;
}

HRESULT CBackground::SetUpComponents()
{
	/* For.Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(SCENE_STATIC, TEXT("Prototype_VIBuffer_LineSphere"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CBackground * CBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBackground*		pInstance = new CBackground(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CBackground");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CBackground::Clone(void * pArg)
{
	CBackground*		pInstance = new CBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CBackground");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CBackground::Free()
{
	__super::Free();
}
