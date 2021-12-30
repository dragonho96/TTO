#include "stdafx.h"
#include "..\Public\StateMachine.h"
#include "IdleState.h"
#include "WalkState.h"
#include "RunState.h"
#include "CrouchState.h"
#include "RifleState.h"
#include "GrenadeState.h"

USING(Client)

CIdleState* CStateMachine::idle = new CIdleState();
CWalkState* CStateMachine::walk = new CWalkState();
CRunState* CStateMachine::run = new CRunState();
CCrouchState* CStateMachine::crouch = new CCrouchState();
CRifleState* CStateMachine::rifle = new CRifleState();
CGrenadeState* CStateMachine::grenade = new CGrenadeState();

CStateMachine::~CStateMachine()
{
}
