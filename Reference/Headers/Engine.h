#pragma once

#include "SceneManager.h"
#include "GameObjectManager.h"
#include "ComponentManager.h"
#include "ImGuiManager.h"
#include "Pipeline.h"
#include "Sound.h"
#include "ScriptObjectManager.h"
#include "LightManager.h"
#include "PxManager.h"
#include "TargetManager.h"

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
	void		DeserializePrefab();
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

	void AddPrefab(string name, YAML::Node node);
	CGameObject* SpawnPrefab(string name);
#pragma endregion

#pragma region SCRIPTOBJECT
	IScriptObject * AddScriptObject(IScriptObject* pObj);
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
	void CloneModel(CGameObject* pObj, string pMeshFilePath, string pMeshFileName, string pShaderFilePath, _bool meshCollider, void* pArg = nullptr);
#pragma endregion

#pragma region LIGHT
	const LIGHTDESC* GetLightDesc(_uint iIndex = 0) const;
	HRESULT AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
#pragma endregion

#pragma region RENDERTARGET
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT Format, _float4 vClearColor);
	HRESULT Add_MRT(string pMRTTag, string pRenderTargetTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, string pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext);
	HRESULT Ready_DebugBuffer(string pTargetTag, _float fX, _float fY, _float fWidth, _float fHeight);
	HRESULT Render_DebugBuffers(string pMRTTag);
	ID3D11ShaderResourceView* GetShaderResourceView(string pTargetTag);
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
	void					PxInitialize();
	void					UpdatePx(_double dDeltaTime);

	PxPhysics*				GetPhysics();
	PxMaterial*				GetMaterial();
	PxControllerManager*	GetControllerManager();
	PxScene*				GetScene();
	PxCooking*				GetCooking();
	void					AddActor(PxRigidActor* pActor);
	void					AddAggregateActor(PxRigidActor* pActor);

	_bool					Raycast(_vector origin, _vector unitDir, _float maxDistance, PxRaycastBuffer& hit, PxQueryFilterData& filterData);
	_bool					Raycast(PxVec3 origin, PxVec3 unitDir, _float maxDistance, PxRaycastBuffer& hit, PxQueryFilterData& filterData);

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
	class CTargetManager*			m_pTargetManager = nullptr;

	class CPxManager*				m_pPxManager = nullptr;
	class CImGuiManager*			m_pImGuiManager = nullptr;
	class CSound*					m_pSoundManager = nullptr;

	USAGE							m_eUsage;

public:
	virtual void Free() override;
};

END