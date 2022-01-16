#include "stdafx.h"
#include "..\Public\Scene_Lobby.h"

#include "Engine.h"
#include "Camera_Fly.h"
#include "EquipButtonManager.h"
#include "GameManager.h"

#pragma region SCRIPTOBJECT
#include "EquipmentPool.h"
#include "Player.h"
#pragma endregion

USING(Client)

CScene_Lobby::CScene_Lobby(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
	: CScene(pDevice, pDeviceContext, iSceneIndex)
{
}

CScene_Lobby * CScene_Lobby::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
{
	CScene_Lobby*		pInstance = new CScene_Lobby(pDevice, pDeviceContext, iSceneIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Lobby");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Lobby::Free()
{
	RELEASE_INSTANCE(CEquipmentPool);
	__super::Free();
}

HRESULT CScene_Lobby::Initialize()
{
	__super::Initialize();
	//m_pEngine->PlaySoundW("CrashMan.mp3", CHANNELID::DIALOGUE);

	CEquipmentPool* pEquipmentPool = GET_INSTANCE(CEquipmentPool);
	RELEASE_INSTANCE(CEquipmentPool);

	m_pEngine->DeserializeScene("../../Assets/Scenes/Scene_Lobby.yaml");

	if (FAILED(ReadyLayerCamera("LAYER_CAMERA")))
		return E_FAIL;

	if (FAILED(ReadyScript()))
		return E_FAIL;

	m_pGameManager = CGameManager::GetInstance();
	m_pGameManager->Initialize();

	return S_OK;
}

_uint CScene_Lobby::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);
	m_pGameManager->Update(TimeDelta);

	return _uint();
}

HRESULT CScene_Lobby::Render()
{
	__super::Render();

	return S_OK;
}



HRESULT CScene_Lobby::ReadyLayerCamera(string pLayerTag)
{
	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 1.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	if (nullptr == m_pEngine->AddGameObject(0, "GameObject_Camera_Lobby", pLayerTag, &CameraDesc))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Lobby::ReadyScript()
{
	m_pEngine->AddScriptObject(CPlayer::Create(nullptr));
	m_pEngine->AddScriptObject(CEquipButtonManager::GetInstance());

	return S_OK;
}



