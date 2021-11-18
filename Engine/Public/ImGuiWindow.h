#pragma once
#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CImGuiWindow abstract : public CBase
{
public:
	explicit CImGuiWindow();
	virtual ~CImGuiWindow() = default;

public:
	virtual void Initialize() PURE;
	virtual void Update() PURE;
	virtual void LateUpdate() PURE;
};
END