#include "stdafx.h"
#include "..\Public\Hierarchy.h"
#include "GameObject.h"
#include "Log.h"
#include "Layer.h"

static int selected = -1;
static bool openPopup = false;

USING(Tool)
CHierarchy::CHierarchy()
	: m_pEngine(CEngine::GetInstance())
{
	Initialize();
}

void CHierarchy::Initialize()
{
}

void CHierarchy::Update()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::Button("Add Empty Object"))
		ImGui::OpenPopup("AddGameObject");
	if (ImGui::BeginPopup("AddGameObject"))
	{
		if (ImGui::MenuItem("GameObject"))
		{
			//Add GameObject 
			CGameObject* pObj = m_pEngine->AddGameObject(0,
				"Prototype_EmptyGameObject", "Default");
			g_pObjFocused = pObj;
			string s = to_string( pObj->GetUUID());
			dynamic_cast<CLog*>(m_pEngine->GetWindow("Log"))->AddLog(s.c_str());
		}
		if (ImGui::MenuItem("UI"))
		{
			//Add GameObject 
			CGameObject* pObj = m_pEngine->AddGameObject(0,
				"Prototype_EmptyUI", "UI");
			g_pObjFocused = pObj;
		}
		ImGui::EndPopup();
	}


	// 여기서 리스트 순회하면서 추가
	unordered_map<string, CLayer*>* layers = m_pEngine->GetLayers();
	for (auto& pair : *layers)
	{
		if (pair.first == "LAYER_TOOL")
			continue;

		list<CGameObject*> pObjList = (pair.second)->GetGameObjectList();
		list<CGameObject*> pObjListNoParent;
		for (auto& obj : pObjList)
		{
			if (!obj->GetParent())
				pObjListNoParent.push_back(obj);
		}
		int iCount = 0;
		for (auto& pObj : pObjListNoParent)
		{
			SetObjectHierarchy(pObj, iCount);
		}
	}
	//list<CGameObject*> pObjList = m_pEngine->GetGameObjectInLayer(0, TEXT("LAYER_TOOL"));
	//list<CGameObject*> pObjListNoParent;
	//for (auto& obj : pObjList)
	//{
	//	if (!obj->GetParent())
	//		pObjListNoParent.push_back(obj);
	//}

	//int iCount = 0;
	//for (auto& pObj : pObjListNoParent)
	//{
	//	SetObjectHierarchy(pObj, iCount);
	//}
	if (openPopup)
	{
		ImGui::OpenPopup("my_select_popup");

		if (ImGui::BeginPopup("my_select_popup"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				/* Delete Object*/
				g_pObjFocused->SetDead();
				g_pObjFocused = nullptr;
				openPopup = !openPopup;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Close"))
			{
				/* Close Popup*/
				openPopup = !openPopup;
			}

			ImGui::EndPopup();
		}
	}

	ImGui::End();
}

void CHierarchy::LateUpdate()
{
}

void CHierarchy::SetObjectHierarchy(CGameObject* pObj, int& iCount)
{
	ImGui::PushID(iCount++);
	ImGuiTreeNodeFlags node_flags = iCount == selected ? ImGuiTreeNodeFlags_Selected : 0;
	bool bOpen = ImGui::TreeNodeEx(pObj->GetName().c_str(), node_flags, pObj->GetName().c_str(), iCount == selected);
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			/* Set Focused Game Object*/
			selected = iCount;
			g_pObjFocused = pObj;
		}
		else if (ImGui::IsMouseClicked(1))
		{
			selected = iCount;
			g_pObjFocused = pObj;
			openPopup = !openPopup;
		}
	}

	if (ImGui::BeginDragDropSource())
	{
		//const CGameObject* obj = pObj;
		ImGui::SetDragDropPayload("GameObject", &pObj, sizeof(pObj), ImGuiCond_Once);
		ImGui::Text("GameObject");
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			CGameObject** droppedObj = (CGameObject**)(payload->Data);
			pObj->AddChild(*droppedObj);
		}
		ImGui::EndDragDropTarget();
	}
	if (bOpen)
	{
		list<CGameObject*> children = pObj->GetChildren();
		for (auto& child : children)
		{
			SetObjectHierarchy(child, iCount);
		}
		ImGui::TreePop();
	}

	//ImGui::Selectable(pObj->GetName().c_str(), iCount == selected);

	ImGui::PopID();
}

void CHierarchy::Free()
{
}
