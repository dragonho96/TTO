#include "..\public\GraphicDevice.h"

IMPLEMENT_SINGLETON(CGraphicDevice)

CGraphicDevice::CGraphicDevice()
{
}

HRESULT CGraphicDevice::ReadyGraphicDevice(HWND hWnd, _uint iWidth, _uint iHeight)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL			Feature_Level;


	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &Feature_Level, &m_pDeviceContext)))
		return E_FAIL;

	/* 멀티샘플링의 지원수준. */

	/* 스왑체인 생성. */
	if (FAILED(ReadySwapChain(hWnd, iWidth, iHeight)))
		return E_FAIL;

	/* 백버퍼에 대한 렌더타겟 뷰를 생성한다. */
	if (FAILED(ReadyBackBufferRenderTargetView(iWidth, iHeight)))
		return E_FAIL;

	/* 뎁스스텐실 버퍼를 생성한다. + 렌더타겟 뷰생성. */
	if (FAILED(ReadyDepthStencilRenderTargetView(iWidth, iHeight)))
		return E_FAIL;

	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilRTV);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	ViewPortDesc.Width = iWidth;
	ViewPortDesc.Height = iHeight;
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.MinDepth = 0.0f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CGraphicDevice::Clear_BackBufferView(_float4 vColor)
{
	if (nullptr == m_pBackBufferRTV)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vColor);

	return S_OK;
}

HRESULT CGraphicDevice::Clear_DepthStencilView(_float fDepth, _uint iStencil)
{
	if (nullptr == m_pDepthStencilRTV)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilRTV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, fDepth, iStencil);

	return S_OK;
}

HRESULT CGraphicDevice::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	m_pSwapChain->Present(0, 0);

	return S_OK;
}


HRESULT CGraphicDevice::ReadySwapChain(HWND hWnd, _uint iWidth, _uint iHeight)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	IDXGIFactory*			pDXGIFactory = nullptr;
	IDXGIAdapter*			pDXGIAdapter = nullptr;
	IDXGIDevice*			pDXGIDevice = nullptr;

	if (FAILED(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice)))
		return E_FAIL;
	if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter)))
		return E_FAIL;
	if (FAILED(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory)))
		return E_FAIL;

	DXGI_SWAP_CHAIN_DESC			SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChainDesc.BufferDesc.Width = iWidth;
	SwapChainDesc.BufferDesc.Height = iHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SampleDesc.Count = 1;

	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pDXGIFactory->CreateSwapChain(m_pDevice, &SwapChainDesc, &m_pSwapChain)))
		return E_FAIL;

	SafeRelease(pDXGIFactory);
	SafeRelease(pDXGIAdapter);
	SafeRelease(pDXGIDevice);

	return S_OK;
}

HRESULT CGraphicDevice::ReadyBackBufferRenderTargetView(_uint iWidth, _uint iHeight)
{
	ID3D11Texture2D*			pBackBufferTexture = nullptr;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = iWidth;
	textureDesc.Height = iHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	m_pDevice->CreateTexture2D(&textureDesc, NULL, &pBackBufferTexture);

	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;


	//if (FAILED(m_pDevice->CreateShaderResourceView(pBackBufferTexture, &shaderResourceViewDesc, &m_pShaderResourceView)))
	//	return E_FAIL;

	m_pDevice->CreateShaderResourceView(pBackBufferTexture, &shaderResourceViewDesc, &m_pShaderResourceView);
	SafeRelease(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphicDevice::ReadyDepthStencilRenderTargetView(_uint iWidth, _uint iHeight)
{
	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;

	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilRTV)))
		return E_FAIL;

	return S_OK;
}




void CGraphicDevice::Free()
{	
	SafeRelease(m_pBackBufferRTV);
	SafeRelease(m_pSwapChain);
	SafeRelease(m_pDeviceContext);
	SafeRelease(m_pDevice);
}

CGraphicDevice * CGraphicDevice::Create()
{
	CGraphicDevice*	pInstance = new CGraphicDevice();

	return pInstance;
}
