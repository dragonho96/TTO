#pragma once

#include "Base.h"


BEGIN(Engine)

class ENGINE_DLL CEngine final : public CBase
{
	DECLARE_SINGLETON(CEngine)private:
private:
	CEngine();
	virtual ~CEngine() = default; 

public:
	static void ReleaseEngine();

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
	class CObjectManager*			m_pObjectManager = nullptr;
	class CComponentManager*		m_pComponentManager = nullptr;
	
	class CPxManager*				m_pPxManager = nullptr;
public:
	virtual void Free() override;
};

END