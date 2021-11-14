#pragma once

#include "SceneManager.h"
#include "GameObjectManager.h"
#include "ComponentManager.h"

BEGIN(Engine)

class ENGINE_DLL CEngine final : public CBase
{
	DECLARE_SINGLETON(CEngine)private:
private:
	CEngine();
	virtual ~CEngine() = default; 

public:
	HRESULT			Initialize(_uint iNumScenes);
	_uint			Update(_double dTimeDelta);
	static void		ReleaseEngine();

#pragma region DEVICE_MANAGER
public:
	HRESULT						ReadyDevice(HWND hWnd, _uint iWidth, _uint iHeight);
	ID3D11Device*				GetDevice();
	ID3D11DeviceContext*		GetDeviceContext();
	IDXGISwapChain*				GetSwapChain();
	ID3D11RenderTargetView*		GetRenderTargetView();
	ID3D11DepthStencilView*		GetDepthStencilRenderTargetView();
	ID3D11ShaderResourceView*	GetShaderResourceView();
	ID3D11Buffer*				GetConstantBuffer();

	XMMATRIX					GetViewMatrix();
	XMMATRIX					GetProjectionMatrix();
	XMMATRIX					GetObjectMatrix();
	void						SetObjectMatrix(XMMATRIX pMat);

	HRESULT ClearBackBufferView(_float4 vColor);
	HRESULT ClearDepthStencilView(_float fDepth, _uint iStencil);

	void	Render();
	HRESULT Present();
	HRESULT ChangeResolution(_uint iWidth, _uint iHeight);
	void	ChangeProj(_uint iWidth, _uint iHeight);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	HRESULT AddTimers(const _tchar* pTimerTag);
	_double ComputeDeltaTime(const _tchar* pTimerTag);
#pragma endregion

#pragma region SCENE
public:
	void		SerializeScene(const string& filePath);
	void		DeserializeScene(const string& filePath);
	HRESULT		SetUpCurrentScene(class CScene* pCurrentScene);
	_uint		UpdateScene(_double DeltaTime);
	HRESULT		RenderScene();
#pragma endregion

#pragma region GAMEOBJECT_MANAGER
public:
	HRESULT AddPrototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT AddGameObject(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);
	void	ClearGameObjectManager(_uint iSceneIndex);
#pragma region INPUT
	void InitializeInput();
	void UpdateInput();

	bool IsKeyUp(_uint key);
	bool IsKeyDown(_uint key);
	bool IsKeyPressed(_uint key);
	bool IsKeyToggled(_uint key);
	bool IsMouseUp(DWORD mouse);
	bool IsMouseDown(DWORD mouse);
	bool IsMousePressed(DWORD mouse);

	_float3 GetMouseMoveValue();

	void InputProc(const HWND hWnd, const UINT message,
		const WPARAM wParam, const LPARAM lParam);
#pragma endregion

#pragma region COMPONENT
	HRESULT AddPrototype(_uint iSceneIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* CloneComponent(_uint iSceneIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	void ClearComponentManager(_uint iSceneIndex);
#pragma endregion

#pragma region PHYSX
	void					UpdatePx(_double dDeltaTime);
	PxPhysics*				GetPhysics();
	PxMaterial*				GetMaterial();
	PxControllerManager*	GetControllerManager();
#pragma endregion

private:
	class CTimerManager*			m_pTimerManager = nullptr;
	class CGraphicDevice*			m_pGraphicDevice = nullptr;
	class CSceneManager*			m_pSceneManager = nullptr;
	class CGameObjectManager*		m_pGameObjectManager = nullptr;
	class CComponentManager*		m_pComponentManager = nullptr;
	class CInputManager*			m_pInputManager = nullptr;
	
	class CPxManager*				m_pPxManager = nullptr;
public:
	virtual void Free() override;
};

END