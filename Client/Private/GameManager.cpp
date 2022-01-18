#include "stdafx.h"
#include "..\Public\GameManager.h"
#include "Player.h"
#include "Effect_Fire.h"
#include "Effect_Muzzle.h"
#include "Effect_Impact.h"
#include "Effect_ImpactSmoke.h"
#include "Effect_Explosion.h"
#include "Camera_Follow.h"
#include "Camera_Lobby.h"
#include "SightIndicator.h"

USING(Client)
IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
}

HRESULT CGameManager::Initialize()
{
	if (g_eCurScene == SCENE_LOBBY)
	{
		list<CGameObject*> camera = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_CAMERA");
		if (camera.size() <= 0)
			return E_FAIL;
		m_CamLobby = dynamic_cast<CCamera_Lobby*>(camera.front());

		m_CamPos.resize((size_t)EQUIPMENT::NONE);
		list<CGameObject*>  camPos = CEngine::GetInstance()->GetGameObjectInLayer(0, "CamPos");
		if (camPos.size() > 0)
		{
			for (auto& child : camPos)
			{
				if (child->GetName().find("Headgear") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::HEADGEAR] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("Torso") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::TORSO] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("Legs") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::LEGS] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("Tool") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::TOOL] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("Backpack") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::BACKPACK] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("Primary") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::PRIMARY] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("Secondary") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::SECONDARY] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
				else if (child->GetName().find("OG") != std::string::npos)
					m_CamPos[(size_t)EQUIPMENT::PRIMARYMAG] = dynamic_cast<CTransform*>(child->GetComponent("Com_Transform"));
			}
		}
		m_CamPos[(size_t)EQUIPMENT::VEST] = m_CamPos[(size_t)EQUIPMENT::TORSO];
		m_CamPos[(size_t)EQUIPMENT::GRENADE] = m_CamPos[(size_t)EQUIPMENT::TORSO];


		ChangeCameraPos(EQUIPMENT::PRIMARYMAG);

		m_pCursor = dynamic_cast<CEmptyUI*>(CEngine::GetInstance()->SpawnPrefab("LobbyCursor"));
	}
	else
	{
		//CGameObject* sceneMask = CEngine::GetInstance()->SpawnPrefab("SceneMask");
		//m_pSceneMask = dynamic_cast<CVIBuffer_RectUI*>(sceneMask->GetComponent("Com_VIBuffer"));
		//m_pSceneMask->SetColor(_float4(0, 0, 0, 0));

		m_pSightIndicator = dynamic_cast<CSightIndicator*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_SightIndicator", "SightIndicator"));
		m_pSightIndicator->SetActive(false);

		//m_pCrosshair = dynamic_cast<CEmptyUI*>(CEngine::GetInstance()->SpawnPrefab("Crosshair"));

		list<class CGameObject*> camera = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_CAMERA");
		if (camera.size() <= 0)
			return E_FAIL;

		m_Camera.resize(camera.size());
		m_Camera[(size_t)CAMERA::FOLLOW] = dynamic_cast<CCamera*>(camera.front());
		m_Camera[(size_t)CAMERA::FLY] = dynamic_cast<CCamera*>(camera.back());

		// SwitchCamera(CAMERA::FLY);
		m_Camera[(size_t)CAMERA::FOLLOW]->SetRolling(true);
		m_Camera[(size_t)CAMERA::FLY]->SetRolling(false);


		list<CGameObject*> barrel = CEngine::GetInstance()->GetGameObjectInLayer(0, "Barrel");
		if (barrel.size() >= 1)
		{
			CGameObject* fireA = CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Fire", "Fire");
			//CGameObject* fireB = CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Fire", "Fire");
			CTransform* barrelATransform = dynamic_cast<CTransform*>(barrel.front()->GetComponent("Com_Transform"));
			//CTransform* barrelBTransform = dynamic_cast<CTransform*>(barrel.back()->GetComponent("Com_Transform"));
			dynamic_cast<CEffect_Fire*>(fireA)->SetPosition(barrelATransform->GetState(CTransform::STATE_POSITION));
			//dynamic_cast<CEffect_Fire*>(fireB)->SetPosition(barrelBTransform->GetState(CTransform::STATE_POSITION));
		}

		// m_pMuzzleEffect = dynamic_cast<CEffect_Muzzle*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Muzzle", "Muzzle"));
		// m_pImpactSmokeEffect = dynamic_cast<CEffect_ImpactSmoke*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_ImpactSmoke", "ImpactSmoke"));
		m_pImpactEffect = dynamic_cast<CEffect_Impact*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Impact", "Impact"));
		m_pExplosion = dynamic_cast<CEffect_Explosion*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Explosion", "Explosion"));
	}
	return S_OK;
}

void CGameManager::Update(_double TimeDelta)
{
	if (g_eCurScene == SCENE_LOBBY)
	{
		_float3 mousePos = CEngine::GetInstance()->GetMousePosition();
		m_pCursor->SetClientPosition(mousePos.x, mousePos.y);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyDown('M'))
		{
			if (m_eCurCamera == CGameManager::CAMERA::FLY)
				SwitchCamera(CAMERA::FOLLOW);
			else
				SwitchCamera(CAMERA::FLY);
		}
		//_float3 mousePos = CEngine::GetInstance()->GetMousePosition();
		//m_pCrosshair->SetClientPosition(mousePos.x, mousePos.y);
	}
	//if (m_fMastAlpha >= 0)
	//{
	//	m_fMastAlpha -= TimeDelta;
	//	m_pSceneMask->SetColor(_float4(0, 0, 0, m_fMastAlpha));
	//}
}

void CGameManager::Shake_Rifle()
{
	if (m_eCurCamera == CGameManager::CAMERA::FOLLOW)
	{
		dynamic_cast<CCamera_Follow*>(m_Camera[(size_t)m_eCurCamera])->Shake_Rifle();
	}
}

void CGameManager::Shake_Grenade()
{
	if (m_eCurCamera == CGameManager::CAMERA::FOLLOW)
	{
		dynamic_cast<CCamera_Follow*>(m_Camera[(size_t)m_eCurCamera])->Shake_Grenade_BeginThread();
	}
}

void CGameManager::SwitchCamera(CAMERA type)
{
	m_Camera[(size_t)m_eCurCamera]->SetRolling(false);
	m_Camera[(size_t)type]->SetRolling(true);
	m_eCurCamera = type;
	m_fMastAlpha = 3.f;
}

void CGameManager::SetShootingSightUI(_bool result, _vector hitPos, _vector cursorPos)
{
	// hitPos위치에 UI띄움
	// hitPos & cursorPos 두 점의 라인을 생성
	if (result)
		m_pSightIndicator->SetPoints(true, std::list<_vector>{hitPos, cursorPos});
	else
		m_pSightIndicator->SetPoints(false);
}


void CGameManager::PlayMuzzleEffect()
{
	if (m_pMuzzleEffect)
	{
		_float angle = dynamic_cast<CCamera_Follow*>(m_Camera[(size_t)CAMERA::FOLLOW])->GetAngle();
		m_pMuzzleEffect->Play(angle);
	}
}

void CGameManager::PlayImpactSmokeEffect(_vector vPos, _vector vNormal)
{
	if (m_pImpactSmokeEffect)
		m_pImpactSmokeEffect->Play(vPos, vNormal);
}

void CGameManager::PlayImpactEffect(_vector vPos)
{
	if (m_pImpactEffect)
		m_pImpactEffect->Play(vPos);
}

void CGameManager::PlayExplosion(_vector vPos)
{
	if (m_pExplosion)
		m_pExplosion->Play(vPos);
}

void CGameManager::ChangeCameraPos(EQUIPMENT eType)
{
	m_CamLobby->SetTarget(
		m_CamPos[(size_t)eType]->GetState(CTransform::STATE_POSITION), 
		m_pPlayer->GetBonePos(eType)
	);
}

void CGameManager::Free()
{
}
