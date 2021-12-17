#pragma once
#include "Client_Defines.h"
BEGIN(Client)
class CEquipmentSerializer
{
public:
	enum class EQUIPMENTTYPE { BASE, WEAPON, GEAR };
public:
	CEquipmentSerializer();

public:
	bool Deserialize(const string& filePath, list<BASEEQUIPDESC*>* listDesc);

	BASEEQUIPDESC* DeserializeBase(YAML::Node& node, BASEEQUIPDESC* _pDesc = nullptr);
	BASEEQUIPDESC* DeserializeWeapon(YAML::Node& node);
	BASEEQUIPDESC* DeserializeGear(YAML::Node& node);
private:
	CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
};

END