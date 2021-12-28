#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CCrouchState : public CStateMachine
{
public:
	CCrouchState() {};
	virtual ~CCrouchState() = default;
public:
	virtual void HandleInput(CStateMachine** pState, CModel* pModel);
	virtual void Update(CStateMachine** pState, CModel* pModel);
	virtual void Enter(CStateMachine** pState, CModel* pModel);
public:
	void CheckAnim(CStateMachine** pState, CModel* pModel);
};

END