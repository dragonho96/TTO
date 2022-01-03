#include "stdafx.h"
#include "..\Public\GrenadeState.h"
#include "RifleState.h"

USING(Client)
void CGrenadeState::HandleInput(CStateMachine ** pState, CPlayer& pPlayer)
{
	if (m_bThrowing)
	{
		if (pPlayer.m_pModel->IsUpperFinished())
		{
			m_bThrowing = false;
		}
		else
			return;
	}

	if (m_bEquipping && pPlayer.m_pModel->IsUpperFinished())
	{
		pPlayer.EquipWeapon(EQUIPMENT::GRENADE);
		m_bEquipping = false;
	}

	if (!m_bUnEquipping && !m_bEquipping && CEngine::GetInstance()->IsKeyDown('1'))
	{
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::UNEQUIP_GRENADE, ANIM_TYPE::UPPER);
		pPlayer.UnEquipWeapon(EQUIPMENT::GRENADE);
		m_bUnEquipping = true;
	}

	if (m_bUnEquipping && pPlayer.m_pModel->IsUpperFinished())
	{
		m_bUnEquipping = false;
		*pState = CStateMachine::rifle;
		(*pState)->Enter(pState, pPlayer);
		return;
	}

	if (m_bEquipping || m_bUnEquipping)
		return;

	if (CEngine::GetInstance()->IsMousePressed(1))
	{
		if (CEngine::GetInstance()->IsMouseDown(0))
		{
			// Wait till animation is over
			pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::THROW_GRENADE, ANIM_TYPE::UPPER);
			pPlayer.ThrowGrenade();
			m_bThrowing = true;
		}
		else
			pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::AIM_GRENADE, ANIM_TYPE::UPPER);
	}
	else
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::IDLE_GRENADE, ANIM_TYPE::UPPER);

}

void CGrenadeState::Update(CStateMachine ** pState, CPlayer& pPlayer)
{
}

void CGrenadeState::Enter(CStateMachine ** pState, CPlayer& pPlayer)
{
	pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::EQUIP_GRENADE, ANIM_TYPE::UPPER);
	m_bEquipping = true;
	m_bUnEquipping = false;
	m_bThrowing = false;
}
