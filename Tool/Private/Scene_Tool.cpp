#include "stdafx.h"
#include "..\Public\Scene_Tool.h"

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
#include "VIBuffer_Terrain.h"
#pragma endregion 

#pragma region OBJECTS
#include "EmptyGameObject.h"
#include "EmptyUI.h"
#pragma endregion

#include "Camera_Tool.h"

USING(Tool)

CScene_Tool::CScene_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
	: CScene(pDevice, pDeviceContext, iLevelIndex)
{
}


HRESULT CScene_Tool::Initialize()
{
	__super::Initialize();

	if (FAILED(ReadyPrototypeComponent()))
		return E_FAIL;

	if (FAILED(ReadyPrototypeGameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera("LAYER_TOOL")))
		return E_FAIL;

	return S_OK;
}

_uint CScene_Tool::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	return _uint();
}

HRESULT CScene_Tool::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Tool::ReadyPrototypeComponent()
{
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_Transform", CTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_RectTransform", CRectTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prepare DebugLine */
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_VIBuffer_LineSphere", CVIBuffer_LineSphere::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_VIBuffer_LineBox", CVIBuffer_LineBox::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_VIBuffer_LineCapsule", CVIBuffer_LineCapsule::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_VIBuffer_RectUI", CVIBuffer_RectUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prepare Collider*/
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_SphereCollider", CSphereCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_BoxCollider", CBoxCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pEngine->AddPrototype(0, "Prototype_CapsuleCollider", CCapsuleCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Tool::ReadyPrototypeGameObject()
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	/* Gameobject Prototype */
	if (FAILED(pEngine->AddPrototype("Prototype_EmptyGameObject", CEmptyGameObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pEngine->AddPrototype("Prototype_EmptyUI", CEmptyUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Tool::Ready_Layer_Camera(string pLayerTag)
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	if (FAILED(pEngine->AddPrototype("GameObject_Camera_Fly", CCamera_Tool::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 1.f, -5.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	if (nullptr == pEngine->AddGameObject(0, "GameObject_Camera_Fly", pLayerTag, &CameraDesc))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}



CScene_Tool * CScene_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
{
	CScene_Tool*		pInstance = new CScene_Tool(pDevice, pDeviceContext, iLevelIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Tool");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Tool::Free()
{
	__super::Free();
}
