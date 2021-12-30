#pragma once
#include "Engine.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEquipmentPool : public CBase
{
	DECLARE_SINGLETON(CEquipmentPool)
private:
	CEquipmentPool();
	virtual ~CEquipmentPool() = default;
public:
	virtual void Free();

public:
	void SetUpEquipmentPool();
	void SetUpModel();
public:
	BASEEQUIPDESC* GetEquipment(EQUIPMENT eType, _uint iIndex = 0);
	BASEEQUIPDESC* FindMagazine(BASEEQUIPDESC* weaponDesc, EQUIPMENT type);

public:
	void AssignMeshContainer(EQUIPMENT eType, _uint iIndex, CMeshContainer* pMeshContainer);
	void AssignModel(EQUIPMENT eType, _uint iIndex, CGameObject* pGameObject);
private:
	vector<vector<BASEEQUIPDESC*>> m_EquipmentPool;
};

END