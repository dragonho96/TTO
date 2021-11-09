#include "..\Public\SceneSerializer.h"



CSceneSerializer::CSceneSerializer()
{
}

//static void SerializeEntity(YAML::Emitter& out)
//{
//	out << YAML::BeginMap;
//	out << YAML::Key << "GameObject" << YAML::Value << "idk";
//	out << YAML::EndMap;
//}

void CSceneSerializer::Serialize(const string & filePath)
{
	//YAML::Emitter out;
	//out << YAML::BeginMap;
	//out << YAML::Key << "Scene";
	//out << YAML::Value << "Name";
	//out << YAML::Key << "Entities";
	//out << YAML::Value << YAML::BeginSeq;

	//// Save GameObjects
	//SerializeEntity(out);

	//out << YAML::EndSeq;
	//out << YAML::EndMap;

	//std::ofstream fout(filePath);
	//fout << out.c_str();
}

bool CSceneSerializer::Deserialize(const string & filePath)
{
	//std::ifstream stream(filePath);
	//std::stringstream strStream;
	//strStream << stream.rdbuf();

	//YAML::Node data = YAML::Load(strStream.str());
	//if (!data["Scene"])
	//	return false;

	//string SceneName = data["Scene"].as<string>();

	//auto entities = data["Entities"];
	//if (entities)
	//{
	//	for (auto entity : entities)
	//	{
	//		string value = entity["GameObject"].as<string>();
	//	}
	//}
}
