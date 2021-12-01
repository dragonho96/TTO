#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;
public:
	virtual HRESULT InitializePrototype(const char* pMeshFilePath, const char* pMeshFileName);
	virtual HRESULT Initialize(void* pArg);

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	_uint				m_iNumVertices = 0;
	_uint				m_iNumFaces = 0;
	VTXMESH*			m_pVertices = nullptr;
	POLYGONINDICES32*	m_pPolygonIndices = nullptr;

	vector<class CMeshContainer*>		m_MeshContainers;

private:
	HRESULT Create_MeshContainer(aiMesh* pMesh, _uint* pStartVertexIndex, _uint* pStartFaceIndex);



public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pMeshFilePath, const char* pMeshFileName);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END