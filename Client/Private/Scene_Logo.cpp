#include "stdafx.h"
#include "..\Public\Scene_Logo.h"

#include "Engine.h"
#include "Background.h"
#include "Scene_Loading.h"

CScene_Logo::CScene_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
	: CScene(pDevice, pDeviceContext, iSceneIndex)
{

}

HRESULT CScene_Logo::Initialize()
{
	__super::Initialize();

	if (FAILED(ReadyPrototypeGameObject()))
		return E_FAIL;

	if (FAILED(ReadyLayerBackGround(TEXT("LAYER_BACKGROUND"))))
		return E_FAIL;

	return S_OK;
}

_uint CScene_Logo::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	CEngine*	pEngine = GET_INSTANCE(CEngine);

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(pEngine->SetUpCurrentScene(CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_GAMEPLAY, SCENE_LOADING))))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CEngine)

	return _uint();
}

HRESULT CScene_Logo::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Logo::ReadyPrototypeGameObject()
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	/* Prototype_BackGround */
	if (FAILED(pEngine->AddPrototype(TEXT("Prototype_BackGround"), CBackground::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Logo::ReadyLayerBackGround(const _tchar* pLayerTag)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	/* For.GameObject_BackGround */
	if (FAILED(pEngine->AddGameObject(SCENE_LOGO, TEXT("Prototype_BackGround"), pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

CScene_Logo * CScene_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
{
	CScene_Logo*		pInstance = new CScene_Logo(pDevice, pDeviceContext, iLevelIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Logo");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	__super::Free();

}
