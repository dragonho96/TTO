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
private:
	CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
};

END