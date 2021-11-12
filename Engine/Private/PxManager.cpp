#include "..\Public\PxManager.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

IMPLEMENT_SINGLETON(CPxManager)

CPxManager::CPxManager()
{
}

void CPxManager::Free()
{
	PX_RELEASE(m_pMaterial);
	PX_RELEASE(m_pCooking);
	PX_RELEASE(m_pControllerManager);
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysics);
	//if (m_pPvd)
	//{
	//	PxPvdTransport* transport = m_pPvd->getTransport();
	//	m_pPvd->release();	m_pPvd = NULL;
	//	PX_RELEASE(transport);
	//}
	PX_RELEASE(m_pFoundation);
}

HRESULT CPxManager::Initialize()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	//m_pPvd = PxCreatePvd(*m_pFoundation);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	PxTolerancesScale scale;
	scale.length = 1000;        // typical length of an object
	scale.speed = 9.81f;         // typical speed of an object, gravity*1s is a reasonable choice

	//m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true, m_pPvd);
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true);

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(scale));
	//if (!mCooking)
	//	throw("PxCreateCooking failed!");

	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -10.f, 0.0f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_pScene = m_pPhysics->createScene(sceneDesc);
	
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 100.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eCULL_BOX, 2.0f);

	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 10.f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0), *m_pMaterial);
	//groundPlane->setActorFlag(PxActorFlag::eVISUALIZATION, TRUE);
	m_pScene->addActor(*groundPlane);
	

	m_pControllerManager = PxCreateControllerManager(*m_pScene);
	return S_OK;
}

void CPxManager::Update(_double DeltaTime)
{
	if (m_pScene)
	{
		m_pScene->simulate((physx::PxReal)DeltaTime);
		m_pScene->fetchResults(true);
	}
}
