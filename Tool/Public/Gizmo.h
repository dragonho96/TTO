#pragma once
#include "ImGuiWindow.h"

BEGIN(Tool)
typedef struct tGizmoMatrix
{
	float matTranslation[3];
	float matRotation[3];
	float matScale[3];
}GIZMOMATRIX;

class CGizmo : public CImGuiWindow
{
public:
	explicit CGizmo();
	virtual ~CGizmo() = default;
	virtual void Free() override;

public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
public:
	void ManipulateGameObject();
	void ManipulateUI();

public:
	void SetObjMat(float* mat);

public:
	const GIZMOMATRIX GetMatrix() { return m_tGizmoMatrix; }
	void SetNewGizmoMatrix(const GIZMOMATRIX& tMat);


private:
	_bool useWindow;
	_uint gizmoCount;
	_float camDistance;
	GIZMOMATRIX m_tGizmoMatrix;
	GIZMOMATRIX m_tNewGizmoMatrix;

	float _view[16];
	float _viewUI[16];
	float _projection[16];
	float _objMat[16];	// Selected Obj Mat

	ImVec2 imageRect;
	ImVec2 winPos;
};

END