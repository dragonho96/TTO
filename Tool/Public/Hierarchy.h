#pragma once

#include "ImGuiWindow.h"

class CHierarchy : public CImGuiWindow
{
public:
	CHierarchy(CToolManager* pToolManager);
	virtual ~CHierarchy() = default;
public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};

