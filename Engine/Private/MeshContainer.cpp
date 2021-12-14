#include "..\Public\MeshContainer.h"
#include "HierarchyNode.h"


CMeshContainer::CMeshContainer()
{
}

HRESULT CMeshContainer::Initialize(const char* pName, _uint iNumFaces, _uint iStartFaceIndex, _uint iStartVertexIndex, _uint iMaterialIndex)
{
	strcpy(m_szName, pName);
	m_iNumFaces = iNumFaces;
	m_iStartFaceIndex = iStartFaceIndex;
	m_iStartVertexIndex = iStartVertexIndex;
	m_iMaterialIndex = iMaterialIndex;

	return S_OK;
}

HRESULT CMeshContainer::Add_Bones(BONEDESC * pBoneDesc)
{
	m_Bones.push_back(pBoneDesc);

	return S_OK;
}

void CMeshContainer::Get_BoneMatrices(_matrix * pBoneMatrices)
{
	_uint		iNumBones = m_Bones.size();

	for (_uint i = 0; i < iNumBones; ++i)
	{
		pBoneMatrices[i] = XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&m_Bones[i]->OffsetMatrix), m_Bones[i]->pHierarchyNode->Get_CombinedTransformationMatrix()));
	}
}

void CMeshContainer::Get_BoneMatrices_Ragdoll(_matrix * pBoneMatrices)
{
	_uint		iNumBones = m_Bones.size();

	for (_uint i = 0; i < iNumBones; ++i)
	{
		pBoneMatrices[i] = XMMatrixTranspose(m_Bones[i]->pHierarchyNode->Get_CombinedTransformationMatrix());
	}
}

HRESULT CMeshContainer::Clone_BoneDesc()
{
	vector<BONEDESC*>		OriginalBones = m_Bones;
	m_Bones.clear();

	for (auto& pOriginalBone : OriginalBones)
	{
		BONEDESC*		pBone = new BONEDESC();
		ZeroMemory(pBone, sizeof(BONEDESC));

		// pBone->pHierarchyNode = pOriginalBone->pHierarchyNode;
		pBone->OffsetMatrix = pOriginalBone->OffsetMatrix;
		pBone->pName = pOriginalBone->pName;
		m_Bones.push_back(pBone);
	}

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(const char* pName, _uint iNumFaces, _uint iStartFaceIndex, _uint iStartVertexIndex, _uint iMaterialIndex)
{
	CMeshContainer*	pInstance = new CMeshContainer();

	if (FAILED(pInstance->Initialize(pName, iNumFaces, iStartFaceIndex, iStartVertexIndex, iMaterialIndex)))
	{
		MSG_BOX("Failed To Creating CMeshContainer");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CMeshContainer * CMeshContainer::Clone()
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(Clone_BoneDesc()))
	{
		SafeRelease(pInstance);
	}


	return pInstance;
}

void CMeshContainer::Free()
{
	for (auto& bone : m_Bones)
		SafeDelete(bone);
	m_Bones.clear();
}