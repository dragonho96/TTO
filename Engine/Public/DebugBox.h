#pragma once

#include "Line.h"

class CDebugBox : public CLine
{
public:
	explicit CDebugBox(_float3 size);
	virtual ~CDebugBox();

public:
	void Render();

public:
	void CreateVertex(_float3* lines);

private:
	_float3	m_Size;
};

