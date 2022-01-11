#include "stdafx.h"
#include "..\public\Effect_Smoke.h"

CEffect_Smoke::CEffect_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Smoke::CEffect_Smoke(const CEffect_Smoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Smoke::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Smoke::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();
	return S_OK;
}

_uint CEffect_Smoke::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;
	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();

	_vector vLook = { 0, 1, 0, 0 };
	int count = 0;
	for (auto& vtx : instanceMatrices)
	{

		vtx->iStartFrame += 32.0f * TimeDelta;
		if (vtx->iStartFrame >= 64)
		{
			vtx->iStartFrame = 0.f;
			// vtx->vPosition = XMFLOAT4(1, 2, 1 + (count * 0.5f), 1);
			vtx->vPosition = XMFLOAT4(0, 0,-0.1f, 1);
		}
		count++;
		//_vector position = XMLoadFloat4(&vtx->vPosition);
		//position += XMVector3Normalize(vLook) * (_float)TimeDelta * 0.5f;
		//XMStoreFloat4(&vtx->vPosition, position);
	}
		
	//m_fFrame += 36.0f * TimeDelta;
	//if (m_fFrame >= 36)
	//	m_fFrame = 0.f;

	return _uint();
}

_uint CEffect_Smoke::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	m_pVIBufferCom->Update(TimeDelta);

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Smoke::Render(_uint iPassIndex)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CEngine*		pEngine = GET_INSTANCE(CEngine);

	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->GetWorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_PROJ)), sizeof(_matrix));

	_uint	iFrame = (_uint)m_fFrame;
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("i_indexUV", &iFrame, sizeof(_uint));

	if (FAILED(m_pVIBufferCom->GetShader()->SetUp_TextureOnShader("g_Texture", m_pTextureCom)))
		return E_FAIL;


	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBufferCom->Render(0);

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CEffect_Smoke::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_RectInstance_Smoke", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/Smoke/T_SmokeBall_01_8_8.tga");

	return S_OK;
}

CEffect_Smoke * CEffect_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Smoke*		pInstance = new CEffect_Smoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_Smoke");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Smoke::Clone(void * pArg)
{
	CEffect_Smoke*		pInstance = new CEffect_Smoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_Smoke");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_Smoke::Free()
{
	__super::Free();

	SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTextureGreyCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pRendererCom);
}
