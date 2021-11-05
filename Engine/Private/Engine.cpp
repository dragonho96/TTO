#include "..\public\Engine.h"
#include "TimerManager.h"
#include "GraphicDevice.h"
#include "SceneSerializer.h"

IMPLEMENT_SINGLETON(CEngine)

CEngine::CEngine()
	: m_pTimerManager(CTimerManager::GetInstance())
	, m_pGraphicDevice(CGraphicDevice::Create())
{
	SafeAddRef(m_pTimerManager);
	SafeAddRef(m_pGraphicDevice);
}

#pragma region TIMER_MANAGER

void CEngine::ReleaseEngine()
{
	if (0 != CEngine::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CEngine");

	if (0 != CTimerManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CTimer_Manager");

	if (0 != CGraphicDevice::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CGraphic_Device");

	
}

HRESULT CEngine::ReadyDevice(HWND hWnd, _uint iWidth, _uint iHeight)
{
	m_pGraphicDevice->ReadyGraphicDevice(hWnd, iWidth, iHeight);
	return S_OK;
}

ID3D11Device * CEngine::GetDevice()
{
	return m_pGraphicDevice->GetDevice();
}

ID3D11DeviceContext * CEngine::GetDeviceContext()
{
	return m_pGraphicDevice->GetDeviceContext();
}

IDXGISwapChain * CEngine::GetSwapChain()
{
	return m_pGraphicDevice->GetSwapChain();
}

ID3D11RenderTargetView * CEngine::GetRenderTargetView()
{
	return m_pGraphicDevice->GetRenderTargetView();
}

ID3D11DepthStencilView * CEngine::GetDepthStencilRenderTargetView()
{
	return m_pGraphicDevice->GetDepthStencilRenderTargetView();
}

ID3D11ShaderResourceView * CEngine::GetShaderResourceView()
{
	return m_pGraphicDevice->GetShaderResourceView();
}

HRESULT CEngine::ClearBackBufferView(_float4 vColor)
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Clear_BackBufferView(vColor);
}

HRESULT CEngine::ClearDepthStencilView(_float fDepth, _uint iStencil)
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Clear_DepthStencilView(fDepth, iStencil);
}

void CEngine::Render()
{
	if (nullptr == m_pGraphicDevice)
		return;

	return m_pGraphicDevice->Render();
}

HRESULT CEngine::Present()
{
	if (nullptr == m_pGraphicDevice)
		return E_FAIL;

	return m_pGraphicDevice->Present();
}


HRESULT CEngine::AddTimers(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return E_FAIL;

	return m_pTimerManager->AddTimers(pTimerTag);
}

_double CEngine::ComputeTimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return 0.0;

	return m_pTimerManager->ComputeDeltaTime(pTimerTag);
}

void CEngine::SerializeScene(const string & filePath)
{
	CSceneSerializer serializer;
	serializer.Serialize(filePath);
}

void CEngine::DeserializeScene(const string & filePath)
{
	CSceneSerializer serializer;
	serializer.Deserialize(filePath);
}

#pragma endregion

void CEngine::Free()
{
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pGraphicDevice);
}