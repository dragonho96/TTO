#include "stdafx.h"
#include "..\Public\Grenade.h"
#include "Enemy.h"
#include "GameManager.h"

USING(Client)
CGrenade::CGrenade(CGameObject * pObj)
	: m_pGameObject(pObj)
{
	Initialize();
}

CGrenade * CGrenade::Create(CGameObject * pObj)
{
	CGrenade*		pInstance = new CGrenade(pObj);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create CEnemy");
		SafeRelease(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CGrenade::Free()
{
	if (m_pGameObject)
		m_pGameObject->SetDead();
}

HRESULT CGrenade::Initialize()
{
	m_pTransform = dynamic_cast<CTransform*>(m_pGameObject->GetComponent("Com_Transform"));
	m_pModel = dynamic_cast<CModel*>(m_pGameObject->GetComponent("Com_Model"));
	m_pCollider = dynamic_cast<CCollider*>(m_pGameObject->GetComponent("Com_Collider"));
	
	if (m_pCollider->GetRigidActor()->is<PxRigidDynamic>())
	{
		m_pCollider->GetRigidActor()->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		PxShape* shape = nullptr;
		m_pCollider->GetRigidActor()->getShapes(&shape, 1);
		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
	}


	m_Timer = "Timer_Explosion" + to_string(m_pGameObject->GetUUID());
	CEngine::GetInstance()->AddTimers(m_Timer);
	return S_OK;
}

void CGrenade::Update(_double deltaTime)
{
	if (!m_bThrown)
	{
		_float4x4 grenadeMatrix = m_pTransform->GetMatrix();
		PxTransform pxGrenadeTransform = { PxVec3{}, PxIdentity };
		memcpy(&pxGrenadeTransform.p, grenadeMatrix.m[3], sizeof(PxVec3));
		m_pCollider->GetRigidActor()->setGlobalPose(pxGrenadeTransform);
		m_pCollider->GetRigidActor()->is<PxRigidDynamic>()->setKinematicTarget(pxGrenadeTransform);
	}
	else
	{
		if (m_pCollider->GetRigidActor()->is<PxRigidDynamic>())
		{
			PxTransform pxGrenadeTransform = m_pCollider->GetRigidActor()->getGlobalPose();
			PxMat44 pxMat(pxGrenadeTransform);
			m_pTransform->SetPxMatrix(pxMat);

			m_TimeExplosion += CEngine::GetInstance()->ComputeDeltaTime(m_Timer);
			//ADDLOG(("m_TimeExplosion : " + to_string(m_TimeExplosion)).c_str());
			if (m_TimeExplosion >= 4.5f)
				Explode();
		}
	}

}

void CGrenade::LapteUpdate(_double deltaTime)
{
}

void CGrenade::Render()
{
}

void CGrenade::OnThrow(_vector dir, _float forcePower)
{
	PxVec3 forceDir;
	memcpy(&forceDir, &dir, sizeof(PxVec3));
	// forceDir *= forcePower;
	
	//ADDLOG(("m_ThrowingDir: " + to_string(XMVectorGetX(dir)) + ", " + to_string(XMVectorGetY(dir)) + ", " + to_string(XMVectorGetZ(dir))).c_str());

	m_pCollider->GetRigidActor()->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	m_pCollider->GetRigidActor()->is<PxRigidDynamic>()->addForce(forceDir, PxForceMode::eIMPULSE);
	
	CEngine::GetInstance()->ComputeDeltaTime(m_Timer);
	m_bThrown = true;
}

void CGrenade::Explode()
{
	static const _float sightRange = 15.f;
	const PxU32 bufferSize = 256;

	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer hit(hitBuffer, bufferSize);           
	PxSphereGeometry overlapShape = PxSphereGeometry(sightRange); 
	PxTransform position; 
	_vector vecPosition = m_pTransform->GetState(CTransform::STATE_POSITION);
	position.q = PxIdentity;
	memcpy(&position.p, &vecPosition, sizeof(_float3));
	PxQueryFilterData fd;
	fd.data.word1 = CPxManager::GROUP4;

	if (CEngine::GetInstance()->GetScene()->overlap(overlapShape, PxTransform(position), hit, fd))
	{
		for (PxU32 i = 0; i < hit.getNbAnyHits(); ++i)
		{
			// filter out player
			// if (hit.getAnyHit(i).actor->userData != nullptr)
			if (hit.getAnyHit(i).actor->userData)
			{
				CCharacter* character = static_cast<CCharacter*>(hit.getAnyHit(i).actor->userData);
				if (dynamic_cast<CEnemy*>(character))
				{
					CEnemy* enemy = static_cast<CEnemy*>(hit.getAnyHit(i).actor->userData);
					PxTransform enemyPos = hit.getAnyHit(i).actor->getGlobalPose();
					//ADDLOG(("Pos : " + to_string(enemyPos.p.x) + ", " + to_string(enemyPos.p.z)).c_str());

					enemy->GetDamage(vecPosition);
				}
			}
		}
	}
	CGameManager::GetInstance()->PlayImpactEffect(vecPosition);
	CGameManager::GetInstance()->PlayExplosion(vecPosition);
	CGameManager::GetInstance()->Shake_Grenade();
	CEngine::GetInstance()->ComputeDeltaTime(m_Timer);
	SetDead();
}
