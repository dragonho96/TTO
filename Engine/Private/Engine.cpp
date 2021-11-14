#include "..\public\Engine.h"
#include "TimerManager.h"
#include "GraphicDevice.h"
#include "SceneSerializer.h"
#include "PxManager.h"
#include "InputManager.h"

IMPLEMENT_SINGLETON(CEngine)

CEngine::CEngine()
	: m_pTimerManager(CTimerManager::GetInstance())
	, m_pGraphicDevice(CGraphicDevice::GetInstance())
	, m_pSceneManager(CSceneManager::GetInstance())
	, m_pGameObjectManager(CGameObjectManager::GetInstance())
	, m_pPxManager(CPxManager::GetInstance())
	, m_pInputManager(CInputManager::GetInstance())
	, m_pComponentManager(CComponentManager::GetInstance())
{
	SafeAddRef(m_pTimerManager);
	SafeAddRef(m_pInputManager);
	SafeAddRef(m_pGraphicDevice);
	SafeAddRef(m_pSceneManager);
	SafeAddRef(m_pPxManager);
	SafeAddRef(m_pGameObjectManager);
	SafeAddRef(m_pComponentManager);
}

#pragma region TIMER_MANAGER

HRESULT CEngine::Initialize(_uint iNumScenes)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	if (FAILED(m_pGameObjectManager->ReserveManager(iNumScenes)))
		return E_FAIL;

	if (FAILED(m_pComponentManager->ReserveManager(iNumScenes)))
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

	if (0 != CInputManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CInputManager");

	if (0 != CTimerManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CTimer_Manager");

	if (0 != CSceneManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CSceneManager");

	if (0 != CGameObjectManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CGameObjectManager");

	if (0 != CComponentManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CComponentManager");

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

HRESULT CEngine::AddPrototype(const _tchar* pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	return m_pGameObjectManager->AddPrototype(pPrototypeTag, pPrototype);

}

HRESULT CEngine::AddGameObject(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void * pArg)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	return m_pGameObjectManager->AddGameObject(iSceneIndex, pPrototypeTag, pLayerTag, pArg);

}

void CEngine::ClearGameObjectManager(_uint iSceneIndex)
{
	if (nullptr == m_pGameObjectManager)
		return;

	m_pGameObjectManager->Clear(iSceneIndex);
}

void CEngine::InitializeInput()
{
	m_pInputManager->Initialize();
}

void CEngine::UpdateInput()
{
	m_pInputManager->Update();
}

bool CEngine::IsKeyUp(_uint key)
{
	return m_pInputManager->IsKeyUp(key);
}

bool CEngine::IsKeyDown(_uint key)
{
	return m_pInputManager->IsKeyDown(key);
}

bool CEngine::IsKeyPressed(_uint key)
{
	return m_pInputManager->IsKeyPressed(key);
}

bool CEngine::IsKeyToggled(_uint key)
{
	return m_pInputManager->IsKeyToggled(key);
}

bool CEngine::IsMouseUp(DWORD mouse)
{
	return m_pInputManager->IsMouseUp(mouse);
}

bool CEngine::IsMouseDown(DWORD mouse)
{
	return m_pInputManager->IsMouseDown(mouse);
}

bool CEngine::IsMousePressed(DWORD mouse)
{
	return m_pInputManager->IsMousePressed(mouse);
}

_float3 CEngine::GetMouseMoveValue()
{
	return m_pInputManager->GetMouseMoveValue();
}

void CEngine::InputProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	m_pInputManager->InputProc(hWnd, message, wParam, lParam);
}

HRESULT CEngine::AddPrototype(_uint iSceneIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponentManager)
		return E_FAIL;

	return m_pComponentManager->AddPrototype(iSceneIndex, pPrototypeTag, pPrototype);
}

CComponent * CEngine::CloneComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponentManager)
		return nullptr;

	return m_pComponentManager->CloneComponent(iSceneIndex, pPrototypeTag, pArg);
}

void CEngine::ClearComponentManager(_uint iSceneIndex)
{
	if (nullptr == m_pComponentManager)
		return;

	m_pComponentManager->Clear(iSceneIndex);
}

void CEngine::UpdatePx(_double dDeltaTime)
{
	m_pPxManager->Update(dDeltaTime);
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
	SafeRelease(m_pComponentManager);
	SafeRelease(m_pGameObjectManager);
	SafeRelease(m_pSceneManager);
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pInputManager);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pPxManager);
}
