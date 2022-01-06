#include "stdafx.h"
#include "..\Public\Enemy.h"
#include "PathFinding.h"

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
	}

	list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	m_pTargetTransform = dynamic_cast<CTransform*>(objList.front()->GetComponent("Com_Transform"));

	m_Timer = "Timer_PathFinding" + m_pGameObject->GetName();
	CEngine::GetInstance()->AddTimers(m_Timer);
	return S_OK;
}

void CEnemy::Update(_double deltaTime)
{
	if (CEngine::GetInstance()->IsKeyDown('6'))
		m_pModel->SetUp_AnimationIndex(1, ANIM_TYPE::NONE);

	//m_fPathFinding += CEngine::GetInstance()->ComputeDeltaTime(m_Timer);
	//if (m_fPathFinding > 1.f)
	//	FindPath();

	if (m_pController)
	{
		// FollowPlayer(deltaTime);
		// Gravity
		m_pController->move(PxVec3(0, -1.f, 0), 0.f, deltaTime, PxControllerFilters{});
	}

	CheckVisibility();
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
			// backward desolve
			break;
		case Client::VISIBILITY::INVISIBLE:
			// red �ܻ�
			// desolve
			break;
		default:
			break;
		}
		m_ePreVisibility = m_eCurVisibility;
	}
}

void CEnemy::FindPath()
{
	m_fPathFinding = 0.f;
	_float3 startPos, targetPos;
	XMStoreFloat3(&startPos, m_pTransform->GetState(CTransform::STATE_POSITION));
	XMStoreFloat3(&targetPos, m_pTargetTransform->GetState(CTransform::STATE_POSITION));
	CPathFinding::GetInstance()->FindPath_Thread(this, startPos, targetPos);
	//list<_vector> listPos = CPathFinding::GetInstance()->FindPath(startPos, targetPos);
	//if (listPos.empty())
	//	return;
	//m_pathPosition = listPos;
}

void CEnemy::FollowPlayer(_double deltaTime)
{
	if (m_pathPosition.empty() || nullptr == m_pController)
		return;
	// Look Position�� path��
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
	_float fSpeedFactor = 40.f;
	m_velocity = XMVector4Normalize(m_velocity);
	m_velocity /= fSpeedFactor;
	m_curVelocity = XMVectorLerp(m_curVelocity, m_velocity, deltaTime * 20.f);


	PxVec3 pxDir;
	memcpy(&pxDir, &m_curVelocity, sizeof(PxVec3));
	m_pController->move(pxDir, 0.f, deltaTime, PxControllerFilters{});
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

