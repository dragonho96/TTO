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

bool CEquipmentSerializer::Deserialize(const string & filePath, vector<BASEEQUIPDESC*>* vecDesc)
{
	std::ifstream stream(filePath);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["EquipmentType"])
		return false;

	string equipmentType = data["EquipmentType"].as<string>();

	EQUIPMENT eType;
	if (equipmentType == "PrimaryWeapon")
		eType = EQUIPMENT::PRIMARY;
	else if (equipmentType == "SecondaryWeapon")
		eType = EQUIPMENT::SECONDARY;
	else if (equipmentType == "PrimaryMagazine")
		eType = EQUIPMENT::PRIMARYMAG;
	else if (equipmentType == "SecondaryMagazine")
		eType = EQUIPMENT::SECONDARYMAG;
	else if (equipmentType == "Grenade")
		eType = EQUIPMENT::GRENADE;
	else if (equipmentType == "Tool")
		eType = EQUIPMENT::TOOL;
	else if (equipmentType == "Headgear")
		eType = EQUIPMENT::HEADGEAR;
	else if (equipmentType == "Torso")
		eType = EQUIPMENT::TORSO;
	else if (equipmentType == "Legs")
		eType = EQUIPMENT::LEGS;
	else if (equipmentType == "Vest")
		eType = EQUIPMENT::VEST;
	else if (equipmentType == "Backpack")
		eType = EQUIPMENT::BACKPACK;


	auto equipments = data["Equipments"];
	vecDesc->reserve((size_t)equipments.size());

	if (equipments)
	{
		for (auto equipment : equipments)
		{
			BASEEQUIPDESC* desc = nullptr;
			switch (eType)
			{
			case Client::EQUIPMENT::GRENADE:
			case Client::EQUIPMENT::TOOL:
				desc = DeserializeBase(equipment);
				break;
			case Client::EQUIPMENT::PRIMARYMAG:
			case Client::EQUIPMENT::SECONDARYMAG:
				desc = DeserializeMagazine(equipment);
				break;
			case Client::EQUIPMENT::PRIMARY:
			case Client::EQUIPMENT::SECONDARY:
				desc = DeserializeWeapon(equipment);
				break;
			case Client::EQUIPMENT::HEADGEAR:
			case Client::EQUIPMENT::TORSO:
			case Client::EQUIPMENT::LEGS:
			case Client::EQUIPMENT::VEST:
			case Client::EQUIPMENT::BACKPACK:
				desc = DeserializeGear(equipment);
				break;
			default:
				break;
			}
			desc->type = eType;
			vecDesc->emplace_back(desc);
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

	if (node["Texture"])
		pDesc->texturePath = node["Texture"].as<string>();

	if (node["Cost"])
		pDesc->cost = node["Cost"].as<_uint>();

	if (node["SlotSize"])
	{
		auto sequence = node["SlotSize"];
		pDesc->slotSize.x = sequence[0].as<_uint>();
		pDesc->slotSize.y = sequence[1].as<_uint>();
	}

	return pDesc;
}

BASEEQUIPDESC* CEquipmentSerializer::DeserializeWeapon(YAML::Node & node)
{
	WEAPONDESC* pDesc = new WEAPONDESC();
	DeserializeBase(node, pDesc);
	pDesc->rpm = node["Rpm"].as<_float>();
	pDesc->accuracy = node["Accuracy"].as<_float>();
	return pDesc;
}

BASEEQUIPDESC * CEquipmentSerializer::DeserializeMagazine(YAML::Node & node)
{
	MAGAZINEDESC* pDesc = new MAGAZINEDESC();
	DeserializeBase(node, pDesc);
	pDesc->magRound = node["Round"].as<_uint>();

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
				_uint2 invenSize;
				invenSize.x = sequence[0].as<_uint>();
				invenSize.y = sequence[1].as<_uint>();
				pDesc->inventories.emplace_back(invenSize);
			}
		}
	}

	return pDesc;
}


