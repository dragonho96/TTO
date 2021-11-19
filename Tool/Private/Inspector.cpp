#include "stdafx.h"
#include "..\Public\Inspector.h"
#include "GameObject.h"
#include "VIBuffer_LineSphere.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"

USING(Tool)
CInspector::CInspector()
{
	Initialize();
}

void CInspector::Initialize()
{
	m_pGizmo = dynamic_cast<CGizmo*>(CEngine::GetInstance()->GetWindow("Gizmo"));
	//m_pGizmo = dynamic_pointer_cast<CGizmo>(m_pToolManager->GetWindow("Gizmo"));
}

void CInspector::Update()
{
	ImGui::Begin("Inspector");
	ImGui::Separator();

	if (g_pObjFocused)
	{
		/* Check if it has focused object*/
		bool open = true;
		/* Get GameObject Name and Change */
		char buf[64];
		sprintf_s(buf, g_pObjFocused->GetName().c_str());
		ImGui::InputText("##Name", buf, IM_ARRAYSIZE(buf));
		g_pObjFocused->SetName(string(buf));

		ImGui::SameLine();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("SphereCollider"))
			{
				/* Add Collider */
				if (FAILED(g_pObjFocused->AddComponent(0, TEXT("Prototype_SphereCollider"), TEXT("Com_Collider"), g_pObjFocused->GetComponent(TEXT("Com_Transform")))))
					MSG_BOX("Failed to AddComponent");
			}
			if (ImGui::MenuItem("BoxCollider"))
			{
				/* Add Collider */
				if (FAILED(g_pObjFocused->AddComponent(0, TEXT("Prototype_BoxCollider"), TEXT("Com_Collider"), g_pObjFocused->GetComponent(TEXT("Com_Transform")))))
					MSG_BOX("Failed to AddComponent");
			}
			if (ImGui::MenuItem("CapsuleCollider"))
			{
				/* Add Collider */
				if (FAILED(g_pObjFocused->AddComponent(0, TEXT("Prototype_CapsuleCollider"), TEXT("Com_Collider"), g_pObjFocused->GetComponent(TEXT("Com_Transform")))))
					MSG_BOX("Failed to AddComponent");
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		DrawTransform();

		ImGui::Separator();

		DrawCollider();
	}

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

void CInspector::DrawCollider()
{
	CComponent* pComponent;
	if (pComponent = g_pObjFocused->GetComponent(TEXT("Com_Collider")))
	{
		if (ImGui::TreeNodeEx("Collider"))
		{
			if (dynamic_cast<CSphereCollider*>(pComponent))
			{
				float radius = dynamic_cast<CSphereCollider*>(pComponent)->GetSize();
				ImGui::DragFloat("Radius", &radius, 0.1f);
				dynamic_cast<CSphereCollider*>(pComponent)->SetSize(radius);
			}
			else if (dynamic_cast<CBoxCollider*>(pComponent))
			{
				_float3 size = dynamic_cast<CBoxCollider*>(pComponent)->GetSize();
				ImGui::DragFloat("X", &size.x, 0.1f);
				ImGui::DragFloat("Y", &size.y, 0.1f);
				ImGui::DragFloat("Z", &size.z, 0.1f);
				dynamic_cast<CBoxCollider*>(pComponent)->SetSize(size);
			}
			else if (dynamic_cast<CCapsuleCollider*>(pComponent))
			{
				pair<float, float> size = dynamic_cast<CCapsuleCollider*>(pComponent)->GetSize();
				ImGui::DragFloat("X", &size.first, 0.1f);
				ImGui::DragFloat("Y", &size.second, 0.1f);
				dynamic_cast<CCapsuleCollider*>(pComponent)->SetSize(size);
			}

			ImGui::TreePop();
		}
	}
}

void CInspector::DrawTransform()
{
	CComponent* pObjTransform = nullptr;
	if (!(pObjTransform = g_pObjFocused->GetComponent(TEXT("Com_Transform"))))
		MSG_BOX("Failed to Get Transform");

	float _objMat[16];
	XMFLOAT4X4 objMat = dynamic_cast<CTransform*>(pObjTransform)->GetMatrix();
	memcpy(_objMat, &objMat, sizeof(XMFLOAT4X4));

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	ImGuizmo::DecomposeMatrixToComponents(_objMat, matrixTranslation, matrixRotation, matrixScale);
	_float3 tr, rt, sc;
	memcpy(&tr, matrixTranslation, sizeof(float) * 3);
	memcpy(&rt, matrixRotation, sizeof(float) * 3);
	memcpy(&sc, matrixScale, sizeof(float) * 3);

	if (ImGui::TreeNodeEx("Transform"))
	{
		DrawVec3("Transform", tr);
		DrawVec3("Rotation", rt);
		DrawVec3("Scale", sc);

		ImGui::TreePop();
	}

	memcpy(matrixTranslation, &tr, sizeof(float) * 3);
	memcpy(matrixRotation, &rt, sizeof(float) * 3);
	memcpy(matrixScale, &sc, sizeof(float) * 3);
	memcpy(&objMat, _objMat, sizeof(XMFLOAT4X4));

	ImGuizmo::RecomposeMatrixFromComponents(
		matrixTranslation,
		matrixRotation,
		matrixScale, _objMat);

	m_pGizmo->SetObjMat(_objMat);
}

void CInspector::Free()
{
}
