#include "..\Public\SceneSerializer.h"
#include "Engine.h"
#include "EmptyGameObject.h"
#include "EmptyUI.h"
#include "Layer.h"

#pragma region UICOM
#include "RectTransform.h"
#include "VIBuffer_RectUI.h"
#include "VIBuffer_Terrain.h"
#pragma endregion

#pragma region COMPONENT
#include "Collider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#pragma endregion

CSceneSerializer::CSceneSerializer()
	: m_pEngine(CEngine::GetInstance())
{
	m_pDevice = m_pEngine->GetDevice();
	m_pDeviceContext = m_pEngine->GetDeviceContext();
}

void CSceneSerializer::Serialize(const string & filePath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

	//list<CGameObject*> objList = m_pEngine->GetGameObjectInLayer(0, TEXT("LAYER_TOOL"));
	unordered_map<string, CLayer*>* layers = m_pEngine->GetLayers();
	for (auto& pair : *layers)
	{
		if (pair.first == "LAYER_TOOL")
			continue;

		list<CGameObject*> objList = (pair.second)->GetGameObjectList();
		for (auto& obj : objList)
		{
			out << YAML::BeginMap;

			out << YAML::Key << "Name" << YAML::Value << obj->GetName();
			out << YAML::Key << "UUID" << YAML::Value << obj->GetUUID();
			out << YAML::Key << "Active" << YAML::Value << obj->IsActive();
			out << YAML::Key << "Layer" << YAML::Value << obj->GetLayer();

			if (dynamic_cast<CEmptyGameObject*>(obj))
				SerializeObject(out, obj);
			else
				SerializeUI(out, obj);

			if (0 < obj->GetChildren().size())
			{
				out << YAML::Key << "Children";
				out << YAML::BeginSeq;

				list<CGameObject*> children = obj->GetChildren();
				for (auto& child : children)
					out << child->GetUUID();

				out << YAML::EndSeq;
			}

			out << YAML::EndMap;

		}
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
			CGameObject* deserializedObject = nullptr;

			if (obj["Type"].as<string>() == "UI")
				deserializedObject = DeserializeUI(obj);
			else
				deserializedObject = DeserializeObject(obj);


		}

		
	}
	if (gameObjects)
	{
		for (auto obj : gameObjects)
		{
			auto children = obj["Children"];
			if (children)
			{
				CGameObject* parent = m_pEngine->FindGameObjectWithUUID(obj["UUID"].as<uint64_t>());
				int seqSize = children.size();
				for (int i = 0; i < children.size(); ++i)
				{
					uint64_t uuid = children[i].as<uint64_t>();
					CGameObject* child = nullptr;
					if (child = m_pEngine->FindGameObjectWithUUID(uuid))
						parent->AddChild(child);
				}
			}
		}
	}
	return true;
}

void CSceneSerializer::SerializeObject(YAML::Emitter & out, CGameObject * obj)
{

	out << YAML::Key << "Type" << YAML::Value << "Object";

	if (obj->GetComponent("Com_Transform"))
	{


		CTransform* transform = dynamic_cast<CTransform*>(obj->GetComponent("Com_Transform"));

		out << YAML::Key << "Com_Transform";
		out << YAML::BeginMap;

		XMMATRIX matrix = XMLoadFloat4x4(&transform->GetMatrix());
		//XMVECTOR tr, rt, sc;
		//XMMatrixDecompose(&sc, &rt, &tr, matrix);

		float _objMat[16];
		memcpy(_objMat, &matrix, sizeof(XMMATRIX));
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(_objMat, matrixTranslation, matrixRotation, matrixScale);


		out << YAML::Key << "Translation";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << matrixTranslation[0] << matrixTranslation[1] << matrixTranslation[2] << YAML::EndSeq;

		out << YAML::Key << "Rotation";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << matrixRotation[0] << matrixRotation[1] << matrixRotation[2] << YAML::EndSeq;

		out << YAML::Key << "Scale";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << matrixScale[0] << matrixScale[1] << matrixScale[2] << YAML::EndSeq;


		out << YAML::EndMap;
	}

	if (obj->GetComponent("Com_Collider"))
	{
		CCollider* collider = dynamic_cast<CCollider*>(obj->GetComponent("Com_Collider"));

		// Check if RB exist
		out << YAML::Key << "Com_Collider";
		out << YAML::BeginMap;

		_float3 center = collider->GetRelativePos();
		out << YAML::Key << "Center";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << center.x << center.y << center.z << YAML::EndSeq;

		if (dynamic_cast<CBoxCollider*>(collider))
		{
			CBoxCollider* boxCollier = dynamic_cast<CBoxCollider*>(collider);
			_float3 boxSize = boxCollier->GetSize();

			out << YAML::Key << "Type" << YAML::Value << "Box";
			out << YAML::Key << "Size" << YAML::Value << YAML::Flow;
			out << YAML::BeginSeq << boxSize.x << boxSize.y << boxSize.z << YAML::EndSeq;
		}
		else if (dynamic_cast<CSphereCollider*>(collider))
		{
			CSphereCollider* sphereCollier = dynamic_cast<CSphereCollider*>(collider);
			_float radius = sphereCollier->GetSize();
			out << YAML::Key << "Type" << YAML::Value << "Sphere";
			out << YAML::Key << "Radius" << YAML::Value << radius;
		}
		else if (dynamic_cast<CCapsuleCollider*>(collider))
		{
			CCapsuleCollider* capsuleCollier = dynamic_cast<CCapsuleCollider*>(collider);
			pair<float, float> capuleSize = capsuleCollier->GetSize();
			out << YAML::Key << "Type" << YAML::Value << "Capsule";
			out << YAML::Key << "Radius" << YAML::Value << capuleSize.first;
			out << YAML::Key << "Height" << YAML::Value << capuleSize.second;
		}

		CCollider::RIGIDBODYDESC desc = collider->GetRigidBodyDesc();
		out << YAML::Key << "RigidBody";
		out << YAML::BeginMap;

		out << YAML::Key << "Enabled" << YAML::Value << desc.bEnabled;
		out << YAML::Key << "Gravity" << YAML::Value << desc.bGravity;
		out << YAML::Key << "Kinematic" << YAML::Value << desc.bKinematic;
		out << YAML::Key << "CharacterController" << YAML::Value << desc.bCC;

		out << YAML::EndMap;

		out << YAML::EndMap;

	}

	if (obj->GetComponent("Com_VIBuffer"))
	{
		CVIBuffer* viBuffer = dynamic_cast<CVIBuffer*>(obj->GetComponent("Com_VIBuffer"));

		out << YAML::Key << "Com_VIBuffer";
		out << YAML::BeginMap;

		if (dynamic_cast<CVIBuffer_Terrain*>(viBuffer))
		{
			CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(viBuffer);
			out << YAML::Key << "Type" << YAML::Value << "Terrain";
			out << YAML::Key << "HeightMapPath" << YAML::Value << pTerrainBuffer->GetHeightMapPath();
			out << YAML::Key << "TexturePath" << YAML::Value << pTerrainBuffer->GetTexturePath();
		}

		out << YAML::EndMap;
	}

	if (obj->GetComponent("Com_Model"))
	{
		CModel* pModel = dynamic_cast<CModel*>(obj->GetComponent("Com_Model"));

		out << YAML::Key << "Com_Model";
		out << YAML::BeginMap;

		out << YAML::Key << "MeshFilePath" << YAML::Value << pModel->GetMeshFilePath();
		out << YAML::Key << "MeshFileName" << YAML::Value << pModel->GetMeshFileName();
		out << YAML::Key << "HasCollider" << YAML::Value << pModel->HasMeshCollider();

		out << YAML::EndMap;
	}

	if (0 < obj->GetChildren().size())
	{
		out << YAML::Key << "Children";
		out << YAML::BeginSeq;

		list<CGameObject*> children = obj->GetChildren();
		for (auto& child : children)
			out << child->GetUUID();

		out << YAML::EndSeq;
	}
}

void CSceneSerializer::SerializeUI(YAML::Emitter & out, CGameObject * obj)
{

	out << YAML::Key << "Type" << YAML::Value << "UI";
	out << YAML::Key << "SortingOrder" << YAML::Value << dynamic_cast<CEmptyUI*>(obj)->GetSortingOrder();

	if (obj->GetComponent("Com_Transform"))
	{
		CRectTransform* rectTransform = dynamic_cast<CRectTransform*>(obj->GetComponent("Com_Transform"));

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

	if (obj->GetComponent("Com_VIBuffer"))
	{
		CVIBuffer_RectUI* buffer = dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent("Com_VIBuffer"));

		out << YAML::Key << "Com_VIBuffer";
		out << YAML::BeginMap;

		out << YAML::Key << "TexturePath" << YAML::Value << buffer->GetTextureFilePath();

		_float4 color = buffer->GetColor();
		out << YAML::Key << "Color";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << color.x << color.y << color.z << color.w << YAML::EndSeq;

		out << YAML::EndMap;
	}
	if (obj->GetComponent("Com_Text"))
	{
		CText* text = dynamic_cast<CText*>(obj->GetComponent("Com_Text"));

		out << YAML::Key << "Com_Text";
		out << YAML::BeginMap;

		out << YAML::Key << "Text" << YAML::Value << text->GetText();

		_float4 color = text->GetColor();
		_float2 scale = text->GetScale();

		out << YAML::Key << "Color";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << color.x << color.y << color.z << color.w << YAML::EndSeq;

		out << YAML::Key << "Scale" << YAML::Flow;
		out << YAML::BeginSeq << scale.x << scale.y << YAML::EndSeq;

		out << YAML::Key << "LayerDepth" << YAML::Value << text->GetLayerDepth();

		out << YAML::EndMap;

	}

}

CGameObject* CSceneSerializer::DeserializeUI(YAML::Node& obj)
{
	auto name = obj["Name"].as<string>();
	auto uuid = obj["UUID"].as<uint64_t>();
	auto layer = obj["Layer"].as<string>();
	auto active = obj["Active"].as<_bool>();

	CGameObject* deserializedObject = m_pEngine->AddGameObject(0, "Prototype_EmptyUI", layer);

	deserializedObject->SetInfo(name, layer, uuid, active);

	auto sortingOrder = obj["SortingOrder"].as<_int>();
	dynamic_cast<CEmptyUI*>(deserializedObject)->SetSortingOrder(sortingOrder);

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

	auto viBuffer = obj["Com_VIBuffer"];
	if (viBuffer)
	{
		CComponent* pTransform = deserializedObject->GetComponent("Com_Transform");
		deserializedObject->AddComponent(0, "Prototype_VIBuffer_RectUI", "Com_VIBuffer", pTransform);
		CComponent* pVIBuffer = deserializedObject->GetComponent("Com_VIBuffer");

		string texturePath = viBuffer["TexturePath"].as<string>();
		if ("" != texturePath)
		{
			dynamic_cast<CVIBuffer_RectUI*>(pVIBuffer)->UpdateTexture(texturePath);
		}

		_float4 color;
		auto sequence = viBuffer["Color"];
		color.x = sequence[0].as<float>();
		color.y = sequence[1].as<float>();
		color.z = sequence[2].as<float>();
		color.w = sequence[3].as<float>();
		dynamic_cast<CVIBuffer_RectUI*>(pVIBuffer)->SetColor(color);
	}
	auto text = obj["Com_Text"];
	if (text)
	{
		CComponent* pTransform = deserializedObject->GetComponent("Com_Transform");
		deserializedObject->AddComponent(0, "Prototype_Text", "Com_Text", pTransform);
		CText* pText = dynamic_cast<CText*>(deserializedObject->GetComponent("Com_Text"));

		string strText = text["Text"].as<string>();

		_float4 color;
		auto sequenceColor = text["Color"];
		color.x = sequenceColor[0].as<float>();
		color.y = sequenceColor[1].as<float>();
		color.z = sequenceColor[2].as<float>();
		color.w = sequenceColor[3].as<float>();

		_float2 scale;
		auto sequenceScale = text["Scale"];
		scale.x = sequenceScale[0].as<float>();
		scale.y = sequenceScale[1].as<float>();

		_float	layerDepth = text["LayerDepth"].as<float>();

		pText->SetTextInfo(strText, layerDepth, color, scale);
	}

	return deserializedObject;

}

CGameObject* CSceneSerializer::DeserializeObject(YAML::Node & obj)
{
	auto name = obj["Name"].as<string>();
	auto uuid = obj["UUID"].as<uint64_t>();
	auto layer = obj["Layer"].as<string>();
	auto active = obj["Active"].as<_bool>();

	CGameObject* deserializedObject = m_pEngine->AddGameObject(0, "Prototype_EmptyGameObject", layer);

	deserializedObject->SetInfo(name, layer, uuid, active);

	auto transformCom = obj["Com_Transform"];
	if (transformCom)
	{
		//_float3 tr, rt, sc;
		float tr[3], rt[3], sc[3];
		auto sequence = transformCom["Translation"];
		tr[0] = sequence[0].as<float>();
		tr[1] = sequence[1].as<float>();
		tr[2] = sequence[2].as<float>();
		sequence = transformCom["Rotation"];
		rt[0] = sequence[0].as<float>();
		rt[1] = sequence[1].as<float>();
		rt[2] = sequence[2].as<float>();
		sequence = transformCom["Scale"];
		sc[0] = sequence[0].as<float>();
		sc[1] = sequence[1].as<float>();
		sc[2] = sequence[2].as<float>();

		float _objMat[16]; XMFLOAT4X4 objMat;
		ImGuizmo::RecomposeMatrixFromComponents(tr, rt, sc, _objMat);
		memcpy(&objMat, _objMat, sizeof(XMFLOAT4X4));

		CComponent* pTransform = deserializedObject->GetComponent("Com_Transform");
		dynamic_cast<CTransform*>(pTransform)->SetMatrix(objMat);
	}
	auto colliderCom = obj["Com_Collider"];
	if (colliderCom)
	{
		auto centerInfo = colliderCom["Center"];
		_float3 center = { 0, 0, 0 };
		center.x = centerInfo[0].as<float>();
		center.y = centerInfo[1].as<float>();
		center.z = centerInfo[2].as<float>();

		CCollider::RIGIDBODYDESC desc;
		ZeroMemory(&desc, sizeof(desc));

		auto rigidBody = colliderCom["RigidBody"];
		desc.bEnabled = rigidBody["Enabled"].as<bool>();
		desc.bGravity = rigidBody["Gravity"].as<bool>();
		desc.bKinematic = rigidBody["Kinematic"].as<bool>();
		desc.bCC = rigidBody["CharacterController"].as<bool>();

		string type = colliderCom["Type"].as<string>();
		if (type == "Box")
		{
			if (deserializedObject->AddComponent(0, "Prototype_BoxCollider", "Com_Collider", deserializedObject->GetComponent("Com_Transform")))
				MSG_BOX("Failed to AddComponent BoxCollider");

			auto sequence = colliderCom["Size"];
			_float3 size;
			size.x = sequence[0].as<float>();
			size.y = sequence[1].as<float>();
			size.z = sequence[2].as<float>();

			CComponent* pCollider = deserializedObject->GetComponent("Com_Collider");
			dynamic_cast<CCollider*>(pCollider)->SetRelativePos(center);
			dynamic_cast<CBoxCollider*>(pCollider)->SetSize(size);
			dynamic_cast<CBoxCollider*>(pCollider)->SetUpRigidActor(&size, desc);
		}
		else if (type == "Sphere")
		{
			if (deserializedObject->AddComponent(0, "Prototype_SphereCollider", "Com_Collider", deserializedObject->GetComponent("Com_Transform")))
				MSG_BOX("Failed to AddComponent Prototype_SphereCollider");

			_float radius = colliderCom["Radius"].as<float>();
			CComponent* pCollider = deserializedObject->GetComponent("Com_Collider");
			dynamic_cast<CCollider*>(pCollider)->SetRelativePos(center);
			dynamic_cast<CSphereCollider*>(pCollider)->SetSize(radius);
			dynamic_cast<CSphereCollider*>(pCollider)->SetUpRigidActor(&radius, desc);
		}
		else if (type == "Capsule")
		{
			if (deserializedObject->AddComponent(0, "Prototype_CapsuleCollider", "Com_Collider", deserializedObject->GetComponent("Com_Transform")))
				MSG_BOX("Failed to AddComponent Prototype_CapsuleCollider");

			pair<float, float> capsuleSize;
			capsuleSize.first = colliderCom["Radius"].as<float>();
			capsuleSize.second = colliderCom["Height"].as<float>();
			CComponent* pCollider = deserializedObject->GetComponent("Com_Collider");
			dynamic_cast<CCollider*>(pCollider)->SetRelativePos(center);
			dynamic_cast<CCapsuleCollider*>(pCollider)->SetSize(capsuleSize);
			dynamic_cast<CCapsuleCollider*>(pCollider)->SetUpRigidActor(&capsuleSize, desc);
		}
	}

	auto viBufferCom = obj["Com_VIBuffer"];
	if (viBufferCom)
	{
		string type = viBufferCom["Type"].as<string>();
		if (type == "Terrain")
		{
			if (deserializedObject->AddComponent(0, "Prototype_VIBuffer_Terrain", "Com_VIBuffer", deserializedObject->GetComponent("Com_Transform")))
				MSG_BOX("Failed to AddComponent Prototype_VIBuffer_Terrain");

			string heightMapPath = viBufferCom["HeightMapPath"].as<string>();
			string texturePath = viBufferCom["TexturePath"].as<string>();

			CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(deserializedObject->GetComponent("Com_VIBuffer"));
			pTerrainBuffer->SetHeightMapPath(heightMapPath);
			pTerrainBuffer->SetTexturePath(texturePath);
		}
	}

	auto modelCom = obj["Com_Model"];
	if (modelCom)
	{
		string meshFilePath = modelCom["MeshFilePath"].as<string>();
		string meshFileName = modelCom["MeshFileName"].as<string>();
		_bool hasCollider = modelCom["HasCollider"].as<_bool>();

		if (deserializedObject->AddComponent(0, "Prototype_Model", "Com_Model", deserializedObject->GetComponent("Com_Transform")))
			MSG_BOX("Failed to AddComponent Prototype_Model");

		CModel* pMesh = dynamic_cast<CModel*>(deserializedObject->GetComponent("Com_Model"));
		pMesh->SetMeshCollider(hasCollider);
		pMesh->CreateBuffer(meshFilePath, meshFileName);
	}

	return deserializedObject;
}
