#pragma once
#include "Engine.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEquipmentPool : public CBase
{
	DECLARE_SINGLETON(CEquipmentPool)
public:
	enum class  EQUIPMENT : int { PRIMARY, SECONDARY, GRENADE, TOOL, HEADGEAR, TORSO, LEGS, VEST, BACKPACK, NONE};
private:
	CEquipmentPool();
	virtual ~CEquipmentPool() = default;
public:
	virtual void Free();

public:
	void SetUpEquipmentPool();

private:
	// primaryWeapon
	// secondaryWeapon
	// Grenade
	// Tool
	vector<list<BASEEQUIPDESC*>> m_EquipmentPool;

	//list<WEAPONDESC*> m_Primary;
	//list<WEAPONDESC*> m_Secondary;
	//list<BASEEQUIPDESC*> m_Grenade;
	//list<BASEEQUIPDESC*> m_Tool;

	//list<GEARDESC*> m_Head;
	//list<GEARDESC*> m_Shirt;
	//list<GEARDESC*> m_Assult;
	//list<GEARDESC*> m_Backpack;
};

END