#pragma once
#include "Engine.h"
BEGIN(Client)

class CIdleState;
class CWalkState;
class CRunState;
class CCrouchState;
class CStateMachine
{
public:
	enum class ANIM { IDLE, WALK_F, WALK_FR, WALK_R, WALK_BR, WALK_B, WALK_BL, WALK_L, WALK_FL,
	RUN_F, RUN_FR, RUN_R, RUN_BR, RUN_B, RUN_BL, RUN_L, RUN_FL, 
	CROUCH_F, CROUCH_FR, CROUCH_R, CROUCH_BR, CROUCH_B, CROUCH_BL, CROUCH_L, CROUCH_FL, CROUCH_IDLE,
	STAND_TO_KNEEL, KNEEL_TO_STAND, TURN_L, TURN_R, TURN_KNEEL_L, TURN_KNEEL_R };

public:
	static CIdleState* idle;
	static CWalkState* walk;
	static CRunState* run;
	static CCrouchState* crouch;

public:
	virtual ~CStateMachine();

public:
	virtual void HandleInput(CStateMachine** pState, CModel* pModel) {};
	virtual void Update(CStateMachine** pState, CModel* pModel) {};
	virtual void Enter(CStateMachine** pState, CModel* pModel) {};
};

END