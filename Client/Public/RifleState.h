#pragma once
#include "StateMachine.h"
BEGIN(Client)
class CRifleState : public CStateMachine 
{
public:
	CRifleState() {};
	virtual ~CRifleState() = default;

	virtual void HandleInput(CStateMachine** pState, CModel* pModel);
	virtual void Update(CStateMachine** pState, CModel* pModel);
	virtual void Enter(CStateMachine** pState, CModel* pModel);

private:
	_bool		m_bUnEquipping = false;
	_bool		m_bEquipping = false;
};

END