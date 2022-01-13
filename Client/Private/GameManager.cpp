#include "stdafx.h"
#include "..\Public\GameManager.h"
#include "Effect_Muzzle.h"
#include "Effect_Impact.h"
#include "Effect_ImpactSmoke.h"
#include "Effect_Explosion.h"
#include "Camera_Follow.h"
#include "SightIndicator.h"

USING(Client)
IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
}

HRESULT CGameManager::Initialize()
{
	//CGameObject* sceneMask = CEngine::GetInstance()->SpawnPrefab("SceneMask");
	//m_pSceneMask = dynamic_cast<CVIBuffer_RectUI*>(sceneMask->GetComponent("Com_VIBuffer"));
	//m_pSceneMask->SetColor(_float4(0, 0, 0, 0));

	m_pSightIndicator = dynamic_cast<CSightIndicator*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_SightIndicator", "SightIndicator"));
	m_pSightIndicator->SetActive(false);

	m_pCrosshair = dynamic_cast<CEmptyUI*>(CEngine::GetInstance()->SpawnPrefab("Crosshair"));

	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_CAMERA");
	if (list.size() <= 0)
		return E_FAIL;

	m_Camera.resize(list.size());
	m_Camera[(size_t)CAMERA::FOLLOW] = dynamic_cast<CCamera*>(list.front());
	m_Camera[(size_t)CAMERA::FLY] = dynamic_cast<CCamera*>(list.back());

	// SwitchCamera(CAMERA::FLY);
	m_Camera[(size_t)CAMERA::FOLLOW]->SetRolling(true);
	m_Camera[(size_t)CAMERA::FLY]->SetRolling(false);

	// m_pMuzzleEffect = dynamic_cast<CEffect_Muzzle*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Muzzle", "Muzzle"));
	m_pImpactSmokeEffect = dynamic_cast<CEffect_ImpactSmoke*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_ImpactSmoke", "ImpactSmoke"));
	// m_pImpactEffect = dynamic_cast<CEffect_Impact*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Impact", "Impact"));
	// m_pExplosion = dynamic_cast<CEffect_Explosion*>(CEngine::GetInstance()->AddGameObject(0, "GameObject_Effect_Explosion", "Explosion"));
	return S_OK;
}

void CGameManager::Update(_double TimeDelta)
{
	if (CEngine::GetInstance()->IsKeyDown('M'))
	{
		if (m_eCurCamera == CGameManager::CAMERA::FLY)
			SwitchCamera(CAMERA::FOLLOW);
		else
			SwitchCamera(CAMERA::FLY);
	}
	_float3 mousePos = CEngine::GetInstance()->GetMousePosition();
	m_pCrosshair->SetClientPosition(mousePos.x, mousePos.y);
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

void CGameManager::Free()
{
}
