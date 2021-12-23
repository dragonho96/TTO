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

public:
	BASEEQUIPDESC* GetEquipment(EQUIPMENT eType, _uint iIndex = 0);
	BASEEQUIPDESC* FindMagazine(BASEEQUIPDESC* weaponDesc, EQUIPMENT type);
private:
	vector<vector<BASEEQUIPDESC*>> m_EquipmentPool;
};

END