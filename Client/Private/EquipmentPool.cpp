#include "stdafx.h"
#include "..\Public\EquipmentPool.h"
#include "EquipmentSerializer.h"

IMPLEMENT_SINGLETON(CEquipmentPool)

CEquipmentPool::CEquipmentPool()
{
	SetUpEquipmentPool();
}

void CEquipmentPool::Free()
{
	for (auto& pool : m_EquipmentPool)
	{
		for (auto& desc : pool)
			SafeDelete(desc);

		pool.clear();
	}
	m_EquipmentPool.clear();
}

void CEquipmentPool::SetUpEquipmentPool()
{
	m_EquipmentPool.resize((size_t)EQUIPMENT::NONE);
	CEquipmentSerializer serializer;

	serializer.Deserialize("../../Assets/Equipments/PrimaryWeapon.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::PRIMARY]);
	serializer.Deserialize("../../Assets/Equipments/SecondaryWeapon.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::SECONDARY]);
	serializer.Deserialize("../../Assets/Equipments/Grenade.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::GRENADE]);
	serializer.Deserialize("../../Assets/Equipments/Tool.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::TOOL]);
	serializer.Deserialize("../../Assets/Equipments/Headgear.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::HEADGEAR]);
	serializer.Deserialize("../../Assets/Equipments/Torso.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::TORSO]);
	serializer.Deserialize("../../Assets/Equipments/Legs.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::LEGS]);
	serializer.Deserialize("../../Assets/Equipments/Vest.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::VEST]);
	serializer.Deserialize("../../Assets/Equipments/Backpack.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::BACKPACK]);

	GEARDESC* gear = (GEARDESC*)m_EquipmentPool[(_uint)EQUIPMENT::LEGS].front();
	int i = 0;

	// Read YAML File
	// Deserialize Equipment
	// Save in list


}
