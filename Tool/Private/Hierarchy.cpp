#include "stdafx.h"
#include "..\Public\Hierarchy.h"


CHierarchy::CHierarchy(CToolManager * pToolManager)
	: CImGuiWindow(pToolManager)
{
}

void CHierarchy::Initialize()
{
}

void CHierarchy::Update()
{
	ImGui::Begin("Hierarchy");

	// ���⼭ ����Ʈ ��ȸ�ϸ鼭 �߰�
	static bool selected = false;
	static bool openPopup = false;

	ImGui::Selectable("selectable", selected);

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			/* Set Focused Game Object*/
			selected = !selected;
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

	ImGui::End();
}

void CHierarchy::LateUpdate()
{
}
