#include "..\Public\TargetManager.h"
#include "RenderTarget.h"
#include "Engine.h"

IMPLEMENT_SINGLETON(CTargetManager)
USING(Engine)

CTargetManager::CTargetManager()
{

}

HRESULT CTargetManager::Initialize(ID3D11DeviceContext * pDeviceContext)
{
	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDepthStencilView);

	return S_OK;
}

HRESULT CTargetManager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, string pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor)
{
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, Format, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::Add_MRT(string pMRTTag, string pRenderTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	SafeAddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::Begin_MRT(ID3D11DeviceContext* pDeviceContext, string pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	// pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();

		ID3D11RenderTargetView*		pRTV = pRenderTarget->Get_RenderTargetView();
		pRenderTargets[iIndex++] = pRTV;
	}
	//if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_TOOL)
	//	pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	pDeviceContext->OMSetRenderTargets(pMRTList->size(), pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTargetManager::Set_MRT(ID3D11DeviceContext * pDeviceContext, string pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	// pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		ID3D11RenderTargetView*		pRTV = pRenderTarget->Get_RenderTargetView();
		pRenderTargets[iIndex++] = pRTV;
	}

	pDeviceContext->OMSetRenderTargets(pMRTList->size(), pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTargetManager::Clear_MRT(ID3D11DeviceContext * pDeviceContext, string pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Clear();
	return S_OK;
}

HRESULT CTargetManager::End_MRT(ID3D11DeviceContext* pDeviceContext)
{
	// Clear textures that was bound before
	//ID3D11ShaderResourceView*const pSRV[8] = { nullptr };
	//CEngine::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 8, pSRV);

	//if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
		pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);

	 //SafeRelease(m_pBackBufferRTV);
	 //SafeRelease(m_pDepthStencilView);
	return S_OK;
}


#ifdef _DEBUG
HRESULT CTargetManager::Ready_DebugBuffer(string pTargetTag, _float fX, _float fY, _float fWidth, _float fHeight)
{
	CRenderTarget*	pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_DebugBuffer(fX, fY, fWidth, fHeight);
}
HRESULT CTargetManager::Render_DebugBuffers(string pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_DebugBuffer();

	return S_OK;
}
ID3D11ShaderResourceView * CTargetManager::GetShaderResourceView(string pTargetTag)
{
	CRenderTarget* renderTarget = Find_RenderTarget(pTargetTag);
	if (renderTarget)
		return renderTarget->GetShaderResourceView();

	return nullptr;
}
#endif // _DEBUG

CRenderTarget * CTargetManager::Find_RenderTarget(string pRenderTagetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), STagFinder(pRenderTagetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTargetManager::Find_MRT(string pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), STagFinder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTargetManager::Free()
{
	SafeRelease(m_pBackBufferRTV);
	SafeRelease(m_pDepthStencilView);

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			SafeRelease(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();



	for (auto& Pair : m_RenderTargets)
		SafeRelease(Pair.second);

	m_RenderTargets.clear();
}
