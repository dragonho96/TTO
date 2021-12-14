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

void CHierarchyNode::Update_CombinedTransformationMatrix_Ragdoll()
{
	if (!strcmp(m_szNodeName, "Pelvis"))
		int i = 0;
	_matrix		TransformationMatrix;
	TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);

	_vector scale, rotation, pos;
	XMMatrixDecompose(&scale, &rotation, &pos, TransformationMatrix);

	// 만약 노드 이름이 pelvis leg... 면 바로 matrix 대입, 아니면 parent따라서 곱함
	if (m_pRagdollRb)
	{

		PxTransform transform = m_pRagdollRb->getGlobalPose();
		PxCapsuleGeometry cap;
		PxShape* shape;
		m_pRagdollRb->getShapes(&shape, 1);
		shape->getCapsuleGeometry(cap);
		float halfLength = ((cap.halfHeight * 2.f) + (cap.radius * 2.f)) * 1.1f / 2.f;
		_vector quat;
		memcpy(&quat, &transform.q, sizeof(_vector));

		PxMat44 m = PxMat44(transform);
		XMMATRIX d3dMat;
		memcpy(&d3dMat, &m, sizeof(XMMATRIX));
		_float4x4 dd;
		XMStoreFloat4x4(&dd, d3dMat);
		// Getting first row = Up Vector
		_vector vecUp = { dd._11, dd._12, dd._13, dd._14 };
		vecUp = XMVector3Normalize(vecUp);

		_vector addPos = vecUp * halfLength;

		_vector vecScale = { 0.01f, 0.01f, 0.01f, 1 };
		_vector vecPos = { transform.p.x, transform.p.y, transform.p.z, 1 };
		if (!strcmp(m_szNodeName, "Pelvis"))
			vecPos = vecPos - addPos;
		//else
		//	vecPos = vecPos + addPos;

		_matrix matWorld2 = XMMatrixAffineTransformation(vecScale, XMVectorSet(0.f, 0.f, 0.f, 0.f), quat, vecPos);
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, matWorld2);
	}
	else if (m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
		//XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_offset) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	//else
	//	XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix);
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