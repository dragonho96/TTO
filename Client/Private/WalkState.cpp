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
	ADDLOG("WALKING");

	if (CEngine::GetInstance()->IsKeyPressed('W'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_FR, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_FL, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_F, ANIM_TYPE::LOWER);
	}
	else if (CEngine::GetInstance()->IsKeyPressed('S'))
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_BR, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_BL, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_B, ANIM_TYPE::LOWER);
	}
	else
	{
		if (CEngine::GetInstance()->IsKeyPressed('D'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_R, ANIM_TYPE::LOWER);
		else if (CEngine::GetInstance()->IsKeyPressed('A'))
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::WALK_L, ANIM_TYPE::LOWER);
		else
			pModel->SetUp_AnimationIndex((_uint)ANIM_LOWER::IDLE, ANIM_TYPE::LOWER);
	}

	//if (!(CEngine::GetInstance()->IsKeyPressed('W') || CEngine::GetInstance()->IsKeyPressed('A') ||
	//	CEngine::GetInstance()->IsKeyPressed('S') || CEngine::GetInstance()->IsKeyPressed('D')))
	//{
	//	*pState = CStateMachine::idle;
	//	(*pState)->Enter(pState, pModel);
	//}
	//else
	//{
	if (CEngine::GetInstance()->IsKeyDown(VK_LSHIFT))
	{
		*pState = CStateMachine::run;
		(*pState)->Enter(pState, pModel);

		ADDLOG("TO RUN@@@@@@@@@@@@");
	}
	//}

	if (CEngine::GetInstance()->IsKeyDown('C'))
	{
		*pState = CStateMachine::crouch;
		(*pState)->Enter(pState, pModel);
	}
}
