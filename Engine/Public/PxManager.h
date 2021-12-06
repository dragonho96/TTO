#pragma once
#include "Base.h"

BEGIN(Engine)
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
	PxScene*				GetScene() { return m_pScene; }
	PxCooking*				GetCooking() { return m_pCooking; }

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

};

END