#include "stdafx.h"
#include "..\Public\RunState.h"
#include "IdleState.h"
#include "WalkState.h"
#include "CrouchState.h"

USING(Client)
void CRunState::HandleInput(CStateMachine ** pState, CPlayer& pPlayer)
{
	CheckAnim(pState, pPlayer);
}

void CRunState::Update(CStateMachine ** pState, CPlayer& pPlayer)
{
}

void CRunState::Enter(CStateMachine ** pState, CPlayer& pPlayer)
{
	pPlayer.m_fSpeedFactor = 30.f;
	CheckAnim(pState, pPlayer);
}

void CRunState::CheckAnim(CStateMachine ** pState, CPlayer& pPlayer)
{
	_float movingAngle = XMConvertToDegrees(pPlayer.m_angleBetweenCamPlayer);
	// XM_PI / 8.f;
	_vector velocity;
	if (XMVectorGetX(XMVector4Length(pPlayer.m_velocity)) == 0 || CEngine::GetInstance()->IsKeyUp(VK_LSHIFT))
	{
		*pState = CStateMachine::walk;
		(*pState)->Enter(pState, pPlayer);
	}
	else if (movingAngle > -22.5f && movingAngle <= 22.5f)
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_F, ANIM_TYPE::LOWER);
	else if (movingAngle > 22.5f && movingAngle <= 67.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_FR, ANIM_TYPE::LOWER);
	else if (movingAngle > 67.5f && movingAngle <= 112.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_R, ANIM_TYPE::LOWER);
	else if (movingAngle > 112.5f && movingAngle <= 157.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_BR, ANIM_TYPE::LOWER);
	else if (movingAngle > 157.5f || movingAngle <= -157.5f)	  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_B, ANIM_TYPE::LOWER);
	else if (movingAngle > -157.5f && movingAngle <= -112.5f)	  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_BL, ANIM_TYPE::LOWER);
	else if (movingAngle > -112.5f && movingAngle <= -67.5f)	  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_L, ANIM_TYPE::LOWER);
	else if (movingAngle > -67.5f && movingAngle <= -22.5f)		  
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_FL, ANIM_TYPE::LOWER);

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		*pState = CStateMachine::crouch;
		(*pState)->Enter(pState, pPlayer);
	}
}
