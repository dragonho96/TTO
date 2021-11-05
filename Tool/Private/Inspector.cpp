#include "stdafx.h"
#include "..\Public\Inspector.h"


CInspector::CInspector(CToolManager * pToolManager)
	: CImGuiWindow(pToolManager)
{
}

void CInspector::Initialize()
{
	m_pGizmo = dynamic_cast<CGizmo*>(m_pToolManager->GetWindow("Gizmo"));
}

void CInspector::Update()
{
	ImGui::Begin("Inspector");
	//if (ImGui::CollapsingHeader("Transform", true))
	//{
	ImGui::Text("GameObject Name");

	ImGui::Separator();

	//ImGui::Text("Transform");
	GIZMOMATRIX mat = m_pGizmo->GetMatrix();
	_float3 tr = { mat.matTranslation[0], mat.matTranslation[1], mat.matTranslation[2] };
	_float3 rt = { mat.matRotation[0], mat.matRotation[1], mat.matRotation[2] };
	_float3 sc = { mat.matScale[0], mat.matScale[1], mat.matScale[2] };

	if (ImGui::TreeNodeEx("Transform"))
	{
		//ImGui::InputFloat3("Tr", mat.matTranslation);
		//ImGui::InputFloat3("Rt", mat.matRotation);
		//ImGui::InputFloat3("Sc", mat.matScale);
		DrawVec3("Transform", tr);
		DrawVec3("Rotation", rt);
		DrawVec3("Scale", sc);

		ImGui::TreePop();
	}

	mat.matTranslation[0] = tr.x;
	mat.matTranslation[1] = tr.y;
	mat.matTranslation[2] = tr.z;
	mat.matRotation[0] = rt.x;
	mat.matRotation[1] = rt.y;
	mat.matRotation[2] = rt.z;
	mat.matScale[0] = sc.x;
	mat.matScale[1] = sc.y;
	mat.matScale[2] = sc.z;

	m_pGizmo->SetNewGizmoMatrix(mat);

	ImGui::Separator();

	//}

	ImGui::End();
}

void CInspector::LateUpdate()
{
}

void CInspector::DrawVec3(const string & label, _float3 & values)
{
	// Make each vec3 control unique
	ImGui::PushID(label.c_str());

	float columnWitdh = 100.f;
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWitdh);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.f;
	ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.1f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.1f, 1.f });
	if (ImGui::Button("X", buttonSize))
		values.x = (int)values.x;
	ImGui::PopStyleColor(3);


	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.7f, 0.1f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.8f, 0.2f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.7f, 0.1f, 1.f });
	if (ImGui::Button("Y", buttonSize))
		values.y = (int)values.y;
	ImGui::PopStyleColor(3);


	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.1f, 0.8f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.2f, 0.9f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.1f, 0.8f, 1.f });
	if (ImGui::Button("Z", buttonSize))
		values.z = (int)values.z;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}