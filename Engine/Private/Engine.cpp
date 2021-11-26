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
	, m_pImGuiManager(CImGuiManager::GetInstance())
	, m_pSoundManager(CSound::GetInstance())
	, m_pPipeline(CPipeline::GetInstance())
	, m_pScriptObjectManager(CScriptObjectManager::GetInstance())
{
	SafeAddRef(m_pScriptObjectManager);
	SafeAddRef(m_pPipeline);
	SafeAddRef(m_pTimerManager);
	SafeAddRef(m_pInputManager);
	SafeAddRef(m_pGraphicDevice);
	SafeAddRef(m_pSceneManager);
	SafeAddRef(m_pPxManager);
	SafeAddRef(m_pGameObjectManager);
	SafeAddRef(m_pComponentManager);
	SafeAddRef(m_pImGuiManager);
	SafeAddRef(m_pSoundManager);
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
	m_pInputManager->Update();
	m_pScriptObjectManager->Update(dTimeDelta);
	m_pGameObjectManager->Update(dTimeDelta);

	m_pScriptObjectManager->LateUpdate(dTimeDelta);
	m_pGameObjectManager->LateUpdate(dTimeDelta);
	return _uint();
}

void CEngine::ReleaseEngine()
{
	if (0 != CEngine::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CEngine");

	if (0 != CSound::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CSound");

	if (0 != CImGuiManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CImGuiManager");

	if (0 != CPxManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CPhysX");

	if (0 != CInputManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CInputManager");

	if (0 != CTimerManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CTimer_Manager");

	if (0 != CSceneManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CSceneManager");

	if (0 != CScriptObjectManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CScriptObjectManager");

	if (0 != CGameObjectManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CGameObjectManager");

	if (0 != CPipeline::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CPipeline");

	if (0 != CComponentManager::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CComponentManager");

	if (0 != CGraphicDevice::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Deleting CGraphic_Device");


	
}

HRESULT CEngine::ReadyDevice(HWND hWnd, _uint iWidth, _uint iHeight)
{
	m_pGraphicDevice->ReadyGraphicDevice(hWnd, iWidth, iHeight);
	m_pPxManager->Initialize();
	InitializeImGui(hWnd, GetDevice(), GetDeviceContext());
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

void CEngine::SetRTV()
{
	return m_pGraphicDevice->SetRTV();
}

void CEngine::SetRTV2()
{
	return m_pGraphicDevice->SetRTV2();
}

void CEngine::Render()
{
	if (nullptr == m_pGraphicDevice)
		return;

	return m_pGraphicDevice->Render();
}

void CEngine::RenderClient()
{
	if (nullptr == m_pGraphicDevice)
		return;

	return m_pGraphicDevice->RenderClient();
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


HRESULT CEngine::AddTimers(string pTimerTag)
{
	if (nullptr == m_pTimerManager)
		return E_FAIL;

	return m_pTimerManager->AddTimers(pTimerTag);
}

_double CEngine::ComputeDeltaTime(string pTimerTag)
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

HRESULT CEngine::AddPrototype(string pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pGameObjectManager)
		return E_FAIL;

	return m_pGameObjectManager->AddPrototype(pPrototypeTag, pPrototype);

}

CGameObject* CEngine::AddGameObject(_uint iSceneIndex, string pPrototypeTag, string pLayerTag, void * pArg)
{
	if (nullptr == m_pGameObjectManager)
		return nullptr;


	return m_pGameObjectManager->AddGameObject(iSceneIndex, pPrototypeTag, pLayerTag, pArg);
}

void CEngine::ClearGameObjectManager(_uint iSceneIndex)
{
	if (nullptr == m_pGameObjectManager)
		return;

	m_pGameObjectManager->Clear(iSceneIndex);
}

list<class CGameObject*> CEngine::GetGameObjectInLayer(_uint iSceneIndex, string pLayerTag)
{
	return m_pGameObjectManager->GetGameObjectInLayer(iSceneIndex, pLayerTag);
}

unordered_map<string, CLayer*>* CEngine::GetLayers()
{
	return m_pGameObjectManager->GetLayers();
}

void CEngine::AddScriptObject(IScriptObject * pObj)
{
	m_pScriptObjectManager->AddObject(pObj);
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

_float3 CEngine::GetMousePosition()
{
	return m_pInputManager->GetMousePosition();
}

void CEngine::InputProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	m_pInputManager->InputProc(hWnd, message, wParam, lParam);
}

HRESULT CEngine::AddPrototype(_uint iSceneIndex, string pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponentManager)
		return E_FAIL;

	return m_pComponentManager->AddPrototype(iSceneIndex, pPrototypeTag, pPrototype);
}

CComponent * CEngine::CloneComponent(_uint iSceneIndex, string pPrototypeTag, void * pArg)
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

_matrix CEngine::GetTransform(CPipeline::TYPE eType)
{
	if (nullptr == m_pPipeline)
		return XMMatrixIdentity();

	return m_pPipeline->Get_Transform(eType);
}

_vector CEngine::GetCamPosition()
{
	if (nullptr == m_pPipeline)
		return XMVectorZero();

	return m_pPipeline->Get_CamPosition();
}

void CEngine::InitializeImGui(HWND hWnd, ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	m_pImGuiManager->Initialize(hWnd, device, deviceContext);
}

void CEngine::UpdateImGui()
{
	m_pImGuiManager->Update();
}

void CEngine::AddWindow(string name, CImGuiWindow * window)
{
	m_pImGuiManager->AddWindow(name, window);
}

CImGuiWindow * CEngine::GetWindow(string name)
{
	return m_pImGuiManager->GetWindow(name);
}

void CEngine::PlaySoundW(string pSoundKey, CHANNELID eID)
{
	m_pSoundManager->PlaySound(pSoundKey, eID);
}

void CEngine::PlayBGM(string pSoundKey)
{
	m_pSoundManager->PlayBGM(pSoundKey);
}

void CEngine::StopSound(CHANNELID eID)
{
	m_pSoundManager->StopSound(eID);
}

void CEngine::StopAll()
{
	m_pSoundManager->StopAll();
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

void CEngine::AddActor(PxRigidActor * pActor)
{
	m_pPxManager->AddActor(pActor);
}

#pragma endregion

void CEngine::Free()
{
	SafeRelease(m_pScriptObjectManager);
	SafeRelease(m_pPipeline);
	SafeRelease(m_pComponentManager);
	SafeRelease(m_pGameObjectManager);
	SafeRelease(m_pSceneManager);
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pInputManager);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pPxManager);
	SafeRelease(m_pImGuiManager);
	SafeRelease(m_pSoundManager);
}
