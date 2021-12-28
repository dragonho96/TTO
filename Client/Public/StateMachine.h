#pragma once
#include "Engine.h"
BEGIN(Client)

class CIdleState;
class CWalkState;
class CStateMachine
{
public:
	enum class ANIM { IDLE, WALK_F, WALK_FR, WALK_R, WALK_BR, WALK_B, WALK_BL, WALK_L, WALK_FL };

public:
	static CIdleState* idle;
	static CWalkState* walk;

public:
	virtual ~CStateMachine();

public:
	virtual void HandleInput(CStateMachine** pState, CModel* pModel) {};
	virtual void Update(CStateMachine** pState, CModel* pModel) {};
	virtual void Enter(CStateMachine** pState, CModel* pModel) {};
};

END