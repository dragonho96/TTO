#include "stdafx.h"
#include "..\Public\StateMachine.h"
#include "IdleState.h"
#include "WalkState.h"

USING(Client)

CIdleState* CStateMachine::idle = new CIdleState();
CWalkState* CStateMachine::walk = new CWalkState();

CStateMachine::~CStateMachine()
{
}
