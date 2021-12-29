#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CGrenadeState : public CStateMachine
{
public:
	CGrenadeState() {}
	virtual ~CGrenadeState() = default;

	virtual void HandleInput(CStateMachine** pState, CModel* pModel);
	virtual void Update(CStateMachine** pState, CModel* pModel);
	virtual void Enter(CStateMachine** pState, CModel* pModel);

private:
	_bool	m_bEquipping = false;
	_bool	m_bUnEquipping = false;
};

END