#pragma once

#include "Base.h"


BEGIN(Engine)

class CMeshContainer final : public CBase
{
public:
	CMeshContainer();
	virtual ~CMeshContainer() = default;
public:
	virtual HRESULT Initialize(const char* pName, _uint iNumFaces, _uint iStartFaceIndex, _uint iStartVertexIndex, _uint iMaterialIndex);
public:
	HRESULT Add_Bones(BONEDESC* pBoneDesc);

public:
	void Get_BoneMatrices(_matrix* pBoneMatrices);

	_uint Get_MeshMaterialIndex() const {
		return m_iMaterialIndex;
	}

	_uint Get_NumFaces() const {
		return m_iNumFaces;
	}

	_uint Get_StartFaceIndex() const {
		return m_iStartFaceIndex;
	}

	_uint Get_StartVertexIndex() const {
		return m_iStartVertexIndex;
	}

private:
	char		m_szName[MAX_PATH] = "";
	_uint		m_iNumFaces = 0;
	_uint		m_iStartFaceIndex = 0;
	_uint		m_iStartVertexIndex = 0;
	_uint		m_iMaterialIndex = 0;

	vector<BONEDESC*>		m_Bones;

public:
	static CMeshContainer* Create(const char* pName, _uint iNumFaces, _uint iStartFaceIndex, _uint iStartVertexIndex, _uint iMaterialIndex);
	virtual void Free() override;
};

END