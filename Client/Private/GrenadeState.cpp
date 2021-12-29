#include "stdafx.h"
#include "..\Public\GrenadeState.h"
#include "RifleState.h"

USING(Client)
void CGrenadeState::HandleInput(CStateMachine ** pState, CModel * pModel)
{
	if (m_bEquipping && pModel->IsUpperFinished())
		m_bEquipping = false;

	if (!m_bUnEquipping && !m_bEquipping && CEngine::GetInstance()->IsKeyDown('1'))
	{
		pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::UNEQUIP_GRENADE, ANIM_TYPE::UPPER);
		m_bUnEquipping = true;
	}

	if (m_bUnEquipping && pModel->IsUpperFinished())
	{
		m_bUnEquipping = false;
		*pState = CStateMachine::rifle;
		(*pState)->Enter(pState, pModel);
		return;
	}

	if (m_bEquipping || m_bUnEquipping)
		return;

	if (CEngine::GetInstance()->IsMousePressed(1))
	{
		if (CEngine::GetInstance()->IsMouseDown(0))
		{
			// Wait till animation is over
			pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::THROW_GRENADE, ANIM_TYPE::UPPER);
		}
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::AIM_GRENADE, ANIM_TYPE::UPPER);
	}
	else
		pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::IDLE_GRENADE, ANIM_TYPE::UPPER);

}

void CGrenadeState::Update(CStateMachine ** pState, CModel * pModel)
{
}

void CGrenadeState::Enter(CStateMachine ** pState, CModel * pModel)
{
	pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::EQUIP_GRENADE, ANIM_TYPE::UPPER);
	m_bEquipping = true;
	m_bUnEquipping = false;
}
