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

	if (FAILED(ReadyViewport(iWidth, iHeight)))
		return E_FAIL;

	////m_pDeviceContext->OMSetRenderTargets(1, m_pBackBufferRTV.GetAddressOf(), m_pDepthStencilRTV.Get());

	SetVertexShader();
	SetPixelShader();
	SetBuffer();

	Initialize(iWidth, iHeight);
	return S_OK;
}

HRESULT CGraphicDevice::Clear_BackBufferView(_float4 vColor)
{
	if (nullptr == m_pBackBufferRTV)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), (_float*)&vColor);

	return S_OK;
}

HRESULT CGraphicDevice::Clear_DepthStencilView(_float fDepth, _uint iStencil)
{
	if (nullptr == m_pDepthStencilRTV)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilRTV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, fDepth, iStencil);

	return S_OK;
}

HRESULT CGraphicDevice::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	m_pSwapChain->Present(0, 0);
	return S_OK;
}

HRESULT CGraphicDevice::SetVertexShader()
{
	// Compile the vertex shader
	HRESULT hr;
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"../../Assets/Shader/Tutorial05.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = m_pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	m_pDeviceContext->IASetInputLayout(g_pVertexLayout.Get());

	return S_OK;
}

HRESULT CGraphicDevice::SetPixelShader()
{
	HRESULT hr;
	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"../../Assets/Shader/Tutorial05.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT CGraphicDevice::SetBuffer()
{
	HRESULT hr;
	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, g_pVertexBuffer.GetAddressOf(), &stride, &offset);

	// Create index buffer
	WORD indices[] =
	{
		//3,1,0,
		//2,1,3,

		//0,5,4,
		//1,5,0,

		//3,4,7,
		//0,4,3,

		//1,6,5,
		//2,6,1,

		//2,7,6,
		//3,7,2,

		//6,4,5,
		//7,4,6,

		// Drawing Debug Cube
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7,
	};
	bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.ByteWidth = sizeof(WORD) * 24;        // 324 vertices needed for 12 lines in a line list

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	m_pDeviceContext->IASetIndexBuffer(g_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	//m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = m_pDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT CGraphicDevice::Initialize(_uint iWidth, _uint iHeight)
{
	// Initialize the world matrix
	g_World1 = XMMatrixIdentity();
	g_World2 = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, iWidth / (FLOAT)iHeight, 0.01f, 100.0f);

	return S_OK;
}

void CGraphicDevice::Render()
{
	// Update our time
	static float t = 0.0f;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	// 1st Cube: Rotate around the origin
	//g_World1 = XMMatrixRotationY(t);

	// 2nd Cube:  Rotate around origin
	XMMATRIX mSpin = XMMatrixRotationZ(-t);
	XMMATRIX mOrbit = XMMatrixRotationY(-t * 2.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

	//g_World2 = mScale * mSpin * mTranslate * mOrbit;

	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.3f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilRTV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pDeviceContext->OMSetRenderTargets(1, m_pBackBufferRTV2.GetAddressOf(), m_pDepthStencilRTV.Get());
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV2.Get(), ClearColor);

	//
	// Update variables for the first cube
	//
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(g_World1);
	cb1.mView = XMMatrixTranspose(g_View);
	cb1.mProjection = XMMatrixTranspose(g_Projection);
	m_pDeviceContext->UpdateSubresource(g_pConstantBuffer.Get(), 0, NULL, &cb1, 0, 0);

	//
	// Render the first cube
	//
	m_pDeviceContext->VSSetShader(g_pVertexShader.Get(), NULL, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, g_pConstantBuffer.GetAddressOf());
	m_pDeviceContext->PSSetShader(g_pPixelShader.Get(), NULL, 0);
	m_pDeviceContext->DrawIndexed(36, 0, 0);

	//
	// Update variables for the second cube
	//
	//ConstantBuffer cb2;
	//cb2.mWorld = XMMatrixTranspose(g_World2);
	//cb2.mView = XMMatrixTranspose(g_View);
	//cb2.mProjection = XMMatrixTranspose(g_Projection);
	//m_pDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb2, 0, 0);

	//
	// Render the second cube
	//
	//m_pDeviceContext->DrawIndexed(36, 0, 0);

	Present();

	m_pDeviceContext->OMSetRenderTargets(1, m_pBackBufferRTV.GetAddressOf(), m_pDepthStencilRTV.Get());
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), ClearColor);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilRTV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

}

HRESULT CGraphicDevice::CompileShaderFromFile(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFileW(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


HRESULT CGraphicDevice::ChangeResolution(_uint iWidth, _uint iHeight)
{
	//static UINT currentMode = 0;
	//currentMode++;
	// Needs to delete pOutput
	//IDXGIOutput *pOutput;
	//m_pSwapChain->GetContainingOutput(&pOutput);

	//DXGI_SWAP_CHAIN_DESC scd;
	//m_pSwapChain->GetDesc(&scd);

	//// Get all possible resolution in modes[]
	//UINT numModes = 1024;
	//DXGI_MODE_DESC modes[1024];
	//pOutput->GetDisplayModeList(scd.BufferDesc.Format, 0, &numModes, modes);
	//DXGI_MODE_DESC mode = modes[currentMode];
	//if (currentMode < numModes) {

	//	TCHAR str[255];
	//	wsprintf(str, TEXT("Switching to mode: %u / %u, %ux%u@%uHz (%u, %u, %u)\n"),
	//		currentMode + 1,
	//		numModes,
	//		mode.Width,
	//		mode.Height,
	//		mode.RefreshRate.Numerator / mode.RefreshRate.Denominator,
	//		mode.Scaling,
	//		mode.ScanlineOrdering,
	//		mode.Format
	//	);
	//	OutputDebugString(str);

	//	m_pSwapChain->ResizeTarget(&(modes[currentMode]));
	//}

	DXGI_MODE_DESC mode;

	mode.Width = iWidth;
	mode.Height = iHeight;
	mode.RefreshRate.Numerator = 60;
	mode.RefreshRate.Denominator = 1;
	mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	m_pSwapChain->ResizeTarget(&mode);

	m_pDeviceContext->ClearState();
	m_pBackBufferRTV.Reset();
	m_pBackBufferRTV2.Reset();
	m_pShaderResourceView.Reset();
	m_pDepthStencilRTV.Reset();
	if (g_pConstantBuffer) g_pConstantBuffer.Reset();
	if (g_pVertexBuffer) g_pVertexBuffer.Reset();
	if (g_pIndexBuffer) g_pIndexBuffer.Reset();
	if (g_pVertexLayout) g_pVertexLayout.Reset();
	if (g_pVertexShader) g_pVertexShader.Reset();
	if (g_pPixelShader) g_pPixelShader.Reset();
	m_pDeviceContext->Flush();

	m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	ReadyBackBufferRenderTargetView(mode.Width, mode.Height);
	ReadyDepthStencilRenderTargetView(mode.Width, mode.Height);
	ReadyViewport(mode.Width, mode.Height);
	SetVertexShader();
	SetPixelShader();
	SetBuffer();

	Initialize(mode.Width, mode.Height);
	return S_OK;
}

void CGraphicDevice::ChangeProj(_uint iWidth, _uint iHeight)
{
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, iWidth / (FLOAT)iHeight, 0.01f, 100.0f);
}


HRESULT CGraphicDevice::ReadySwapChain(HWND hWnd, _uint iWidth, _uint iHeight)
{
	//if (nullptr == m_pDevice)
	//	return E_FAIL;

	IDXGIFactory*			pDXGIFactory = nullptr;
	IDXGIAdapter*			pDXGIAdapter = nullptr;
	IDXGIDevice*			pDXGIDevice = nullptr;

	if (FAILED(m_pDevice.Get()->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice)))
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

	if (FAILED(pDXGIFactory->CreateSwapChain(m_pDevice.Get(), &SwapChainDesc, &m_pSwapChain)))
		return E_FAIL;

	/*pDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);*/

	SafeRelease(pDXGIFactory);
	SafeRelease(pDXGIAdapter);
	SafeRelease(pDXGIDevice);

	//m_pSwapChain->SetFullscreenState(true, NULL);
	return S_OK;
}

HRESULT CGraphicDevice::ReadyBackBufferRenderTargetView(_uint iWidth, _uint iHeight)
{
	ID3D11Texture2D*			pBackBufferTexture = nullptr;
	ID3D11Texture2D*			pBackBufferTexture2 = nullptr;

	// Look what this is
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;



	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = iWidth;
	textureDesc.Height = iHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // DXGI_FORMAT_D24_UNORM_S8_UINT;DXGI_FORMAT_R16G16B16A16_FLOAT
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//m_pDevice->CreateTexture2D(&textureDesc, NULL, &pBackBufferTexture);
	//if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pBackBufferRTV)))
	//	return E_FAIL;

	m_pDevice->CreateTexture2D(&textureDesc, NULL, &pBackBufferTexture2);
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture2, NULL, &m_pBackBufferRTV2)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;


	if (FAILED(m_pDevice->CreateShaderResourceView(pBackBufferTexture2, &shaderResourceViewDesc, &m_pShaderResourceView)))
		return E_FAIL;

	//m_pDevice->CreateShaderResourceView(pBackBufferTexture, &shaderResourceViewDesc, &m_pShaderResourceView);
	SafeRelease(pBackBufferTexture);
	SafeRelease(pBackBufferTexture2);

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

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = TextureDesc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &descDSV, &m_pDepthStencilRTV)))
		return E_FAIL;

	pDepthStencilTexture->Release();
	return S_OK;
}

HRESULT CGraphicDevice::ReadyViewport(_uint iWidth, _uint iHeight)
{
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




void CGraphicDevice::Free()
{
	//if (m_pDeviceContext) m_pDeviceContext->ClearState();
	//if (m_pDeviceContext) m_pDeviceContext->Flush();
}

//CGraphicDevice * CGraphicDevice::Create()
//{
//	CGraphicDevice*	pInstance = new CGraphicDevice();
//
//	return pInstance;
//}
