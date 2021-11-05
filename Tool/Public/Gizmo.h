#pragma once
#include "ImGuiWindow.h"

typedef struct tGizmoMatrix
{
	float matTranslation[3];
	float matRotation[3];
	float matScale[3];
}GIZMOMATRIX;

class CGizmo : public CImGuiWindow
{
public:
	CGizmo(CToolManager* pToolManager);
	virtual ~CGizmo() = default;

public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

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
	float _projection[16];
	float _objMat[16];	// Selected Obj Mat

};

