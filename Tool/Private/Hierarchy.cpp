#include "stdafx.h"
#include "..\Public\Hierarchy.h"
#include "GameObject.h"
#include "Log.h"

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
				TEXT("Prototype_EmptyGameObject"), TEXT("LAYER_TOOL"));
			g_pObjFocused = pObj;
			string s = to_string( pObj->GetUUID());
			dynamic_cast<CLog*>(m_pEngine->GetWindow("Log"))->AddLog(s.c_str());
		}
		if (ImGui::MenuItem("UI"))
		{
			//Add GameObject 
			CGameObject* pObj = m_pEngine->AddGameObject(0,
				TEXT("Prototype_EmptyUI"), TEXT("LAYER_TOOL"));
			g_pObjFocused = pObj;
		}
		ImGui::EndPopup();
	}


	// 여기서 리스트 순회하면서 추가
	static int selected = -1;
	static bool openPopup = false;
	list<CGameObject*> pObjList = m_pEngine->GetGameObjectInLayer(0, TEXT("LAYER_TOOL"));

	int iCount = 0;
	for (auto& pObj : pObjList)
	{
		ImGui::PushID(iCount++);
		ImGui::Selectable(pObj->GetName().c_str(), iCount == selected);

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

		ImGui::PopID();
	}
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

void CHierarchy::Free()
{
}
