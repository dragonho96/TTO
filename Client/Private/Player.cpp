#include "stdafx.h"
#include "..\Public\Player.h"
#include "Engine.h"
#include "HierarchyNode.h"
#include "EquipmentPool.h"
#include "StateMachine.h"
#include "WalkState.h"
#include "RifleState.h"
#include "Enemy.h"

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
	{
		m_pController = m_pCollider->GetController();
		m_pController->getActor()->userData = this;
	}

	// EquipmentPool 에 meshcontainer 등록
	//AssignMeshContainter();
	FindBones();

	m_pGameObject->AddComponent(0, "Prototype_Equipment", "Com_Equipment");
	m_pEquipment = dynamic_cast<CEquipment*>(m_pGameObject->GetComponent("Com_Equipment"));



	m_pWeaponInHand = m_pPrimaryWeapon;

	m_pModel->SetAnimSeperate(true);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::EQUIP_RIFLE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::UNEQUIP_RIFLE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::EQUIP_GRENADE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::UNEQUIP_GRENADE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_LOWER::TURN_R, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_LOWER::TURN_L, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_LOWER::TURN_KNEEL_R, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_LOWER::TURN_KNEEL_L, false);

	m_pLowerState = CStateMachine::walk;
	m_pLowerState->Enter(&m_pLowerState, *this);

	m_pUpperState = CStateMachine::rifle;
	m_pUpperState->Enter(&m_pUpperState, *this);


	list<CGameObject*> camList = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_CAMERA");
	if (camList.size() <= 0)
		return E_FAIL;
	CGameObject* m_pCam = camList.front();
	m_pCameraTransform = dynamic_cast<CTransform*>(m_pCam->GetComponent("Com_Transform"));


	if (FAILED(CEngine::GetInstance()->AddTimers("Raycast")))
		return FALSE;

	if (FAILED(CEngine::GetInstance()->AddTimers("CheckEnemyInSight")))
		return FALSE;


	return S_OK;
}

void CPlayer::Update(_double deltaTime)
{
	if (!m_pGameObject)
		return;

	static bool startRagdoll = false;

	if (m_pController)
	{
		// Look Vector
		//m_pTransform->RotateAxis(m_pTransform->GetState(CTransform::STATE_UP), 0.001f);
		_vector look = m_pCameraTransform->GetState(CTransform::STATE_LOOK);
		// _vector look = m_pTransform->GetState(CTransform::STATE_LOOK);
		look = XMVector4Normalize(XMVectorSetW(XMVectorSetY(look, 0.f), 0.f));
		_vector right = m_pCameraTransform->GetState(CTransform::STATE_RIGHT);

		if (CEngine::GetInstance()->IsKeyPressed('W'))
		{
			m_velocity = look;
			if (CEngine::GetInstance()->IsKeyPressed('D'))
				m_velocity = XMVectorAdd(m_velocity, right);
			else if (CEngine::GetInstance()->IsKeyPressed('A'))
				m_velocity = XMVectorSubtract(m_velocity, right);
		}
		else if (CEngine::GetInstance()->IsKeyPressed('S'))
		{
			m_velocity = -look;
			if (CEngine::GetInstance()->IsKeyPressed('D'))
				m_velocity = XMVectorAdd(m_velocity, right);
			else if (CEngine::GetInstance()->IsKeyPressed('A'))
				m_velocity = XMVectorSubtract(m_velocity, right);
		}
		else if (CEngine::GetInstance()->IsKeyPressed('D'))
			m_velocity = right;
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			m_velocity = -right;
		else
			m_velocity = XMVectorZero();

		// m_velocity = m_velocity.getNormalized();
		m_velocity = XMVector4Normalize(m_velocity);
		m_velocity /= m_fSpeedFactor;
		// m_velocity = XMVectorClamp(m_velocity, XMVectorZero(), _vector{ 0.15f, 0.15f, 0.15f });
		m_curVelocity = XMVectorLerp(m_curVelocity, m_velocity, deltaTime * 10.f);


		PxVec3 pxDir;
		memcpy(&pxDir, &m_curVelocity, sizeof(PxVec3));
		// GRAVITY
		m_pController->move(PxVec3(0, -1.f, 0), 0.f, deltaTime, PxControllerFilters{});
		m_pController->move(pxDir, 0.001f, deltaTime, PxControllerFilters{});


		// Get angle between CameraLook and PlayerLook
		_vector playerLook = m_pTransform->GetState(CTransform::STATE_LOOK);
		playerLook = XMVector4Normalize(XMVectorSetW(XMVectorSetY(playerLook, 0.f), 0.f));

		_vector movingDir;
		memcpy(&movingDir, &m_velocity, sizeof(PxVec3));
		movingDir = XMVector4Normalize(XMVectorSetW(XMVectorSetY(movingDir, 0.f), 0.f));

		m_angleBetweenCamPlayer = XMVectorGetX(XMVector4AngleBetweenVectors(playerLook, movingDir));
		if (XMVectorGetY(XMVector3Cross(playerLook, movingDir)) < 0)
			m_angleBetweenCamPlayer *= -1;



		TimeRaycast += CEngine::GetInstance()->ComputeDeltaTime("Raycast");
		if (TimeRaycast > 0.01f)
		{
			TimeRaycast = 0.0;
			_vector vRayDir = GetPickingDir();
			_vector vCamPos = m_pCameraTransform->GetState(CTransform::STATE_POSITION);

			//const PxU32 bufferSize = 256;
			//PxRaycastHit hitBuffer[bufferSize];
			//PxRaycastBuffer hit(hitBuffer, bufferSize);
			PxRaycastBuffer hit;
			PxQueryFilterData filterData;
			filterData.data.word0 = CPxManager::GROUP1;
			// filterData.data.word1 = CPxManager::GROUP2;
			if (CEngine::GetInstance()->Raycast(vCamPos, vRayDir, 20.f, hit, filterData))
			{
				if (hit.block.actor->userData != this)
				{
					PxU32 distance = hit.block.distance;
					PxU32 faceIndex = hit.block.faceIndex;
					// PxU32 faceIndex = hit.getTouch(0).faceIndex;
					PxVec3 hitPos = hit.block.position;
					PxVec3 hitNormal = hit.block.normal;
					if (hit.block.actor->userData)
						int i = 0;

					if (CEngine::GetInstance()->IsMouseDown(0))
					{
						ADDLOG(("Hit Distance: " + to_string(distance)).c_str());
						ADDLOG(("FaceIndex: " + to_string(faceIndex)).c_str());
						string logPos = "" + to_string(hitPos.x) + " " + to_string(hitPos.y) + " " + to_string(hitPos.z);
						string logStr = "" + to_string(hitNormal.x) + " " + to_string(hitNormal.y) + " " + to_string(hitNormal.z);
						ADDLOG(("Hit Pos: " + logPos).c_str());
						ADDLOG(("Hit Normal: " + logStr).c_str());
						IScriptObject* hitObject = static_cast<IScriptObject*>(hit.block.actor->userData);
						if (dynamic_cast<CEnemy*>(hitObject))
							dynamic_cast<CEnemy*>(hitObject)->GetShot();
					}

					_vector myLookPos = { hitPos.x, hitPos.y, hitPos.z, 0 };


					m_targetUpperRotation.x = GetXAxisAngle(myLookPos);
					m_targetUpperRotation.y = GetYAxisAngle(myLookPos);

					_float	fFollowSpeed = deltaTime * 15.f;
					m_curUpperRotation.x = Lerp(m_curUpperRotation.x, m_targetUpperRotation.x, fFollowSpeed);
					m_curUpperRotation.y = Lerp(m_curUpperRotation.y, m_targetUpperRotation.y, fFollowSpeed);

					m_pModel->SetUpperRotationAngle(m_curUpperRotation);

					// Rotating Body
					fFollowSpeed = deltaTime * 8.f;
					m_curBodyRotation = Lerp(m_curBodyRotation, m_targetBodyRotation, fFollowSpeed);
					m_pTransform->SetUpRotation(_vector{ 0, 1, 0, 0 }, m_curBodyRotation);

				}
				//_uint numHits = hit.getNbAnyHits();
				//ADDLOG(("Num Hits: " + to_string(numHits)).c_str());
				//_uint closestUnitIndex = numHits - 1;
				//_uint firstUnitIndex = 0;

				//PxU32 distance = hit.getTouch(closestUnitIndex).distance;
				//ADDLOG(("Hit Distance: " + to_string(distance)).c_str());
				//PxU32 faceIndex = hit.getTouch(closestUnitIndex).faceIndex;
				//ADDLOG(("FaceIndex: " + to_string(faceIndex)).c_str());
				//// PxU32 faceIndex = hit.getTouch(0).faceIndex;
				//PxVec3 hitPos = hit.getTouch(closestUnitIndex).position;
				//PxVec3 hitNormal = hit.getTouch(closestUnitIndex).normal;

				//string logPos = "" + to_string(hitPos.x) + " " + to_string(hitPos.y) + " " + to_string(hitPos.z);
				//ADDLOG(("Hit Pos: " + logPos).c_str());
				//string logStr = "" + to_string(hitNormal.x) + " " + to_string(hitNormal.y) + " " + to_string(hitNormal.z);
				//ADDLOG(("Hit Normal: " + logStr).c_str());
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

			CheckEnemyInSight();

		}
	}

	m_pLowerState->HandleInput(&m_pLowerState, *this);
	m_pLowerState->Update(&m_pLowerState, *this);

	m_pUpperState->HandleInput(&m_pUpperState, *this);
	m_pUpperState->Update(&m_pUpperState, *this);

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
	m_pModel->Play_Animation(deltaTime);
	UpdateWeaponTransform();
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

_float CPlayer::GetXAxisAngle(_vector hitPos)
{
	_matrix spineMat = m_pSpineBone->pHierarchyNode->Get_CombinedTransformationMatrix() * m_pTransform->GetWorldMatrix();
	_vector mySpineTempA, mySpineTempB, mySpinePos;
	XMMatrixDecompose(&mySpineTempA, &mySpineTempB, &mySpinePos, spineMat);
	mySpinePos = XMVectorSetW(mySpinePos, 0);

	// mySpinePos-> hitpos
	_vector hitposDir = XMVectorSubtract(hitPos, mySpinePos);
	// mySpinePos-> hitpos.y = myspinepos.y
	_vector lookPos = { XMVectorGetX(hitPos), XMVectorGetY(mySpinePos), XMVectorGetZ(hitPos) };
	_vector lookDir = XMVectorSubtract(lookPos, mySpinePos);
	_vector xVecAxisAngle = XMVector3AngleBetweenVectors(hitposDir, lookDir);
	_float xAxisAngle = XMVectorGetX(xVecAxisAngle);

	_vector rootRight = m_pTransform->GetState(CTransform::STATE_UP);
	_vector angleBetweenUpVector = XMVector3AngleBetweenVectors(rootRight, hitposDir);
	_float rightVecAngle = XMVectorGetX(angleBetweenUpVector);
	if (XMConvertToRadians(90) > rightVecAngle)
		xAxisAngle *= -1;

	return xAxisAngle;
}

_float CPlayer::GetYAxisAngle(_vector hitPos)
{
	_vector vecRootPos = m_pTransform->GetState(CTransform::STATE_POSITION);
	vecRootPos = XMVectorSetW(vecRootPos, 0);
	hitPos = XMVectorSetY(hitPos, XMVectorGetY(vecRootPos));
	_vector XZDir = XMVectorSubtract(hitPos, vecRootPos);

	_vector rootLook = m_pTransform->GetState(CTransform::STATE_LOOK);
	_vector yVecAxisAngle = XMVector3AngleBetweenVectors(rootLook, XZDir);
	_float yAxisAngle = XMVectorGetX(yVecAxisAngle);

	_vector crossResult = XMVector3Cross(rootLook, XZDir);
	//ADDLOG(("cross: " + to_string(XMVectorGetX(crossResult)) + ", " + to_string(XMVectorGetY(crossResult)) + ", " + to_string(XMVectorGetZ(crossResult))).c_str());

	_vector rootRight = m_pTransform->GetState(CTransform::STATE_RIGHT);
	_vector angleBetweenRightVector = XMVector3AngleBetweenVectors(rootRight, XZDir);
	_float rightVecAngle = XMVectorGetX(angleBetweenRightVector);
	if (XMVectorGetY(crossResult) < 0.f)
		yAxisAngle *= -1;

	//// -70 or 70 이상 넘어가면 body rotate
	if (XMConvertToDegrees(yAxisAngle) > 70.f)
	{
		m_turn90.first = true;
		_vector rotateAngle = XMVector3AngleBetweenVectors(XZDirPrev, XZDir);
		m_targetBodyRotation += XMConvertToDegrees(XMVectorGetX(rotateAngle));
		XZDirPrev = XZDir;
	}
	else if (XMConvertToDegrees(yAxisAngle) < -70.f)
	{
		m_turn90.second = true;
		_vector rotateAngle = XMVector3AngleBetweenVectors(XZDirPrev, XZDir);
		m_targetBodyRotation -= XMConvertToDegrees(XMVectorGetX(rotateAngle));
		XZDirPrev = XZDir;
	}
	else
		m_turn90 = { false, false };

	return yAxisAngle;
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

void CPlayer::CheckEnemyInSight()
{
	static const _float sightRange = 15.f;
	const PxU32 bufferSize = 256;

	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer hit(hitBuffer, bufferSize);            // [out] Overlap results
	PxSphereGeometry overlapShape = PxSphereGeometry(sightRange);  // [in] shape to test for overlaps
	PxTransform position;    // [in] initial shape pose (at distance=0)
	_vector vecPosition = m_pTransform->GetState(CTransform::STATE_POSITION);
	position.q = PxIdentity;
	memcpy(&position.p, &vecPosition, sizeof(_float3));
	PxQueryFilterData fd;
	////fd.flags = PxQueryFlag::eSTATIC;
	// fd.data.word0 = CPxManager::GROUP1;
	fd.data.word1 = CPxManager::GROUP4;
	//fd.flags = PxQueryFlag::eANY_HIT;
	// bool status = CEngine::GetInstance()->GetScene()->overlap(overlapShape, shapePose, hit, fd);
	// bool status = CEngine::GetInstance()->GetScene()->overlap(PxBoxGeometry(10.f, 10, 10), PxTransform(shapePose), hit, fd);
	
	// first check if enemy is in the range
	if (CEngine::GetInstance()->GetScene()->overlap(overlapShape, PxTransform(position), hit, fd))
	{
		for (PxU32 i = 0; i < hit.getNbAnyHits(); ++i)
		{
			// filter out player
			if (hit.getAnyHit(i).actor->userData != this)
			{
				// ADDLOG(("Pos : " + to_string(enemyPos.p.x) + ", " + to_string(enemyPos.p.z)).c_str());


				// check if there is obstacle between player and enemy
				CEnemy* enemy = static_cast<CEnemy*>(hit.getAnyHit(i).actor->userData);
				PxTransform enemyPos = hit.getAnyHit(i).actor->getGlobalPose();
				PxTransform controllerPos = m_pController->getActor()->getGlobalPose();
				PxVec3 rayDir = (enemyPos.p - controllerPos.p).getNormalized();
				PxRaycastBuffer rayHit;
				PxQueryFilterData filterData;
				// filterData.data.word0 = CPxManager::GROUP1;
				filterData.data.word1 = CPxManager::GROUP3;
				if (CEngine::GetInstance()->Raycast(controllerPos.p, rayDir, sightRange, rayHit, filterData))
					enemy->SetVisibility(VISIBILITY::INVISIBLE);
				else
					enemy->SetVisibility(VISIBILITY::VISIBLE);
			}
		}
	}
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
	m_pSpineBone = m_pModel->Find_Bone("spine_02");
}

void CPlayer::Render()
{
}
