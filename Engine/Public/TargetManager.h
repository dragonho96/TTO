#pragma once

#include "Base.h"

BEGIN(Engine)

class CTargetManager final : public CBase
{
	DECLARE_SINGLETON(CTargetManager)
public:
	CTargetManager();
	virtual ~CTargetManager() = default;

public:
	HRESULT Initialize(ID3D11DeviceContext * pDeviceContext);
public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);
	HRESULT Add_MRT(string pMRTTag, string pRenderTargetTag);

	/* 내가 지정한 렌더타겟 그룹을 장치에 바인드해놓는다. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, string pMRTTag);
	HRESULT Set_MRT(ID3D11DeviceContext* pDeviceContext, string pMRTTag);
	HRESULT Clear_MRT(ID3D11DeviceContext* pDeviceContext, string pMRTTag);

	HRESULT Begin_SingleRT(ID3D11DeviceContext* pDeviceContext, string pTargetTag);

	/* 원상태로 복구한다. */
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext);

public:
	HRESULT Ready_DebugBuffer(string pTargetTag, _float fX, _float fY, _float fWidth, _float fHeight);
	HRESULT Render_DebugBuffers(string pMRTTag);

public:
	ID3D11ShaderResourceView* GetShaderResourceView(string pTargetTag);

private:
	unordered_map<string, class CRenderTarget*>			m_RenderTargets;
	typedef unordered_map<string, class CRenderTarget*>	RENDERTARGETS;

	unordered_map<string, list<class CRenderTarget*>>			m_MRTs;
	typedef unordered_map<string, list<class CRenderTarget*>>	MRTS;

private:
	ID3D11RenderTargetView*				m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*				m_pDepthStencilView = nullptr;

public:
	class CRenderTarget* Find_RenderTarget(string pRenderTagetTag);
	list<class CRenderTarget*>* Find_MRT(string pMRTTag);

public:
	virtual void Free() override;
};

END