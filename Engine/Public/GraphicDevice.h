#pragma once

#include "Base.h"

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
	HRESULT Present();

public:
	ID3D11Device*				GetDevice() { return m_pDevice; }
	ID3D11DeviceContext*		GetDeviceContext() { return m_pDeviceContext; }
	IDXGISwapChain*				GetSwapChain() { return m_pSwapChain; }
	ID3D11RenderTargetView*		GetRenderTargetView() { return m_pBackBufferRTV; }
	ID3D11DepthStencilView*		GetDepthStencilRenderTargetView() { return m_pDepthStencilRTV; }
	ID3D11ShaderResourceView*	GetShaderResourceView() { return m_pShaderResourceView; }

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext	= nullptr;
	IDXGISwapChain*				m_pSwapChain = nullptr;
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilRTV = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView = nullptr;

public:
	HRESULT ReadySwapChain(HWND hWnd, _uint iWidth, _uint iHeight);
	HRESULT ReadyBackBufferRenderTargetView(_uint iWidth, _uint iHeight);
	HRESULT ReadyDepthStencilRenderTargetView(_uint iWidth, _uint iHeight);


public:
	virtual void Free() override;
	static CGraphicDevice* Create();
};

END