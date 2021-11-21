#include "..\Public\BoxCollider.h"
#include "Transform.h"


CBoxCollider::CBoxCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCollider(pDevice, pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
	, m_vSize(1.f, 1.f, 1.f)
{

}

CBoxCollider::CBoxCollider(const CBoxCollider & rhs)
	: CCollider(rhs)
	, m_pEngine(CEngine::GetInstance())
	, m_vSize(1.f, 1.f, 1.f)
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

	m_pDebugLine = pEngine->CloneComponent(0, TEXT("Prototype_VIBuffer_LineBox"), m_pObjTransform);
	if (nullptr == m_pDebugLine)
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

void CBoxCollider::SetUpPhysX()
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

void CBoxCollider::SetSize(_float3 vSize)
{
	if (vSize.x != m_vSize.x || vSize.y != m_vSize.y || vSize.z != m_vSize.z)
	{
		m_vSize = vSize;
		dynamic_cast<CVIBuffer_LineBox*>(m_pDebugLine)->SetSize(m_vSize);
	}
}

