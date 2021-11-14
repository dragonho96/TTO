#pragma once

#include "ImGuiWindow.h"
BEGIN(Tool)
class CHierarchy : public CImGuiWindow
{
public:
	explicit CHierarchy();
	virtual ~CHierarchy() = default;
	virtual void Free() override;

public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};

END