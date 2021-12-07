#pragma once

#include "Component.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg); 
	HRESULT Render(_uint iMaterialIndex, _uint iPassIndex);

public:
	HRESULT Bind_Buffers();
	HRESULT SetUp_TextureOnShader(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType);
private:
	HRESULT Create_MeshContainer(aiMesh* pMesh, _uint* pStartVertexIndex, _uint* pStartFaceIndex);
	HRESULT Create_VertexIndexBuffer(string pShaderFilePath);
	HRESULT Create_Materials(aiMaterial*	pMaterial, string pMeshFilePath);
	HRESULT Create_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT Sort_MeshesByMaterial();
	HRESULT SetUp_SkinnedInfo();
	CHierarchyNode* Find_HierarchyNode(const char* pBoneName);


public:
	HRESULT CreateBuffer(string pMeshFilePath, string pMeshFileName, string pShaderFilePath = "../../Assets/Shader/Shader_Mesh.fx");
	void RemoveBuffer();
private:
	void CreatePxMesh();

public:
	_uint Get_NumMaterials() const {
		return (_uint)m_ModelTextures.size();
	}
	string GetMeshFilePath() { return m_pMeshFilePath; }
	string GetMeshFileName() { return m_pMeshFileName; }
	string GetShaderFilePath() { return m_pShaderFilePath; }


private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;


private:
	_uint				m_iNumVertices = 0;
	_uint				m_iNumFaces = 0;
	VTXMESH*			m_pVertices = nullptr;
	POLYGONINDICES32*	m_pPolygonIndices = nullptr;

	vector<class CMeshContainer*>			m_MeshContainers;

	vector<vector<class CMeshContainer*>>	m_SortByMaterialMesh;
	vector<MODELTEXTURES*>					m_ModelTextures;
	vector<class CHierarchyNode*>			m_HierarchyNodes;

private:
	ComRef<ID3D11Buffer>			m_pVB = nullptr;
	ComRef<ID3D11Buffer>			m_pIB = nullptr;
	_uint					m_iStride = 0;

protected:
	vector<EFFECTDESC>			m_EffectDescs;
	ID3DX11Effect*				m_pEffect = nullptr;

	Scope<class CShader>			m_pShader = nullptr;
	class CTransform*				m_pTransform = nullptr;

protected:
	string m_pMeshFilePath = "";
	string m_pMeshFileName = "";
	string m_pShaderFilePath = "";

private:
	PxVec3*				m_pxVertices = nullptr;
	PxU32*				m_pxIndices = nullptr;
	PxTriangleMesh*		m_pPxMesh = nullptr;


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END