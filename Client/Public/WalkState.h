#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CWalkState : public CStateMachine
{
public:
	CWalkState() {}
	virtual ~CWalkState() = default;
public:
	virtual void HandleInput(CStateMachine** pState, CModel* pModel);
	virtual void Update(CStateMachine** pState, CModel* pModel);
	virtual void Enter(CStateMachine** pState, CModel* pModel);
private:
	void SwitchAnim(CStateMachine** pState, CModel* pModel);
};

END