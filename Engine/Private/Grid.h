#pragma once

#include "Base.h"
#include "Node.h"
BEGIN(Engine)
class CGrid : public CBase
{
public:
	explicit CGrid(_uint _sizeX, _uint _sizeZ, _uint _sizeInterval)
		: m_iSizeX(_sizeX), m_iSizeZ(_sizeZ), m_iSizeInterval(_sizeInterval) {}
	virtual ~CGrid() = default;

public:
	void CreateGrid();

private:
	_uint	m_iSizeX;
	_uint	m_iSizeZ;
	_uint	m_iSizeInterval;
	CNode*	m_Nodes;
public:
	virtual void Free() override;

};

END