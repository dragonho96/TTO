#include "stdafx.h"
#include "..\Public\RifleState.h"
#include "GrenadeState.h"

USING(Client)

void CRifleState::HandleInput(CStateMachine ** pState, CModel * pModel)
{
	if (m_bEquipping && pModel->IsUpperFinished())
		m_bEquipping = false;

	if (!m_bUnEquipping && !m_bEquipping && CEngine::GetInstance()->IsKeyDown('3'))
	{
		pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::UNEQUIP_RIFLE, ANIM_TYPE::UPPER);
		m_bUnEquipping = true;
	}
	if (m_bUnEquipping && pModel->IsUpperFinished())
	{
		m_bUnEquipping = false;
		*pState = CStateMachine::grenade;
		(*pState)->Enter(pState, pModel);
		return;
	}

	if (m_bEquipping || m_bUnEquipping)
		return;

	pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::IDLE_RIFLE, ANIM_TYPE::UPPER);
}

void CRifleState::Update(CStateMachine ** pState, CModel * pModel)
{
}

void CRifleState::Enter(CStateMachine ** pState, CModel * pModel)
{
	pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::EQUIP_RIFLE, ANIM_TYPE::UPPER);
	m_bEquipping = true;
	m_bUnEquipping = false;
}
