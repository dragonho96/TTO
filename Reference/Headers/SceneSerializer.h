#pragma once
//#include "Engine_Defines.h"
#include "Engine.h"

BEGIN(Engine)

class ENGINE_DLL CSceneSerializer
{
public:
	// TODO: Constructor must take a scene as parameter
	CSceneSerializer();

public:
	void Serialize(const string& filePath);
	bool Deserialize(const string& filePath);
	void SerializeObject(YAML::Emitter& out, CGameObject* obj);
	void SerializeUI(YAML::Emitter& out, CGameObject* obj);

	void SerializePrefab(CGameObject* obj);
	void DeserializePrefab();
	CGameObject* SpawnPrefab(YAML::Node data);

	CGameObject* DeserializeUI(YAML::Node& obj, _bool bSpawn = false);
	CGameObject* DeserializeObject(YAML::Node& obj, _bool bSpawn = false);

private:
	CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

};

END