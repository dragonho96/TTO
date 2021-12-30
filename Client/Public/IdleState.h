#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CIdleState : public CStateMachine
{
public:
	CIdleState() {}
	virtual ~CIdleState() = default;

	virtual void HandleInput(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Update(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Enter(CStateMachine** pState, CPlayer& pPlayer);
};

END