#pragma once
#include "ImGuiWindow.h"
#include "Gizmo.h"

class CInspector : public CImGuiWindow
{
public:
	CInspector(CToolManager* pToolManager);
	virtual ~CInspector() = default;

public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void DrawVec3(const string& label, _float3& values);

private:
	//std::shared_ptr<CGizmo>		m_pGizmo;
	CGizmo*		m_pGizmo;
};