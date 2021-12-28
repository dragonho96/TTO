#include "stdafx.h"
#include "..\Public\IdleState.h"
#include "WalkState.h"

USING(Client)
void CIdleState::HandleInput(CStateMachine** pState, CModel* pModel)
{
	if (CEngine::GetInstance()->IsKeyPressed('W') || CEngine::GetInstance()->IsKeyPressed('A') ||
		CEngine::GetInstance()->IsKeyPressed('S') || CEngine::GetInstance()->IsKeyPressed('D'))
	{
		*pState = CStateMachine::walk;
		(*pState)->Enter(pState, pModel);
		// CStateMachine::walk->Enter(pState, pModel);
	}
}

void CIdleState::Update(CStateMachine** pState, CModel* pModel)
{
}

void CIdleState::Enter(CStateMachine** pState, CModel* pModel)
{
	pModel->SetUp_AnimationIndex((_uint)ANIM::IDLE);
}
