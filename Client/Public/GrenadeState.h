#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CGrenadeState : public CStateMachine
{
public:
	CGrenadeState() {}
	virtual ~CGrenadeState() = default;

	virtual void HandleInput(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Update(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Enter(CStateMachine** pState, CPlayer& pPlayer);

private:
	_bool	m_bEquipping = false;
	_bool	m_bUnEquipping = false;
	_bool	m_bThrowing = false;
};

END