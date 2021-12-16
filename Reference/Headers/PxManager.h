#pragma once
#include "Base.h"

BEGIN(Engine)
class MyCollisionCallBack : public PxSimulationEventCallback
{
public:

	std::atomic_int SafeTrigger;
	MyCollisionCallBack() { SafeTrigger = 0; }
	PxRigidActor* triggerActor;

	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];
			//only interested in touches found and lost
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				SafeTrigger = 1;
			}
		}
	}
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs) {};
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {};
	virtual void onWake(PxActor**, PxU32) {};
	virtual void onSleep(PxActor**, PxU32) {};

	virtual void onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {};

};
class CPxManager : public CBase
{
	DECLARE_SINGLETON(CPxManager)

public:
	enum ActiveGroup
	{
		GROUP1 = (1 << 0),
		GROUP2 = (1 << 1),
		GROUP3 = (1 << 2),
		GROUP4 = (1 << 3),
	};
public:
	struct FilterGroup
	{
		enum Enum
		{
			eCC = (1 << 0),
			eRAGDOLL = (1 << 1),
			//eMINE_LINK = (1 << 2),
			//eCRAB = (1 << 3),
			//eHEIGHTFIELD = (1 << 4),
		};
	};

private:
	CPxManager();
	virtual ~CPxManager() = default;
	virtual void Free() override;

public:
	HRESULT Initialize();
	void Update(_double DeltaTime);

public:
	PxPhysics*				GetPhysics() { return m_pPhysics; }
	PxMaterial*				GetMaterial() { return m_pMaterial; }
	PxControllerManager*	GetControllerManager() { return m_pControllerManager; }
	void					AddActor(PxRigidActor* pActor);
	void					AddAggregateActor(PxRigidActor* pActor);
	PxScene*				GetScene() { return m_pScene; }
	PxCooking*				GetCooking() { return m_pCooking; }

	void					OnContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs);

private:
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;
	PxFoundation*			m_pFoundation = NULL;
	PxPhysics*				m_pPhysics = NULL;
	PxDefaultCpuDispatcher*	m_pDispatcher = NULL;
	PxScene*				m_pScene = NULL;
	PxMaterial*				m_pMaterial = NULL;
	PxPvd*                  m_pPvd = NULL;
	PxCooking*				m_pCooking = NULL;
	PxControllerManager*	m_pControllerManager = NULL;
	// PxAggregate*			m_pAggregate = NULL;
	MyCollisionCallBack*	m_pMyCallback = NULL;
};


END