#pragma once

#include "Base.h"
#include "Transform.h"
#include "Line.h"

BEGIN(Engine)

class CGraphicDevice final : public CBase
{
	DECLARE_SINGLETON(CGraphicDevice)

private:
	CGraphicDevice();
	virtual ~CGraphicDevice() = default;
public:
	HRESULT ReadyGraphicDevice(HWND hWnd, _uint iWidth, _uint iHeight);
	HRESULT Clear_BackBufferView(_float4 vColor);
	HRESULT Clear_DepthStencilView(_float fDepth, _uint iStencil);

public:
	HRESULT SetVertexShader();
	HRESULT SetPixelShader();
	HRESULT SetBuffer();

	HRESULT SetSphereVertexShader();
	HRESULT SetSpherePixelShader();
	HRESULT SetSphereBuffer();

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight);
	void Render();
	void RenderClient();
	HRESULT Present();

public:
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT ChangeResolution(_uint iWidth, _uint iHeight);

public:
	ID3D11Device*				GetDevice() { return m_pDevice.Get(); }
	ID3D11DeviceContext*		GetDeviceContext() { return m_pDeviceContext.Get(); }
	IDXGISwapChain*				GetSwapChain() { return m_pSwapChain.Get(); }
	ID3D11RenderTargetView*		GetRenderTargetView() { return m_pBackBufferRTV.Get(); }
	ID3D11DepthStencilView*		GetDepthStencilRenderTargetView() { return m_pDepthStencilRTV.Get(); }
	ID3D11ShaderResourceView*	GetShaderResourceView() { return m_pShaderResourceView.Get(); }

	// Take this to camera later
public:
	XMMATRIX					GetViewMatrix() { return g_View; }
	XMMATRIX					GetProjectionMatrix() { return g_Projection; }
	ID3D11Buffer*				GetConstantBuffer() { return g_pConstantBuffer.Get(); }


	void						ChangeProj(_uint iWidth, _uint iHeight);


private:
	ComRef<ID3D11Device>				m_pDevice = nullptr;
	ComRef<ID3D11DeviceContext>			m_pDeviceContext	= nullptr;
	ComRef<IDXGISwapChain>				m_pSwapChain = nullptr;
	ComRef<ID3D11RenderTargetView>		m_pBackBufferRTV = nullptr;
	ComRef<ID3D11RenderTargetView>		m_pBackBufferRTV2 = nullptr;
	ComRef<ID3D11DepthStencilView>		m_pDepthStencilRTV = nullptr;
	ComRef<ID3D11ShaderResourceView>	m_pShaderResourceView = nullptr;

	ComRef<ID3D11VertexShader>			g_pVertexShader = NULL;
	ComRef<ID3D11PixelShader>			g_pPixelShader = NULL;
	ComRef<ID3D11InputLayout>			g_pVertexLayout = NULL;
	ComRef<ID3D11Buffer>				g_pVertexBuffer = NULL;
	ComRef<ID3D11Buffer>				g_pIndexBuffer = NULL;
	ComRef<ID3D11Buffer>				g_pConstantBuffer = NULL;
	ComRef<ID3D11Buffer>				g_pLightBuffer = NULL;
	ComRef<ID3D11SamplerState>			m_sampleState = NULL;

	XMMATRIX					g_World1;
	XMMATRIX					g_World2;
	XMMATRIX					g_View;
	XMMATRIX					g_Projection;


public:
	HRESULT ReadySwapChain(HWND hWnd, _uint iWidth, _uint iHeight);
	HRESULT ReadyBackBufferRenderTargetView(_uint iWidth, _uint iHeight);
	HRESULT ReadyDepthStencilRenderTargetView(_uint iWidth, _uint iHeight);
	HRESULT ReadyViewport(_uint iWidth, _uint iHeight);
	HRESULT ReadyConstantBuffer();
	void	SetLightBuffer();

public:
	virtual void Free() override;
	//static CGraphicDevice* Create();
};

END