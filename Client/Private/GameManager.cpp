#include "stdafx.h"
#include "..\Public\GameManager.h"
USING(Client)
IMPLEMENT_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
}

HRESULT CGameManager::Initialize()
{
	CGameObject* sceneMask = CEngine::GetInstance()->SpawnPrefab("SceneMask");
	m_pSceneMask = dynamic_cast<CVIBuffer_RectUI*>(sceneMask->GetComponent("Com_VIBuffer"));
	m_pSceneMask->SetColor(_float4(0, 0, 0, 0));

	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_CAMERA");
	if (list.size() <= 0)
		return E_FAIL;

	m_Camera.resize(list.size());
	m_Camera[(size_t)CAMERA::FOLLOW] = dynamic_cast<CCamera*>(list.front());
	m_Camera[(size_t)CAMERA::FLY] = dynamic_cast<CCamera*>(list.back());

	// SwitchCamera(CAMERA::FLY);
	m_Camera[(size_t)CAMERA::FOLLOW]->SetRolling(false);
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
	if (m_fMastAlpha >= 0)
	{
		m_fMastAlpha -= TimeDelta;
		m_pSceneMask->SetColor(_float4(0, 0, 0, m_fMastAlpha));
	}
}

void CGameManager::SwitchCamera(CAMERA type)
{
	m_Camera[(size_t)m_eCurCamera]->SetRolling(false);
	m_Camera[(size_t)type]->SetRolling(true);
	m_eCurCamera = type;
	m_fMastAlpha = 3.f;
}

void CGameManager::Free()
{
}
