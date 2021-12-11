#include "stdafx.h"
#include "..\Public\Player.h"
#include "PxManager.h"
#include "BoxCollider.h"

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
	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (list.size() <= 0)
		return E_FAIL;

	m_pGameObject = list.front();
	m_pTransform = dynamic_cast<CTransform*>(m_pGameObject->GetComponent("Com_Transform"));
	m_pCollider = dynamic_cast<CCollider*>(m_pGameObject->GetComponent("Com_Collider"));
	m_pModel = dynamic_cast<CModel*>(m_pGameObject->GetComponent("Com_Model"));
	m_pController = m_pCollider->GetController();
	return S_OK;
}

void CPlayer::Update(_double deltaTime)
{
	if (m_pController)
	{
		// Look Vector
		//m_pTransform->RotateAxis(m_pTransform->GetState(CTransform::STATE_UP), 0.001f);
		_vector look = m_pTransform->GetState(CTransform::STATE_LOOK);
		PxVec3 pxLook;
		memcpy(&pxLook, &look, sizeof(PxVec3));

		_vector right = m_pTransform->GetState(CTransform::STATE_RIGHT);
		PxVec3 pxRight;
		memcpy(&pxRight, &right, sizeof(PxVec3));

		_float fSpeedFactor = 10.f;
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
}

void CPlayer::LapteUpdate(_double deltaTime)
{
	static int anim = 0;
	if (CEngine::GetInstance()->IsKeyDown('0'))
		anim = 0;
	if (CEngine::GetInstance()->IsKeyDown('1'))
		anim = 1;

	m_pModel->SetUp_AnimationIndex(anim);
	m_pModel->Play_Animation(deltaTime);
}
