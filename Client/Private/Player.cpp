#include "stdafx.h"
#include "..\Public\Player.h"

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

		if (CEngine::GetInstance()->IsKeyPressed(VK_UP))
		{
			m_pController->move(pxLook, 0.f, deltaTime, PxControllerFilters{});
		}
		if (CEngine::GetInstance()->IsKeyPressed(VK_DOWN))
		{
			m_pController->move(-pxLook, 0.f, deltaTime, PxControllerFilters{});
		}
		if (CEngine::GetInstance()->IsKeyPressed(VK_RIGHT))
		{
			m_pController->move(pxRight, 0.f, deltaTime, PxControllerFilters{});
		}
		if (CEngine::GetInstance()->IsKeyPressed(VK_LEFT))
		{
			m_pController->move(-pxRight, 0.f, deltaTime, PxControllerFilters{});
		}

		PxExtendedVec3 pos = m_pController->getPosition();
		_float3 newPos = { (float)pos.x, (float)pos.y, (float)pos.z };
		m_pTransform->SetState(CTransform::STATE_POSITION, XMLoadFloat3(&newPos));

		m_pController->move(PxVec3(0, -1.f, 0), 0.f, deltaTime, PxControllerFilters{});
	}
}

void CPlayer::LapteUpdate(_double deltaTime)
{

}
