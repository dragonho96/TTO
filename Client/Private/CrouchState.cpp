#include "stdafx.h"
#include "..\Public\CrouchState.h"
#include "WalkState.h"
#include "RunState.h"

USING(Client)
void CCrouchState::HandleInput(CStateMachine ** pState, CPlayer& pPlayer)
{
	CheckAnim(pState, pPlayer);
}

void CCrouchState::Update(CStateMachine ** pState, CPlayer& pPlayer)
{
}

void CCrouchState::Enter(CStateMachine ** pState, CPlayer& pPlayer)
{
	m_bTurnning = false;
	pPlayer.m_fSpeedFactor = 100.f;
	CheckAnim(pState, pPlayer);
}

void CCrouchState::CheckAnim(CStateMachine ** pState, CPlayer& pPlayer)
{
	_float movingAngle = XMConvertToDegrees(pPlayer.m_angleBetweenCamPlayer);
	// XM_PI / 8.f;
	if (XMVectorGetX(XMVector4Length(pPlayer.m_velocity)) == 0 || CEngine::GetInstance()->IsKeyUp(VK_LSHIFT))
	{
		if (pPlayer.m_turn90.first)
		{
			pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::TURN_KNEEL_R, ANIM_TYPE::LOWER);
			m_bTurnning = true;
		}
		else if (pPlayer.m_turn90.second)
		{
			pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::TURN_KNEEL_L, ANIM_TYPE::LOWER);
			m_bTurnning = true;
		}
		else
		{
			if (m_bTurnning && pPlayer.m_pModel->IsLowerFinished())
				m_bTurnning = false;
			if (!m_bTurnning)
				pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_IDLE, ANIM_TYPE::LOWER);
		}
	}
	else if (movingAngle > -22.5f && movingAngle <= 22.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_F, ANIM_TYPE::LOWER);
	else if (movingAngle > 22.5f && movingAngle <= 67.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_FR, ANIM_TYPE::LOWER);
	else if (movingAngle > 67.5f && movingAngle <= 112.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_R, ANIM_TYPE::LOWER);
	else if (movingAngle > 112.5f && movingAngle <= 157.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_BR, ANIM_TYPE::LOWER);
	else if (movingAngle > 157.5f || movingAngle <= -157.5f)	  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_B, ANIM_TYPE::LOWER);
	else if (movingAngle > -157.5f && movingAngle <= -112.5f)	  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_BL, ANIM_TYPE::LOWER);
	else if (movingAngle > -112.5f && movingAngle <= -67.5f)	  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_L, ANIM_TYPE::LOWER);
	else if (movingAngle > -67.5f && movingAngle <= -22.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_FL, ANIM_TYPE::LOWER);

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		if (CEngine::GetInstance()->IsKeyPressed(VK_LSHIFT))
		{
			*pState = CStateMachine::run;
			(*pState)->Enter(pState, pPlayer);
		}
		else
		{
			*pState = CStateMachine::walk;
			(*pState)->Enter(pState, pPlayer);
		}
	}
}
