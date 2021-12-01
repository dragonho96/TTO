#pragma once

#include "Base.h"


BEGIN(Engine)

class CMeshContainer final : public CBase
{
public:
	CMeshContainer();
	virtual ~CMeshContainer();
public:
	_uint			m_iStartVertexIndex;
	_uint			m_iNumVertices;
public:
	static CMeshContainer* Create();
	virtual void Free() override;
};

END