#include "..\public\HierarchyNode.h"
#include "Channel.h"


CHierarchyNode::CHierarchyNode()
{

}

HRESULT CHierarchyNode::Initialize(const char * pNodeName, _fmatrix TransformationMatrix, CHierarchyNode * pParent, _uint iDepth, ANIM_TYPE eType)
{
	strcpy_s(m_szNodeName, pNodeName);
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(TransformationMatrix));
	m_pParent = pParent;
	m_iDepth = iDepth;
	m_eType = eType;

	return S_OK;
}

HRESULT CHierarchyNode::Add_Channel(_uint iAnimationIndex, CChannel * pChannel)
{
	m_Channels[iAnimationIndex] = pChannel;

	//SafeAddRef(pChannel);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iAnimationIndex, _uint iAnimationIndex_Upper, ANIM_TYPE eType)
{
	_matrix		TransformationMatrix;
	_uint		animIndex;

	if (eType == ANIM_TYPE::UPPER)
		animIndex = iAnimationIndex_Upper;
	else
		animIndex = iAnimationIndex;

	if (nullptr == m_Channels[animIndex])
		TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	else
		TransformationMatrix = m_Channels[animIndex]->Get_TransformationMatrix();

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix);
}

void CHierarchyNode::Update_CombinedTransformationMatrix_Ragdoll()
{
	_matrix		TransformationMatrix;
	TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);

	_vector scale, rotation, pos;
	XMMatrixDecompose(&scale, &rotation, &pos, TransformationMatrix);

	// 만약 노드 이름이 pelvis leg... 면 바로 matrix 대입, 아니면 parent따라서 곱함
	if (m_pRagdollRb)
	{
		PxTransform transform = m_pRagdollRb->getGlobalPose();

		PxShape* shape;
		m_pRagdollRb->getShapes(&shape, 1);

		PxCapsuleGeometry cap;
		shape->getCapsuleGeometry(cap);
		float halfLength = ((cap.halfHeight * 2.f) + (cap.radius * 2.f)) * 1.1f / 2.f;

		_vector quat;
		memcpy(&quat, &transform.q, sizeof(_vector));

		PxMat44 pxMat = PxMat44(transform);
		_float4x4 mat;
		memcpy(&mat, &pxMat, sizeof(_float4x4));
		// Getting first row = Up Vector
		_vector vecUp = { mat._11, mat._12, mat._13, mat._14 };
		vecUp = XMVector3Normalize(vecUp);

		_vector addPos = vecUp * halfLength;

		// _vector vecScale = { 0.01f, 0.01f, 0.01f, 1 };
		_vector vecScale = { 1.f, 1.f, 1.f, 1 };
		_vector vecPos = { transform.p.x, transform.p.y, transform.p.z, 1 };

		vecPos = vecPos - addPos;

		_matrix matWorld2 = XMMatrixAffineTransformation(vecScale, XMVectorSet(0.f, 0.f, 0.f, 0.f), quat, vecPos);
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, matWorld2);
	}
	else if (m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
}

void CHierarchyNode::Reserve_Channels(_uint iNumAnimation)
{
	m_Channels.resize(iNumAnimation);
}

CHierarchyNode * CHierarchyNode::Create(const char * pNodeName, _fmatrix TransformationMatrix, CHierarchyNode * pParent, _uint iDepth, ANIM_TYPE eType)
{
	CHierarchyNode*		pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize(pNodeName, TransformationMatrix, pParent, iDepth, eType)))
	{
		MSG_BOX("Failed to Create CHierarchyNode");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{

}