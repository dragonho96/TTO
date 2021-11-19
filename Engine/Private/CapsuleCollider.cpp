#include "..\Public\CapsuleCollider.h"
#include "Transform.h"


CCapsuleCollider::CCapsuleCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCollider(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
{

}

CCapsuleCollider::CCapsuleCollider(const CCapsuleCollider & rhs)
	: CCollider(rhs)
	, m_pEngine(CEngine::GetInstance())
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

void CCapsuleCollider::SetUpPhysX()
{
	//PxRigidDynamic* aSphereActor = thePhysics->createRigidDynamic(PxTransform(position));
	//PxTransform relativePose(PxQuat(PxHalfPi, PxVec(0, 0, 1)));
	//PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*aSphereActor,
	//	PxCapsuleGeometry(radius, halfHeight), aMaterial);
	//aCapsuleShape->setLocalPose(relativePose);
	//PxRigidBodyExt::updateMassAndInertia(*aSphereActor, capsuleDensity);
	//aScene->addActor(aCapsuleActor);


	PxTransform transform(PxVec3(), PxQuat());
	PxMaterial* pMaterial = m_pEngine->GetMaterial();
	PxShape* pShape = m_pEngine->GetPhysics()->createShape( PxSphereGeometry(1.f), *pMaterial, true);

	PxRigidDynamic* pActor = m_pEngine->GetPhysics()->createRigidDynamic(PxTransform(PxVec3(0, 0, 0)));
	pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	pActor->attachShape(*pShape);

	m_pEngine->AddActor(pActor);
	//pActor->Setglo
}

void CCapsuleCollider::SetSize(float fRadius, float fHeight)
{
	if (m_fRadius != fRadius || m_fHeight != fHeight)
	{
		m_fRadius = fRadius;
		m_fHeight = fHeight;

		dynamic_cast<CVIBuffer_LineCapsule*>(m_pDebugLine)->SetSize(m_fRadius, m_fHeight);
	}
}

