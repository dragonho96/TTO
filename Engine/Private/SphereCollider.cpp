#include "..\Public\SphereCollider.h"
#include "Transform.h"
#include "Engine.h"

CSphereCollider::CSphereCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCollider(pDevice, pDeviceContext)
	, m_fRadius(0.5f)
{

}

CSphereCollider::CSphereCollider(const CSphereCollider & rhs)
	: CCollider(rhs)
	, m_fRadius(0.5f)
{

}


CSphereCollider * CSphereCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSphereCollider*	pInstance = new CSphereCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CSphereCollider");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CSphereCollider::Clone(void * pArg)
{
	CComponent*	pInstance = new CSphereCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CSphereCollider");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CSphereCollider::Free()
{
	__super::Free();
}

HRESULT CSphereCollider::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSphereCollider::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpDebugLine()))
		return E_FAIL;

	return S_OK;
}


HRESULT CSphereCollider::SetUpDebugLine(/* SIZE DESC */)
{
	m_pDebugLine = m_pEngine->CloneComponent(0, "Prototype_VIBuffer_LineSphere", m_pObjTransform);
	if (nullptr == m_pDebugLine)
		return E_FAIL;

	return S_OK;
}

void CSphereCollider::SetUpRigidActor(void* pShapeInfo, RIGIDBODYDESC desc, CGameObject* obj)
{
	m_RigidBodyDesc = desc;
	_float inputRadius = 0.0f;
	memcpy(&inputRadius, pShapeInfo, sizeof(_float));
	SetSize(inputRadius);

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
	{
		_vector position = m_pObjTransform->GetState(CTransform::STATE_POSITION);
		PxTransform transform;
		memcpy(&transform.p, &position, sizeof(_float3));
		transform.p.x += m_vRelativePos.x;
		transform.p.y += m_vRelativePos.y;
		transform.p.z += m_vRelativePos.z;

		XMVECTOR quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pObjTransform->GetMatrix()));
		memcpy(&transform.q, &quat, sizeof(_float4));
		PxReal radius = m_fRadius;
		PxMaterial* pMaterial = m_pEngine->GetMaterial();
		PxShape* meshShape = m_pEngine->GetPhysics()->createShape(PxSphereGeometry(radius), *pMaterial);

		// Make Dynamic
		if (desc.bEnabled)
		{
			m_pRigidActor = m_pEngine->GetPhysics()->createRigidDynamic(transform);
			m_pRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !desc.bGravity);
			m_pRigidActor->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, desc.bKinematic);
		}
		// Make Static
		else
		{
			m_pRigidActor = m_pEngine->GetPhysics()->createRigidStatic(transform);
		}
		m_pRigidActor->userData = obj;
		m_pRigidActor->attachShape(*meshShape);
		m_pEngine->AddActor(m_pRigidActor);
	}
}

void CSphereCollider::SetSize(float fRadius)
{
	if (m_fRadius == fRadius)
		return;

	m_fRadius = fRadius;
	dynamic_cast<CVIBuffer_LineSphere*>(m_pDebugLine)->SetSize(m_fRadius);
}
