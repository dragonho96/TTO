#pragma once

#include "Line.h"

class CDebugCapsule	: public CLine
{
public:
	enum class AXIS { AXIS_X, AXIS_Y, AXIS_Z };

public:
	CDebugCapsule(float radius, float height, AXIS axis = AXIS::AXIS_Y, int stackCount = 20, int sliceCount = 20);
	~CDebugCapsule();

public:
	void InitializePxController();
	void Update();

public:
	void SetPosition(_float3 pos);
	void UpdateWorldMatrix();

public:
	void CreateVertexFilled();
	void CreateVertex();
	void Render();

private:
	float m_fRadius;
	float m_fHeight;
	int m_iStackCount;
	int m_iSliceCount;

	AXIS m_eAxis;

	UINT m_iLineCount;
	_float3* m_pLines;

	PxController* controller;
	_bool m_bInit = false;
};

