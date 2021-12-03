#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Shader.h"
#include "Engine.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
{

}

HRESULT CModel::InitializePrototype(string pMeshFilePath, string pMeshFileName, string pShaderFilePath)
{
	m_pShader = make_unique<CShader>(pShaderFilePath);

	char		szFullPath[MAX_PATH] = "";

	//strcpy_s(szFullPath, pMeshFilePath);
	//strcat_s(szFullPath, pMeshFileName);

	string strFullPath = pMeshFilePath + pMeshFileName;

	m_pScene = m_Importer.ReadFile(strFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (nullptr == m_pScene)
		return E_FAIL;


	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		m_iNumVertices += m_pScene->mMeshes[i]->mNumVertices;
		m_iNumFaces += m_pScene->mMeshes[i]->mNumFaces;
	}

	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);
	m_pxVertices = new PxVec3[m_iNumVertices];
	ZeroMemory(m_pxVertices, sizeof(PxVec3) * m_iNumVertices);

	m_pPolygonIndices = new POLYGONINDICES32[m_iNumFaces];
	ZeroMemory(m_pPolygonIndices, sizeof(POLYGONINDICES32) * m_iNumFaces);
	m_pxIndices = new PxU32[m_iNumFaces * 3];
	ZeroMemory(m_pxIndices, sizeof(PxU32) * m_iNumFaces * 3);

	_uint		iStartVertexIndex = 0;
	_uint		iStartFaceIndex = 0;


	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		if (FAILED(Create_MeshContainer(m_pScene->mMeshes[i], &iStartVertexIndex, &iStartFaceIndex)))
			return E_FAIL;
	}

	m_iStride = sizeof(VTXMESH);

	if (FAILED(Create_VertexIndexBuffer(pShaderFilePath)))
		return E_FAIL;

	m_ModelTextures.reserve(m_pScene->mNumMaterials);

	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		if (FAILED(Create_Materials(m_pScene->mMaterials[i], pMeshFilePath)))
			return E_FAIL;
	}

	CreatePxMesh();

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	_uint		iOffSet = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &m_iStride, &iOffSet);
	m_pDeviceContext->IASetIndexBuffer(m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pShader->Render(iPassIndex);
	//m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex].pLayout);
	//if (FAILED(m_EffectDescs[iPassIndex].pPass->Apply(0, m_pDeviceContext)))
	//	return E_FAIL;

	for (auto& pMeshContainer : m_MeshContainers)
	{
	}

	return S_OK;
}

HRESULT CModel::Create_MeshContainer(aiMesh * pMesh, _uint* pStartVertexIndex, _uint* pStartFaceIndex)
{
	_uint		iStartVertexIndex = *pStartVertexIndex;

	for (_uint i = 0; i < pMesh->mNumVertices; ++i)
	{
		memcpy(&m_pVertices[*pStartVertexIndex].vPosition, &pMesh->mVertices[i], sizeof(_float3));

		memcpy(&m_pVertices[*pStartVertexIndex].vNormal, &pMesh->mNormals[i], sizeof(_float3));

		memcpy(&m_pVertices[*pStartVertexIndex].vTexUV, &pMesh->mTextureCoords[0][i], sizeof(_float2));

		memcpy(&m_pVertices[*pStartVertexIndex].vTangent, &pMesh->mTangents[i], sizeof(_float3));

		// PhysX
		memcpy(&m_pxVertices[*pStartVertexIndex], &pMesh->mVertices[i], sizeof(PxVec3));

		++(*pStartVertexIndex);
	}

	_uint		iStartFaceIndex = *pStartFaceIndex;

	for (_uint i = 0; i < pMesh->mNumFaces; ++i)
	{
		m_pPolygonIndices[*pStartFaceIndex]._0 = pMesh->mFaces[i].mIndices[0];
		m_pPolygonIndices[*pStartFaceIndex]._1 = pMesh->mFaces[i].mIndices[1];
		m_pPolygonIndices[*pStartFaceIndex]._2 = pMesh->mFaces[i].mIndices[2];

		m_pxIndices[(*pStartFaceIndex * 3)] = pMesh->mFaces[i].mIndices[0];
		m_pxIndices[(*pStartFaceIndex * 3) + 1] = pMesh->mFaces[i].mIndices[1];
		m_pxIndices[(*pStartFaceIndex * 3) + 2] = pMesh->mFaces[i].mIndices[2];
		++(*pStartFaceIndex);
	}


	CMeshContainer*			pMeshContainer = CMeshContainer::Create(pMesh->mName.data, pMesh->mNumFaces, iStartFaceIndex, iStartVertexIndex);
	if (nullptr == pMeshContainer)
		return E_FAIL;

	m_MeshContainers.push_back(pMeshContainer);

	return S_OK;
}

HRESULT CModel::Create_VertexIndexBuffer(string pShaderFilePath)
{
	/* For.VertexBuffer */
	D3D11_BUFFER_DESC			BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));

	BufferDesc.ByteWidth = sizeof(VTXMESH) * m_iNumVertices;
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = sizeof(VTXMESH);

	D3D11_SUBRESOURCE_DATA		SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	SubResourceData.pSysMem = m_pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pVB)))
		return E_FAIL;


	/* For.IndexBuffer */
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));

	BufferDesc.ByteWidth = sizeof(POLYGONINDICES32) * m_iNumFaces;
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResourceData.pSysMem = m_pPolygonIndices;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pIB)))
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Create_Materials(aiMaterial* pMaterial, string pMeshFilePath)
{
	MODELTEXTURES*		pModelTexture = new MODELTEXTURES;
	ZeroMemory(pModelTexture, sizeof(MODELTEXTURES));

	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		aiString		strTexturePath;

		if (FAILED(pMaterial->GetTexture(aiTextureType(i), 0, &strTexturePath)))
			continue;

		char	szTextureFileName[MAX_PATH] = "";
		char	szExt[MAX_PATH] = "";

		_splitpath(strTexturePath.data, nullptr, nullptr, szTextureFileName, szExt);
		strcat_s(szTextureFileName, szExt);

		//char		szFullPath[MAX_PATH] = "";
		//strcpy_s(szFullPath, pMeshFilePath);
		//strcat_s(szFullPath, szTextureFileName);

		//_tchar		szWideFullPath[MAX_PATH] = TEXT("");

		// Maybe?
		string strFullPath = pMeshFilePath + szTextureFileName;
		//MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);

		if (!strcmp(szExt, ".dds"))
			pModelTexture->pModelTexture[i] = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_DDS, strFullPath);
		else if (!strcmp(szExt, ".tga"))
			pModelTexture->pModelTexture[i] = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, strFullPath);
		else
			pModelTexture->pModelTexture[i] = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_WIC, strFullPath);
	}


	m_ModelTextures.push_back(pModelTexture);

	return S_OK;
}



void CModel::CreatePxMesh()
{
	CEngine* pEngine = CEngine::GetInstance();

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = m_iNumVertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = m_pxVertices;

	meshDesc.triangles.count = m_iNumFaces;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = m_pxIndices;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = pEngine->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
		return;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	m_pPxMesh = pEngine->GetPhysics()->createTriangleMesh(readBuffer);

	PxTransform transform;
	transform.q = PxIdentity;
	transform.p = { 0, 0, 0 };
	PxRigidStatic* m_pRigidActor = pEngine->GetPhysics()->createRigidStatic(transform);
	PxMaterial* mat = CEngine::GetInstance()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	PxShape* shape = pEngine->GetPhysics()->createShape(PxTriangleMeshGeometry(m_pPxMesh), *mat);
	m_pRigidActor->attachShape(*shape);
	pEngine->AddActor(m_pRigidActor);
	return;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, string pMeshFilePath, string pMeshFileName, string pShaderFilePath)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(pMeshFilePath, pMeshFileName, pShaderFilePath)))
	{
		MSG_BOX("Failed To Creating CModel");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CModel");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	for (auto& container : m_MeshContainers)
		SafeRelease(container);

	for (auto& texture : m_ModelTextures)
		for (auto& tex : texture->pModelTexture)
			SafeRelease(tex);

	SafeDeleteArray(m_pVertices);
	SafeDeleteArray(m_pPolygonIndices);
	SafeDeleteArray(m_pxVertices);
	SafeDeleteArray(m_pxIndices);

	__super::Free();
}