#pragma once
#include "Base.h"
BEGIN(Engine)
class IScriptObject : public CBase
{
public:
	virtual ~IScriptObject() = default;
	virtual void Free() override;

public:
	virtual void Update(_double deltaTime) = 0;
	virtual void LapteUpdate(_double deltaTime) = 0;

	// Inherited via CBase
};

END