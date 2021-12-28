#include "stdafx.h"
#include "..\Public\WalkState.h"
#include "IdleState.h"
#include "RunState.h"
#include "CrouchState.h"

USING(Client)

void CWalkState::HandleInput(CStateMachine** pState, CModel* pModel)
{
	SwitchAnim(pState, pModel);
}

void CWalkState::Update(CStateMachine** pState, CModel* pModel)
{
}

void CWalkState::Enter(CStateMachine** pState, CModel* pModel)
{
	SwitchAnim(pState, pModel);
}

void CWalkState::SwitchAnim(CStateMachine** pState, CModel* pModel)
{
	if (CEngine::GetInstance()->IsKeyPressed('W'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_FR);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_FL);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_F);
	}
	else if (CEngine::GetInstance()->IsKeyPressed('S'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_BR);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_BL);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_B);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_R);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM::WALK_L);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM::IDLE);
	}

	//if (!(CEngine::GetInstance()->IsKeyPressed('W') || CEngine::GetInstance()->IsKeyPressed('A') ||
	//	CEngine::GetInstance()->IsKeyPressed('S') || CEngine::GetInstance()->IsKeyPressed('D')))
	//{
	//	*pState = CStateMachine::idle;
	//	(*pState)->Enter(pState, pModel);
	//}
	//else
	//{
	if (CEngine::GetInstance()->IsKeyPressed(VK_LSHIFT))
	{
		*pState = CStateMachine::run;
		(*pState)->Enter(pState, pModel);
	}
	//}

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		*pState = CStateMachine::crouch;
		(*pState)->Enter(pState, pModel);
	}
}
