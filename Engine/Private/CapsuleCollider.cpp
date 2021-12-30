#include "..\Public\CapsuleCollider.h"
#include "Transform.h"
#include "PxManager.h"
#include "Engine.h"

CCapsuleCollider::CCapsuleCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCollider(pDevice, pDeviceContext)
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
{

}

CCapsuleCollider::CCapsuleCollider(const CCapsuleCollider & rhs)
	: CCollider(rhs)
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
{

}


CCapsuleCollider * CCapsuleCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCapsuleCollider*	pInstance = new CCapsuleCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CCapsuleCollider");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CCapsuleCollider::Clone(void * pArg)
{
	CComponent*	pInstance = new CCapsuleCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CCapsuleCollider");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CCapsuleCollider::Free()
{
	__super::Free();
}

HRESULT CCapsuleCollider::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCapsuleCollider::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpDebugLine()))
		return E_FAIL;

	return S_OK;
}


HRESULT CCapsuleCollider::SetUpDebugLine(/* SIZE DESC */)
{
	m_pDebugLine = m_pEngine->CloneComponent(0, "Prototype_VIBuffer_LineCapsule", m_pObjTransform);
	if (nullptr == m_pDebugLine)
		return E_FAIL;

	return S_OK;
}

void CCapsuleCollider::SetUpRigidActor(void* pShapeInfo, RIGIDBODYDESC desc, CGameObject* obj)
{
	m_RigidBodyDesc = desc;

	pair<float, float> capsuleSize;
	memcpy(&capsuleSize, pShapeInfo, sizeof(pair<float, float>));
	SetSize(capsuleSize);

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
	{
		PxReal radius = capsuleSize.first;
		PxReal height = capsuleSize.second + (radius * 2);
		PxMaterial* pMaterial = m_pEngine->GetMaterial();
		_vector position = m_pObjTransform->GetState(CTransform::STATE_POSITION);

		PxTransform transform;
		memcpy(&transform.p, &position, sizeof(_float3));

		if (desc.bCC)
		{
			PxCapsuleControllerDesc desc;
			desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
			desc.radius = radius;
			desc.height = height;
			desc.slopeLimit = 0.f/*cosf(D3DXToRadian(45.f))*/;
			//desc.stepOffset = 0.2f;
			desc.upDirection = PxVec3(0.0, 1.0, 0.0);
			desc.material = pMaterial;
			desc.position = PxExtendedVec3(transform.p.x, transform.p.y, transform.p.z);
			desc.contactOffset = 0.001f;
			desc.userData = obj;
			//desc.behaviorCallback = 
			//desc.reportCallback = &m_callback;
			//CE_ASSERT(desc.isValid(), "Capsule is not valid");
			if (CEngine::GetInstance()->GetControllerManager())
				m_pController = CEngine::GetInstance()->GetControllerManager()->createController(desc);
			PxShape* shape = nullptr;
			m_pController->getActor()->getShapes(&shape, 1);
			m_pController->getActor()->userData = obj;
			if (m_pController->getActor()->userData)
				int i = 0;
			PxFilterData filterData;
			filterData.word0 = CPxManager::GROUP1;
			// filterData.word1 = CPxManager::GROUP1;
			shape->setQueryFilterData(filterData);
			//filterData.word0 = CPxManager::FilterGroup::eCC; // word0 = own ID
			//filterData.word1 = CPxManager::FilterGroup::eRAGDOLL; // word0 = own ID
			//shape->setSimulationFilterData(filterData);
			//shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			//shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			return;
		}

		XMVECTOR quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pObjTransform->GetMatrix()));
		memcpy(&transform.q, &quat, sizeof(_float4));

		/* TODO: take half height?? */
		PxShape* meshShape = m_pEngine->GetPhysics()->createShape(PxCapsuleGeometry(radius, height), *pMaterial);
		/* Fix to upright standing capsule */
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		relativePose.p.x += m_vRelativePos.x;
		relativePose.p.y += m_vRelativePos.y;
		relativePose.p.z += m_vRelativePos.z;
		meshShape->setLocalPose(relativePose);

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

void CCapsuleCollider::SetSize(pair<float, float> size)
{
	if (m_fRadius != size.first || m_fHeight != size.second)
	{
		m_fRadius = size.first;
		m_fHeight = size.second;

		dynamic_cast<CVIBuffer_LineCapsule*>(m_pDebugLine)->SetSize(m_fRadius, m_fHeight);
	}
}

