#pragma once
#include "StateMachine.h"

BEGIN(Client)
class CCrouchState : public CStateMachine
{
public:
	CCrouchState() {};
	virtual ~CCrouchState() = default;
public:
	virtual void HandleInput(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Update(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Enter(CStateMachine** pState, CPlayer& pPlayer);
public:
	void CheckAnim(CStateMachine** pState, CPlayer& pPlayer);
private:
	_bool	m_bTurnning = false;
};

END