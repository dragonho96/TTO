#include "stdafx.h"
#include "..\Public\Scene_Test.h"

#include "Engine.h"
#include "Background.h"
#include "VIBuffer.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#pragma region COMPONENTS
#include "Transform.h"
#include "RectTransform.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "VIBuffer_LineSphere.h"
#include "VIBuffer_LineBox.h"
#include "VIBuffer_LineCapsule.h"
#include "VIBuffer_RectUI.h"
#pragma endregion 

#pragma region OBJECTS
#include "EmptyGameObject.h"
#include "EmptyUI.h"
#pragma endregion
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
	__super::Free();
}

HRESULT CScene_Test::Initialize()
{
	__super::Initialize();
	//m_pEngine->PlaySoundW("CrashMan.mp3", CHANNELID::DIALOGUE);

	if (FAILED(ReadyPrototypeGameObject()))
		return E_FAIL;

	if (FAILED(ReadyLayerTerrain(TEXT("LAYER_TERRAIN"))))
		return E_FAIL;

	m_pEngine->DeserializeScene("../../Assets/Scenes/SerializeScene.yaml");
	return S_OK;
}

_uint CScene_Test::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	return _uint();
}

HRESULT CScene_Test::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Test::ReadyPrototypeGameObject()
{
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, TEXT("Prototype_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, TEXT("Prototype_Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, TEXT("Prototype_RectTransform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, TEXT("Prototype_VIBuffer_RectUI"), CVIBuffer_RectUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/* Prototype_BackGround */
	if (FAILED(m_pEngine->AddPrototype(TEXT("Prototype_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* Gameobject Prototype */
	if (FAILED(m_pEngine->AddPrototype(TEXT("Prototype_EmptyGameObject"), CEmptyGameObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(TEXT("Prototype_EmptyUI"), CEmptyUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Test::ReadyLayerTerrain(const _tchar * pLayerTag)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	if (nullptr == pEngine->AddGameObject(SCENE_TEST, TEXT("Prototype_Terrain"), pLayerTag))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

