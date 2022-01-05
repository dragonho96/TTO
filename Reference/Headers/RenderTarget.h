#pragma once

#include "Base.h"

BEGIN(Engine)
class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;
public:
	ID3D11RenderTargetView* Get_RenderTargetView() {
		return m_pRenderTargetView;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	ID3D11Texture2D*				m_pTexture2D = nullptr;
	ID3D11RenderTargetView*			m_pRenderTargetView = nullptr;
	ID3D11ShaderResourceView*		m_pShaderResourceView = nullptr;

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);
	virtual void Free() override;
};

END