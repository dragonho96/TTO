#pragma once
#include "Base.h"

BEGIN(Engine)
class CPxManager : public CBase
{
	DECLARE_SINGLETON(CPxManager)

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