#include "stdafx.h"
#include "..\public\Effect_Fire.h"

CEffect_Fire::CEffect_Fire(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Fire::CEffect_Fire(const CEffect_Fire & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Fire::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Fire::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();
	return S_OK;
}

_uint CEffect_Fire::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;

	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		_float4x4 matView;
		memcpy(&matView, &m_pEngine->GetTransform(CPipeline::D3DTS_VIEW), sizeof(_float4x4));
		_float4x4 f44Bill;
		memcpy(&f44Bill, &XMMatrixIdentity(), sizeof(_float4x4));

		// 먼저 뷰행렬의 Y축 회전값을 얻어온다.
		f44Bill._11 = matView._11;
		f44Bill._13 = matView._13;
		f44Bill._31 = matView._31;
		f44Bill._33 = matView._33;

		// 역행렬 만들기.
		_matrix matBill = XMLoadFloat4x4(&f44Bill);
		matBill = XMMatrixInverse(nullptr, matBill);
		_float fSize = 1.5f;
		memcpy(&instanceMatrices[i]->vRight, &(matBill.r[0] * fSize), sizeof(_float4));
		memcpy(&instanceMatrices[i]->vUp, &(matBill.r[1] * fSize), sizeof(_float4));
		memcpy(&instanceMatrices[i]->vLook, &matBill.r[2], sizeof(_float4));

		instanceMatrices[i]->iStartFrame += 36 * TimeDelta;
		if (instanceMatrices[i]->iStartFrame >= 36)
			instanceMatrices[i]->iStartFrame = 0.f;
	}

	return _uint();
}

_uint CEffect_Fire::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	m_pVIBufferCom->Update(TimeDelta);

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Fire::Render(_uint iPassIndex)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CEngine*		pEngine = GET_INSTANCE(CEngine);

	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->GetWorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_PROJ)), sizeof(_matrix));

	_uint	iFrame = (_uint)m_fFrame;
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("i_indexUV", &iFrame, sizeof(_uint));

	if (FAILED(m_pVIBufferCom->GetShader()->SetUp_TextureOnShader("g_FireColorTexture", m_pTextureCom)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->GetShader()->SetUp_TextureOnShader("g_FireGreyTexture", m_pTextureGreyCom)))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBufferCom->Render(0);

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CEffect_Fire::SetPosition(_vector vPos)
{
	_float fOffsetY = 1.5f;
	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		memcpy(&instanceMatrices[i]->vPosition, &XMVectorSetW(vPos, 1.f), sizeof(_vector));
		instanceMatrices[i]->vPosition.y += fOffsetY;
	}
}

HRESULT CEffect_Fire::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_RectInstance_Fire", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/Fire/T_Fire_Tiled_D.tga");
	m_pTextureGreyCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/Fire/T_Fire_SubUV.tga");

	return S_OK;
}

CEffect_Fire * CEffect_Fire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Fire*		pInstance = new CEffect_Fire(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_Fire");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Fire::Clone(void * pArg)
{
	CEffect_Fire*		pInstance = new CEffect_Fire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_Fire");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_Fire::Free()
{
	__super::Free();

	//SafeRelease(m_pTransformCom);
	//SafeRelease(m_pTextureCom);
	//SafeRelease(m_pTextureGreyCom);
	//SafeRelease(m_pVIBufferCom);
	//SafeRelease(m_pRendererCom);
}
