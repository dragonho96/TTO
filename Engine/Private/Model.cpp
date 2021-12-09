#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Shader.h"
#include "Engine.h"
#include "HierarchyNode.h"

#include "Animation.h"
#include "Channel.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumFaces(rhs.m_iNumFaces)
	, m_pVertices(rhs.m_pVertices)
	, m_pPolygonIndices(rhs.m_pPolygonIndices)
	, m_MeshContainers(rhs.m_MeshContainers)
	, m_SortByMaterialMesh(rhs.m_SortByMaterialMesh)
	, m_ModelTextures(rhs.m_ModelTextures)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iStride(rhs.m_iStride)
	, m_EffectDescs(rhs.m_EffectDescs)
	, m_pEffect(rhs.m_pEffect)
{
}

HRESULT CModel::InitializePrototype()
{
	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	if (pArg)
		m_pTransform = (CTransform*)pArg;

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
	m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &m_iStride, &iOffSet);
	m_pDeviceContext->IASetIndexBuffer(m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
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

	m_pShader->Render(iPassIndex);

	_matrix			BoneMatrices[128];

	for (auto& pMeshContainer : m_SortByMaterialMesh[iMaterialIndex])
	{
		m_pDeviceContext->DrawIndexed(pMeshContainer->Get_NumFaces() * 3,
			pMeshContainer->Get_StartFaceIndex() * 3,
			pMeshContainer->Get_StartVertexIndex());
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

HRESULT CModel::Create_HierarchyNodes(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{
	_matrix		TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformationMatrix, pParent, iDepth);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	pHierarchyNode->Reserve_Channels(m_pScene->mNumAnimations);

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1);

	return S_OK;
}

HRESULT CModel::Sort_MeshesByMaterial()
{
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

			pBoneDesc->pHierarchyNode = Find_HierarchyNode(pBone->mName.data);
			memcpy(&pBoneDesc->OffsetMatrix, &XMMatrixTranspose(XMMATRIX(pBone->mOffsetMatrix[0])), sizeof(_matrix));

			SetRagdollBoneDesc(pBone->mName.data, pBoneDesc);

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
}

HRESULT CModel::SetUp_AnimationIndex(_uint iAnimationIndex)
{
	if (iAnimationIndex >= m_Animations.size())
		return E_FAIL;

	m_iAnimationIndex = iAnimationIndex;

	return S_OK;
}

HRESULT CModel::Update_CombinedTransformationMatrices(_double TimeDelta)
{
	if (m_Animations.empty())
		return E_FAIL;

	m_Animations[m_iAnimationIndex]->Update_TransformationMatrices(TimeDelta);

	for (auto& pHierarchyNodes : m_HierarchyNodes)
		pHierarchyNodes->Update_CombinedTransformationMatrix(m_iAnimationIndex);

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


HRESULT CModel::CreateBuffer(string pMeshFilePath, string pMeshFileName, string pShaderFilePath)
{
	if (pMeshFileName.length() == 0 || pMeshFileName.length() == 0)
		return S_OK;

	RemoveBuffer();

	m_pShader = make_unique<CShader>(pShaderFilePath);

	m_pMeshFilePath = pMeshFilePath;
	m_pMeshFileName = pMeshFileName;
	m_pShaderFilePath = pShaderFilePath;

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


	m_ModelTextures.reserve(m_pScene->mNumMaterials);

	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		if (FAILED(Create_Materials(m_pScene->mMaterials[i], pMeshFilePath)))
			return E_FAIL;
	}

	if (FAILED(Sort_MeshesByMaterial()))
		return E_FAIL;

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT && m_bMeshCollider)
		CreatePxMesh();

	if (false == m_pScene->HasAnimations())
	{
		if (FAILED(Create_VertexIndexBuffer(pShaderFilePath)))
			return E_FAIL;

		return S_OK;
	}

	int numAnim = m_pScene->mNumAnimations;
	Create_HierarchyNodes(m_pScene->mRootNode);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest) {
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(SetUp_SkinnedInfo()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffer(pShaderFilePath)))
		return E_FAIL;

	if (FAILED(SetUp_AnimationInfo()))
		return E_FAIL;

	if (FAILED(Update_CombinedTransformationMatrices(0.0)))
		return E_FAIL;

	CreateRagdollRbs();


	return S_OK;
}

void CModel::RemoveBuffer()
{
	m_pShader.reset();

	for (auto& anim : m_Animations)
		SafeRelease(anim);
	m_Animations.clear();

	for (auto& container : m_MeshContainers)
		SafeRelease(container);
	m_MeshContainers.clear();

	for (auto& texture : m_ModelTextures)
	{
		for (auto& tex : texture->pModelTexture)
			SafeRelease(tex);
		SafeDelete(texture);
	}
	m_ModelTextures.clear();

	for (auto& mesh : m_SortByMaterialMesh)
		mesh.clear();
	m_SortByMaterialMesh.clear();

	for (auto& node : m_HierarchyNodes)
		SafeRelease(node);
	m_HierarchyNodes.clear();


	SafeDeleteArray(m_pVertices);
	SafeDeleteArray(m_pPolygonIndices);
	SafeDeleteArray(m_pxVertices);
	SafeDeleteArray(m_pxIndices);
	m_pVB.Reset();
	m_pIB.Reset();
	m_Importer.FreeScene();
}

void CModel::SetRagdollBoneDesc(string name, BONEDESC* desc)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), name.c_str());
	});

	if (iter == m_HierarchyNodes.end())
		return;

	if (name == "Pelvis" ||
		name == "LArm" || name == "LElbowSplitter" || name == "LHand" ||
		name == "LLeg" || name == "LKneeSplitter" || name == "LFoot" ||
		name == "RArm" || name == "RElbowSplitter" || name == "RHand" ||
		name == "RLeg" || name == "RKneeSplitter" || name == "RFoot" ||
		name == "Neck" || name == "Head")
		m_RagdollBones.emplace(name, desc);
}

void CModel::CreateRagdollRbs()
{
	BONEDESC* Pelvis = m_RagdollBones["Pelvis"];

	BONEDESC* LArm = m_RagdollBones["LArm"];
	BONEDESC* LElbowSplitter = m_RagdollBones["LElbowSplitter"];
	BONEDESC* LHand = m_RagdollBones["LHand"];

	BONEDESC* LLeg = m_RagdollBones["LLeg"];
	BONEDESC* LKneeSplitter = m_RagdollBones["LKneeSplitter"];
	BONEDESC* LFoot = m_RagdollBones["LFoot"];

	BONEDESC* RLeg = m_RagdollBones["RLeg"];
	BONEDESC* RKneeSplitter = m_RagdollBones["RKneeSplitter"];
	BONEDESC* RFoot = m_RagdollBones["RFoot"];

	BONEDESC* RArm = m_RagdollBones["RArm"];
	BONEDESC* RElbowSplitter = m_RagdollBones["RElbowSplitter"];
	BONEDESC* RHand = m_RagdollBones["RHand"];

	BONEDESC* Neck = m_RagdollBones["Neck"];
	BONEDESC* Head = m_RagdollBones["Head"];

	CreateCapsuleRb(LLeg, LKneeSplitter, "LThigh");
	CreateCapsuleRb(LKneeSplitter, LFoot, "LCalf");
	CreateCapsuleRb(RLeg, RKneeSplitter, "RThigh");
	CreateCapsuleRb(RKneeSplitter, RFoot, "RCalf");
	CreateCapsuleRb(LArm, LElbowSplitter, "LArm");
	CreateCapsuleRb(LElbowSplitter, LHand, "LForearm");
	CreateCapsuleRb(RArm, RElbowSplitter, "RArm");
	CreateCapsuleRb(RElbowSplitter, RHand, "RForearm");
	CreateCapsuleRb(Pelvis, Neck, "Body");


	CreateD6Joint("LThigh", "LCalf", "LKneeSplitter");
	CreateD6Joint("RThigh", "RCalf", "RKneeSplitter");
	CreateD6Joint("Body", "LThigh", "Pelvis");
	CreateD6Joint("Body", "RThigh", "Pelvis");
	CreateD6Joint("Body", "LArm", "LArm");
	CreateD6Joint("Body", "RArm", "RArm");
	CreateD6Joint("LArm", "LForearm", "LElbowSplitter");
	CreateD6Joint("RArm", "RForearm", "RElbowSplitter");


	for (auto& rb : m_RagdollRbs)
		rb.second->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
}

void CModel::CreateCapsuleRb(BONEDESC * parent, BONEDESC * child, string name)
{
	_float radius = 0.05f;
	_vector scale, rotation, pos;
	_vector cscale, crotation, cpos;

	_float4x4 parentMat = parent->pHierarchyNode->Get_CombinedTransformationMatrix();
	_float4x4 childMat = child->pHierarchyNode->Get_CombinedTransformationMatrix();

	_float4x4 parentOffset = parent->OffsetMatrix;
	_float4x4 childOffset = child->OffsetMatrix;

	XMMatrixDecompose(&scale, &rotation, &pos, XMLoadFloat4x4(&parentMat));
	XMMatrixDecompose(&cscale, &crotation, &cpos, XMLoadFloat4x4(&childMat));
	//_matrix newParentMat = XMMatrixMultiply(m_pTransform->GetWorldMatrix(), XMMatrixTranspose(XMLoadFloat4x4(&parentMat)));
	//_matrix newChildMat = XMMatrixMultiply(m_pTransform->GetWorldMatrix(), XMMatrixTranspose(XMLoadFloat4x4(&childMat)));
	//_matrix newParentMat = XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&parentMat)), m_pTransform->GetWorldMatrix());
	//_matrix newChildMat = XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&childMat)), m_pTransform->GetWorldMatrix());
	//_matrix newParentMat = XMMatrixMultiply(XMLoadFloat4x4(&parentMat), m_pTransform->GetWorldMatrix());
	//_matrix newChildMat = XMMatrixMultiply(XMLoadFloat4x4(&childMat), m_pTransform->GetWorldMatrix());

	_matrix newParentMat = XMMatrixMultiply(XMMatrixInverse(nullptr, XMLoadFloat4x4(&parentOffset)), m_pTransform->GetWorldMatrix());
	_matrix newChildMat = XMMatrixMultiply(XMMatrixInverse(nullptr, XMLoadFloat4x4(&childOffset)), m_pTransform->GetWorldMatrix());
	

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
	//PxTransform local;
	//local.p = { XMVectorGetX(midPos), XMVectorGetY(midPos) , XMVectorGetZ(midPos)};
	//local.q = PxIdentity;
	//shape->setLocalPose(local);

	_vector position = m_pTransform->GetState(CTransform::STATE_POSITION);
	PxTransform transform;
	memcpy(&transform.p, &midPos, sizeof(_float3));
	//XMVECTOR quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransform->GetMatrix()));
	XMVECTOR quat = XMQuaternionRotationMatrix(newParentMat);
	memcpy(&transform.q, &quat, sizeof(_float4));
	//transform.p.x *= -1;
	//transform.p.z *= -1;
	PxRigidDynamic* body = CEngine::GetInstance()->GetPhysics()->createRigidDynamic(transform);
	body->setMass(1.f);
	body->attachShape(*shape);
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	CEngine::GetInstance()->AddActor(body);

	m_RagdollRbs.emplace(name, body);
}

void CModel::CreateD6Joint(string parent, string child, string jointBone)
{
	BONEDESC* pJointBone = m_RagdollBones[jointBone];

	PxRigidDynamic* parentRb = m_RagdollRbs[parent];
	PxRigidDynamic* childRb = m_RagdollRbs[child];

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

	ConfigD6Joint(3.14 / 4.f, 3.14f / 4.f, -3.14f / 8.f, 3.14f / 8.f, joint);
}

void CModel::ConfigD6Joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint * joint)
{
	joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);

	joint->setSwingLimit(physx::PxJointLimitCone(swing0, swing1));
	joint->setTwistLimit(physx::PxJointAngularLimitPair(twistLo, twistHi));
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