#include "stdafx.h"
#include "..\Public\Player.h"
#include "Engine.h"
#include "GameManager.h"
#include "HierarchyNode.h"
#include "EquipmentPool.h"
#include "WalkState.h"
#include "RifleState.h"
#include "Enemy.h"
#include "Grenade.h"
#include "Effect_Trajectory.h"


USING(Client)

CPlayer::CPlayer(CGameObject* pObj)
	: CCharacter(pObj)
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

	// Get UI Image
	CGameObject* pUI_Primary = CEngine::GetInstance()->GetGameObjectInLayer(0, "UI_Primary").front();
	m_pUI_Primary = dynamic_cast<CVIBuffer_RectUI*>(pUI_Primary->GetComponent("Com_VIBuffer"));
	CGameObject* pUI_Grenade = CEngine::GetInstance()->GetGameObjectInLayer(0, "UI_Grenade").front();
	m_pUI_Grenade = dynamic_cast<CVIBuffer_RectUI*>(pUI_Grenade->GetComponent("Com_VIBuffer"));
	m_pUI_Primary->SetColor(_float4{ 1.f, 0.5f, 0.f, 1.f });
	m_pUI_Grenade->SetColor(_float4{ 1.f, 1.f, 1.f, 1.f });

	m_pGrenadeTrajectory = CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Trajectory", "Trajectory");
	m_pGrenadeTrajectory->SetActive(false);

	// EquipmentPool 에 meshcontainer 등록
	AssignMeshContainter();
	FindBones();

	m_pGameObject->AddComponent(0, "Prototype_Equipment", "Com_Equipment");
	m_pEquipment = dynamic_cast<CEquipment*>(m_pGameObject->GetComponent("Com_Equipment"));
	m_pPrimaryWeapon = m_pEquipment->GetCurrentEquipment(EQUIPMENT::PRIMARY)->model;
	m_pSecondaryWeapon = m_pEquipment->GetCurrentEquipment(EQUIPMENT::SECONDARY)->model;
	m_pGrenade = m_pEquipment->GetCurrentEquipment(EQUIPMENT::GRENADE)->model;
	m_pTool = m_pEquipment->GetCurrentEquipment(EQUIPMENT::TOOL)->model;
	m_pWeaponInHand = m_pPrimaryWeapon;

	if (g_eCurScene == SCENE_TEST)
	{
		m_pRifleLight = CEngine::GetInstance()->SpawnPrefab("RifleLight");
		m_pRifleLightTransform = dynamic_cast<CTransform*>(m_pRifleLight->GetComponent("Com_Transform"));

		m_pMuzzleLight = CEngine::GetInstance()->SpawnPrefab("RifleMuzzleLight");
		m_pMuzzleLightTransform = dynamic_cast<CTransform*>(m_pMuzzleLight->GetComponent("Com_Transform"));
		m_pMuzzleLightCom = dynamic_cast<CLight*>(m_pMuzzleLight->GetComponent("Com_Light"));
		m_pMuzzleLightCom->SetRange(m_fCurMuzzleLightRange);
	}

	// Setting Non-Looping Animation
	m_pModel->SetAnimSeperate(true);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::EQUIP_RIFLE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::UNEQUIP_RIFLE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::EQUIP_GRENADE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::UNEQUIP_GRENADE, false);
	m_pModel->SetAnimationLoop((_uint)CStateMachine::ANIM_UPPER::THROW_GRENADE, false);
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


	if (g_eCurScene == SCENE_TEST)
	{
		if (FAILED(CEngine::GetInstance()->AddTimers("Raycast")))
			return FALSE;
		if (FAILED(CEngine::GetInstance()->AddTimers("CheckEnemyInSight")))
			return FALSE;
	}


	CGameManager::GetInstance()->RegisterPlayer(this);
	return S_OK;
}

void CPlayer::Update(_double deltaTime)
{
	if (!m_pGameObject)
		return;


	static bool startRagdoll = false;

	if (m_pController/* && CGameManager::GetInstance()->GetCurrentCamera() == CGameManager::CAMERA::FOLLOW*/
		&& g_eCurScene == SCENE_TEST)
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
		// m_pController->move(pxDir, 0.001f, deltaTime, PxControllerFilters{});
		static bool moving = true;
		if (CEngine::GetInstance()->IsKeyDown('T'))
			moving = false;

		if (moving)
			m_pController->move(pxDir, 0.001f, deltaTime, PxControllerFilters{});
		//else
		//	m_pController->move(PxVec3{ 0, 0, 0 }, 0.001f, deltaTime, PxControllerFilters{});
		if (g_eCurScene != SCENE_LOBBY)
			m_pController->move(PxVec3(0, -1.f, 0), 0.f, deltaTime, PxControllerFilters{});

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
			PxRaycastBuffer hit;
			PxQueryFilterData filterData;
			filterData.data.word0 = CPxManager::GROUP1;
			if (CEngine::GetInstance()->Raycast(vCamPos, vRayDir, 100.f, hit, filterData))
			{
				// Player를 제외한 actor
				if (hit.block.actor->userData != this)
				{
					PxVec3 hitPos = hit.block.position;
					PxVec3 hitNormal = hit.block.normal;

					m_cursorHitPos = { hitPos.x, hitPos.y, hitPos.z, 0 };
				}
			}

			RaycastRifleToHitPos();
		}

		TimeCheckEnemyInSight += CEngine::GetInstance()->ComputeDeltaTime("CheckEnemyInSight");
		if (TimeCheckEnemyInSight > 1.f)
		{
			CheckEnemyInSight();
			TimeCheckEnemyInSight = 0.f;
		}

		RotateBody(deltaTime);

		if (CEngine::GetInstance()->IsMousePressed(0))
			Shot(deltaTime);

		if (m_pWeaponInHand == m_pPrimaryWeapon)
		{
			UpdateRifleLightTransform(m_pPrimaryWeapon);
			UpdateRifleMuzzleLightRange(deltaTime);
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
	if (g_eCurScene == SCENE_LOBBY)
	{
		m_pPrimaryWeapon = m_pEquipment->GetCurrentEquipment(EQUIPMENT::PRIMARY)->model;
		m_pSecondaryWeapon = m_pEquipment->GetCurrentEquipment(EQUIPMENT::SECONDARY)->model;
		m_pGrenade = m_pEquipment->GetCurrentEquipment(EQUIPMENT::GRENADE)->model;
		m_pTool = m_pEquipment->GetCurrentEquipment(EQUIPMENT::TOOL)->model;
		
		m_pWeaponInHand = m_pPrimaryWeapon;
	}

	if (m_pWeaponInHand != m_pPrimaryWeapon)
		SetObjectTransform(m_pPrimaryWeapon, m_pSlingBone);

	SetObjectTransform(m_pWeaponInHand, m_pHandBone);
	SetObjectTransform(m_pSecondaryWeapon, m_pRThighBone);
	SetObjectTransform(m_pGrenade, m_pGrenadeBone);
	SetObjectTransform(m_pTool, m_pToolBone);
}

void CPlayer::UpdateRifleLightTransform(CGameObject * pWeapon)
{
	__super::UpdateRifleLightTransform(pWeapon);
	m_pRifleLightTransform->SetMatrix(m_pMuzzleLightTransform->GetMatrix());
}

void CPlayer::EquipWeapon(EQUIPMENT eType)
{
	if (eType == EQUIPMENT::PRIMARY)
	{
		m_pWeaponInHand = m_pPrimaryWeapon;
	}
	else if (eType == EQUIPMENT::GRENADE)
	{
		m_pWeaponInHand = CEngine::GetInstance()->SpawnPrefab("Frag");
		m_pGrenadeInHand = dynamic_cast<CGrenade*>(CEngine::GetInstance()->AddScriptObject(CGrenade::Create(m_pWeaponInHand)));
	}
}

void CPlayer::UnEquipWeapon(EQUIPMENT eType)
{
	if (eType == EQUIPMENT::PRIMARY)
	{
		m_pWeaponInHand = nullptr;
	}
	else if (eType == EQUIPMENT::GRENADE)
	{
		if (m_pGrenadeInHand)
			m_pGrenadeInHand->SetDead();
		m_pGrenadeInHand = nullptr;
		m_pWeaponInHand = nullptr;
	}
}

void CPlayer::ChangeGear(EQUIPMENT eType, _uint iIndex)
{
	CEquipmentPool* equipmentPool = CEquipmentPool::GetInstance();

	equipmentPool->GetEquipment(eType, iIndex)->mesh->SetActive(true);
}

void CPlayer::RotateBody(_double deltaTime)
{
	m_targetUpperRotation.x = GetXAxisAngle(m_cursorHitPos);
	m_targetUpperRotation.y = GetYAxisAngle(m_cursorHitPos);

	_float	fFollowSpeed = deltaTime * 15.f;
	m_curUpperRotation.x = Lerp(m_curUpperRotation.x, m_targetUpperRotation.x, fFollowSpeed);
	m_curUpperRotation.y = Lerp(m_curUpperRotation.y, m_targetUpperRotation.y, fFollowSpeed);

	m_pModel->SetUpperRotationAngle(m_curUpperRotation);

	// Rotating Body
	fFollowSpeed = deltaTime * 8.f;
	m_curBodyRotation = Lerp(m_curBodyRotation, m_targetBodyRotation, fFollowSpeed);
	m_pTransform->SetUpRotation(_vector{ 0, 1, 0, 0 }, m_curBodyRotation);
}

void CPlayer::Shot(_double deltaTime)
{
	static const _double fShotDelay = 0.1;

	if (m_ShotTime >= fShotDelay)
	{
		// hitPos = m_rifleHitPos
		CGameManager::GetInstance()->PlayMuzzleEffect();
		CGameManager::GetInstance()->PlayImpactSmokeEffect(m_rifleHitPos, m_rifleHitNormal);

		IScriptObject* hitObject = static_cast<IScriptObject*>(m_pHitActor->userData);
		if (dynamic_cast<CEnemy*>(hitObject))
			dynamic_cast<CEnemy*>(hitObject)->GetDamage(m_pTransform->GetState(CTransform::STATE_POSITION));

		CGameManager::GetInstance()->Shake_Rifle();

		m_ShotTime = 0.0f;
		m_fMuzzleLightRange = 3.f;
	}

	m_ShotTime += deltaTime;
}

void CPlayer::ThrowGrenade()
{
	if (m_pGrenadeInHand)
	{
		m_pGrenadeInHand->OnThrow(m_ThrowingVelocity, 0);
		m_pGrenadeInHand = nullptr;
		m_pWeaponInHand = nullptr;
	}
}

void CPlayer::SetGrenadeTrajectory(_bool result)
{
	if (!result)
	{
		if (m_pGrenadeTrajectory)
			m_pGrenadeTrajectory->SetActive(false);
		return;
	}

	if (nullptr == m_pGrenadeInHand)
		return;
	if (m_pGrenadeTrajectory)
		m_pGrenadeTrajectory->SetActive(true);
	_vector pos = XMVectorSetW(m_pTransform->GetState(CTransform::STATE_POSITION), 0.f);
	_float length = XMVectorGetX(XMVector3Length(m_cursorHitPos - pos));
	_float time = log10(length + 1);

	_vector scale, rot, origin;
	_matrix originMatrix = m_pHandBone->pHierarchyNode->Get_CombinedTransformationMatrix() * m_pTransform->GetWorldMatrix();
	XMMatrixDecompose(&scale, &rot, &origin, originMatrix);
	origin = XMVectorSetW(origin, 0.f);

	// Calculate Velocity
	_vector distance = m_cursorHitPos - origin;
	_vector distanceXZ = distance;
	distanceXZ = XMVectorSetW(XMVectorSetY(distanceXZ, 0.f), 0.f);

	_float sY = XMVectorGetY(distance);
	_float sXZ = XMVectorGetX(XMVector4Length(distanceXZ));

	_float Vxz = sXZ / time;
	_float Vy = (sY / time) + (0.5f * 9.81 * time);

	m_ThrowingVelocity = XMVector3Normalize(distanceXZ);
	m_ThrowingVelocity *= Vxz;
	m_ThrowingVelocity = XMVectorSetY(m_ThrowingVelocity, Vy);

	// Calucate Pos In Time
	VisualizeTrajectory(origin, m_ThrowingVelocity, time);
}

void CPlayer::VisualizeTrajectory(_vector origin, _vector initialVelocity, _float time)
{
	list<_vector> listPoints;
	_uint lineSegment = 20;
	for (int i = 0; i < lineSegment; ++i)
	{
		_vector pos = CalculatePosInTime(origin, initialVelocity, (time / (_float)lineSegment) * (i + 1));
		listPoints.push_back(pos);
	}

	if (m_pGrenadeTrajectory)
		dynamic_cast<CEffect_Trajectory*>(m_pGrenadeTrajectory)->SetPoints(listPoints);
}

_vector CPlayer::CalculatePosInTime(_vector origin, _vector initialVelocity, _float time)
{
	origin = XMVectorSetW(origin, 0.f);
	initialVelocity = XMVectorSetW(initialVelocity, 0.f);

	_vector Vxz = initialVelocity;
	Vxz = XMVectorSetY(Vxz, 0.f);


	_vector result = origin + initialVelocity * time;
	_float sY = (-0.5f * 9.81 * (time * time)) + (XMVectorGetY(initialVelocity) * time) + XMVectorGetY(origin);

	result = XMVectorSetY(result, sY);

	return result;
}

_float CPlayer::GetXAxisAngle(_vector hitPos)
{
	_matrix spineMat = m_pSpineBone->pHierarchyNode->Get_CombinedTransformationMatrix() * m_pTransform->GetWorldMatrix();
	_vector mySpineTempA, mySpineTempB, mySpinePos;
	XMMatrixDecompose(&mySpineTempA, &mySpineTempB, &mySpinePos, spineMat);
	mySpinePos = XMVectorSetW(mySpinePos, 0);

	_vector hitposDir;
	// mySpinePos-> hitpos
	if (m_pGrenadeTrajectory)
	{
		if (m_pGrenadeTrajectory->IsActive())
			hitposDir = XMVectorSetW(m_ThrowingVelocity, 0.f);
		else
			hitposDir = XMVectorSubtract(hitPos, mySpinePos);
	}
	else
		hitposDir = XMVectorSubtract(hitPos, mySpinePos);

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

	//ADDLOG(to_string(XMConvertToDegrees( xAxisAngle)).c_str());
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
	static const _float sightRange = 20.f;
	const PxU32 bufferSize = 256;

	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer hit(hitBuffer, bufferSize);            // [out] Overlap results
	PxSphereGeometry overlapShape = PxSphereGeometry(sightRange);  // [in] shape to test for overlaps
	PxTransform position;    // [in] initial shape pose (at distance=0)
	_vector vecPosition = m_pTransform->GetState(CTransform::STATE_POSITION);
	position.q = PxIdentity;
	memcpy(&position.p, &vecPosition, sizeof(_float3));

	PxQueryFilterData fd;
	fd.data.word1 = CPxManager::GROUP4;

	// first check if enemy is in the range
	if (CEngine::GetInstance()->GetScene()->overlap(overlapShape, PxTransform(position), hit, fd))
	{
		for (PxU32 i = 0; i < hit.getNbAnyHits(); ++i)
		{
			// filter out player
			if (hit.getAnyHit(i).actor->userData != this)
			{
				// check if there is obstacle between player and enemy
				CEnemy* enemy = static_cast<CEnemy*>(hit.getAnyHit(i).actor->userData);
				PxTransform enemyPos = hit.getAnyHit(i).actor->getGlobalPose();
				PxTransform controllerPos = m_pController->getActor()->getGlobalPose();
				PxVec3 rayDir = (enemyPos.p - controllerPos.p).getNormalized();
				_float range = (enemyPos.p - controllerPos.p).magnitude();
				PxRaycastBuffer rayHit;
				PxQueryFilterData filterData;
				filterData.data.word1 = CPxManager::GROUP3;
				if (CEngine::GetInstance()->Raycast(controllerPos.p, rayDir, range, rayHit, filterData))
					enemy->SetVisibility(VISIBILITY::INVISIBLE);
				else
					enemy->SetVisibility(VISIBILITY::VISIBLE);
			}
		}
	}
}

void CPlayer::RaycastRifleToHitPos()
{
	_vector origin;
	_vector dir;
	_float range;

	_matrix spineMat = m_pSpineBone->pHierarchyNode->Get_CombinedTransformationMatrix() * m_pTransform->GetWorldMatrix();
	_vector mySpineTempA, mySpineTempB;
	XMMatrixDecompose(&mySpineTempA, &mySpineTempB, &origin, spineMat);
	origin = XMVectorSetW(origin, 0);
	dir = XMVector3Normalize(m_cursorHitPos - origin);
	if (XMVectorGetY(m_cursorHitPos) != 0.f)
		int i = 0;
	range = XMVectorGetX(XMVector3Length(m_cursorHitPos - origin)) + 1.f;

	PxRaycastBuffer hit;
	PxQueryFilterData filterData;
	filterData.data.word1 = CPxManager::GROUP3;
	filterData.data.word2 = CPxManager::GROUP3;
	if (CEngine::GetInstance()->Raycast(origin, dir, range, hit, filterData))
	{
		PxU32 distance = hit.block.distance;
		PxU32 faceIndex = hit.block.faceIndex;
		PxVec3 hitPos = hit.block.position;
		PxVec3 hitNormal = hit.block.normal;

		memcpy(&m_rifleHitPos, &hitPos, sizeof(PxVec3));
		m_rifleHitPos = XMVectorSetW(m_rifleHitPos, 1.f);
		memcpy(&m_rifleHitNormal, &hitNormal, sizeof(PxVec3));
		m_rifleHitPos = XMVectorSetW(m_rifleHitPos, 0.f);

		PxVec3 cursorPos = { XMVectorGetX(m_cursorHitPos), XMVectorGetY(m_cursorHitPos), XMVectorGetZ(m_cursorHitPos) };

		if ((hitPos - cursorPos).normalize() < 0.5f)
			CGameManager::GetInstance()->SetShootingSightUI(false);
		else
			CGameManager::GetInstance()->SetShootingSightUI(true, m_rifleHitPos, m_cursorHitPos);

		m_pHitActor = hit.block.actor;
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
	m_pSlingBone = m_pModel->Find_Bone("sling");
	m_pHeadBone = m_pModel->Find_Bone("sling");
}

_vector CPlayer::GetBonePos(EQUIPMENT eType)
{
	_vector out = {0.f, };
	BONEDESC* bone = nullptr;
	switch (eType)
	{
	case Client::EQUIPMENT::PRIMARY:
		bone = m_pHandBone;
		break;
	case Client::EQUIPMENT::PRIMARYMAG:
		bone = m_pSpineBone;
		break;
	case Client::EQUIPMENT::SECONDARY:
		bone = m_pRThighBone;
		break;
	case Client::EQUIPMENT::GRENADE:
		bone = m_pGrenadeBone;
		break;
	case Client::EQUIPMENT::TOOL:
		bone = m_pToolBone;
		break;
	case Client::EQUIPMENT::HEADGEAR:
		bone = m_pHeadBone;
		break;
	case Client::EQUIPMENT::TORSO:
		bone = m_pSlingBone;
		break;
	case Client::EQUIPMENT::LEGS:
		bone = m_pRThighBone;
		break;
	case Client::EQUIPMENT::VEST:
		bone = m_pSlingBone;
		break;
	case Client::EQUIPMENT::BACKPACK:
		bone = m_pSpineBone;
		break;
	case Client::EQUIPMENT::NONE:
		break;
	default:
		break;
	}

	if (bone)
		out = GetBonePosition(bone);

	return out;
}

void CPlayer::Render()
{
}
