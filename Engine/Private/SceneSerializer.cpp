#include "..\Public\SceneSerializer.h"
#include "EmptyGameObject.h"
#include "EmptyUI.h"

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

	list<CGameObject*> objList = m_pEngine->GetGameObjectInLayer(0, TEXT("LAYER_TOOL"));
	for (auto& obj : objList)
	{
		if (dynamic_cast<CEmptyGameObject*>(obj))
			SerializeObject(out, obj);
		else
			SerializeUI(out, obj);
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
				DeserializeUI(obj);
			else
				DeserializeObject(obj);
		}
	}

	return true;
}

void CSceneSerializer::SerializeObject(YAML::Emitter & out, CGameObject * obj)
{
	out << YAML::BeginMap;

	out << YAML::Key << "Type" << YAML::Value << "Object";
	out << YAML::Key << "UUID" << YAML::Value << obj->GetUUID();

	if (obj->GetComponent(TEXT("Com_Transform")))
	{
		CTransform* transform = dynamic_cast<CTransform*>(obj->GetComponent(TEXT("Com_Transform")));

		out << YAML::Key << "Com_Transform";
		out << YAML::BeginMap;

		XMMATRIX matrix = XMLoadFloat4x4(&transform->GetMatrix());
		XMVECTOR tr, rt, sc;
		XMMatrixDecompose(&sc, &rt, &tr, matrix);
		out << YAML::Key << "Translation";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << XMVectorGetX(tr) << XMVectorGetY(tr) << XMVectorGetZ(tr) << YAML::EndSeq;

		out << YAML::Key << "Rotation";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << XMVectorGetX(rt) << XMVectorGetY(rt) << XMVectorGetZ(rt) << YAML::EndSeq;

		out << YAML::Key << "Scale";
		out << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << XMVectorGetX(sc) << XMVectorGetY(sc) << XMVectorGetZ(sc) << YAML::EndSeq;


		out << YAML::EndMap;
	}

	if (obj->GetComponent(TEXT("Com_Collider")))
	{
		CCollider* collider = dynamic_cast<CCollider*>(obj->GetComponent(TEXT("Com_Collider")));

		// Check if RB exist
		out << YAML::Key << "Com_Collider";
		out << YAML::BeginMap;

		if (dynamic_cast<CBoxCollider*>(collider))
		{
			CBoxCollider* boxCollier = dynamic_cast<CBoxCollider*>(collider);
			_float3 boxSize = boxCollier->GetSize();

			out << YAML::Key << "Type" << YAML::Value << "Box";
			out << YAML::Key << "Size" << YAML::Value << YAML::Flow;
			out << YAML::BeginSeq << boxSize.x << boxSize.y << boxSize.z << YAML::EndSeq;
		}

		// TODO: Sphere and Capsule

		CCollider::RIGIDBODYDESC desc = collider->GetRigidBodyDesc();
		out << YAML::Key << "RigidBody";
		out << YAML::BeginMap;

		out << YAML::Key << "Enabled" << YAML::Value << desc.bEnabled;
		out << YAML::Key << "Gravity" << YAML::Value << desc.bGravity;
		out << YAML::Key << "Kinematic" << YAML::Value << desc.bKinematic;
		
		out << YAML::EndMap;
		
		out << YAML::EndMap;

	}

	if (obj->GetComponent(TEXT("Com_VIBuffer")))
	{
		CVIBuffer* viBuffer = dynamic_cast<CVIBuffer*>(obj->GetComponent(TEXT("Com_VIBuffer")));

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

	out << YAML::EndMap;
}

void CSceneSerializer::SerializeUI(YAML::Emitter & out, CGameObject * obj)
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

void CSceneSerializer::DeserializeUI(YAML::Node& obj)
{
	CGameObject* deserializedObject = m_pEngine->AddGameObject(0, TEXT("Prototype_EmptyUI"), TEXT("LAYER_TOOL"));
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
		CComponent* pTransform = deserializedObject->GetComponent(TEXT("Com_Transform"));
		deserializedObject->AddComponent(0, TEXT("Prototype_VIBuffer_RectUI"), TEXT("Com_VIBuffer"), pTransform);
		CComponent* pVIBuffer = deserializedObject->GetComponent(TEXT("Com_VIBuffer"));;

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
}

void CSceneSerializer::DeserializeObject(YAML::Node & obj)
{
	CGameObject* deserializedObject = m_pEngine->AddGameObject(0, TEXT("Prototype_EmptyGameObject"), TEXT("LAYER_TOOL"));
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

		CComponent* pTransform = deserializedObject->GetComponent(TEXT("Com_Transform"));
		dynamic_cast<CTransform*>(pTransform)->SetMatrix(objMat);
	}
	auto colliderCom = obj["Com_Collider"];
	if (colliderCom)
	{
		CCollider::RIGIDBODYDESC desc;
		ZeroMemory(&desc, sizeof(desc));

		auto rigidBody = colliderCom["RigidBody"];
		desc.bEnabled = rigidBody["Enabled"].as<bool>();
		desc.bGravity = rigidBody["Gravity"].as<bool>();
		desc.bKinematic = rigidBody["Kinematic"].as<bool>();

		string type = colliderCom["Type"].as<string>();
		if (type == "Box")
		{
			if(deserializedObject->AddComponent(0, TEXT("Prototype_BoxCollider"), TEXT("Com_Collider"), deserializedObject->GetComponent(TEXT("Com_Transform"))))
				MSG_BOX("Failed to AddComponent BoxCollider");

			auto sequence = colliderCom["Size"];
			_float3 size;
			size.x = sequence[0].as<float>();
			size.y = sequence[1].as<float>();
			size.z = sequence[2].as<float>();

			CComponent* pCollider = deserializedObject->GetComponent(TEXT("Com_Collider"));
			dynamic_cast<CBoxCollider*>(pCollider)->SetUpRigidActor(&size, desc);
		}
	}

	auto viBufferCom = obj["Com_VIBuffer"];
	if (viBufferCom)
	{
		string type = viBufferCom["Type"].as<string>();
		if (type == "Terrain")
		{
			if (deserializedObject->AddComponent(0, TEXT("Prototype_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), deserializedObject->GetComponent(TEXT("Com_Transform"))))
				MSG_BOX("Failed to AddComponent Prototype_VIBuffer_Terrain");

			string heightMapPath = viBufferCom["HeightMapPath"].as<string>();
			string texturePath = viBufferCom["TexturePath"].as<string>();

			CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(deserializedObject->GetComponent(TEXT("Com_VIBuffer")));
			pTerrainBuffer->SetHeightMapPath(heightMapPath);
			pTerrainBuffer->SetTexturePath(texturePath);
		}
	}
}
