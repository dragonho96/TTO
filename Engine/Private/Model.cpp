#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Shader.h"
#include "Engine.h"
#include "HierarchyNode.h"

#include "Animation.h"
#include "Channel.h"
#include "PxManager.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_pMeshFilePath(rhs.m_pMeshFilePath)
	, m_pMeshFileName(rhs.m_pMeshFileName)
	, m_bMeshCollider(rhs.m_bMeshCollider)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumFaces(rhs.m_iNumFaces)
	, m_pVertices(rhs.m_pVertices)
	, m_pPolygonIndices(rhs.m_pPolygonIndices)
	, m_pxVertices(rhs.m_pxVertices)
	, m_pxIndices(rhs.m_pxIndices)
	, m_ModelTextures(rhs.m_ModelTextures)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iStride(rhs.m_iStride)
	, m_iAnimationIndex(rhs.m_iAnimationIndex)
{
	for (auto& pPrototypeMeshContainer : rhs.m_MeshContainers)
	{
		CMeshContainer*	pMeshContainer = pPrototypeMeshContainer->Clone();
		m_MeshContainers.push_back(pMeshContainer);
	}
}

HRESULT CModel::InitializePrototype()
{
	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	if (!m_pScene || m_pMeshFileName.length() <= 0)
		return S_OK;

	if (pArg)
		m_pTransform = (CTransform*)pArg;

	m_bSimulateRagdoll = false;

	m_pShader = make_unique<CShader>("../../Assets/Shader/Shader_Mesh.fx");

	_uint		iStartVertexIndex = 0;
	_uint		iStartFaceIndex = 0;

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT && m_bMeshCollider)
		CreatePxMesh();

	Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0, ANIM_TYPE::NONE, XMMatrixIdentity());

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	for (auto& pMeshContainer : m_MeshContainers)
	{
		vector<BONEDESC*>	Bones = pMeshContainer->Get_BoneDesc();

		for (auto& pBoneDesc : Bones)
		{
			pBoneDesc->pHierarchyNode = Find_HierarchyNode(pBoneDesc->pName);
			SetRagdollBoneDesc(pBoneDesc);
		}
	}

	if (FAILED(Sort_MeshesByMaterial()))
		return E_FAIL;

	if (FAILED(SetUp_AnimationInfo()))
		return E_FAIL;

	Update_CombinedTransformationMatrices(0.0);

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT &&
		0 < m_Animations.size())
		CreateRagdollRbs();

	// handGunNode = Find_HierarchyNode("ik_hand_gun");
	// handGunNode = Find_HierarchyNode("hand_r");

	return S_OK;
}

_fmatrix CModel::Get_BoneMatrix(const char * pBoneName)
{
	CHierarchyNode*		pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return XMMatrixIdentity();

	return pNode->Get_OffsetMatrix() * pNode->Get_CombinedTransformationMatrix();
}

HRESULT CModel::CreateBuffer(string pMeshFilePath, string pMeshFileName, string pShaderFilePath)
{
	if (pMeshFileName.length() == 0 || pMeshFileName.length() == 0)
		return S_OK;

	RemoveBuffer();

	// TODO: Shader Copy?
	m_pShader = make_unique<CShader>(pShaderFilePath);

	m_pMeshFilePath = pMeshFilePath;
	m_pMeshFileName = pMeshFileName;
	m_pShaderFilePath = pShaderFilePath;

	string strFullPath = pMeshFilePath + pMeshFileName;

	m_pScene = m_Importer.ReadFile(strFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (nullptr == m_pScene)
		return E_FAIL;

	//FILESYSTEM::path pFullPath = FILESYSTEM::directory_entry(strFullPath).path();
	//string fileName = pFullPath.stem().string();
	//string fileExtension = pFullPath.extension().string();
	////
	//std::string path = "C:\\Users\\drago\\Documents\\GitHub\\TTO\\Assets\\Meshes\\Binary\\";
	//string strFinalFullPath = path + fileName + ".glb";


	//ifstream fin(strFinalFullPath, (ios::in | ios::ate | ios::binary));
	//fin.seekg(0, ios::end);   // 끝위치 이동
	//long fileSize = fin.tellg();  // 파일사이즈 구하긔
	//fin.seekg(0, ios::beg);   // 다시 시작으로 갖다놓긔

	//unsigned char* buffer = new unsigned char[fileSize];

	//fin.read((char*)buffer, fileSize);
	//fin.close();

	//m_pScene = m_Importer.ReadFileFromMemory(buffer, fileSize/*, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace*/);
	//delete[] buffer;


	//if (nullptr == m_pScene)
	//{
	//}


	//m_pScene = m_Importer.ReadFile(strFinalFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	//if (nullptr == m_pScene)
	//{
	//	m_pScene = m_Importer.ReadFile(strFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	//	Assimp::Exporter Exporter;
	//	auto descr = Exporter.GetExportFormatDescription(10);
	//	string id = descr->id;
	//	if (aiReturn::aiReturn_FAILURE == Exporter.Export(m_pScene, descr->id, strFinalFullPath))
	//		return E_FAIL;

	//	m_pScene = m_Importer.ReadFile(strFinalFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	//}

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

	m_iStride = sizeof(VTXMESH);

	_uint		iStartVertexIndex = 0;
	_uint		iStartFaceIndex = 0;

	m_ModelTextures.reserve(m_pScene->mNumMaterials);

	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		if (FAILED(Create_Materials(m_pScene->mMaterials[i], pMeshFilePath)))
			return E_FAIL;
	}

	// ADDED
	m_MeshContainers.reserve(m_pScene->mNumMeshes);

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		if (FAILED(Create_MeshContainer(m_pScene->mMeshes[i], &iStartVertexIndex, &iStartFaceIndex, XMMatrixIdentity())))
			return E_FAIL;
	}

	if (false == m_pScene->HasAnimations())
	{
		if (FAILED(Create_VertexIndexBuffer(pShaderFilePath)))
			return E_FAIL;

		return S_OK;
	}

	//int numAnim = m_pScene->mNumAnimations;
	//_matrix		ScaleMatrix, RotationMatrix, TranslationMatrix;
	//_matrix		ModelPivotMatrix;
	//ScaleMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//// RotationMatrix = XMMatrixRotationX(XMConvertToRadians(90.0f));
	//ModelPivotMatrix = ScaleMatrix;
	// Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0, ModelPivotMatrix);

	Create_HierarchyNodes(m_pScene->mRootNode, nullptr, 0, ANIM_TYPE::NONE ,XMMatrixIdentity());

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest) {
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(SetUp_SkinnedInfo()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffer(pShaderFilePath)))
		return E_FAIL;

	if (FAILED(SetUp_AnimationInfo()))
		return E_FAIL;

	// HRESULT result = CreateRagdollRbs();

	//if (FAILED(CreateRagdollRbs()))
	//	return E_FAIL;

	//if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT && 0 < m_Animations.size())
	//	CreateRagdollRbs();

	// return result;
	return S_OK;
}

HRESULT CModel::Bind_Buffers()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->GetMatrix())), sizeof(_matrix));
	m_pShader->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW)), sizeof(_matrix));
	m_pShader->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_PROJ)), sizeof(_matrix));

	_uint		iOffSet = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffSet);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

HRESULT CModel::SetUp_TextureOnShader(const char * pConstantName, _uint iMaterialIndex, aiTextureType eTextureType)
{
	m_pShader->SetUp_TextureOnShader(pConstantName, m_ModelTextures, iMaterialIndex, eTextureType);
	return S_OK;
}

HRESULT CModel::Render(_uint iMaterialIndex, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	if (nullptr == m_pShader)
		return E_FAIL;

	//if (0 < m_Animations.size())
	//	iPassIndex = 1;
	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
	{
		if (m_bSimulateRagdoll)
			iPassIndex = 2;
		else if (0 < m_Animations.size())
			iPassIndex = 1;
		else
			iPassIndex = 0;
	}
	else
		iPassIndex = 0;
	// m_pShader->Render(iPassIndex);

	m_pShader->SetInputLayout(iPassIndex);

	_matrix			BoneMatrices[512];

	for (auto& pMeshContainer : m_SortByMaterialMesh[iMaterialIndex])
	{
		if (!pMeshContainer->IsActive())
			continue;

		if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
		{
			ZeroMemory(BoneMatrices, sizeof(_matrix) * 512);

			pMeshContainer->Get_BoneMatrices(BoneMatrices);
			// pMeshContainer->Get_BoneMatrices_Ragdoll(BoneMatrices);

			m_pShader->SetUp_ValueOnShader("g_BoneMatrices", BoneMatrices, sizeof(_matrix) * 512);
		}

		m_pShader->Apply(iPassIndex);

		m_pDeviceContext->DrawIndexed(pMeshContainer->Get_NumFaces() * 3,
			pMeshContainer->Get_StartFaceIndex() * 3,
			pMeshContainer->Get_StartVertexIndex());
	}

	return S_OK;
}


HRESULT CModel::Create_MeshContainer(aiMesh * pMesh, _uint* pStartVertexIndex, _uint* pStartFaceIndex, _fmatrix PivotMatrix)
{
	_uint		iStartVertexIndex = *pStartVertexIndex;

	for (_uint i = 0; i < pMesh->mNumVertices; ++i)
	{
		memcpy(&m_pVertices[*pStartVertexIndex].vPosition, &pMesh->mVertices[i], sizeof(_float3));
		// ADDED
		_vector		vPosition;
		vPosition = XMLoadFloat3(&m_pVertices[*pStartVertexIndex].vPosition);
		vPosition = XMVector3TransformCoord(vPosition, PivotMatrix);
		XMStoreFloat3(&m_pVertices[*pStartVertexIndex].vPosition, vPosition);

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


	CMeshContainer*			pMeshContainer = CMeshContainer::Create(
		pMesh->mName.data,
		pMesh->mNumFaces,
		iStartFaceIndex,
		iStartVertexIndex,
		pMesh->mMaterialIndex
	);

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
		aiString		strMatName = pMaterial->GetName();
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

		if (!strcmp(szExt, ".dds") || !strcmp(szExt, ".DDS"))
			pModelTexture->pModelTexture[i] = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_DDS, strFullPath);
		else if (!strcmp(szExt, ".tga") || !strcmp(szExt, ".TGA") || !strcmp(szExt, ".Tga"))
			pModelTexture->pModelTexture[i] = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, strFullPath);
		else
			pModelTexture->pModelTexture[i] = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_WIC, strFullPath);
	}


	m_ModelTextures.push_back(pModelTexture);

	return S_OK;
}

HRESULT CModel::Create_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth, ANIM_TYPE eType, _fmatrix PivotMatrix)
{
	_matrix		TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	// ADDED
	//_matrix		ScaleMatrix, RotationMatrix, TranslationMatrix;
	//_matrix		ModelPivotMatrix;
	//ScaleMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//// RotationMatrix = XMMatrixRotationX(XMConvertToRadians(90.0f));
	//ModelPivotMatrix = ScaleMatrix;

	if (eType == ANIM_TYPE::NONE)
	{
		if (!strcmp(pNode->mName.data, "spine_01"))
			eType = ANIM_TYPE::UPPER;
		else if (!strcmp(pNode->mName.data, "thigh_l") || !strcmp(pNode->mName.data, "thigh_r"))
			eType = ANIM_TYPE::LOWER;
	}

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix * PivotMatrix, pParent, iDepth, eType);
	if (nullptr == pHierarchyNode)
		return E_FAIL;


	pHierarchyNode->Reserve_Channels(m_pScene->mNumAnimations);

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1, eType);

	return S_OK;
}

HRESULT CModel::Sort_MeshesByMaterial()
{
	_uint		iNumMaterials = m_ModelTextures.size();

	m_SortByMaterialMesh.resize(iNumMaterials);

	for (auto& pMeshContainer : m_MeshContainers)
	{
		_uint	iMeshMaterialIndex = pMeshContainer->Get_MeshMaterialIndex();
		if (iMeshMaterialIndex >= m_pScene->mNumMaterials)
			return E_FAIL;
		m_SortByMaterialMesh[iMeshMaterialIndex].push_back(pMeshContainer);
		// Safe_AddRef(pMeshContainer);
	}
	return S_OK;
}

HRESULT CModel::SetUp_SkinnedInfo()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];
			if (nullptr == pBone)
				return E_FAIL;

			BONEDESC*	pBoneDesc = new BONEDESC;
			ZeroMemory(pBoneDesc, sizeof(BONEDESC));
			pBoneDesc->pName = pBone->mName.data;
			pBoneDesc->pHierarchyNode = Find_HierarchyNode(pBone->mName.data);

			_matrix		OffsetMatrix;
			memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_matrix));
			XMStoreFloat4x4(&pBoneDesc->OffsetMatrix, XMMatrixTranspose(OffsetMatrix));
			pBoneDesc->pHierarchyNode->Set_OffSetMatrix(XMMatrixTranspose(OffsetMatrix));
			// memcpy(&pBoneDesc->OffsetMatrix, &XMMatrixTranspose(XMMATRIX(pBone->mOffsetMatrix[0])), sizeof(_matrix));

			//CHierarchyNode* h = pBoneDesc->pHierarchyNode;
			//// memcpy(&h->m_offset, &XMMatrixTranspose(XMMATRIX(pBone->mOffsetMatrix[0])), sizeof(_float4x4));
			//XMStoreFloat4x4(&h->m_offset, XMMatrixTranspose(OffsetMatrix));

			//SetRagdollBoneDesc(pBone->mName.data, pBoneDesc);

			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				_uint	iVertexIndex = pBone->mWeights[k].mVertexId + pMeshContainer->Get_StartVertexIndex();

				if (0.0f == m_pVertices[iVertexIndex].vBlendWeight.x)
				{
					m_pVertices[iVertexIndex].vBlendIndex.x = j;
					m_pVertices[iVertexIndex].vBlendWeight.x = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == m_pVertices[iVertexIndex].vBlendWeight.y)
				{
					m_pVertices[iVertexIndex].vBlendIndex.y = j;
					m_pVertices[iVertexIndex].vBlendWeight.y = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == m_pVertices[iVertexIndex].vBlendWeight.z)
				{
					m_pVertices[iVertexIndex].vBlendIndex.z = j;
					m_pVertices[iVertexIndex].vBlendWeight.z = pBone->mWeights[k].mWeight;
				}
				else
				{
					m_pVertices[iVertexIndex].vBlendIndex.w = j;
					m_pVertices[iVertexIndex].vBlendWeight.w = pBone->mWeights[k].mWeight;
				}
			}
			pMeshContainer->Add_Bones(pBoneDesc);
		}
	}

	return S_OK;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pBoneName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pBoneName);
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

BONEDESC * CModel::Find_Bone(string pBoneName)
{
	for (auto& pMeshContainer : m_MeshContainers)
	{
		vector<BONEDESC*>	Bones = pMeshContainer->Get_BoneDesc();

		for (auto& pBoneDesc : Bones)
		{
			if (pBoneDesc->pHierarchyNode->Get_Name() == pBoneName)
				return pBoneDesc;
		}
	}

	return nullptr;
}

HRESULT CModel::Play_Animation(_double TimeDelta)
{
	if (m_bSimulateRagdoll)
		Update_CombinedTransformationMatrix_Ragdoll();
	else
		Update_CombinedTransformationMatrices(TimeDelta);

	return S_OK;
}

HRESULT CModel::Blend_Animation(_double TimeDelta)
{
	// Upper꺼 Lower꺼 각각 해줘야한다
	// Lower
	m_Animations[m_iAnimationIndex]->Update_TransformationMatrices(TimeDelta);
	if (m_iAnimationIndex != m_iPrevAnimationIndex)
	{
		_float ratio = (m_fBlendTime / m_fBlendDuration);
		m_Animations[m_iAnimationIndex]->Blend_Animation(m_Animations[m_iPrevAnimationIndex], ratio);
		m_fBlendTime += TimeDelta;
		if (m_fBlendTime >= m_fBlendDuration)
			m_iPrevAnimationIndex = m_iAnimationIndex;
	}

	// Upper
	m_bFinished_Upper = m_Animations[m_iAnimationIndex_Upper]->Update_TransformationMatrices(TimeDelta);
	if (m_iAnimationIndex_Upper != m_iPrevAnimationIndex_Upper)
	{
		_float ratio = (m_fBlendTime_Upper / m_fBlendDuration);
		m_Animations[m_iAnimationIndex_Upper]->Blend_Animation(m_Animations[m_iPrevAnimationIndex_Upper], ratio);
		m_fBlendTime_Upper += TimeDelta;
		if (m_fBlendTime_Upper >= m_fBlendDuration)
			m_iPrevAnimationIndex_Upper = m_iAnimationIndex_Upper;
	}

	return S_OK;
}

void CModel::SetAnimationLoop(_uint idx, _bool result)
{
	m_Animations[idx]->SetLoop(result);
}

HRESULT CModel::SetUp_AnimationInfo()
{
	_uint		iNumAnimation = m_pScene->mNumAnimations;

	for (_uint i = 0; i < iNumAnimation; ++i)
	{
		aiAnimation*	pAnim = m_pScene->mAnimations[i];
		if (nullptr == pAnim)
			return E_FAIL;

		CAnimation*		pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim*		pNodeAnim = pAnim->mChannels[j];
			if (nullptr == pNodeAnim)
				return E_FAIL;

			CChannel*		pChannel = CChannel::Create(pNodeAnim->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			_uint		iMaxKeyFrame = max(pNodeAnim->mNumScalingKeys, pNodeAnim->mNumRotationKeys);
			iMaxKeyFrame = max(iMaxKeyFrame, pNodeAnim->mNumPositionKeys);


			for (_uint k = 0; k < iMaxKeyFrame; ++k)
			{
				KEYFRAMEDESC*		pKeyFrame = new KEYFRAMEDESC;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAMEDESC));

				if (k < pNodeAnim->mNumScalingKeys)
				{
					memcpy(&pKeyFrame->vScale, &pNodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mScalingKeys[k].mTime;
				}

				if (k < pNodeAnim->mNumRotationKeys)
				{
					pKeyFrame->vRotation.x = pNodeAnim->mRotationKeys[k].mValue.x;
					pKeyFrame->vRotation.y = pNodeAnim->mRotationKeys[k].mValue.y;
					pKeyFrame->vRotation.z = pNodeAnim->mRotationKeys[k].mValue.z;
					pKeyFrame->vRotation.w = pNodeAnim->mRotationKeys[k].mValue.w;
					pKeyFrame->Time = pNodeAnim->mRotationKeys[k].mTime;
				}

				if (k < pNodeAnim->mNumPositionKeys)
				{
					memcpy(&pKeyFrame->vPosition, &pNodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mPositionKeys[k].mTime;
				}

				pChannel->Add_KeyFrameDesc(pKeyFrame);
			}
			Add_ChannelToHierarchyNode(i, pChannel);

			pAnimation->Add_Channel(pChannel);
		}

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::SetUp_AnimationIndex(_uint iAnimationIndex, ANIM_TYPE eType)
{
	if (iAnimationIndex >= m_Animations.size())
		return E_FAIL;

	if (eType == ANIM_TYPE::LOWER)
	{
		if (m_iAnimationIndex == iAnimationIndex)
			return S_OK;

		m_iPrevAnimationIndex = m_iAnimationIndex;
		m_iAnimationIndex = iAnimationIndex;

		m_Animations[iAnimationIndex]->ResetCurrentTime();
		m_fBlendTime = 0.f;
	}
	else if (eType == ANIM_TYPE::UPPER)
	{
		if (m_iAnimationIndex_Upper == iAnimationIndex)
			return S_OK;

		m_iPrevAnimationIndex_Upper = m_iAnimationIndex_Upper;
		m_iAnimationIndex_Upper = iAnimationIndex;

		m_Animations[iAnimationIndex]->ResetCurrentTime();
		m_fBlendTime_Upper = 0.f;

		m_bFinished_Upper = false;
	}

	return S_OK;
}

HRESULT CModel::Update_CombinedTransformationMatrices(_double TimeDelta)
{
	if (m_Animations.empty() || m_iAnimationIndex >= m_Animations.size())
		return E_FAIL;

	Blend_Animation(TimeDelta);
	// m_Animations[m_iAnimationIndex]->Update_TransformationMatrices(TimeDelta);

	// 여기서 Spine쪽으로 가면 Upper anim을 따르고 thigh쪽이라면 Lower anim 을 따른다
	ANIM_TYPE type = ANIM_TYPE::NONE;
	for (auto& pHierarchyNodes : m_HierarchyNodes)
	{
		if (pHierarchyNodes->Get_Type() == ANIM_TYPE::UPPER)
			type = ANIM_TYPE::UPPER;
		else if (pHierarchyNodes->Get_Type() == ANIM_TYPE::LOWER)
			type = ANIM_TYPE::LOWER;

		pHierarchyNodes->Update_CombinedTransformationMatrix(m_iAnimationIndex, m_iAnimationIndex_Upper, type);
	}

	// Set ragdoll rb position
	for (auto& ragdollRb : m_RagdollRbs)
		SetRagdollRbTransform(ragdollRb.second);

	return S_OK;
}

HRESULT CModel::Update_CombinedTransformationMatrix_Ragdoll()
{
	for (auto& pHierarchyNodes : m_HierarchyNodes)
		pHierarchyNodes->Update_CombinedTransformationMatrix_Ragdoll();

	return S_OK;
}

void CModel::Add_ChannelToHierarchyNode(_uint iAnimationindex, CChannel * pChannel)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)->bool
	{
		return !strcmp(pNode->Get_Name(), pChannel->Get_Name());
	});

	if (iter != m_HierarchyNodes.end())
		(*iter)->Add_Channel(iAnimationindex, pChannel);
}






CMeshContainer * CModel::GetMeshContainerByName(string name)
{
	auto	iter = find_if(m_MeshContainers.begin(), m_MeshContainers.end(), [&](CMeshContainer* pContainer)->bool
	{
		return pContainer->GetName() == name;
	});
	if (iter != m_MeshContainers.end())
		return *iter;

	return nullptr;
}

void CModel::SetRagdollBoneDesc(BONEDESC* desc)
{
	//auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	//{
	//	return !strcmp(pNode->Get_Name(), desc->pName);
	//});

	//if (iter == m_HierarchyNodes.end())
	//	return;

	string name = desc->pName;
	//if (name == "pelvis" ||
	//	name == "upperarm_l" || name == "lowerarm_l" || name == "hand_l" ||
	//	name == "thigh_l" || name == "calf_l" || name == "foot_l" ||
	//	name == "upperarm_r" || name == "lowerarm_r" || name == "hand_r" ||
	//	name == "thigh_r" || name == "calf_r" || name == "foot_r" ||
	//	name == "neck_01" || name == "head")
	if (name == "pelvis" ||
		name == "upperarm_l" || name == "lowerarm_l" || name == "hand_l" ||
		name == "upperarm_r" || name == "lowerarm_r" || name == "hand_r" ||
		name == "thigh_l" || name == "calf_l" || name == "foot_l" ||
		name == "thigh_r" || name == "calf_r" || name == "foot_r" ||
		name == "neck_01" || name == "head")
		m_RagdollBones.emplace(name, desc);
}

HRESULT CModel::CreateRagdollRbs()
{
	BONEDESC* pelvis = m_RagdollBones["pelvis"];

	BONEDESC* upperarm_l = m_RagdollBones["upperarm_l"];
	BONEDESC* lowerarm_l = m_RagdollBones["lowerarm_l"];
	BONEDESC* hand_l = m_RagdollBones["hand_l"];

	BONEDESC* thigh_l = m_RagdollBones["thigh_l"];
	BONEDESC* calf_l = m_RagdollBones["calf_l"];
	BONEDESC* foot_l = m_RagdollBones["foot_l"];

	BONEDESC* thigh_r = m_RagdollBones["thigh_r"];
	BONEDESC* calf_r = m_RagdollBones["calf_r"];
	BONEDESC* foot_r = m_RagdollBones["foot_r"];

	BONEDESC* upperarm_r = m_RagdollBones["upperarm_r"];
	BONEDESC* lowerarm_r = m_RagdollBones["lowerarm_r"];
	BONEDESC* hand_r = m_RagdollBones["hand_r"];

	BONEDESC* neck_01 = m_RagdollBones["neck_01"];
	BONEDESC* head = m_RagdollBones["head"];

	CreateCapsuleRb(thigh_l, calf_l, "thigh_l");
	CreateCapsuleRb(calf_l, foot_l, "calf_l");
	CreateCapsuleRb(thigh_r, calf_r, "thigh_r");
	CreateCapsuleRb(calf_r, foot_r, "calf_r");
	CreateCapsuleRb(upperarm_l, lowerarm_l, "upperarm_l");
	CreateCapsuleRb(lowerarm_l, hand_l, "lowerarm_l");
	CreateCapsuleRb(upperarm_r, lowerarm_r, "upperarm_r");
	CreateCapsuleRb(lowerarm_r, hand_r, "lowerarm_r");
	CreateCapsuleRb(pelvis, neck_01, "body");

	CreateSphereRb(head, "head");

	CreateD6Joint("thigh_l", "calf_l", "calf_l");
	CreateD6Joint("thigh_r", "calf_r", "calf_r");
	CreateD6Joint("body", "thigh_l", "pelvis");
	CreateD6Joint("body", "thigh_r", "pelvis");
	CreateD6Joint("body", "upperarm_l", "upperarm_l");
	CreateD6Joint("body", "upperarm_r", "upperarm_r");
	CreateD6Joint("upperarm_l", "lowerarm_l", "lowerarm_l");
	CreateD6Joint("upperarm_r", "lowerarm_r", "lowerarm_r");

	CreateD6Joint("body", "head", "head");


	//for (auto& rb : m_RagdollRbs)
	//	rb.second->pRb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, !m_bSimulateRagdoll);

	return S_OK;
}

void CModel::CreateCapsuleRb(BONEDESC * parent, BONEDESC * child, string name)
{
	_float radius = 0.05f;
	_vector scale, rotation, pos;
	_vector cscale, crotation, cpos;

	_matrix parentMat = parent->pHierarchyNode->Get_CombinedTransformationMatrix();
	_matrix childMat = child->pHierarchyNode->Get_CombinedTransformationMatrix();

	_matrix parentOffset = XMLoadFloat4x4(&parent->OffsetMatrix);
	_matrix childOffset = XMLoadFloat4x4(&child->OffsetMatrix);

	XMMatrixDecompose(&scale, &rotation, &pos, parentMat);
	XMMatrixDecompose(&cscale, &crotation, &cpos, childMat);

	_matrix newParentMat = XMMatrixMultiply(XMMatrixInverse(nullptr, parentOffset), m_pTransform->GetWorldMatrix());
	_matrix newChildMat = XMMatrixMultiply(XMMatrixInverse(nullptr, childOffset), m_pTransform->GetWorldMatrix());


	XMMatrixDecompose(&scale, &rotation, &pos, newParentMat);
	XMMatrixDecompose(&cscale, &crotation, &cpos, newChildMat);

	_float4x4 newChildFloat4x4;
	_float4x4 newParentFloat4x4;
	_vector pVec, cVec;
	XMStoreFloat4x4(&newChildFloat4x4, newChildMat);
	XMStoreFloat4x4(&newParentFloat4x4, newParentMat);
	memcpy(&pVec, newParentFloat4x4.m[3], sizeof(_vector));
	memcpy(&cVec, newChildFloat4x4.m[3], sizeof(_vector));
	_vector length = XMVector3Length(XMVectorSubtract(pVec, cVec));
	XMVECTOR midPos = XMVectorDivide(XMVectorAdd(pVec, cVec), FXMVECTOR{ 2.f, 2.f, 2.f, 2.f });
	_float len = XMVectorGetX(length) * .9f;

	_float len_minus_2r = len - (2.f * radius);
	_float half_height = abs(len_minus_2r / 2.f);

	PxShape* shape = CEngine::GetInstance()->GetPhysics()->createShape(PxCapsuleGeometry(radius, half_height), *CEngine::GetInstance()->GetMaterial());
	// shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

	//PxFilterData filterData;
	////filterData.word0 = CPxManager::FilterGroup::eCC; // word0 = own ID
	////filterData.word1 = CPxManager::FilterGroup::eRAGDOLL; // word0 = own ID
	//filterData.word0 = CPxManager::GROUP1;
	//filterData.word1 = CPxManager::GROUP2;
	//shape->setSimulationFilterData(filterData);
	//shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	//PxFilterData filterData;
	//filterData.word0 = CPxManager::GROUP1;
	//filterData.word1 = CPxManager::GROUP2;
	//shape->setQueryFilterData(filterData);

	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	//_vector newRotation = XMVector3InverseRotate(_vector{ 0, 0, 1}, rotation);
	////shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);


	PxTransform transform;
	memcpy(&transform.p, &midPos, sizeof(_float3));
	memcpy(&transform.q, &rotation, sizeof(_float4));
	PxRigidDynamic* body = CEngine::GetInstance()->GetPhysics()->createRigidDynamic(transform);
	body->setMass(10.f);
	body->attachShape(*shape);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	// body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

	CEngine::GetInstance()->AddActor(body);

	RAGDOLLBONEDESC* ragdollBoneDesc = new RAGDOLLBONEDESC();
	ragdollBoneDesc->pRb = body;
	ragdollBoneDesc->pParentBone = parent;
	ragdollBoneDesc->pChildBone = child;

	m_RagdollRbs.emplace(name, ragdollBoneDesc);
	parent->pHierarchyNode->AddRagdollRb(body);
}

void CModel::CreateSphereRb(BONEDESC * parent, string name)
{
	_float radius = 0.06f;
	_vector scale, rotation, pos;

	_matrix parentMat = parent->pHierarchyNode->Get_CombinedTransformationMatrix();
	_matrix parentOffset = XMLoadFloat4x4(&parent->OffsetMatrix);

	XMMatrixDecompose(&scale, &rotation, &pos, parentMat);

	_matrix newParentMat = XMMatrixMultiply(XMMatrixInverse(nullptr, parentOffset), m_pTransform->GetWorldMatrix());


	XMMatrixDecompose(&scale, &rotation, &pos, newParentMat);

	_float4x4 newParentFloat4x4;
	_vector pVec;
	XMStoreFloat4x4(&newParentFloat4x4, newParentMat);
	memcpy(&pVec, newParentFloat4x4.m[3], sizeof(_vector));

	PxShape* shape = CEngine::GetInstance()->GetPhysics()->createShape(PxSphereGeometry(radius), *CEngine::GetInstance()->GetMaterial());

	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);


	////shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	PxTransform transform;
	memcpy(&transform.p, &pVec, sizeof(_float3));
	memcpy(&transform.q, &rotation, sizeof(_float4));
	PxRigidDynamic* body = CEngine::GetInstance()->GetPhysics()->createRigidDynamic(transform);
	body->setMass(10.f);
	body->attachShape(*shape);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	// body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

	CEngine::GetInstance()->AddActor(body);

	RAGDOLLBONEDESC* ragdollBoneDesc = new RAGDOLLBONEDESC();
	ragdollBoneDesc->pRb = body;
	ragdollBoneDesc->pParentBone = parent;
	ragdollBoneDesc->pChildBone = nullptr;

	m_RagdollRbs.emplace(name, ragdollBoneDesc);
	parent->pHierarchyNode->AddRagdollRb(body);
}

void CModel::CreateD6Joint(string parent, string child, string jointBone)
{
	BONEDESC* pJointBone = m_RagdollBones[jointBone];

	PxRigidDynamic* parentRb = m_RagdollRbs[parent]->pRb;
	PxRigidDynamic* childRb = m_RagdollRbs[child]->pRb;

	_float4x4 joingOffset = pJointBone->OffsetMatrix;
	_matrix newParentMat = XMMatrixMultiply(XMMatrixInverse(nullptr, XMLoadFloat4x4(&joingOffset)), m_pTransform->GetWorldMatrix());
	_vector scale, rotation, pos;
	XMMatrixDecompose(&scale, &rotation, &pos, newParentMat);

	PxTransform transform;
	memcpy(&transform.p, &pos, sizeof(_float3));
	memcpy(&transform.q, &rotation, sizeof(_float4));

	PxD6Joint* joint = PxD6JointCreate(*CEngine::GetInstance()->GetPhysics(),
		parentRb,
		parentRb->getGlobalPose().transformInv(transform),
		childRb,
		childRb->getGlobalPose().transformInv(transform));

	joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

	ConfigD6Joint(3.14f / 2.f, 3.14f / 2.f, -3.14f / 8.f, 3.14f / 8.f, joint);
}

void CModel::ConfigD6Joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint * joint)
{
	joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);

	joint->setSwingLimit(physx::PxJointLimitCone(swing0, swing1));
	joint->setTwistLimit(physx::PxJointAngularLimitPair(twistLo, twistHi));
}

void CModel::SetRagdollRbTransform(RAGDOLLBONEDESC * ragdollBoneDesc)
{
	PxTransform transform;
	// XMMATRIX matParent = XMMatrixMultiply(XMLoadFloat4x4(&ragdollBoneDesc->pParentBone->OffsetMatrix), ragdollBoneDesc->pParentBone->pHierarchyNode->Get_CombinedTransformationMatrix());
	// XMMATRIX matChild = XMMatrixMultiply(XMLoadFloat4x4(&ragdollBoneDesc->pChildBone->OffsetMatrix), ragdollBoneDesc->pChildBone->pHierarchyNode->Get_CombinedTransformationMatrix());
	_vector scale, rotation, pos;
	_float4x4 newParentFloat4x4;
	_vector pVec, cVec;

	XMMATRIX matParent = ragdollBoneDesc->pParentBone->pHierarchyNode->Get_CombinedTransformationMatrix();
	matParent = XMMatrixMultiply(matParent, XMLoadFloat4x4(&m_pTransform->GetMatrix()));
	XMMatrixDecompose(&scale, &rotation, &pos, matParent);
	XMStoreFloat4x4(&newParentFloat4x4, matParent);
	memcpy(&pVec, newParentFloat4x4.m[3], sizeof(_vector));

	if (ragdollBoneDesc->pChildBone)
	{
		XMMATRIX matChild = ragdollBoneDesc->pChildBone->pHierarchyNode->Get_CombinedTransformationMatrix();
		matChild = XMMatrixMultiply(matChild, XMLoadFloat4x4(&m_pTransform->GetMatrix()));


		_float4x4 newChildFloat4x4;
		XMStoreFloat4x4(&newChildFloat4x4, matChild);
		memcpy(&cVec, newChildFloat4x4.m[3], sizeof(_vector));
		XMVECTOR midPos = XMVectorDivide(XMVectorAdd(pVec, cVec), FXMVECTOR{ 2.f, 2.f, 2.f, 2.f });

		memcpy(&transform.p, &midPos, sizeof(_float3));
		memcpy(&transform.q, &rotation, sizeof(_float4));
		ragdollBoneDesc->pRb->setGlobalPose(transform, false);
		ragdollBoneDesc->pRb->setKinematicTarget(transform);
		// ragdollBoneDesc->pRb->putToSleep();
	}
	else
	{
		memcpy(&transform.p, &pVec, sizeof(_float3));
		memcpy(&transform.q, &rotation, sizeof(_float4));
		ragdollBoneDesc->pRb->setGlobalPose(transform, false);
		ragdollBoneDesc->pRb->setKinematicTarget(transform);
		ragdollBoneDesc->pRb->putToSleep();
	}
}

void CModel::SetRagdollSimulate(_bool result)
{
	m_bSimulateRagdoll = result;

	for (auto& rb : m_RagdollRbs)
	{
		rb.second->pRb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, !result);
		rb.second->pRb->wakeUp();
		rb.second->pRb->setLinearVelocity(PxVec3(PxZero));
		rb.second->pRb->setAngularVelocity(PxVec3(PxZero));
	}
	//if (m_bSimulateRagdoll)
	//{
	//	PxShape* shape = nullptr;
	//	rb.second->pRb->getShapes(&shape, 1);
	//	// shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	//}

	for (auto& rb : m_RagdollRbs)
	{
		rb.second->pRb->setLinearVelocity(PxVec3(PxZero));
		rb.second->pRb->setAngularVelocity(PxVec3(PxZero));
	}
}

PxRigidDynamic * CModel::GetRagdollRb(string name)
{
	return m_RagdollRbs[name]->pRb;
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
	_vector position = m_pTransform->GetState(CTransform::STATE_POSITION);
	XMVECTOR quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransform->GetMatrix()));
	memcpy(&transform.p, &position, sizeof(_float3));
	memcpy(&transform.q, &quat, sizeof(_float4));

	PxRigidStatic* m_pRigidActor = pEngine->GetPhysics()->createRigidStatic(transform);
	PxMaterial* mat = CEngine::GetInstance()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	PxShape* shape = pEngine->GetPhysics()->createShape(PxTriangleMeshGeometry(m_pPxMesh), *mat);

	PxFilterData filterData;
	filterData.word0 = CPxManager::GROUP1;
	// filterData.word1 = CPxManager::GROUP2;
	shape->setQueryFilterData(filterData);

	m_pRigidActor->attachShape(*shape);
	pEngine->AddActor(m_pRigidActor);

	return;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
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
	RemoveBuffer();

	__super::Free();
}

void CModel::RemoveBuffer()
{
	if (false == m_isCloned)
	{
		SafeDeleteArray(m_pVertices);
		SafeDeleteArray(m_pPolygonIndices);
		SafeDeleteArray(m_pxVertices);
		SafeDeleteArray(m_pxIndices);

		for (auto& texture : m_ModelTextures)
		{
			for (auto& tex : texture->pModelTexture)
				SafeRelease(tex);
			SafeDelete(texture);
		}

		SafeRelease(m_pVB);
		SafeRelease(m_pIB);
	}
	m_pShader.reset();

	m_RagdollBones.clear();

	for (auto& rb : m_RagdollRbs)
		SafeDelete(rb.second);
	m_RagdollRbs.clear();

	for (auto& anim : m_Animations)
		SafeRelease(anim);
	m_Animations.clear();

	for (auto& container : m_MeshContainers)
		SafeRelease(container);
	m_MeshContainers.clear();


	m_ModelTextures.clear();

	// TODO: Release
	for (auto& mesh : m_SortByMaterialMesh)
		mesh.clear();
	m_SortByMaterialMesh.clear();

	for (auto& node : m_HierarchyNodes)
		SafeRelease(node);
	m_HierarchyNodes.clear();

	//m_pVB.Reset();
	//m_pIB.Reset();


	m_Importer.FreeScene();
}