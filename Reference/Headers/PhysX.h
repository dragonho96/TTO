#pragma once
#include "Base.h"

BEGIN(Engine)
class CPhysX : public CBase
{
	DECLARE_SINGLETON(CPhysX)

private:
	CPhysX();
	virtual ~CPhysX() = default;
	virtual void Free() override;

public:
	HRESULT Initialize();
	void Update(_double DeltaTime);

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
	PxControllerManager*	m_pControlManager = NULL;

};

END