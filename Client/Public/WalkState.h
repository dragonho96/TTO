#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CWalkState : public CStateMachine
{
public:
	CWalkState() {}
	virtual ~CWalkState() = default;
public:
	virtual void HandleInput(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Update(CStateMachine** pState, CPlayer& pPlayer);
	virtual void Enter(CStateMachine** pState, CPlayer& pPlayer);
private:
	void SwitchAnim(CStateMachine** pState, CPlayer& pPlayer);
private:
	_bool		m_bTurnning = false;

};

END