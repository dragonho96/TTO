#include "..\Public\Collider.h"
#include "Engine.h"

USING(Engine)

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_pEngine(CEngine::GetInstance())
	, m_RigidBodyDesc(rhs.m_RigidBodyDesc)
{
}

void CCollider::Free()
{
	__super::Free();
	if (m_isCloned)
		SafeRelease(m_pDebugLine);
}

HRESULT CCollider::InitializePrototype()
{
	ZeroMemory(&m_RigidBodyDesc, sizeof(m_RigidBodyDesc));
	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	if (pArg)
		m_pObjTransform = (CTransform*)pArg;

	return S_OK;
}

_uint CCollider::LateUpdate(double deltaTime)
{
	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
	{
		if (m_pController)
		{
			/* Adjusting mesh position */
			PxExtendedVec3 pos = m_pController->getPosition();
			pos.x -= m_vRelativePos.x;
			pos.y -= m_vRelativePos.y;
			pos.z -= m_vRelativePos.z;
			m_pObjTransform->SetPxPosition(pos);

			/* Back to px transform */
			_float4x4 childMat;
			XMStoreFloat4x4(&childMat, XMMatrixIdentity());
			memcpy(&childMat.m[3][0], &m_vRelativePos, sizeof(_float3));
			_matrix newMat = XMMatrixMultiply(XMLoadFloat4x4(&childMat), XMLoadFloat4x4(&m_pObjTransform->GetMatrix()));
			memcpy(&m_pxMat, &newMat, sizeof(_float4x4));
		}
		// Check if its dynamic or static
		else if (m_pRigidActor)
		{
			/* for static actor */
			PxTransform actorTransform = m_pRigidActor->getGlobalPose();
			PxMat44 pxMat = PxMat44(actorTransform);
			//pxMat.column3.x -= m_vRelativePos.x;
			//pxMat.column3.y -= m_vRelativePos.y;
			//pxMat.column3.z -= m_vRelativePos.z;
			// m_pObjTransform->SetPxMatrix(pxMat);
			memcpy(&m_pxMat, &pxMat, sizeof(_float4x4));
		}
	}

	return _uint();
}

HRESULT CCollider::Render()
{
	if (m_pDebugLine)
	{
		if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
			dynamic_cast<CVIBuffer*>(m_pDebugLine)->RenderDebug(m_pxMat);
		else
			dynamic_cast<CVIBuffer*>(m_pDebugLine)->RenderDebug(m_vRelativePos);
	}
	return S_OK;
}

void CCollider::ReleaseController()
{
	if (m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
	}
}

