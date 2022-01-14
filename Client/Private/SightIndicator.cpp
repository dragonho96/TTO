#include "stdafx.h"
#include "..\Public\SightIndicator.h"


CSightIndicator::CSightIndicator(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSightIndicator::CSightIndicator(const CSightIndicator & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSightIndicator::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSightIndicator::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pShader = m_pVIBufferCom->GetShader();

	m_pNoSightCross = CEngine::GetInstance()->SpawnPrefab("NoSightCross");
	m_pNoSightEye = CEngine::GetInstance()->SpawnPrefab("NoSightEye");
	m_pNoSightCross->SetActive(false);
	m_pNoSightEye->SetActive(false);

	return S_OK;
}
_uint CSightIndicator::Update(_double TimeDelta)
{
	if (0 > __super::Update(TimeDelta))
		return -1;


	return _uint();
}

_uint CSightIndicator::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (0 > __super::LateUpdate(TimeDelta))
		return -1;

	m_pVIBufferCom->Update(TimeDelta);

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CSightIndicator::Render(_uint iPassIndex)
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

	_float thickness = 0.01f;
	_float4 color = { 0.f, 0.f, 0.f, 1.f };
	m_pShader->SetUp_ValueOnShader("g_fThickness", &thickness, sizeof(_float));
	m_pShader->SetUp_ValueOnShader("g_vColor", &color, sizeof(_vector));


	if (FAILED(m_pShader->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Render()))
		return E_FAIL;

	// pass 다른걸고==
	m_pVIBufferCom->Render(1);


	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CSightIndicator::SetPoints(_bool active, list<_vector> points)
{
	if (active)
	{
		SetActive(true);
		m_pNoSightCross->SetActive(true);
		m_pNoSightEye->SetActive(true);

		m_pVIBufferCom->SetPoints(points);
		_vector hitPos = points.front();
		_vector mousePosition = points.back();

		// Cross UI
		_float2 screenSize = CEngine::GetInstance()->GetCurrentWindowSize();

		hitPos = XMVectorSetW(hitPos, 1.f);
		// world -> view
		_matrix matView = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW);
		_vector vUIHitPos = XMVector4Transform(hitPos, matView);

		// view -> proj
		_matrix matProj = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_PROJ);
		vUIHitPos = XMVector4Transform(vUIHitPos, matProj);

		vUIHitPos = XMVectorSetX(vUIHitPos, XMVectorGetX(vUIHitPos) / XMVectorGetW(vUIHitPos));
		vUIHitPos = XMVectorSetY(vUIHitPos, XMVectorGetY(vUIHitPos) / XMVectorGetW(vUIHitPos));

		vUIHitPos = XMVectorSetX(vUIHitPos, (XMVectorGetX(vUIHitPos) + 1) * screenSize.x * 0.5f);
		vUIHitPos = XMVectorSetY(vUIHitPos, (1.f - (XMVectorGetY(vUIHitPos) + 1) * 0.5f) * screenSize.y);

		// ADDLOG((to_string(XMVectorGetX(vUIHitPos)) + ", " + to_string(XMVectorGetY(vUIHitPos))).c_str());
		dynamic_cast<CEmptyUI*>(m_pNoSightCross)->SetClientPosition(XMVectorGetX(vUIHitPos), XMVectorGetY(vUIHitPos));

		// Eye UI
		_float3 mousePos = CEngine::GetInstance()->GetMousePosition();
		dynamic_cast<CEmptyUI*>(m_pNoSightEye)->SetClientPosition(mousePos.x + 30.f, mousePos.y + 30.f);
	}
	else
	{
		SetActive(false);
		m_pNoSightCross->SetActive(false);
		m_pNoSightEye->SetActive(false);
	}
}

HRESULT CSightIndicator::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::SetUpComponents(SCENE_STATIC, "Prototype_VIBuffer_Line_Single", "Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_WIC, "../../Assets/Textures/Trajectory/Trajectory.png");


	return S_OK;
}

CSightIndicator * CSightIndicator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSightIndicator*		pInstance = new CSightIndicator(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CSightIndicator");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CSightIndicator::Clone(void * pArg)
{
	CSightIndicator*		pInstance = new CSightIndicator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CSightIndicator");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CSightIndicator::Free()
{
	__super::Free();

	//SafeRelease(m_pTransformCom);
	SafeRelease(m_pTextureCom);
	//SafeRelease(m_pVIBufferCom);
	//SafeRelease(m_pRendererCom);
}