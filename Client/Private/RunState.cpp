#include "stdafx.h"
#include "..\Public\RunState.h"
#include "IdleState.h"
#include "WalkState.h"
#include "CrouchState.h"

USING(Client)
void CRunState::HandleInput(CStateMachine ** pState, CModel * pModel)
{
	CheckAnim(pState, pModel);
}

void CRunState::Update(CStateMachine ** pState, CModel * pModel)
{
}

void CRunState::Enter(CStateMachine ** pState, CModel * pModel)
{
	CheckAnim(pState, pModel);
}

void CRunState::CheckAnim(CStateMachine ** pState, CModel * pModel)
{
	ADDLOG("RUNNING");

	if (CEngine::GetInstance()->IsKeyPressed('W'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_FR, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_FL, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_F, ANIM_TYPE::LOWER);
	}
	else if (CEngine::GetInstance()->IsKeyPressed('S'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_BR, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_BL, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_B, ANIM_TYPE::LOWER);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_R, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::RUN_L, ANIM_TYPE::LOWER);
		else
		{
			*pState = CStateMachine::walk;
			(*pState)->Enter(pState, pModel);
		}
	}

	if (CEngine::GetInstance()->IsKeyUp(VK_LSHIFT))
	{
		*pState = CStateMachine::walk;
		(*pState)->Enter(pState, pModel);
		ADDLOG("TO WALK@@@@@@@@@@@@@@");
	}

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		*pState = CStateMachine::crouch;
		(*pState)->Enter(pState, pModel);
	}
}
