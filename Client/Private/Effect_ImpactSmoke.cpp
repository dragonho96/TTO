#include "stdafx.h"
#include "..\public\Effect_ImpactSmoke.h"

CEffect_ImpactSmoke::CEffect_ImpactSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_ImpactSmoke::CEffect_ImpactSmoke(const CEffect_ImpactSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_ImpactSmoke::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_ImpactSmoke::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();

	size_t iNumInstance = m_pVIBufferCom->GetInstanceMatrices().size();
	m_InstanceForce.resize(iNumInstance);

	m_vNormalDir = { 1.f, 1.f, 0.f, 0.f };
	// Reset();
	return S_OK;
}

_uint CEffect_ImpactSmoke::Update(_double TimeDelta)
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

		memcpy(&instanceMatrices[i]->vRight, &matBill.r[0], sizeof(_float4));
		memcpy(&instanceMatrices[i]->vUp, &matBill.r[1], sizeof(_float4));
		memcpy(&instanceMatrices[i]->vLook, &matBill.r[2], sizeof(_float4));

		_vector pos = XMLoadFloat4(&instanceMatrices[i]->vPosition);
		_vector moveDistance = m_InstanceForce[i].vForceDir * m_InstanceForce[i].fForcePower * TimeDelta;
		_vector gravity = _vector{ 0, 0.f, 0, 0 } *TimeDelta;
		pos += moveDistance;
		pos += gravity;
		memcpy(&instanceMatrices[i]->vPosition, &pos, sizeof(_vector));

		instanceMatrices[i]->iStartFrame += 100.f * TimeDelta;
		if (instanceMatrices[i]->iStartFrame >= 64)
			Reset();

		m_InstanceForce[i].fForcePower -= 2 * TimeDelta;
		if (m_InstanceForce[i].fForcePower < 0.f)
			Reset();

	}

	return _uint();
}

_uint CEffect_ImpactSmoke::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	m_pVIBufferCom->Update(TimeDelta);

	// return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
	return -1;
}

HRESULT CEffect_ImpactSmoke::Render()
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

void CEffect_ImpactSmoke::Reset()
{
	m_pTransformCom->SetLook(m_vNormalDir);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vector{ 0, 0, 0, 1 });
	// m_pTransformCom->SetState(CTransform::STATE_LOOK, m_vNormalDir);

	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		_vector pos = { 0, 0, 0, 1 };
		memcpy(&instanceMatrices[i]->vPosition, &pos, sizeof(_vector));
		instanceMatrices[i]->iStartFrame = 0;

		_matrix mat = m_pTransformCom->GetWorldMatrix() * XMMatrixRotationRollPitchYaw(XMConvertToRadians(10 - (rand() % 20)), 0.f, XMConvertToRadians(10 - (rand() % 20)));
		m_InstanceForce[i].vForceDir = XMVector3Normalize(mat.r[CTransform::STATE_LOOK]);
		m_InstanceForce[i].fForcePower = (_float)(2);
	}
}

HRESULT CEffect_ImpactSmoke::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	//if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_RectInstance_ImpactSmoke", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/ImpactSmoke/Smoke2a_2K.tga");
	//m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/ImpactSmoke/T_dustBurst_02_8x8_CA.tga");
	// m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/ImpactSmoke/T_SmokeBall_01_8_8.tga");
	
	return S_OK;
}

CEffect_ImpactSmoke * CEffect_ImpactSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_ImpactSmoke*		pInstance = new CEffect_ImpactSmoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_ImpactSmoke");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_ImpactSmoke::Clone(void * pArg)
{
	CEffect_ImpactSmoke*		pInstance = new CEffect_ImpactSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_ImpactSmoke");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_ImpactSmoke::Free()
{
	__super::Free();

	SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTextureGreyCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pRendererCom);
}
