#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CRunState : public CStateMachine
{
public:
	CRunState() {};
	virtual ~CRunState() = default;
public:
	virtual void HandleInput(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Update(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Enter(CStateMachine** pState, CPlayer& pPlayer);
public:
	void CheckAnim(CStateMachine** pState, CPlayer& pPlayer);
};

END