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
#include "Camera_Fly.h"
#include "Camera_Follow.h"
#include "Node.h"
#include "Grid.h"
#pragma endregion

#include "Player.h"
#include "EquipButtonManager.h"
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

	m_pEngine->DeserializeScene("../../Assets/Scenes/SerializeScene.yaml");

	if (FAILED(ReadyLayerCamera("LAYER_CAMERA")))
		return E_FAIL;

	ReadyScript();

	//if (FAILED(ReadyLayerGrid("LAYER_GRID")))
	//	return E_FAIL;

	m_pPathFinding = CPathFinding::GetInstance();
	m_pPathFinding->Initialize();

	return S_OK;
}

_uint CScene_Test::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	m_pPathFinding->Update();
	return _uint();
}

HRESULT CScene_Test::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Test::ReadyPrototypeGameObject()
{
	/* Transform */
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_Transform", CTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_RectTransform", CRectTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Buffer */
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_VIBuffer_LineBox", CVIBuffer_LineBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_VIBuffer_LineSphere", CVIBuffer_LineSphere::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_VIBuffer_LineCapsule", CVIBuffer_LineCapsule::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_VIBuffer_RectUI", CVIBuffer_RectUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_VIBuffer_Rect", CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Collider*/
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_BoxCollider", CBoxCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_SphereCollider", CSphereCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_CapsuleCollider", CCapsuleCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Text */
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_Text", CText::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	//if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_Model", CModel::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	/* Gameobject Prototype */
	if (FAILED(m_pEngine->AddPrototype("Prototype_EmptyGameObject", CEmptyGameObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype("Prototype_EmptyUI", CEmptyUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;



	/* PathFinding*/
	if (FAILED(m_pEngine->AddPrototype("GameObject_Node", CNode::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype("GameObject_Grid", CGrid::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CScene_Test::ReadyLayerCamera(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	if (FAILED(pEngine->AddPrototype("GameObject_Camera_Fly", CCamera_Fly::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pEngine->AddPrototype("GameObject_Camera_Follow", CCamera_Follow::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 1.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	if (nullptr == pEngine->AddGameObject(0, "GameObject_Camera_Fly", pLayerTag, &CameraDesc))
		return E_FAIL;

	if (nullptr == pEngine->AddGameObject(0, "GameObject_Camera_Follow", pLayerTag, &CameraDesc))
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

	// m_pEngine->AddScriptObject(CEquipButtonManager::GetInstance());
	return S_OK;
}



