#include "stdafx.h"
#include "..\Public\CrouchState.h"
#include "WalkState.h"
#include "RunState.h"

USING(Client)
void CCrouchState::HandleInput(CStateMachine ** pState, CModel * pModel)
{
	CheckAnim(pState, pModel);
}

void CCrouchState::Update(CStateMachine ** pState, CModel * pModel)
{
}

void CCrouchState::Enter(CStateMachine ** pState, CModel * pModel)
{
	CheckAnim(pState, pModel);
}

void CCrouchState::CheckAnim(CStateMachine ** pState, CModel * pModel)
{
	if (CEngine::GetInstance()->IsKeyPressed('W'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_FR, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_FL, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_F, ANIM_TYPE::LOWER);
	}
	else if (CEngine::GetInstance()->IsKeyPressed('S'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_BR, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_BL, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_B, ANIM_TYPE::LOWER);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_R, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_L, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::CROUCH_IDLE, ANIM_TYPE::LOWER);
	}

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		if (CEngine::GetInstance()->IsKeyPressed(VK_LSHIFT))
		{
			*pState = CStateMachine::run;
			(*pState)->Enter(pState, pModel);
		}
		else
		{
			*pState = CStateMachine::walk;
			(*pState)->Enter(pState, pModel);
		}
	}
}
