#pragma once
#include "ImGuiWindow.h"
#include "Gizmo.h"
BEGIN(Tool)
class CInspector : public CImGuiWindow
{
public:
	explicit CInspector();
	virtual ~CInspector() = default;
	virtual void Free() override;

public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void UpdateGameObject();
	void UpdateUI();

public:
	void DrawVec3(const string& label, _float3& values);
	void DrawRectDesc(const string& label, _float& x, _float& y);
	void DrawCollider();
	void DrawTransform();
	void DrawRectTransform();

private:
	//std::shared_ptr<CGizmo>		m_pGizmo;
	CGizmo*		m_pGizmo;

};
END