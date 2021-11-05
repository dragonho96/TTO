#pragma once
#include "ToolManager.h"
USING(Engine)
class CImGuiWindow abstract
{
public:
	explicit CImGuiWindow(CToolManager* pToolManager);
	virtual ~CImGuiWindow() = default;

public:
	virtual void Initialize() PURE;
	virtual void Update() PURE;
	virtual void LateUpdate() PURE;

protected:
	CEngine*		m_pEngine;
	CToolManager*	m_pToolManager;
};
