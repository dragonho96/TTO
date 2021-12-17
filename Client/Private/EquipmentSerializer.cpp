#include "stdafx.h"
#include "Client_Defines.h"
#include "Engine.h"
#include "EquipmentSerializer.h"

USING(Client)

CEquipmentSerializer::CEquipmentSerializer()
	: m_pEngine(CEngine::GetInstance())
{
	m_pDevice = m_pEngine->GetDevice();
	m_pDeviceContext = m_pEngine->GetDeviceContext();
}

bool CEquipmentSerializer::Deserialize(const string & filePath, list<BASEEQUIPDESC*>* listDesc)
{
	std::ifstream stream(filePath);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["EquipmentType"])
		return false;

	string equipmentType = data["EquipmentType"].as<string>();

	EQUIPMENTTYPE eType;
	if (equipmentType == "PrimaryWeapon" || equipmentType == "SecondaryWeapon")
		eType = EQUIPMENTTYPE::WEAPON;
	else if (equipmentType == "Grenade" || equipmentType == "Tool")
		eType = EQUIPMENTTYPE::BASE;
	else
		eType = EQUIPMENTTYPE::GEAR;


	auto equipments = data["Equipments"];
	if (equipments)
	{
		for (auto equipment : equipments)
		{
			switch (eType)
			{
			case Client::CEquipmentSerializer::EQUIPMENTTYPE::BASE:
				listDesc->emplace_back(DeserializeBase(equipment));
				break;
			case Client::CEquipmentSerializer::EQUIPMENTTYPE::WEAPON:
				listDesc->emplace_back(DeserializeWeapon(equipment));
				break;
			case Client::CEquipmentSerializer::EQUIPMENTTYPE::GEAR:
				listDesc->emplace_back(DeserializeGear(equipment));
				break;
			default:
				break;
			}
		}
	}

	return true;
}

BASEEQUIPDESC* CEquipmentSerializer::DeserializeBase(YAML::Node & node, BASEEQUIPDESC* _pDesc)
{
	BASEEQUIPDESC* pDesc = (_pDesc) ? _pDesc : new BASEEQUIPDESC();

	pDesc->name = node["Name"].as<string>();
	pDesc->background = node["Background"].as<string>();
	pDesc->weight = node["Weight"].as<_float>();

	if (node["Cost"])
		pDesc->cost = node["Cost"].as<_uint>();

	if (node["SlotSize"])
	{
		auto sequence = node["SlotSize"];
		pDesc->slotX = sequence[0].as<_uint>();
		pDesc->slotY = sequence[1].as<_uint>();
	}

	return pDesc;
}

BASEEQUIPDESC* CEquipmentSerializer::DeserializeWeapon(YAML::Node & node)
{
	WEAPONDESC* pDesc = new WEAPONDESC();
	DeserializeBase(node, pDesc);
	pDesc->rpm = node["Rpm"].as<_float>();
	pDesc->rpm = node["Rpm"].as<_float>();
	return pDesc;
}

BASEEQUIPDESC* CEquipmentSerializer::DeserializeGear(YAML::Node & node)
{
	GEARDESC* pDesc = new GEARDESC();
	DeserializeBase(node, pDesc);
	pDesc->mobility = node["Mobility"].as<_float>();
	pDesc->dexterity = node["Dexterity"].as<_float>();

	if (node["Inventory"])
	{
		auto inventory = node["Inventory"];
		_uint count = inventory.size();
		for (_uint i = 0; i < count; ++i)
		{
			auto sequence = inventory[i];
			_uint slotCount = sequence[2].as<_uint>();
			for (_uint j = 0; j < slotCount; ++j)
			{
				_float2 invenSize;
				invenSize.x = sequence[0].as<_uint>();
				invenSize.y = sequence[1].as<_uint>();
				pDesc->inventories.emplace_back(invenSize);
			}
		}
	}

	return pDesc;
}


