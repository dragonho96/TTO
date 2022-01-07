#include "stdafx.h"
#include "..\Public\Scene_Test.h"

#include "Engine.h"
#include "Background.h"
#include "Terrain.h"

#include "Camera_Fly.h"
#include "Camera_Follow.h"
#include "Node.h"
#include "Grid.h"

#include "Player.h"
#include "Enemy.h"
#include "EquipButtonManager.h"
#include "EquipmentPool.h"
USING(Client)

CScene_Test::CScene_Test(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
	: CScene(pDevice, pDeviceContext, iSceneIndex)
{
}

CScene_Test * CScene_Test::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
{
	CScene_Test*		pInstance = new CScene_Test(pDevice, pDeviceContext, iSceneIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Test");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Test::Free()
{
	RELEASE_INSTANCE(CEquipmentPool);
	SafeRelease(m_pPathFinding);
	SafeRelease(m_pGameManager);
	__super::Free();
}

HRESULT CScene_Test::Initialize()
{
	__super::Initialize();
	//m_pEngine->PlaySoundW("CrashMan.mp3", CHANNELID::DIALOGUE);
	m_pEngine->DeserializeScene("../../Assets/Scenes/ModelTest.yaml");

	CEquipmentPool* pEquipmentPool = GET_INSTANCE(CEquipmentPool);
	RELEASE_INSTANCE(CEquipmentPool);
	
	if (FAILED(ReadyLayerLights("LAYER_LIGHT")))
		return E_FAIL;

	if (FAILED(ReadyLayerCamera("LAYER_CAMERA")))
		return E_FAIL;

	ReadyScript();


	//if (FAILED(ReadyLayerGrid("LAYER_GRID")))
	//	return E_FAIL;

	//m_pPathFinding = CPathFinding::GetInstance();
	//m_pPathFinding->Initialize();

	m_pGameManager = CGameManager::GetInstance();
	m_pGameManager->Initialize();

	return S_OK;
}

_uint CScene_Test::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	//if (m_pPathFinding)
	//	m_pPathFinding->Update();
	if (m_pGameManager)
		m_pGameManager->Update(TimeDelta);
	return _uint();
}

HRESULT CScene_Test::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Test::ReadyLayerLights(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pEngine->AddLight(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Test::ReadyLayerCamera(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 3.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	if (nullptr == pEngine->AddGameObject(0, "GameObject_Camera_Follow", pLayerTag, &CameraDesc))
		return E_FAIL;
	if (nullptr == pEngine->AddGameObject(0, "GameObject_Camera_Fly", pLayerTag, &CameraDesc))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}


HRESULT CScene_Test::ReadyLayerGrid(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	CGrid::GRIDDESC desc = { 20, 20, 1.2f };


	if (nullptr == pEngine->AddGameObject(0, "GameObject_Grid", pLayerTag, &desc))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Test::ReadyScript()
{
	m_pEngine->AddScriptObject(CPlayer::Create(nullptr));
	m_pEngine->AddScriptObject(CTerrain::Create(nullptr));

	// Get Enemy number and create enemy scripts

	list<class CGameObject*> listEnemy = CEngine::GetInstance()->GetGameObjectInLayer(0, "Enemy");
	for(auto& iter : listEnemy)
		m_pEngine->AddScriptObject(CEnemy::Create(iter));



	// m_pEngine->AddScriptObject(CEquipButtonManager::GetInstance());
	return S_OK;
}



