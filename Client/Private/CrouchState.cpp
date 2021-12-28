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
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_FR);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_FL);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_F);
	}
	else if (CEngine::GetInstance()->IsKeyPressed('S'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_BR);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_BL);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_B);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_R);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_L);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::CROUCH_IDLE);
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
