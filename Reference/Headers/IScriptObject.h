#pragma once
#include "Base.h"
BEGIN(Engine)
class IScriptObject : public CBase
{
public:
	virtual ~IScriptObject() = default;

public:
	virtual void Update(_double deltaTime) = 0;
	virtual void LapteUpdate(_double deltaTime) = 0;

public:
	void SetDead() { m_bDead = false; };
	_bool	IsDead() { return m_bDead; }
protected:
	_bool		m_bDead = false;

	// Inherited via CBase
};

END