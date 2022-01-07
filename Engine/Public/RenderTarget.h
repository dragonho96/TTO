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
		return m_pRenderTargetView.Get();
	}

	ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView.Get(); }

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);
	HRESULT Clear();

public:
	HRESULT Ready_DebugBuffer(_float fX, _float fY, _float fWidth, _float fHeight);
	HRESULT Render_DebugBuffer();
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	ComRef<ID3D11Texture2D>					m_pTexture2D = nullptr;
	ComRef<ID3D11RenderTargetView>			m_pRenderTargetView = nullptr;
	ComRef<ID3D11ShaderResourceView>		m_pShaderResourceView = nullptr;
	_float4									m_vClearColor;

private:
	class CVIBuffer_Rect_Viewport*		m_pDebugBuffer = nullptr;

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);
	virtual void Free() override;
};

END