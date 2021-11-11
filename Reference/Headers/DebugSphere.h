#pragma once

#include "Line.h"

class CDebugSphere : public CLine
{
public:
	explicit CDebugSphere(float radius, int sliceCount = 36);
	virtual ~CDebugSphere();

public:
	void Render();

public:
	void CreateVertex();

private:
	float range;
	UINT sliceCount;
	UINT lineCount;

	_float3* lines;
};

