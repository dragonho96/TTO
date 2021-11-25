#include "..\Public\CapsuleCollider.h"
#include "Transform.h"


CCapsuleCollider::CCapsuleCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCollider(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
{

}

CCapsuleCollider::CCapsuleCollider(const CCapsuleCollider & rhs)
	: CCollider(rhs)
	, m_pEngine(CEngine::GetInstance())
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
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	m_pDebugLine = pEngine->CloneComponent(0, TEXT("Prototype_VIBuffer_LineCapsule"), m_pObjTransform);
	if (nullptr == m_pDebugLine)
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CCapsuleCollider::SetUpRigidActor(void* pShapeInfo, RIGIDBODYDESC desc)
{
	m_RigidBodyDesc = desc;
	pair<float, float> capsuleSize;
	memcpy(&capsuleSize, pShapeInfo, sizeof(pair<float, float>));
	PxReal radius = capsuleSize.first;
	PxReal height = capsuleSize.second;
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
		//desc.behaviorCallback = 
		//desc.reportCallback = &m_callback;
		//CE_ASSERT(desc.isValid(), "Capsule is not valid");

		m_pController = CEngine::GetInstance()->GetControllerManager()->createController(desc);
		return;
	}




	XMVECTOR quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pObjTransform->GetMatrix()));
	memcpy(&transform.q, &quat, sizeof(_float4));

	PxShape* meshShape = m_pEngine->GetPhysics()->createShape(PxCapsuleGeometry(radius, height), *pMaterial);
	/* Fix to upright standing capsule */
	PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
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

	m_pRigidActor->attachShape(*meshShape);
	m_pEngine->AddActor(m_pRigidActor);



	//PxTransform transform(PxVec3(), PxQuat());
	//PxMaterial* pMaterial = m_pEngine->GetMaterial();
	//PxShape* pShape = m_pEngine->GetPhysics()->createShape( PxSphereGeometry(1.f), *pMaterial, true);

	//PxRigidDynamic* pActor = m_pEngine->GetPhysics()->createRigidDynamic(PxTransform(PxVec3(0, 0, 0)));
	//pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	//pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//pActor->attachShape(*pShape);

	//m_pEngine->AddActor(pActor);
	//pActor->Setglo
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

