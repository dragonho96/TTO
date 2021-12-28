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
	if (CEngine::GetInstance()->IsKeyPressed('W'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_FR);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_FL);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_F);
	}
	else if (CEngine::GetInstance()->IsKeyPressed('S'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_BR);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_BL);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_B);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_R);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::RUN_L);
		else
		{
			*pState = CStateMachine::walk;
			(*pState)->Enter(pState, pModel);
		}
	}

	if (!CEngine::GetInstance()->IsKeyPressed(VK_LSHIFT))
	{
		*pState = CStateMachine::walk;
		(*pState)->Enter(pState, pModel);
	}

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		*pState = CStateMachine::crouch;
		(*pState)->Enter(pState, pModel);
	}
}
