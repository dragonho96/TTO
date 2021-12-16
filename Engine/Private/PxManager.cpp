#include "..\Public\PxManager.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

IMPLEMENT_SINGLETON(CPxManager)


PxFilterFlags myFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//let triggers through
	//if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	//{
	//	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	//	return PxFilterFlag::eDEFAULT;
	//}

	//generate contacts for all that were not filtered above
	//pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	//trigger the contact callBack for pairs (A, B) where
	//the filtermask of A contains the ID of B and vice versa
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;
		return PxFilterFlag::eDEFAULT;
	}
	else
	{
		return PxFilterFlag::eDEFAULT;
	}

	// return PxFilterFlag::eDEFAULT;


	//bool s0 = PxGetFilterObjectType(attributes0) == PxFilterObjectType::eARTICULATION;
	//bool s1 = PxGetFilterObjectType(attributes1) == PxFilterObjectType::eRIGID_STATIC;

	//if (filterData0.word0 == PxFilterObjectFlag::eKINEMATIC && filterData1.word0 == PxFilterObjectFlag::eKINEMATIC)
	//	return PxFilterFlag::eSUPPRESS;	//NOTE: Waiting on physx fix for refiltering on aggregates. For now use supress which automatically tests when changes to simulation happen

		// Find out which channels the objects are in

	// ignore kinematic-kinematic interactions which don't involve a destructible
	//if (k0 && k1)
	//{
	//	return PxFilterFlag::eSUPPRESS;	//NOTE: Waiting on physx fix for refiltering on aggregates. For now use supress which automatically tests when changes to simulation happen
	//}

	return PxFilterFlag::eDEFAULT;
}


CPxManager::CPxManager()
{
}

void CPxManager::Free()
{
	if (!m_pScene)
		return;

	m_pScene->flushQueryUpdates();
	SafeDelete(m_pMyCallback);
	PX_RELEASE(m_pMaterial);
	PX_RELEASE(m_pCooking);
	PX_RELEASE(m_pControllerManager);
	// PX_RELEASE(m_pAggregate);
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysics);
	if (m_pPvd)
	{
		PxPvdTransport* transport = m_pPvd->getTransport();
		m_pPvd->release();	m_pPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_pFoundation);
}

HRESULT CPxManager::Initialize()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	PxTolerancesScale scale;
	scale.length = 1;        // typical length of an object
	scale.speed = 9.81f;         // typical speed of an object, gravity*1s is a reasonable choice

#ifdef _DEBUG
	m_pPvd = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	// bool status = m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	bool status = m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true, m_pPvd);
	PxInitExtensions(*m_pPhysics, m_pPvd);
#else
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true, nullptr);
	PxInitExtensions(*m_pPhysics, nullptr);
#endif

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(scale));


	//m_pPvd = PxCreatePvd(*m_pFoundation);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//// bool status = m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	//bool status = m_pPvd->connect(*transport, PxPvdInstrumentationFlag::ePROFILE);
	//m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true, m_pPvd);
	//PxInitExtensions(*m_pPhysics, m_pPvd);

	////m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true);
	//	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(scale));

	//if (!mCooking)
	//	throw("PxCreateCooking failed!");


	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;

	// sceneDesc.filterShader = myFilterShader;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_pScene = m_pPhysics->createScene(sceneDesc);

	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCULL_BOX, 2.0f);



	//m_pAggregate = m_pPhysics->createAggregate(PxU32(128), false);
	//m_pScene->addAggregate(*m_pAggregate);

	m_pMyCallback = new MyCollisionCallBack();
	m_pScene->setSimulationEventCallback(m_pMyCallback);
	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 10.f, 0.6f);
	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0), *m_pMaterial);
	groundPlane->setActorFlag(PxActorFlag::eVISUALIZATION, TRUE);


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

void CPxManager::AddActor(PxRigidActor * pActor)
{
	if (nullptr == pActor)
	{
		MSG_BOX("Failed To Creating CSphereCollider");
		return;
	}

	m_pScene->addActor(*pActor);
}

void CPxManager::AddAggregateActor(PxRigidActor * pActor)
{
	if (nullptr == pActor)
	{
		MSG_BOX("Failed To Creating CSphereCollider");
		return;
	}

	// m_pAggregate->addActor(*pActor);
}

_bool CPxManager::Raycast(_vector origin, _vector unitDir, _float maxDistance, PxRaycastBuffer & hit)
{
	PxVec3 pxOrigin;
	PxVec3 pxUnitDir;
	PxReal pxMaxDistance = maxDistance;

	memcpy(&pxOrigin, &origin, sizeof(PxVec3));
	memcpy(&pxUnitDir, &unitDir, sizeof(PxVec3));

	PxQueryFilterData filterData;
	filterData.data.word0 = CPxManager::GROUP1;
	// filterData.data.word1 = CPxManager::GROUP2;
	_bool result = m_pScene->raycast(pxOrigin, pxUnitDir, maxDistance, hit, PxHitFlag::eDEFAULT, filterData);
	return result;
}

