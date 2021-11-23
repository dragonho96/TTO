#include "..\Public\SceneSerializer.h"
#include "EmptyGameObject.h"
#include "EmptyUI.h"

#pragma region UICOM
#include "RectTransform.h"
#include "VIBuffer_RectUI.h"
#pragma endregion

CSceneSerializer::CSceneSerializer()
	: m_pEngine(CEngine::GetInstance())
{
	m_pDevice = m_pEngine->GetDevice();
	m_pDeviceContext = m_pEngine->GetDeviceContext();
}
static void SerializeObject(YAML::Emitter& out, CGameObject* obj)
{

}
static void SerializeUI(YAML::Emitter& out, CGameObject* obj)
{
	out << YAML::BeginMap;

	out << YAML::Key << "Type" << YAML::Value << "UI";
	out << YAML::Key << "UUID" << YAML::Value << obj->GetUUID();

	if (obj->GetComponent(TEXT("Com_Transform")))
	{
		CRectTransform* rectTransform = dynamic_cast<CRectTransform*>(obj->GetComponent(TEXT("Com_Transform")));

		out << YAML::Key << "Com_Transform";
		out << YAML::BeginMap;

		CRectTransform::RECTTRANSFORMDESC desc = rectTransform->GetTransformDesc();
		out << YAML::Key << "Position";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << desc.posX << desc.posY << YAML::EndSeq;

		out << YAML::Key << "Size";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << desc.sizeX << desc.sizeY << YAML::EndSeq;

		out << YAML::EndMap;
	}

	if (obj->GetComponent(TEXT("Com_VIBuffer")))
	{
		CVIBuffer_RectUI* buffer = dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent(TEXT("Com_VIBuffer")));

		out << YAML::Key << "Com_VIBuffer";
		out << YAML::BeginMap;

		out << YAML::Key << "TexturePath" << YAML::Value << buffer->GetTextureFilePath();

		_float4 color = buffer->GetColor();
		out << YAML::Key << "Color";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << color.x << color.y << color.z << color.z << YAML::EndSeq;

		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}
static void SerializeGameObject(YAML::Emitter& out, CGameObject* obj)
{
	if (dynamic_cast<CEmptyGameObject*>(obj))
		SerializeObject(out, obj);
	else
		SerializeUI(out, obj);
}

void CSceneSerializer::Serialize(const string & filePath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

	list<CGameObject*> objList = m_pEngine->GetGameObjectInLayer(0, TEXT("LAYER_TOOL"));
	for (auto& obj : objList)
	{
		SerializeGameObject(out, obj);
	}
	// Save GameObjects

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filePath);
	fout << out.c_str();
}

bool CSceneSerializer::Deserialize(const string & filePath)
{
	std::ifstream stream(filePath);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Scene"])
		return false;

	string SceneName = data["Scene"].as<string>();

	auto gameObjects = data["GameObjects"];
	if (gameObjects)
	{
		for (auto obj : gameObjects)
		{
			if (obj["Type"].as<string>() == "UI")
			{
				CGameObject* deserializedObject = m_pEngine->AddGameObject(0, TEXT("Prototype_EmptyUI"), TEXT("LAYER_TEST"));
				auto transformCom = obj["Com_Transform"];
				if (transformCom)
				{
					CRectTransform::RECTTRANSFORMDESC desc;

					auto sequence = transformCom["Position"];
					desc.posX = sequence[0].as<float>();
					desc.posY = sequence[1].as<float>();
					sequence = transformCom["Size"];
					desc.sizeX = sequence[0].as<float>();
					desc.sizeY = sequence[1].as<float>();

					dynamic_cast<CEmptyUI*>(deserializedObject)->SetRectTransform(desc);
				}

			}
		}
	}

	return true;
}
