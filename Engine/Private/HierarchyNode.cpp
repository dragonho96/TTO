#include "..\public\HierarchyNode.h"
#include "Channel.h"


CHierarchyNode::CHierarchyNode()
{

}

HRESULT CHierarchyNode::Initialize(const char * pNodeName, _fmatrix TransformationMatrix, CHierarchyNode * pParent, _uint iDepth)
{
	strcpy_s(m_szNodeName, pNodeName);
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(TransformationMatrix));
	m_pParent = pParent;
	m_iDepth = iDepth;

	return S_OK;
}

HRESULT CHierarchyNode::Add_Channel(_uint iAnimationIndex, CChannel * pChannel)
{
	m_Channels[iAnimationIndex] = pChannel;

	//SafeAddRef(pChannel);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iAnimationIndex)
{
	_matrix		TransformationMatrix;

	if (nullptr == m_Channels[iAnimationIndex])
		TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	else
		TransformationMatrix = m_Channels[iAnimationIndex]->Get_TransformationMatrix();

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix);
}

void CHierarchyNode::Reserve_Channels(_uint iNumAnimation)
{
	m_Channels.resize(iNumAnimation);
}

CHierarchyNode * CHierarchyNode::Create(const char * pNodeName, _fmatrix TransformationMatrix, CHierarchyNode * pParent, _uint iDepth)
{
	CHierarchyNode*		pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize(pNodeName, TransformationMatrix, pParent, iDepth)))
	{
		MSG_BOX("Failed to Create CHierarchyNode");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{

}