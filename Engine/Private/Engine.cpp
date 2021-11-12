#include "..\public\Engine.h"
#include "TimerManager.h"
#include "GraphicDevice.h"
#include "SceneSerializer.h"
#include "SceneManager.h"
#include "PxManager.h"
#include "GameObjectManager.h"

IMPLEMENT_SINGLETON(CEngine)

CEngine::CEngine()
	: m_pTimerManager(CTimerManager::GetInstance())
	, m_pGraphicDevice(CGraphicDevice::GetInstance())
	, m_pSceneManager(CSceneManager::GetInstance())
	, m_pGameObjectManager(CGameObjectManager::GetInstance())
	, m_pPxManager(CPxManager::GetInstance())
{
	SafeAddRef(m_pTimerManager);
	SafeAddRef(m_pGraphicDevice);
	SafeAddRef(m_pSceneManager);
	SafeAddRef(m_pPxManager);
	SafeAddRef(m_pGameObjectManager);
}

#pragma region TIMER_MANAGER

HRESULT CEngine::Initialize(_uint iNumScenes)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	/* 엔진에 초기화가 필요한 처리를 수앻낟. */
	if (FAILED(m_pGameObjectManager->ReserveManager(iNumScenes)))
		return E_FAIL;

	return S_OK;
}

_uint CEngine::Update(_double dTimeDelta)
{
	return _uint();
}

void CEngine::ReleaseEngine()
{
	if (0 != CEngine::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CEngine");

	if (0 != CPxManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CPhysX");

	if (0 != CTimerManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CTimer_Manager");

	if (0 != CSceneManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CSceneManager");

	if (0 != CGraphicDevice::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CGraphic_Device");

	
}

HRESULT CEngine::ReadyDevice(HWND hWnd, _uint iWidth, _uint iHeight)
{
	m_pGraphicDevice->ReadyGraphicDevice(hWnd, iWidth, iHeight);
	m_pPxManager->Initialize();
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

ID3D11ShaderResourceView* CEngine::GetShaderResourceView()
{
	return m_pGraphicDevice->GetShaderResourceView();
}

ID3D11Buffer * CEngine::GetConstantBuffer()
{
	return m_pGraphicDevice->GetConstantBuffer();
}

XMMATRIX CEngine::GetViewMatrix()
{
	return m_pGraphicDevice->GetViewMatrix();
}

XMMATRIX CEngine::GetProjectionMatrix()
{
	return m_pGraphicDevice->GetProjectionMatrix();
}

XMMATRIX CEngine::GetObjectMatrix()
{
	return m_pGraphicDevice->GetObjectMatrix();
}

void CEngine::SetObjectMatrix(XMMATRIX pMat)
{
	m_pGraphicDevice->SetObjectMatrix(pMat);
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

HRESULT CEngine::ChangeResolution(_uint iWidth, _uint iHeight)
{
	return m_pGraphicDevice->ChangeResolution(iWidth, iHeight);
}

void CEngine::ChangeProj(_uint iWidth, _uint iHeight)
{
	m_pGraphicDevice->ChangeProj(iWidth, iHeight);
}


HRESULT CEngine::AddTimers(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return E_FAIL;

	return m_pTimerManager->AddTimers(pTimerTag);
}

_double CEngine::ComputeDeltaTime(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return 0.0;

	return m_pTimerManager->ComputeDeltaTime(pTimerTag);
}

void CEngine::SerializeScene(const string & filePath)
{
	//CSceneSerializer serializer;
	//serializer.Serialize(filePath);
}

void CEngine::DeserializeScene(const string & filePath)
{
	//CSceneSerializer serializer;
	//serializer.Deserialize(filePath);
}

HRESULT CEngine::SetUpCurrentScene(CScene * pCurrentScene)
{
	if (nullptr == m_pSceneManager)
		return E_FAIL;

	return m_pSceneManager->SetUpCurrentScene(pCurrentScene);
}

_uint CEngine::UpdateScene(_double DeltaTime)
{
	if (nullptr == m_pSceneManager)
		return -1;

	return m_pSceneManager->UpdateScene(DeltaTime);
}

HRESULT CEngine::RenderScene()
{
	if (nullptr == m_pSceneManager)
		return E_FAIL;

	return m_pSceneManager->RenderScene();
}

HRESULT CEngine::AddPrototype(const string sPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	return m_pGameObjectManager->AddPrototype(sPrototypeTag, pPrototype);

}

HRESULT CEngine::AddGameObject(_uint iSceneIndex, const string sPrototypeTag, const string sLayerTag, void * pArg)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	return m_pGameObjectManager->AddGameObject(iSceneIndex, sPrototypeTag, sLayerTag, pArg);

}

void CEngine::Clear(_uint iSceneIndex)
{
	if (nullptr == m_pGameObjectManager)
		return;

	m_pGameObjectManager->Clear(iSceneIndex);
}

PxPhysics * CEngine::GetPhysics()
{
	return m_pPxManager->GetPhysics();
}

PxMaterial * CEngine::GetMaterial()
{
	return m_pPxManager->GetMaterial();
}

PxControllerManager * CEngine::GetControllerManager()
{
	return m_pPxManager->GetControllerManager();
}

#pragma endregion

void CEngine::Free()
{
	SafeRelease(m_pGameObjectManager);
	SafeRelease(m_pSceneManager);
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pPxManager);
}
