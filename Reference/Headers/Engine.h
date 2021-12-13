#pragma once

#include "SceneManager.h"
#include "GameObjectManager.h"
#include "ComponentManager.h"
#include "ImGuiManager.h"
#include "Pipeline.h"
#include "Sound.h"
#include "ScriptObjectManager.h"
#include "LightManager.h"

BEGIN(Engine)

class ENGINE_DLL CEngine final : public CBase
{
	DECLARE_SINGLETON(CEngine)private:
public:
	enum class USAGE { USAGE_CLIENT, USAGE_TOOL, USAGE_NONE };
private:
	CEngine();
	virtual ~CEngine() = default;

public:
	HRESULT			Initialize(_uint iNumScenes, USAGE usage);
	_uint			Update(_double dTimeDelta);
	static void		ReleaseEngine();
	USAGE			GetCurrentUsage() { return m_eUsage; }

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
	SpriteBatch*				GetSpriteBatch();
	SpriteFont*					GetSpriteFont();

	_float2						GetCurrentWindowSize();


	XMMATRIX					GetViewMatrix();
	XMMATRIX					GetProjectionMatrix();

	HRESULT						ClearBackBufferView(_float4 vColor);
	HRESULT						ClearDepthStencilView(_float fDepth, _uint iStencil);

	void						SetRTV();
	void						SetRTV2();
	void						Render();
	void						RenderClient();
	HRESULT						Present();
	HRESULT						ChangeResolution(_uint iWidth, _uint iHeight);
	void						ChangeProj(_uint iWidth, _uint iHeight);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	HRESULT AddTimers(string pTimerTag);
	_double ComputeDeltaTime(string pTimerTag);
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
	HRESULT AddPrototype(string pPrototypeTag, class CGameObject* pPrototype);
	CGameObject* AddGameObject(_uint iSceneIndex, string pPrototypeTag, string pLayerTag, void* pArg = nullptr);
	void	ClearGameObjectManager(_uint iSceneIndex);
	list<class CGameObject*> GetGameObjectInLayer(_uint iSceneIndex, string pLayerTag);
	unordered_map<string, CLayer*>* GetLayers();
	void AddGameObjectWithName(string name, CGameObject* pObj);
	void AddGameObjectWithUUID(uint64_t uuid, CGameObject* pObj);
	CGameObject* FindGameObjectWithName(string name);
	CGameObject* FindGameObjectWithUUID(uint64_t uuid);
#pragma endregion

#pragma region SCRIPTOBJECT
	void AddScriptObject(IScriptObject* pObj);
#pragma endregion 

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
	_float3 GetMousePosition();

	void InputProc(const HWND hWnd, const UINT message,
		const WPARAM wParam, const LPARAM lParam);
#pragma endregion

#pragma region COMPONENT
	HRESULT AddPrototype(_uint iSceneIndex, string pPrototypeTag, class CComponent* pPrototype);
	class CComponent* CloneComponent(_uint iSceneIndex, string pPrototypeTag, void* pArg = nullptr);
	void ClearComponentManager(_uint iSceneIndex);
	CComponent* CloneModel(string pMeshFilePath, string pMeshFileName, string pShaderFilePath, _bool meshCollider, void* pArg = nullptr);
#pragma endregion

#pragma region LIGHT
	const LIGHTDESC* GetLightDesc(_uint iIndex = 0) const;
	HRESULT AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
#pragma endregion

#pragma region PIPELINE
	_matrix GetTransform(CPipeline::TYPE eType);
	_vector GetCamPosition();
#pragma endregion

#pragma region IMGUI
	void InitializeImGui(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void UpdateImGui();
	void AddWindow(string name, class CImGuiWindow* window);
	CImGuiWindow* GetWindow(string name);
#pragma endregion

#pragma region SOUND
	void PlaySoundW(string pSoundKey, CHANNELID eID);
	void PlayBGM(string pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();
#pragma endregion

#pragma region PHYSX
	void					PxInitalize();
	void					UpdatePx(_double dDeltaTime);
	PxPhysics*				GetPhysics();
	PxMaterial*				GetMaterial();
	PxControllerManager*	GetControllerManager();
	void					AddActor(PxRigidActor* pActor);
	void					AddAggregateActor(PxRigidActor* pActor);
	PxScene*				GetScene();
	PxCooking*				GetCooking();
#pragma endregion

private:
	class CTimerManager*			m_pTimerManager = nullptr;
	class CGraphicDevice*			m_pGraphicDevice = nullptr;
	class CSceneManager*			m_pSceneManager = nullptr;
	class CGameObjectManager*		m_pGameObjectManager = nullptr;
	class CComponentManager*		m_pComponentManager = nullptr;
	class CInputManager*			m_pInputManager = nullptr;
	class CPipeline*				m_pPipeline = nullptr;
	class CScriptObjectManager*		m_pScriptObjectManager = nullptr;
	class CLightManager*			m_pLightManager = nullptr;
	class CModelManager*			m_pModelManager = nullptr;

	class CPxManager*				m_pPxManager = nullptr;
	class CImGuiManager*			m_pImGuiManager = nullptr;
	class CSound*					m_pSoundManager = nullptr;

	USAGE							m_eUsage;

public:
	virtual void Free() override;
};

END