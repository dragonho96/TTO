#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CSceneSerializer
{
public:
	// TODO: Constructor must take a scene as parameter
	CSceneSerializer();

public:
	void Serialize(const string& filePath);
	bool Deserialize(const string& filePath);

private:
	// Scene;
};

END