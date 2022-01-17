#include "stdafx.h"
#include "..\Public\Effect_Trajectory.h"


CEffect_Trajectory::CEffect_Trajectory(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Trajectory::CEffect_Trajectory(const CEffect_Trajectory & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Trajectory::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trajectory::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();

	list<CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "Sphere");
	if (list.size() > 0)
	{
		m_pSphere = list.front();
		m_pSphereTransform = dynamic_cast<CTransform*>(m_pSphere->GetComponent("Com_Transform"));
		CModel* model = dynamic_cast<CModel*>(m_pSphere->GetComponent("Com_Model"));

		m_pSphereShader = model->GetShader();
	}


	return S_OK;
}
_uint CEffect_Trajectory::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;


	return _uint();
}

_uint CEffect_Trajectory::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	if (!IsActive())
	{
		m_pSphere->SetActive(false);
		return S_OK;
	}
	else
		m_pSphere->SetActive(true);


	m_pVIBufferCom->Update(TimeDelta);
	


	static float fPower = 1.f;
	static float factor = 1.f;
	if (fPower > 5.f)
		factor = -1.f;
	if (fPower < 0.5f)
		factor = 1.f;
	fPower += TimeDelta * 3.f * factor;
		_vector camPos = CEngine::GetInstance()->GetCamPosition();
	_vector color = _vector{ 1, 0, 0, 0 };

	m_pSphereShader->SetUp_ValueOnShader("g_vCamPosition", &camPos, sizeof(_vector));
	m_pSphereShader->SetUp_ValueOnShader("vColor", &color, sizeof(_vector));
	m_pSphereShader->SetUp_ValueOnShader("fPower", &fPower, sizeof(float));


	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Trajectory::Render(_uint iPassIndex)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (!IsActive())
		return S_OK;


	CEngine*		pEngine = GET_INSTANCE(CEngine);

	// m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->GetWorldMatrix()), sizeof(_matrix));
	// m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_VIEW)), sizeof(_matrix));
	// m_pVIBufferCom->GetShader()->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pEngine->GetTransform(CPipeline::D3DTS_PROJ)), sizeof(_matrix));
	m_pShader->SetUp_ValueOnShader("g_vCamPosition", &CEngine::GetInstance()->GetCamPosition(), sizeof(_vector));
	
	_float thickness = 0.03f;
	m_pShader->SetUp_ValueOnShader("g_fThickness", &thickness, sizeof(_float));

	if (FAILED(m_pShader->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBufferCom->Render(0);


	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CEffect_Trajectory::SetPoints(list<_vector> points)
{
	m_pVIBufferCom->SetPoints(points);
	_vector lastPosition = points.back();
	lastPosition = XMVectorSetW(lastPosition, 1.f);

	m_pSphereTransform->SetState(CTransform::STATE_POSITION, lastPosition);
}

HRESULT CEffect_Trajectory::SetUp_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_Line", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_WIC, "../../Assets/Textures/Trajectory/Trajectory.png");


	return S_OK;
}

CEffect_Trajectory * CEffect_Trajectory::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Trajectory*		pInstance = new CEffect_Trajectory(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEffect_Trajectory");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Trajectory::Clone(void * pArg)
{
	CEffect_Trajectory*		pInstance = new CEffect_Trajectory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEffect_Trajectory");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEffect_Trajectory::Free()
{
	__super::Free();

	//SafeRelease(m_pTransformCom);
	//SafeRelease(m_pTextureCom);
	//SafeRelease(m_pVIBufferCom);
	//SafeRelease(m_pRendererCom);
}