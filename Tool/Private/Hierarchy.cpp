#include "stdafx.h"
#include "..\Public\Hierarchy.h"
#include "GameObject.h"

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

	if (ImGui::Button("Add Empty GameObject"))
	{
		//Add GameObject 
		CGameObject* pObj = m_pEngine->AddGameObject(0,
			TEXT("Prototype_EmptyGameObject"), TEXT("LAYER_TOOL"));
		g_pObjFocused = pObj;

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
				openPopup = !openPopup;
		}
		if (openPopup)
		{
			ImGui::OpenPopup("my_select_popup");

			if (ImGui::BeginPopup("my_select_popup"))
			{
				if (ImGui::MenuItem("Delete"))
				{
					/* Delete Object*/

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
		ImGui::PopID();

	}

	ImGui::End();
}

void CHierarchy::LateUpdate()
{
}

void CHierarchy::Free()
{
}
