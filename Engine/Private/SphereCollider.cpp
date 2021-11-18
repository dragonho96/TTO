#include "..\Public\SphereCollider.h"
#include "Transform.h"


CSphereCollider::CSphereCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
{

}

CSphereCollider::CSphereCollider(const CSphereCollider & rhs)
	: CComponent(rhs)
	, m_pEngine(CEngine::GetInstance())
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

CSphereCollider * CSphereCollider::Clone(void * pArg)
{
	CSphereCollider*	pInstance = new CSphereCollider(*this);

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

	if(m_isCloned)
		SafeRelease(m_pDebugLineSphere);
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

	if (pArg)
		m_pObjTransform = (CTransform*)pArg;

	if (FAILED(SetUpDebugLine()))
		return E_FAIL;

	SetUpPhysX();

	return S_OK;
}

void CSphereCollider::Render()
{
	if (m_pDebugLineSphere)
		dynamic_cast<CVIBuffer*>(m_pDebugLineSphere)->Render();
}

HRESULT CSphereCollider::SetUpDebugLine(/* SIZE DESC */)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	m_pDebugLineSphere = pEngine->CloneComponent(0, TEXT("Prototype_VIBuffer_LineSphere"), m_pObjTransform);
	if (nullptr == m_pDebugLineSphere)
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CSphereCollider::SetUpPhysX()
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

void CSphereCollider::SetRadius(float fRadius)
{
	if (m_fRadius == fRadius)
		return;

	m_fRadius = fRadius;
	dynamic_cast<CVIBuffer_LineSphere*>(m_pDebugLineSphere)->SetRadius(m_fRadius);
}
