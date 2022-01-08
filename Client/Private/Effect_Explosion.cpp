#include "stdafx.h"
#include "..\public\Effect_Explosion.h"

CEffect_Explosion::CEffect_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Explosion::CEffect_Explosion(const CEffect_Explosion & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Explosion::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Explosion::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();
	return S_OK;
}

_uint CEffect_Explosion::Update(_double TimeDelta)
{
	if (!m_bPlaying)
		return 0;

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
		_float fSize = 10.f;
		memcpy(&instanceMatrices[i]->vRight, &(matBill.r[0] * fSize), sizeof(_float4));
		memcpy(&instanceMatrices[i]->vUp, &(matBill.r[1] * fSize), sizeof(_float4));
		memcpy(&instanceMatrices[i]->vLook, &matBill.r[2], sizeof(_float4));

		//_vector pos = XMLoadFloat4(&instanceMatrices[i]->vPosition);
		//_vector moveDistance = m_InstanceForce[i].vForceDir * m_InstanceForce[i].fForcePower * TimeDelta;
		//_vector gravity = _vector{ 0, 0.f, 0, 0 } *TimeDelta;
		//pos += moveDistance;
		//pos += gravity;
		//memcpy(&instanceMatrices[i]->vPosition, &pos, sizeof(_vector));

		instanceMatrices[i]->iStartFrame += 64 * TimeDelta;
		if (instanceMatrices[i]->iStartFrame >= 64)
			m_bPlaying = false;
			// instanceMatrices[i]->iStartFrame = 0.f;
	}

	//m_fFrame += 36.0f * TimeDelta;
	//if (m_fFrame >= 36)
	//	m_fFrame = 0.f;

	return _uint();
}

_uint CEffect_Explosion::LateUpdate(_double TimeDelta)
{
	if (!m_bPlaying)
		return 0;

	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	m_pVIBufferCom->Update(TimeDelta);

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Explosion::Render()
{
	if (!m_bPlaying)
		return S_OK;
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

void CEffect_Explosion::Play(_vector vPos)
{
	m_bPlaying = true;
	Reset(vPos);
}

void CEffect_Explosion::Reset(_vector vPos)
{
	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		_vector pos = { 0, 0, 0, 1 };
		// memcpy(&instanceMatrices[i]->vPosition, &XMVectorSetW(vPos, 1.f), sizeof(_vector));
		memcpy(&instanceMatrices[i]->vPosition, &XMVectorSetW(vPos, 1.f),  sizeof(_vector));
		instanceMatrices[i]->iStartFrame = 0;
	}
}

HRESULT CEffect_Explosion::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_RectInstance_ImpactSmoke", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/Explosion/Fireballs1_2K.tga");
	// m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/Explosion/Smoke1c_2k.tga");

	return S_OK;
}

CEffect_Explosion * CEffect_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Explosion*		pInstance = new CEffect_Explosion(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_Explosion");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Explosion::Clone(void * pArg)
{
	CEffect_Explosion*		pInstance = new CEffect_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_Explosion");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_Explosion::Free()
{
	__super::Free();

	SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTextureGreyCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pRendererCom);
}
