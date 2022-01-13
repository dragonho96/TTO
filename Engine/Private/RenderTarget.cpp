#include "..\Public\RenderTarget.h"
#include "VIBuffer_Rect_Viewport.h"

USING(Engine)
CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	//SafeAddRef(m_pDeviceContext);
	//SafeAddRef(m_pDevice);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor)
{
	m_vClearColor = vClearColor;

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;

	TextureDesc.Format = Format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC		RenderTargetViewDesc;
	ZeroMemory(&RenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RenderTargetViewDesc.Format = Format;
	RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RenderTargetViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), &RenderTargetViewDesc, &m_pRenderTargetView)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC		ShaderResourceViewDesc;
	ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	ShaderResourceViewDesc.Format = Format;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &ShaderResourceViewDesc, &m_pShaderResourceView)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	if (nullptr == m_pRenderTargetView)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), (_float*)&m_vClearColor);

	return S_OK;
}


HRESULT CRenderTarget::Ready_DebugBuffer(_float fX, _float fY, _float fWidth, _float fHeight)
{
	m_pDebugBuffer = CVIBuffer_Rect_Viewport::Create(m_pDevice, m_pDeviceContext, fX, fY, fWidth, fHeight, "../Shaders/Shader_Rect_Viewport.fx");
	if (nullptr == m_pDebugBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Render_DebugBuffer()
{
	m_pDebugBuffer->GetShader()->SetUp_TextureOnShader("g_DiffuseTexture", m_pShaderResourceView.Get());
	// m_pDebugBuffer->GetShader()->Render(0);

	m_pDebugBuffer->Render();
	return S_OK;
}

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor)
{
	CRenderTarget*	pInstance = new CRenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, Format, vClearColor)))
	{
		MSG_BOX("Failed To Create CRenderTarget");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	SafeRelease(m_pDebugBuffer);

	//SafeRelease(m_pShaderResourceView);
	//SafeRelease(m_pRenderTargetView);
	//SafeRelease(m_pTexture2D);

	//SafeRelease(m_pDeviceContext);
	//SafeRelease(m_pDevice);
}
