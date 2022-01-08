#include "stdafx.h"
#include "..\public\Effect_Muzzle.h"

CEffect_Muzzle::CEffect_Muzzle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Muzzle::CEffect_Muzzle(const CEffect_Muzzle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Muzzle::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Muzzle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();

	list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "AKM");
	if (objList.size() > 0)
		m_pTargetTransform = dynamic_cast<CTransform*>(objList.front()->GetComponent("Com_Transform"));

	size_t iNumInstance = m_pVIBufferCom->GetInstanceMatrices().size();
	m_InstanceForce.resize(iNumInstance);

	Reset(0.f);
	return S_OK;
}

_uint CEffect_Muzzle::Update(_double TimeDelta)
{
	if (!m_bPlaying)
		return 0;

	if (0 > __super::Update(TimeDelta))
		return -1;

	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
	{
		instanceMatrices[i]->iStartFrame += 100 * TimeDelta;
		if (instanceMatrices[i]->iStartFrame >= 36)
			m_bPlaying = false;
	}
	

	return _uint();
}

_uint CEffect_Muzzle::LateUpdate(_double TimeDelta)
{
	if (!m_bPlaying)
		return 0;

	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	_vector camPos = CEngine::GetInstance()->GetCamPosition();
	m_pTransformCom->LookAtForLandObject(camPos);

	m_pVIBufferCom->Update(TimeDelta);

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Muzzle::Render()
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

	m_pVIBufferCom->Render(2);

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CEffect_Muzzle::Play(_float fCamAngle)
{
	Reset(fCamAngle);
	m_bPlaying = true;
}

void CEffect_Muzzle::Reset(_float fCamAngle)
{
	// Look백터와 0, 0, 1 사이각을 구함
	_vector up = XMVector3Normalize(m_pTargetTransform->GetState(CTransform::STATE_UP));
	_vector right = XMVector3Normalize(m_pTargetTransform->GetState(CTransform::STATE_RIGHT));
	_vector Look = {0.f, 0.f, 1.f};
	_vector xVecAxisAngle = XMVector3AngleBetweenVectors(up, Look);
	_float xAxisAngle = XMVectorGetX(xVecAxisAngle);

	_vector angleBetweenUpVector = XMVector3AngleBetweenVectors(up, _vector{ 1.f, 0.f, 0.f });
	_float rightVecAngle = XMVectorGetX(angleBetweenUpVector);
	if (XMConvertToRadians(90) > rightVecAngle)
		xAxisAngle *= -1;

	_matrix matNew = XMMatrixRotationAxis( _vector{ 0.f, 0.f, 1.f }, xAxisAngle + fCamAngle);
	m_pTransformCom->SetMatrix(matNew);
	m_pTransformCom->SetScale({ 0.4f, 0.4f, 0.4f });

	_vector offsetPosition = up * 0.75f + right * -0.07f;
	_vector vPosition = m_pTargetTransform->GetState(CTransform::STATE_POSITION);
	vPosition += offsetPosition;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPosition);

	vector<VTXMATRIX*> instanceMatrices = m_pVIBufferCom->GetInstanceMatrices();
	for (int i = 0; i < instanceMatrices.size(); ++i)
		instanceMatrices[i]->iStartFrame = 0;
}

HRESULT CEffect_Muzzle::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_RectInstance_Muzzle", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/Muzzle/T_muzzle_01_subUv6x6.tga");

	//m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/ImpactSmoke/T_dustBurst_02_8x8_CA.tga");
	// m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Textures/Effects/ImpactSmoke/T_SmokeBall_01_8_8.tga");

	return S_OK;
}

CEffect_Muzzle * CEffect_Muzzle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Muzzle*		pInstance = new CEffect_Muzzle(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_Muzzle");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Muzzle::Clone(void * pArg)
{
	CEffect_Muzzle*		pInstance = new CEffect_Muzzle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_Muzzle");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_Muzzle::Free()
{
	__super::Free();

	SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTextureGreyCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pRendererCom);
}
