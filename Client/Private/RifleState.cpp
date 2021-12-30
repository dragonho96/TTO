#include "stdafx.h"
#include "..\Public\RifleState.h"
#include "GrenadeState.h"

USING(Client)

void CRifleState::HandleInput(CStateMachine ** pState, CPlayer& pPlayer)
{
	if (m_bEquipping && pPlayer.m_pModel->IsUpperFinished())
		m_bEquipping = false;

	if (!m_bUnEquipping && !m_bEquipping && CEngine::GetInstance()->IsKeyDown('3'))
	{
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::UNEQUIP_RIFLE, ANIM_TYPE::UPPER);
		m_bUnEquipping = true;
	}
	if (m_bUnEquipping && pPlayer.m_pModel->IsUpperFinished())
	{
		m_bUnEquipping = false;
		*pState = CStateMachine::grenade;
		(*pState)->Enter(pState, pPlayer);
		return;
	}

	if (m_bEquipping || m_bUnEquipping)
		return;

	pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::IDLE_RIFLE, ANIM_TYPE::UPPER);
}

void CRifleState::Update(CStateMachine ** pState, CPlayer& pPlayer)
{
}

void CRifleState::Enter(CStateMachine ** pState, CPlayer& pPlayer)
{
	pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::EQUIP_RIFLE, ANIM_TYPE::UPPER);
	m_bEquipping = true;
	m_bUnEquipping = false;
}
