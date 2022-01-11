#include "..\public\Light.h"
#include "VIBuffer_Rect_Viewport.h"
#include "TargetManager.h"
#include "Engine.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}
CLight::CLight(const CLight & rhs)
	: CComponent(rhs)
	, m_LightDesc(rhs.m_LightDesc)
{
}
HRESULT CLight::Initialize(const LIGHTDESC & LightDesc, CTransform* pTransform)
{
	m_pTransform = pTransform;

	if (LightDesc.eType == tagLightDesc::LIGHT_END)
	{
		m_LightDesc.eType = LIGHTDESC::LIGHT_POINT;
		m_LightDesc.vLightPos = _float4(0.f, 0.f, 0.f, 1.f);
		m_LightDesc.fLightRange = 10.f;
		m_LightDesc.fLightAngle = 0.f;
		m_LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		m_LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
		m_LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	}
	else
		m_LightDesc = LightDesc;

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		ViewportDesc;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pVIBuffer = CVIBuffer_Rect_Viewport::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, "../Bin/ShaderFiles/Shader_Rect_Viewport.fx");
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	CLightManager::GetInstance()->AddLight(this);

	return S_OK;
}



HRESULT CLight::Render_Light()
{
	_uint iPassIndex = 1;
	if (LIGHTDESC::LIGHT_DIRECTION == m_LightDesc.eType)
	{
		iPassIndex = 1;
		// m_pTransform->GetState(CTransform::STATE_LOOK);
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightDir", &m_pTransform->GetState(CTransform::STATE_LOOK), sizeof(_float4));
	}
	else if (LIGHTDESC::LIGHT_POINT == m_LightDesc.eType)
	{
		iPassIndex = 2;
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightPos", &m_pTransform->GetState(CTransform::STATE_POSITION), sizeof(_float4));
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_fLightRange", &m_LightDesc.fLightRange, sizeof(_float));
	}
	else if (LIGHTDESC::LIGHT_SPOT == m_LightDesc.eType)
	{
		iPassIndex = 3;
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightDir", &m_pTransform->GetState(CTransform::STATE_LOOK), sizeof(_float4));
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightPos", &m_pTransform->GetState(CTransform::STATE_POSITION), sizeof(_float4));
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_fLightRange", &m_LightDesc.fLightRange, sizeof(_float));
		m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_fLightAngle", &m_LightDesc.fLightAngle, sizeof(_float));
	}


	m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4));
	m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4));

	_matrix viewMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW);
	viewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	// viewMatrix = XMMatrixIdentity();
	m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_ViewMatrixInv123", &XMMatrixTranspose(viewMatrix), sizeof(_float4x4));

	_matrix projMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_PROJ);
	projMatrix = XMMatrixInverse(nullptr, projMatrix);
	m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_ProjMatrixInv", &XMMatrixTranspose(projMatrix), sizeof(_float4x4));

	_vector camPos = CEngine::GetInstance()->GetCamPosition();
	m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vCamPosition", &CEngine::GetInstance()->GetCamPosition(), sizeof(_float4));

	CTargetManager*		pTargetManager = GET_INSTANCE(CTargetManager);

	ID3D11ShaderResourceView*	pNormalSRV = pTargetManager->GetShaderResourceView("Target_Normal");
	if (nullptr == pNormalSRV)
		return E_FAIL;

	m_pVIBuffer->GetShader()->SetUp_TextureOnShader("g_NormalTexture", pNormalSRV);

	ID3D11ShaderResourceView*	pDepthSRV = pTargetManager->GetShaderResourceView("Target_Depth");
	if (nullptr == pDepthSRV)
		return E_FAIL;

	m_pVIBuffer->GetShader()->SetUp_TextureOnShader("g_DepthTexture", pDepthSRV);

	m_pVIBuffer->Render(iPassIndex);

	RELEASE_INSTANCE(CTargetManager);


	return S_OK;
}

_matrix CLight::GetViewMatrix()
{
	//_vector upVector = m_pTransform->GetState(CTransform::STATE_UP);
	//_vector	posVector = m_pTransform->GetState(CTransform::STATE_POSITION);
	//_vector lookVector = m_pTransform->GetState(CTransform::STATE_LOOK);
	//return XMMatrixLookAtLH(posVector, lookVector, upVector);

	return XMMatrixInverse(nullptr, m_pTransform->GetWorldMatrix());
}

_matrix CLight::GetProjMatrix()
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), _float(1920) / 1080.0f, 0.2f, 300.f);
	// return XMMatrixOrthographicLH(XMConvertToRadians(60.0f), _float(1920) / 1080.0f, 0.2f, 300.f);
}

_vector CLight::GetPosition()
{
	return m_pTransform->GetState(CTransform::STATE_POSITION);
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc, CTransform* pTransform)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(LightDesc, pTransform)))
	{
		MSG_BOX("Failed To Creating CLight");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CLight::Clone(void * pArg)
{
	return nullptr;
}

void CLight::Free()
{
	SafeRelease(m_pVIBuffer);
	//SafeRelease(m_pDeviceContext);
	//SafeRelease(m_pDevice);
}