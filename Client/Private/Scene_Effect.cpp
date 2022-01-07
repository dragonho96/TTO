#include "stdafx.h"
#include "..\Public\Scene_Effect.h"

#include "Engine.h"
#include "Terrain.h"
#include "Effect_Trajectory.h"

#include "Camera_Fly.h"

USING(Client)

CScene_Effect::CScene_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
	: CScene(pDevice, pDeviceContext, iSceneIndex)
{
}

CScene_Effect * CScene_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
{
	CScene_Effect*		pInstance = new CScene_Effect(pDevice, pDeviceContext, iSceneIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Effect");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Effect::Free()
{
	__super::Free();
}

HRESULT CScene_Effect::Initialize()
{
	__super::Initialize();
	//m_pEngine->PlaySoundW("CrashMan.mp3", CHANNELID::DIALOGUE);
	m_pEngine->DeserializeScene("../../Assets/Scenes/Effect.yaml");

	if (FAILED(ReadyLayerLights("LAYER_LIGHT")))
		return E_FAIL;

	if (FAILED(ReadyLayerCamera("LAYER_CAMERA")))
		return E_FAIL;

	ReadyScript();


	//if (FAILED(ReadyLayerEffect("LAYER_EFFECT")))
	//	return E_FAIL;

	// m_pTrajectory = CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Trajectory", "Trajectory");

	// m_pEffect = CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Impact", "Impact");
	// m_pEffect = CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_ImpactSmoke", "ImpactSmoke");


	return S_OK;
}

_uint CScene_Effect::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	// Setup Point values

	//list<_vector> points;
	//for (int i = 0; i < 10; ++i)
	//	points.emplace_back(_vector{i * 0.5f, 0, 0, 0});
	//dynamic_cast<CEffect_Trajectory*>(m_pTrajectory)->SetPoints(points);






	return _uint();
}

HRESULT CScene_Effect::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Effect::ReadyLayerLights(string pLayerTag)
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

HRESULT CScene_Effect::ReadyLayerCamera(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 3.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	if (nullptr == pEngine->AddGameObject(0, "GameObject_Camera_Fly", pLayerTag, &CameraDesc))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Effect::ReadyLayerEffect(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	//if (nullptr == pEngine->AddGameObject(0, "GameObject_Effect_Fire", pLayerTag))
	//	return E_FAIL;

	if (nullptr == pEngine->AddGameObject(0, "GameObject_Effect_Smoke", pLayerTag))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Effect::ReadyScript()
{
	// m_pEngine->AddScriptObject(CTerrain::Create(nullptr));

	// m_pEngine->AddScriptObject(CEquipButtonManager::GetInstance());
	return S_OK;
}



