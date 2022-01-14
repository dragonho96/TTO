#include "stdafx.h"
#include "..\Public\Enemy.h"
#include "PathFinding.h"

unsigned int APIENTRY Dissolve_Thread(void* pArg)
{
	CEnemy*		pEnemy = (CEnemy*)pArg;

	EnterCriticalSection(&pEnemy->Get_CS());

	pEnemy->Dissolve();

	LeaveCriticalSection(&pEnemy->Get_CS());

	return 0;
}

USING(Client)
CEnemy::CEnemy(CGameObject * pObj)
	:CCharacter(pObj)
{

}

CEnemy * CEnemy::Create(CGameObject * pObj)
{
	CEnemy*		pInstance = new CEnemy(pObj);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create CEnemy");
		SafeRelease(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CEnemy::Free()
{
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);

		DeleteCriticalSection(&m_CS);

		DeleteObject(m_hThread);
	}
}

HRESULT CEnemy::Initialize()
{
	InitializeCriticalSection(&m_CS);

	m_pTransform = dynamic_cast<CTransform*>(m_pGameObject->GetComponent("Com_Transform"));
	m_pModel = dynamic_cast<CModel*>(m_pGameObject->GetComponent("Com_Model"));
	m_pCollider = dynamic_cast<CCollider*>(m_pGameObject->GetComponent("Com_Collider"));
	if (m_pCollider)
	{
		m_pController = m_pCollider->GetController();
		m_pController->getActor()->userData = this;
		PxShape* shape = nullptr;
		m_pController->getActor()->getShapes(&shape, 1);
		PxFilterData filterData;
		filterData.word0 = CPxManager::GROUP1;
		filterData.word1 = CPxManager::GROUP4;
		filterData.word2 = CPxManager::GROUP3;
		shape->setQueryFilterData(filterData);
	}

	// Add Weapon
	m_pHandBone = m_pModel->Find_Bone("item_r");
	m_pWeapon = CEngine::GetInstance()->SpawnPrefab("AKM");
	// Add MuzzleLight
	if (g_eCurScene == SCENE_TEST)
	{
		m_pMuzzleLight = CEngine::GetInstance()->SpawnPrefab("RifleMuzzleLight");
		m_pMuzzleLightTransform = dynamic_cast<CTransform*>(m_pMuzzleLight->GetComponent("Com_Transform"));
		m_pMuzzleLightCom = dynamic_cast<CLight*>(m_pMuzzleLight->GetComponent("Com_Light"));
		m_pMuzzleLightCom->SetRange(m_fCurMuzzleLightRange);

		list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
		m_pTargetTransform = dynamic_cast<CTransform*>(objList.front()->GetComponent("Com_Transform"));


		m_Timer = "Timer_PathFinding" + to_string(m_pGameObject->GetUUID());
		CEngine::GetInstance()->AddTimers(m_Timer);
	}

	return S_OK;
}

void CEnemy::Update(_double deltaTime)
{
	if (g_eCurScene == SCENE_TEST)
	{
		m_fPathFinding += CEngine::GetInstance()->ComputeDeltaTime(m_Timer);
		if (m_fPathFinding > 1.f)
			FindPath();

		if (m_pController)
		{
			Move(deltaTime);
			SetObjectTransform(m_pWeapon, m_pHandBone);
			UpdateRifleMuzzleLightRange(deltaTime);
			UpdateRifleLightTransform(m_pWeapon);
		}
	}

	CheckVisibility();

	if (m_fDissolveCutoff > 0.f)
		dynamic_cast<CEmptyGameObject*>(m_pGameObject)->SetRenderGroup(CRenderer::RENDER_ALPHA);
	else
		dynamic_cast<CEmptyGameObject*>(m_pGameObject)->SetRenderGroup(CRenderer::RENDER_NONALPHA);

	m_pModel->SetDissolve(m_fDissolveCutoff);
}

void CEnemy::LapteUpdate(_double deltaTime)
{
	m_pModel->Play_Animation(deltaTime);
}

void CEnemy::Render()
{
}

void CEnemy::CheckVisibility()
{
	if (m_ePreVisibility != m_eCurVisibility)
	{
		switch (m_eCurVisibility)
		{
		case Client::VISIBILITY::VISIBLE:
			m_pWeapon->SetActive(true);
			break;
		case Client::VISIBILITY::INVISIBLE:
			m_pWeapon->SetActive(false);
			break;
		default:
			break;
		}
		m_ePreVisibility = m_eCurVisibility;

		m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Dissolve_Thread, this, 0, nullptr);

		if (0 == m_hThread)
			return;
	}
}

void CEnemy::Move(_double deltaTime)
{
	_vector playerPosition = m_pTargetTransform->GetState(CTransform::STATE_POSITION);
	_vector myPosition = m_pTransform->GetState(CTransform::STATE_POSITION);

	_float distance = XMVectorGetX(XMVector3Length(playerPosition - myPosition));

	if (distance < 5.f)
	{
		m_eCurAnim = ANIM_ENEMY::WALK;
		m_fSpeedFactor = 40.f;
	}
	else if (distance < 10.f)
	{
		m_eCurAnim = ANIM_ENEMY::RUN;
		m_fSpeedFactor = 20.f;
	}
	else
		m_eCurAnim = ANIM_ENEMY::IDLE;


	m_pModel->SetUp_AnimationIndex((size_t)m_eCurAnim, ANIM_TYPE::NONE);
	if (m_eCurAnim != ANIM_ENEMY::IDLE)
		FollowPlayer(deltaTime);

	// Gravity
	m_pController->move(PxVec3(0, -1.f, 0), 0.f, deltaTime, PxControllerFilters{});
}

void CEnemy::FindPath()
{
	m_fPathFinding = 0.f;
	_float3 startPos, targetPos;
	XMStoreFloat3(&startPos, m_pTransform->GetState(CTransform::STATE_POSITION));
	XMStoreFloat3(&targetPos, m_pTargetTransform->GetState(CTransform::STATE_POSITION));
	CPathFinding::GetInstance()->FindPath_Thread(this, startPos, targetPos);
}

void CEnemy::FollowPlayer(_double deltaTime)
{
	if (m_pathPosition.empty() || nullptr == m_pController)
		return;

	// Look PositionÀÌ path·Î
	_vector myPos = m_pTransform->GetState(CTransform::STATE_POSITION);
	_vector pathPos = m_pathPosition.back();
	myPos = XMVectorSetW(XMVectorSetY(myPos, 0), 0);
	pathPos = XMVectorSetW(XMVectorSetY(pathPos, 0), 0);

	_vector m_velocity = pathPos - myPos;
	if (0.1 >= XMVectorGetX(XMVector4Length(m_velocity)))
	{
		m_pathPosition.pop_back();
		if (m_pathPosition.empty())
			return;

		m_velocity = myPos - pathPos;
	}


	m_pTransform->LookAtForLandObject(m_pTargetTransform->GetState(CTransform::STATE_POSITION));
	m_velocity = XMVector4Normalize(m_velocity);
	m_velocity /= m_fSpeedFactor;
	m_curVelocity = XMVectorLerp(m_curVelocity, m_velocity, deltaTime * 20.f);


	PxVec3 pxDir;
	memcpy(&pxDir, &m_curVelocity, sizeof(PxVec3));
	m_pController->move(pxDir, 0.f, deltaTime, PxControllerFilters{});


	if (m_eCurAnim == CEnemy::ANIM_ENEMY::WALK)
		Shot(deltaTime);
}

void CEnemy::Shot(_double deltaTime)
{
	static _double fShotDelay = 0.5;

	if (m_ShotTime >= fShotDelay)
	{
		mt19937 engine(rand());
		uniform_real_distribution<_float> distribution(0.1f, 0.5f);//
		auto generator = bind(distribution, engine);

		fShotDelay = generator();

		m_ShotTime = 0.0f;
		m_fMuzzleLightRange = 3.f;

	}

	m_ShotTime += deltaTime;
}

void CEnemy::GetDamage(_vector sourceLocation)
{
	if (m_pController)
	{
		m_pCollider->ReleaseController();
		m_pController = nullptr;
		m_pModel->SetRagdollSimulate(true);

		_vector position = m_pTransform->GetState(CTransform::STATE_POSITION);
		_vector damageDir = XMVector3Normalize(position - sourceLocation);
		PxVec3 pxDamageDir;
		memcpy(&pxDamageDir, &damageDir, sizeof(PxVec3));
		_float fDamagePower = XMVectorGetX(XMVector4Length(position - sourceLocation)) * 500.f;
		//m_pModel->GetRagdollRb("body")->addForce(pxDamageDir.getNormalized(), PxForceMode::eIMPULSE);
		m_pModel->GetRagdollRb("body")->addForce(pxDamageDir * fDamagePower, PxForceMode::eIMPULSE);
		ADDLOG(("Hit Damage: " + to_string(fDamagePower)).c_str());

		string logStr = "" + to_string(pxDamageDir.x) + " " + to_string(pxDamageDir.y) + " " + to_string(pxDamageDir.z);
		ADDLOG(("Hit Dir: " + logStr).c_str());
	}
}

void CEnemy::Dissolve()
{
	CEngine::GetInstance()->AddTimers("Timer_Dissolve");
	CEngine::GetInstance()->ComputeDeltaTime("Timer_Dissolve");

	_float fSpeed = 3.f;

	if (m_eCurVisibility == VISIBILITY::VISIBLE)
	{
		while (m_fDissolveCutoff >= 0.f)
		{
			m_fDissolveCutoff -= CEngine::GetInstance()->ComputeDeltaTime("Timer_Dissolve") * fSpeed;
		}
		m_fDissolveCutoff = 0.f;
	}
	else
	{
		while (m_fDissolveCutoff <= 1.f)
		{
			m_fDissolveCutoff += CEngine::GetInstance()->ComputeDeltaTime("Timer_Dissolve") * fSpeed;
		}
		m_fDissolveCutoff = 1.f;
	}

}


