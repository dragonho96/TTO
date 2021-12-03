#pragma once

#include "Base.h"


BEGIN(Engine)

class CMeshContainer final : public CBase
{
public:
	CMeshContainer();
	virtual ~CMeshContainer() = default;
public:
	virtual HRESULT Initialize(const char* pName, _uint iNumFaces, _uint iStartFaceIndex, _uint iStartVertexIndex);
private:
	char		m_szName[MAX_PATH] = "";
	_uint		m_iNumFaces = 0;
	_uint		m_iStartFaceIndex = 0;
	_uint		m_iStartVertexIndex = 0;
public:
	static CMeshContainer* Create(const char* pName, _uint iNumFaces, _uint iStartFaceIndex, _uint iStartVertexIndex);
	virtual void Free() override;
};

END