#include "stdafx.h"
#include "..\public\Effect_Impact.h"
static _vector impactColor = _vector{ 1, 0, 0, 1 };
static _float2 impactSize = _float2{ 0.05f, 0.05f };
CEffect_Impact::CEffect_Impact(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Impact::CEffect_Impact(const CEffect_Impact & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Impact::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Impact::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();
	size_t iNumInstance = m_pVIBufferCom->GetInstanceMatrices().size();
	m_InstanceForce.resize(iNumInstance);

	m_vNormalDir = { 0.f, 1.f, 0.f, 0.f };
	Reset();
	return S_OK;
}

_uint CEffect_Impact::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;
	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		_vector pos = XMLoadFloat4(&instanceMatrices[i]->vPosition);
		_vector moveDistance = m_InstanceForce[i].vForceDir * m_InstanceForce[i].fForcePower * TimeDelta;
		_vector gravity = _vector{ 0, -5.f, 0, 0 } * TimeDelta;
		pos += moveDistance;
		pos += gravity;
		memcpy(&instanceMatrices[i]->vPosition, &pos, sizeof(_vector));
		m_InstanceForce[i].fForcePower -= 13 * TimeDelta;
		if (m_InstanceForce[i].fForcePower < 0.f)
			Reset();
	}

	impactColor = XMVectorSetX(impactColor, XMVectorGetX(impactColor) + TimeDelta);

	return _uint();
}

_uint CEffect_Impact::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	m_pVIBufferCom->Update(TimeDelta);

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Impact::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CEngine*		pEngine = GET_INSTANCE(CEngine);

	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->GetWorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_PROJ)), sizeof(_matrix));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_vCamPosition", &m_pEngine->GetCamPosition(), sizeof(_vector));
	

	
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("m_vColor", &impactColor, sizeof(_vector));
	m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("m_vSize", &impactSize, sizeof(_float2));


	//_uint	iFrame = (_uint)m_fFrame;
	//m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("i_indexUV", &iFrame, sizeof(_uint));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBufferCom->Render(1);

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CEffect_Impact::Reset()
{
	m_pTransformCom->SetLook(m_vNormalDir);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vector{ 0, 0, 0, 1 });
	// m_pTransformCom->SetState(CTransform::STATE_LOOK, m_vNormalDir);

	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		_vector pos = { 0, 0, 0, 1 };
		memcpy(&instanceMatrices[i]->vPosition, &pos, sizeof(_vector));

		_matrix mat = m_pTransformCom->GetWorldMatrix() * XMMatrixRotationRollPitchYaw(XMConvertToRadians(10 - (rand() % 20)), 0.f, XMConvertToRadians(10 - (rand() % 20)));
		m_InstanceForce[i].vForceDir = XMVector3Normalize(mat.r[CTransform::STATE_LOOK]);
		m_InstanceForce[i].fForcePower = (_float)(11);
	}
	impactColor = _vector{ 0, 1, 0, 1 };
}

HRESULT CEffect_Impact::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_PointInstance_Impact", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CEffect_Impact * CEffect_Impact::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Impact*		pInstance = new CEffect_Impact(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_Impact");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Impact::Clone(void * pArg)
{
	CEffect_Impact*		pInstance = new CEffect_Impact(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_Impact");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_Impact::Free()
{
	__super::Free();

	SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTextureGreyCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pRendererCom);
}
