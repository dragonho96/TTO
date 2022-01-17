#include "stdafx.h"
#include "..\Public\EquipmentPool.h"
#include "EquipmentSerializer.h"

IMPLEMENT_SINGLETON(CEquipmentPool)

CEquipmentPool::CEquipmentPool()
{
	SetUpEquipmentPool();
	SetUpModel();
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
	serializer.Deserialize("../../Assets/Equipments/PrimaryMagazine.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::PRIMARYMAG]);
	serializer.Deserialize("../../Assets/Equipments/SecondaryWeapon.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::SECONDARY]);
	serializer.Deserialize("../../Assets/Equipments/SecondaryMagazine.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::SECONDARYMAG]);
	serializer.Deserialize("../../Assets/Equipments/Grenade.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::GRENADE]);
	serializer.Deserialize("../../Assets/Equipments/Tool.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::TOOL]);
	
	serializer.Deserialize("../../Assets/Equipments/Headgear.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::HEADGEAR]);
	serializer.Deserialize("../../Assets/Equipments/Torso.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::TORSO]);
	serializer.Deserialize("../../Assets/Equipments/Legs.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::LEGS]);
	serializer.Deserialize("../../Assets/Equipments/Vest.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::VEST]);
	serializer.Deserialize("../../Assets/Equipments/Backpack.yaml", &m_EquipmentPool[(_uint)EQUIPMENT::BACKPACK]);
}

void CEquipmentPool::SetUpModel()
{
	// Primary Weapon
	CGameObject* pGameObject = CEngine::GetInstance()->SpawnPrefab("AKM");
	AssignModel(EQUIPMENT::PRIMARY, 0, pGameObject);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("M4A1");
	//AssignModel(EQUIPMENT::PRIMARY, 1, pGameObject);
	//pGameObject->SetActive(false);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("M24");
	//AssignModel(EQUIPMENT::PRIMARY, 2, pGameObject);
	//pGameObject->SetActive(false);

	//// Secondary Weapon
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("M9");
	//AssignModel(EQUIPMENT::SECONDARY, 0, pGameObject);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("M1911");
	//AssignModel(EQUIPMENT::SECONDARY, 1, pGameObject);
	//pGameObject->SetActive(false);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("G18");
	//AssignModel(EQUIPMENT::SECONDARY, 2, pGameObject);
	//pGameObject->SetActive(false);

	// GrenadeFrag
	pGameObject = CEngine::GetInstance()->SpawnPrefab("Frag");
	AssignModel(EQUIPMENT::GRENADE, 0, pGameObject);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("SmokeShell");
	//AssignModel(EQUIPMENT::GRENADE, 1, pGameObject);
	//pGameObject->SetActive(false); 
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("Stun");
	//AssignModel(EQUIPMENT::GRENADE, 2, pGameObject);
	//pGameObject->SetActive(false);

	// Tool
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("MedKit");
	//AssignModel(EQUIPMENT::TOOL, 0, pGameObject);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("Claymore");
	//AssignModel(EQUIPMENT::TOOL, 1, pGameObject);
	//pGameObject->SetActive(false);
	//pGameObject = CEngine::GetInstance()->SpawnPrefab("WireCutter");
	//AssignModel(EQUIPMENT::TOOL, 2, pGameObject);
	//pGameObject->SetActive(false);
}

BASEEQUIPDESC * CEquipmentPool::GetEquipment(EQUIPMENT eType, _uint iIndex)
{
	return m_EquipmentPool[(_uint)eType][iIndex];
}

BASEEQUIPDESC * CEquipmentPool::FindMagazine(BASEEQUIPDESC * weaponDesc, EQUIPMENT type)
{
	string name = weaponDesc->name;
	auto iter = find_if(m_EquipmentPool[(size_t)type].begin(), m_EquipmentPool[(size_t)type].end(), [&](BASEEQUIPDESC* magazine) {
		if (magazine->name.find(name) != string::npos)
			return true;
		
		return false;
	});
	
	if (iter != m_EquipmentPool[(size_t)type].end())
		return *iter;

	return nullptr;
}

void CEquipmentPool::AssignMeshContainer(EQUIPMENT eType, _uint iIndex, CMeshContainer * pMeshContainer)
{
	m_EquipmentPool[(size_t)eType][iIndex]->mesh = pMeshContainer;
}

void CEquipmentPool::AssignModel(EQUIPMENT eType, _uint iIndex, CGameObject * pGameObject)
{
	m_EquipmentPool[(size_t)eType][iIndex]->model = pGameObject;
}
