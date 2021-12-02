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
	virtual HRESULT InitializePrototype(string pMeshFilePath, string pMeshFileName, string pShaderFilePath);
	virtual HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iPassIndex);

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	_uint				m_iNumVertices = 0;
	_uint				m_iNumFaces = 0;
	VTXMESH*			m_pVertices = nullptr;
	POLYGONINDICES32*	m_pPolygonIndices = nullptr;

	vector<class CMeshContainer*>		m_MeshContainers;
	vector<MODELTEXTURES*>				m_ModelTextures;

private:
	ID3D11Buffer*			m_pVB = nullptr;
	ID3D11Buffer*			m_pIB = nullptr;
	_uint					m_iStride = 0;

protected:
	vector<EFFECTDESC>			m_EffectDescs;
	ID3DX11Effect*				m_pEffect = nullptr;

	Scope<class CShader>			m_pShader;

private:
	HRESULT Create_MeshContainer(aiMesh* pMesh, _uint* pStartVertexIndex, _uint* pStartFaceIndex);
	HRESULT Create_VertexIndexBuffer(string pShaderFilePath);
	HRESULT Create_Materials(aiMaterial*	pMaterial, string pMeshFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string pMeshFilePath, string pMeshFileName, string pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END