#include "stdafx.h"
#include "..\Public\Equipment.h"

USING(Client)

CEquipment::CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CEquipment::CEquipment(const CEquipment & rhs)
	: CComponent(rhs)
{
}

HRESULT CEquipment::InitializePrototype()
{

	return S_OK;
}

HRESULT CEquipment::Initialize(void * pArg)
{
	return S_OK;
}

CEquipment * CEquipment::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEquipment*	pInstance = new CEquipment(pDevice, pDeviceContext);
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Create CEquipment");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CEquipment::Clone(void * pArg)
{
	CEquipment*	pInstance = new CEquipment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create CEquipment");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CEquipment::Free()
{
	__super::Free();
}
