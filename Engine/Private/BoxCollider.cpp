#include "..\Public\BoxCollider.h"
#include "Transform.h"
#include "PxManager.h"

CBoxCollider::CBoxCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCollider(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
	, m_Size(1.f, 1.f, 1.f)
{

}

CBoxCollider::CBoxCollider(const CBoxCollider & rhs)
	: CCollider(rhs)
	, m_pEngine(CEngine::GetInstance())
	, m_Size(1.f, 1.f, 1.f)
{

}


CBoxCollider * CBoxCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBoxCollider*	pInstance = new CBoxCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CBoxCollider");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CBoxCollider::Clone(void * pArg)
{
	CComponent*	pInstance = new CBoxCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CBoxCollider");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CBoxCollider::Free()
{
	__super::Free();
}

HRESULT CBoxCollider::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxCollider::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpDebugLine()))
		return E_FAIL;

	return S_OK;
}


HRESULT CBoxCollider::SetUpDebugLine(/* SIZE DESC */)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	m_pDebugLine = pEngine->CloneComponent(0, "Prototype_VIBuffer_LineBox", m_pObjTransform);
	if (nullptr == m_pDebugLine)
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CBoxCollider::SetUpRigidActor(void* pShapeInfo, RIGIDBODYDESC desc)
{
	m_RigidBodyDesc = desc;
	//PxRigidDynamic* aSphereActor = thePhysics->createRigidDynamic(PxTransform(position));
	//PxTransform relativePose(PxQuat(PxHalfPi, PxVec(0, 0, 1)));
	//PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*aSphereActor,
	//	PxCapsuleGeometry(radius, halfHeight), aMaterial);
	//aCapsuleShape->setLocalPose(relativePose);
	//PxRigidBodyExt::updateMassAndInertia(*aSphereActor, capsuleDensity);
	//aScene->addActor(aCapsuleActor);

	//physx::PxRigidStatic* sphere = physx::PxCreateStatic(*phy.getPhysics(), global_tf, physx::PxSphereGeometry(physx::PxReal(0.5)), *phy.getMaterial());
	
	// TODO: Add Static and Dynamic RigidBody
	// Create Shape with input size
	// 1. Check if it has RB
	// 2. No RB? -> RigidStatic
	// 3. Yes RB? -> DynamicStatic

	// TODO: Add Static RB First ( if there is no rb)

	memcpy(&m_Size, pShapeInfo, sizeof(_float3));
	_vector position = m_pObjTransform->GetState(CTransform::STATE_POSITION);
	PxTransform transform;
	memcpy(&transform.p, &position, sizeof(_float3));
	XMVECTOR quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pObjTransform->GetMatrix()));
	memcpy(&transform.q, &quat, sizeof(_float4));
	PxVec3 geoSize;
	memcpy(&geoSize, &m_Size, sizeof(_float3));
	geoSize = geoSize / 2;
	PxMaterial* pMaterial = m_pEngine->GetMaterial();
	PxShape* meshShape = m_pEngine->GetPhysics()->createShape(PxBoxGeometry(geoSize), *pMaterial);
	PxFilterData filterData;
	filterData.word0 = CPxManager::GROUP2;
	meshShape->setQueryFilterData(filterData);
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

	m_pRigidActor->userData = this;
	m_pRigidActor->attachShape(*meshShape);
	m_pEngine->AddActor(m_pRigidActor);

	






	//PxTransform transform(PxVec3(), PxQuat());
	//PxShape* pShape = m_pEngine->GetPhysics()->createShape( PxSphereGeometry(1.f), *pMaterial, true);

	//PxRigidDynamic* pActor = m_pEngine->GetPhysics()->createRigidDynamic(PxTransform(PxVec3(0, 0, 0)));
	//pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	//pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//pActor->attachShape(*pShape);

	//m_pEngine->AddActor(pActor);
	//pActor->Setglo
}

void CBoxCollider::SetSize(_float3 vSize)
{
	if (vSize.x != m_Size.x || vSize.y != m_Size.y || vSize.z != m_Size.z)
	{
		m_Size = vSize;
		dynamic_cast<CVIBuffer_LineBox*>(m_pDebugLine)->SetSize(m_Size);
	}
}

