#include "stdafx.h"
#include "..\Public\Player.h"
#include "Engine.h"
#include "HierarchyNode.h"
#include "EquipmentPool.h"
#include "StateMachine.h"
#include "IdleState.h"

USING(Client)

CPlayer::CPlayer(CGameObject* pObj)
{
}

CPlayer* CPlayer::Create(CGameObject* pObj)
{
	CPlayer*		pInstance = new CPlayer(pObj);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create CPlayer");
		SafeRelease(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Free()
{
}

HRESULT CPlayer::Initialize()
{
	list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (objList.size() <= 0)
		return E_FAIL;

	m_pGameObject = objList.front();
	m_pTransform = dynamic_cast<CTransform*>(m_pGameObject->GetComponent("Com_Transform"));
	m_pModel = dynamic_cast<CModel*>(m_pGameObject->GetComponent("Com_Model"));
	m_pCollider = dynamic_cast<CCollider*>(m_pGameObject->GetComponent("Com_Collider"));
	if (m_pCollider)
		m_pController = m_pCollider->GetController();

	// EquipmentPool ¿¡ meshcontainer µî·Ï
	//AssignMeshContainter();
	//FindBones();

	m_pGameObject->AddComponent(0, "Prototype_Equipment", "Com_Equipment");
	m_pEquipment = dynamic_cast<CEquipment*>(m_pGameObject->GetComponent("Com_Equipment"));



	m_pWeaponInHand = m_pPrimaryWeapon;


	m_pState = CStateMachine::idle;
	m_pState->Enter(&m_pState, m_pModel);

	// list.pop_front();
	// CGameObject* gameObject1 = list.front();
	// m_pModel1 = dynamic_cast<CModel*>(gameObject1->GetComponent("Com_Model"));

	list<CGameObject*> camList = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_CAMERA");
	if (camList.size() <= 0)
		return E_FAIL;

	// TODO: Organize cameras
	// camList.pop_front();
	//CGameObject* m_pCam = camList.front();
	//m_pCameraTransform = dynamic_cast<CTransform*>(m_pCam->GetComponent("Com_Transform"));

	return S_OK;
}

void CPlayer::Update(_double deltaTime)
{
	if (!m_pGameObject)
		return;

	m_pState->HandleInput(&m_pState, m_pModel);
	m_pState->Update(&m_pState, m_pModel);

	static bool startRagdoll = false;

	// UpdateWeaponTransform();


	if (m_pController)
	{
		// Look Vector
		//m_pTransform->RotateAxis(m_pTransform->GetState(CTransform::STATE_UP), 0.001f);
		_vector look = m_pCameraTransform->GetState(CTransform::STATE_LOOK);
		look = XMVector4Normalize(XMVectorSetY(look, 0.f));
		PxVec3 pxLook;
		memcpy(&pxLook, &look, sizeof(PxVec3));


		_vector right = m_pCameraTransform->GetState(CTransform::STATE_RIGHT);
		PxVec3 pxRight;
		memcpy(&pxRight, &right, sizeof(PxVec3));

		_float fSpeedFactor = 20.f;
		if (CEngine::GetInstance()->IsKeyPressed(VK_UP))
		{
			m_pController->move(pxLook / fSpeedFactor, 0.f, deltaTime, PxControllerFilters{});
		}
		if (CEngine::GetInstance()->IsKeyPressed(VK_DOWN))
		{
			m_pController->move(-pxLook / fSpeedFactor, 0.f, deltaTime, PxControllerFilters{});
		}
		if (CEngine::GetInstance()->IsKeyPressed(VK_RIGHT))
		{
			m_pController->move(pxRight / fSpeedFactor, 0.f, deltaTime, PxControllerFilters{});
		}
		if (CEngine::GetInstance()->IsKeyPressed(VK_LEFT))
		{
			m_pController->move(-pxRight / fSpeedFactor, 0.f, deltaTime, PxControllerFilters{});
		}
		m_pController->move(PxVec3(0, -1.f, 0), 0.f, deltaTime, PxControllerFilters{});

		if (CEngine::GetInstance()->IsMouseDown(1))
		{
			_vector vRayDir = GetPickingDir();
			_vector vCamPos = m_pCameraTransform->GetState(CTransform::STATE_POSITION);

			const PxU32 bufferSize = 256;
			PxRaycastHit hitBuffer[bufferSize];
			PxRaycastBuffer hit(hitBuffer, bufferSize);
			if (CEngine::GetInstance()->Raycast(vCamPos, vRayDir, 20.f, hit))
			{
				_uint numHits = hit.getNbAnyHits();
				ADDLOG(("Num Hits: " + to_string(numHits)).c_str());
				_uint closestUnitIndex = numHits - 1;

				PxU32 distance = hit.getTouch(closestUnitIndex).distance;
				ADDLOG(("Hit Distance: " + to_string(distance)).c_str());
				PxU32 faceIndex = hit.getTouch(closestUnitIndex).faceIndex;
				ADDLOG(("FaceIndex: " + to_string(faceIndex)).c_str());
				// PxU32 faceIndex = hit.getTouch(0).faceIndex;
				PxVec3 hitPos = hit.getTouch(closestUnitIndex).position;
				PxVec3 hitNormal = hit.getTouch(closestUnitIndex).normal;

				string logPos = "" + to_string(hitPos.x) + " " + to_string(hitPos.y) + " " + to_string(hitPos.z);
				ADDLOG(("Hit Pos: " + logPos).c_str());
				string logStr = "" + to_string(hitNormal.x) + " " + to_string(hitNormal.y) + " " + to_string(hitNormal.z);
				ADDLOG(("Hit Normal: " + logStr).c_str());
			}
		}


		//// Filter Raycast
		//PxScene* scene;
		//PxVec3 origin = ...;                 // [in] Ray origin
		//PxVec3 unitDir = ...;                // [in] Normalized ray direction
		//PxReal maxDistance = ...;            // [in] Raycast max distance
		//PxRaycastBuffer hit;                 // [out] Raycast results

		//									 // [in] Define filter for static objects only
		//PxQueryFilterData filterData(PxQueryFlag::eSTATIC);

		//// Raycast against static objects only
		//// The main result from this call is the boolean 'status'
		//bool status = scene->raycast(origin, unitDir, maxDistance, hit, PxHitFlag::eDEFAULT, filterData);

		if (CEngine::GetInstance()->IsKeyPressed('O'))
		{
			const PxU32 bufferSize = 256;

			PxOverlapHit hitBuffer[bufferSize];
			PxOverlapBuffer hit(hitBuffer, bufferSize);            // [out] Overlap results
			PxGeometry overlapShape = PxSphereGeometry(5.f);  // [in] shape to test for overlaps
			PxTransform shapePose;    // [in] initial shape pose (at distance=0)
			_vector position = m_pTransform->GetState(CTransform::STATE_POSITION);
			shapePose.q = PxIdentity;
			memcpy(&shapePose.p, &position, sizeof(_float3));
			PxQueryFilterData fd;
			//fd.flags = PxQueryFlag::eSTATIC;
			fd.data.word0 = CPxManager::GROUP1;
			bool status = CEngine::GetInstance()->GetScene()->overlap(overlapShape, shapePose, hit, fd);
			if (status)
			{
				for (PxU32 i = 0; i < hit.nbTouches; ++i)
				{
					if (static_cast<CBoxCollider*>(hit.touches[i].actor->userData))
					{
						int i = 0;
					}
				}
			}
		}
	}

	if (CEngine::GetInstance()->IsKeyDown('9'))
	{
		if (m_pController)
		{
			m_pCollider->ReleaseController();
			m_pController = nullptr;
		}
		m_pModel->SetRagdollSimulate(true);
	}
	if (CEngine::GetInstance()->IsKeyDown('8'))
	{
		m_pModel->SetRagdollSimulate(false);
	}

}

void CPlayer::LapteUpdate(_double deltaTime)
{
	// Ragdoll Anim

	//static int anim = 0;
	//if (CEngine::GetInstance()->IsKeyDown('1'))
	//	anim = 0;
	//if (CEngine::GetInstance()->IsKeyDown('2'))
	//	anim = 1;
	//if (CEngine::GetInstance()->IsKeyDown('3'))
	//	anim = 2;

	//m_pModel->SetUp_AnimationIndex(anim);
	m_pModel->Play_Animation(deltaTime);
}

void CPlayer::UpdateWeaponTransform()
{
	m_pPrimaryWeapon = m_pEquipment->GetCurrentEquipment(EQUIPMENT::PRIMARY)->model;
	m_pSecondaryWeapon = m_pEquipment->GetCurrentEquipment(EQUIPMENT::SECONDARY)->model;
	m_pGrenade = m_pEquipment->GetCurrentEquipment(EQUIPMENT::GRENADE)->model;
	m_pTool = m_pEquipment->GetCurrentEquipment(EQUIPMENT::TOOL)->model;

	SetObjectTransform(m_pPrimaryWeapon, m_pHandBone);
	SetObjectTransform(m_pSecondaryWeapon, m_pRThighBone);
	SetObjectTransform(m_pGrenade, m_pGrenadeBone);
	SetObjectTransform(m_pTool, m_pToolBone);
}

void CPlayer::SetObjectTransform(CGameObject * pObj, BONEDESC * pBone)
{
	if (pObj)
	{
		_float4x4 pos;
		_matrix matPos;

		if (m_pModel->IsSimulatingRagdoll())
			matPos = pBone->pHierarchyNode->Get_CombinedTransformationMatrix();
		else
			matPos = pBone->pHierarchyNode->Get_CombinedTransformationMatrix() * m_pTransform->GetWorldMatrix();
		XMStoreFloat4x4(&pos, matPos);

		dynamic_cast<CTransform*>(pObj->GetComponent("Com_Transform"))->SetMatrix(pos);
	}
}

void CPlayer::ChangeWeapon(EQUIPMENT eType, _uint iIndex)
{

}

void CPlayer::ChangeGear(EQUIPMENT eType, _uint iIndex)
{
	CEquipmentPool* equipmentPool = CEquipmentPool::GetInstance();

	equipmentPool->GetEquipment(eType, iIndex)->mesh->SetActive(true);
}

_vector CPlayer::GetPickingDir()
{
	_float2 winSize = CEngine::GetInstance()->GetCurrentWindowSize();
	_float3 vMouseInView = CEngine::GetInstance()->GetMousePosition();
	_float3 vMouseInProj = _float3(0.f, 0.f, 0.f);

	// Viewport -> Proj
	vMouseInProj.x = vMouseInView.x / (winSize.x * 0.5f) - 1.f;
	vMouseInProj.y = 1.f - vMouseInView.y / (winSize.y * 0.5f);

	// Proj -> Viewspace
	_matrix matProj = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_PROJ);
	matProj = XMMatrixInverse(nullptr, matProj);
	_vector vMouse = XMVector3TransformCoord(XMLoadFloat3(&vMouseInProj), matProj);

	// Cam pos in view space is _float3(0.f, 0.f, 0.f)
	_vector vRayPos = { 0, 0, 0 };
	_vector vRayDir = XMVectorSetW(vMouse, 0) - vRayPos;
	vRayDir = XMVector4Normalize(vRayDir);

	// Viewspace -> Worldspace
	_matrix matView = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW);
	matView = XMMatrixInverse(nullptr, matView);
	vRayDir = XMVector3TransformNormal(vRayDir, matView);
	vRayDir = XMVector4Normalize(vRayDir);

	return vRayDir;
}

void CPlayer::AssignMeshContainter()
{
	CEquipmentPool* equipmentPool = CEquipmentPool::GetInstance();

	// HEADGEAR
	CMeshContainer* pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_H_BaseballCap_01");
	equipmentPool->AssignMeshContainer(EQUIPMENT::HEADGEAR, 0, pMeshContainer);
	pMeshContainer->SetActive(true);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_H_Mk6Helmet_03");
	equipmentPool->AssignMeshContainer(EQUIPMENT::HEADGEAR, 1, pMeshContainer);
	pMeshContainer->SetActive(false);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_H_Balaclava_01");
	equipmentPool->AssignMeshContainer(EQUIPMENT::HEADGEAR, 2, pMeshContainer);
	pMeshContainer->SetActive(false);

	// TORSO
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_UB_BDUJacketLong_NPC");
	equipmentPool->AssignMeshContainer(EQUIPMENT::TORSO, 0, pMeshContainer);
	pMeshContainer->SetActive(true);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_UB_Flora");
	equipmentPool->AssignMeshContainer(EQUIPMENT::TORSO, 1, pMeshContainer);
	pMeshContainer->SetActive(false);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_UB_TShirt_NPC");
	equipmentPool->AssignMeshContainer(EQUIPMENT::TORSO, 2, pMeshContainer);
	pMeshContainer->SetActive(false);

	// LEGS
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_LB_BDUPants_01");
	equipmentPool->AssignMeshContainer(EQUIPMENT::LEGS, 0, pMeshContainer);
	pMeshContainer->SetActive(true);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_LB_BDUPants_02");
	equipmentPool->AssignMeshContainer(EQUIPMENT::LEGS, 1, pMeshContainer);
	pMeshContainer->SetActive(false);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_LB_WZ93_Trousers_01");
	equipmentPool->AssignMeshContainer(EQUIPMENT::LEGS, 2, pMeshContainer);
	pMeshContainer->SetActive(false);

	// VEST
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_V_6sh92");
	equipmentPool->AssignMeshContainer(EQUIPMENT::VEST, 0, pMeshContainer);
	pMeshContainer->SetActive(true);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_V_AliceVest");
	equipmentPool->AssignMeshContainer(EQUIPMENT::VEST, 1, pMeshContainer);
	pMeshContainer->SetActive(false);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_V_Strap");
	equipmentPool->AssignMeshContainer(EQUIPMENT::VEST, 2, pMeshContainer);
	pMeshContainer->SetActive(false);

	// BACKPACK
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_B_Bergen");
	equipmentPool->AssignMeshContainer(EQUIPMENT::BACKPACK, 0, pMeshContainer);
	pMeshContainer->SetActive(true);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_B_AliceBackpack");
	equipmentPool->AssignMeshContainer(EQUIPMENT::BACKPACK, 1, pMeshContainer);
	pMeshContainer->SetActive(false);
	pMeshContainer = m_pModel->GetMeshContainerByName("SK_CH_B_PatrolPack");
	equipmentPool->AssignMeshContainer(EQUIPMENT::BACKPACK, 2, pMeshContainer);
	pMeshContainer->SetActive(false);
}

void CPlayer::FindBones()
{
	m_pHandBone = m_pModel->Find_Bone("item_r");
	m_pRThighBone = m_pModel->Find_Bone("thigh_twist_01_r");
	m_pGrenadeBone = m_pModel->Find_Bone("slot_grenade");
	m_pToolBone = m_pModel->Find_Bone("slot_gadget");
}

void CPlayer::Render()
{
}
