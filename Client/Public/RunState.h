#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CRunState : public CStateMachine
{
public:
	CRunState() {};
	virtual ~CRunState() = default;
public:
	virtual void HandleInput(CStateMachine** pState, CModel* pModel);
	virtual void Update(CStateMachine** pState, CModel* pModel);
	virtual void Enter(CStateMachine** pState, CModel* pModel);
public:
	void CheckAnim(CStateMachine** pState, CModel* pModel);
};

END