#include "..\Public\TargetManager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTargetManager)
USING(Engine)

CTargetManager::CTargetManager()
{

}

HRESULT CTargetManager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor)
{
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, Format, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
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

HRESULT CTargetManager::Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		ID3D11RenderTargetView*		pRTV = pRenderTarget->Get_RenderTargetView();

		/*pRTV->Clear();*/
		pRenderTargets[iIndex++] = pRTV;
	}

	pDeviceContext->OMSetRenderTargets(pMRTList->size(), pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTargetManager::End_MRT()
{
	return S_OK;
}



CRenderTarget * CTargetManager::Find_RenderTarget(const _tchar * pRenderTagetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pRenderTagetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTargetManager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTargetManager::Free()
{
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
