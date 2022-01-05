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
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);

	/* ���� ������ ����Ÿ�� �׷��� ��ġ�� ���ε��س��´�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);

	/* �����·� �����Ѵ�. */
	HRESULT End_MRT();


private:
	unordered_map<const _tchar*, class CRenderTarget*>			m_RenderTargets;
	typedef unordered_map<const _tchar*, class CRenderTarget*>	RENDERTARGETS;

	unordered_map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef unordered_map<const _tchar*, list<class CRenderTarget*>>	MRTS;

private:
	ID3D11RenderTargetView*				m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*				m_pDepthStencilView = nullptr;

private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pRenderTagetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END