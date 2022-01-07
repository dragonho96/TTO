#include "..\public\Light.h"
#include "VIBuffer_Rect_Viewport.h"
#include "TargetManager.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	//SafeAddRef(m_pDeviceContext);
	//SafeAddRef(m_pDevice);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	_uint		iNumViewports = 1;

	D3D11_VIEWPORT		ViewportDesc;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pVIBuffer = CVIBuffer_Rect_Viewport::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, "../Bin/ShaderFiles/Shader_Rect_Viewport.fx");
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::Render_Light()
{
	m_pVIBuffer->GetShader()->SetUp_ValueOnShader("g_vLightDir", &m_LightDesc.vLightDir, sizeof(_float4));

	CTargetManager*		pTargetManager = GET_INSTANCE(CTargetManager);

	ID3D11ShaderResourceView*	pSRV = pTargetManager->GetShaderResourceView("Target_Normal");
	if (nullptr == pSRV)
		return E_FAIL;

	m_pVIBuffer->GetShader()->SetUp_TextureOnShader("g_NormalTexture", pSRV);
	m_pVIBuffer->Render(1);

	RELEASE_INSTANCE(CTargetManager);


	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed To Creating CLight");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	SafeRelease(m_pVIBuffer);
	//SafeRelease(m_pDeviceContext);
	//SafeRelease(m_pDevice);
}