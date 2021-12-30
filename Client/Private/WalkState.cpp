#include "stdafx.h"
#include "..\Public\WalkState.h"
#include "IdleState.h"
#include "RunState.h"
#include "CrouchState.h"

USING(Client)

void CWalkState::HandleInput(CStateMachine** pState, CPlayer& pPlayer)
{
	SwitchAnim(pState, pPlayer);
}

void CWalkState::Update(CStateMachine** pState, CPlayer& pPlayer)
{
}

void CWalkState::Enter(CStateMachine** pState, CPlayer& pPlayer)
{
	SwitchAnim(pState, pPlayer);
}

void CWalkState::SwitchAnim(CStateMachine** pState, CPlayer& pPlayer)
{
	_float movingAngle = XMConvertToDegrees(pPlayer.m_angleBetweenCamPlayer);
	// XM_PI / 8.f;
	_vector velocity;
	if (XMVectorGetX(XMVector4Length(pPlayer.m_velocity)) == 0)
	{
		// Turn
		if (pPlayer.m_turn90.first)
		{
			pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::TURN_R, ANIM_TYPE::LOWER);
			m_bTurnning = true;
		}
		else if (pPlayer.m_turn90.second)
		{
			pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::TURN_L, ANIM_TYPE::LOWER);
			m_bTurnning = true;
		}
		else
		{
			if (m_bTurnning && pPlayer.m_pModel->IsLowerFinished())
				m_bTurnning = false;
			if (!m_bTurnning)
				pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::IDLE, ANIM_TYPE::LOWER);
		}

	}
	else if (movingAngle > -22.5f && movingAngle <= 22.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_F, ANIM_TYPE::LOWER);
	else if (movingAngle > 22.5f && movingAngle <= 67.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_FR, ANIM_TYPE::LOWER);
	else if (movingAngle > 67.5f && movingAngle <= 112.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_R, ANIM_TYPE::LOWER);
	else if (movingAngle > 112.5f && movingAngle <= 157.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_BR, ANIM_TYPE::LOWER);
	else if (movingAngle > 157.5f || movingAngle <= -157.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_B, ANIM_TYPE::LOWER);
	else if (movingAngle > -157.5f && movingAngle <= -112.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_BL, ANIM_TYPE::LOWER);
	else if (movingAngle > -112.5f && movingAngle <= -67.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_L, ANIM_TYPE::LOWER);
	else if (movingAngle > -67.5f && movingAngle <= -22.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_FL, ANIM_TYPE::LOWER);

	if (CEngine::GetInstance()->IsKeyDown(VK_LSHIFT))
	{
		*pState = CStateMachine::run;
		(*pState)->Enter(pState, pPlayer);
	}

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		*pState = CStateMachine::crouch;
		(*pState)->Enter(pState, pPlayer);
	}
}
