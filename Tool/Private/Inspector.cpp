#include "stdafx.h"
#include "..\Public\Inspector.h"
#include "GameObject.h"
#include "VIBuffer_LineSphere.h"
#include "VIBuffer_RectUI.h"
#include "VIBuffer_Terrain.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "EmptyGameObject.h"
#include "RectTransform.h"
#include "Engine.h"
#include "Log.h"

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
		/* Get GameObject Name and Change */
		char buf[64];
		sprintf_s(buf, g_pObjFocused->GetName().c_str());
		ImGui::InputText("##Name", buf, IM_ARRAYSIZE(buf));
		g_pObjFocused->SetName(string(buf));

		ImGui::Separator();

		sprintf_s(buf, g_pObjFocused->GetLayer().c_str());
		ImGui::InputText("Layer", buf, IM_ARRAYSIZE(buf));
		g_pObjFocused->SetLayer(string(buf));


		if (dynamic_cast<CEmptyGameObject*>(g_pObjFocused))
			UpdateGameObject();
		else
			UpdateUI();
	}

	ImGui::End();

}

void CInspector::LateUpdate()
{
}

void CInspector::UpdateGameObject()
{
	ImGui::SameLine();

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");
	if (ImGui::BeginPopup("AddComponent"))
	{
		ImGui::Text("Collider");
		ImGui::Indent();
		if (ImGui::MenuItem("SphereCollider"))
		{
			/* Add Collider */
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_SphereCollider", "Com_Collider", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent");
		}
		if (ImGui::MenuItem("BoxCollider"))
		{
			/* Add Collider */
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_BoxCollider", "Com_Collider", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent");
		}
		if (ImGui::MenuItem("CapsuleCollider"))
		{
			/* Add Collider */
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_CapsuleCollider", "Com_Collider", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent");
		}
		ImGui::Unindent();

		ImGui::Separator();

		if (ImGui::MenuItem("Terrain"))
		{
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_VIBuffer_Terrain", "Com_VIBuffer", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent");
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Model"))
		{
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_Model", "Com_Model", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent Model");
		}

		ImGui::EndPopup();
	}

	ImGui::Separator();

	DrawTransform();

	ImGui::Separator();

	DrawBuffer();

	ImGui::Separator();

	DrawModel();

	ImGui::Separator();

	DrawCollider();
}

void CInspector::UpdateUI()
{
	ImGui::SameLine();

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");
	if (ImGui::BeginPopup("AddComponent"))
	{
		if (ImGui::MenuItem("Image"))
		{
			/* Add Collider */
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_VIBuffer_RectUI", "Com_VIBuffer", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent");
		}
		if (ImGui::MenuItem("Text"))
		{
			/* Add Collider */
			if (FAILED(g_pObjFocused->AddComponent(0, "Prototype_Text", "Com_Text", g_pObjFocused->GetComponent("Com_Transform"))))
				MSG_BOX("Failed to AddComponent");
		}

		ImGui::EndPopup();
	}


	DrawRectTransform();

	DrawImage();

	DrawTextUI();
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

void CInspector::DrawRectDesc(const string & label, _float & x, _float & y)
{
	ImGui::PushID(label.c_str());

	float columnWitdh = 100.f;
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWitdh);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.f;
	ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.1f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.1f, 1.f });
	if (ImGui::Button("X", buttonSize))
		x = (int)x;
	ImGui::PopStyleColor(3);


	ImGui::SameLine();
	ImGui::DragFloat("##X", &x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.7f, 0.1f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.8f, 0.2f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.7f, 0.1f, 1.f });
	if (ImGui::Button("Y", buttonSize))
		y = (int)y;
	ImGui::PopStyleColor(3);


	ImGui::SameLine();
	ImGui::DragFloat("##Y", &y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}

void CInspector::DrawImage()
{
	CComponent* pComponent;
	if (pComponent = g_pObjFocused->GetComponent("Com_VIBuffer"))
	{
		ImGui::Separator();

		if (ImGui::TreeNodeEx("Image"))
		{
			// TODO: Check if it has image source
			string imageName = dynamic_cast<CVIBuffer_RectUI*>(pComponent)->GetTextureFilePath().c_str();
			imageName = imageName == "" ? "None" : imageName;
			ImGui::Text(imageName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
				{
					//string s = *(static_cast<std::string *>(payload->Data));
					//const int s = *(const int*)(payload->Data);
					const char* s = (const char*)(payload->Data);
					//string sdf = "ss" + to_string(s);
					dynamic_cast<CVIBuffer_RectUI*>(pComponent)->UpdateTexture(s);
				}
				ImGui::EndDragDropTarget();
			}



			_float4& color = dynamic_cast<CVIBuffer_RectUI*>(pComponent)->GetColor();
			ImGui::ColorEdit4("MyColor##2f", (float*)&color, ImGuiColorEditFlags_Float);
			ImGui::TreePop();
		}

	}
}

void CInspector::DrawTextUI()
{
	CComponent* pComponent;
	if (pComponent = g_pObjFocused->GetComponent("Com_Text"))
	{
		CText* pText = dynamic_cast<CText*>(pComponent);
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Text"))
		{
			ImGui::InputTextMultiline("Text", &pText->GetText());

			_float2& scale = pText->GetScale();
			DrawRectDesc("Scale", scale.x, scale.y);

			_float4& color = pText->GetColor();
			ImGui::ColorEdit4("MyColor##2f", (float*)&color, ImGuiColorEditFlags_Float);
			ImGui::TreePop();
		}
	}
}

void CInspector::DrawCollider()
{
	CComponent* pComponent;
	if (pComponent = g_pObjFocused->GetComponent("Com_Collider"))
	{
		bool bDelete = false;
		bool open = ImGui::TreeNodeEx("Collider");
		ImGui::SameLine(ImGui::GetWindowWidth() - 10);
		if (ImGui::Button("X##RemoveComponent"))
			bDelete = true;

		if (open)
		{
			if (dynamic_cast<CSphereCollider*>(pComponent))
			{
				float radius = dynamic_cast<CSphereCollider*>(pComponent)->GetSize();
				ImGui::DragFloat("Radius", &radius, 0.1f, 0.1f, 100.f, "%.3f", ImGuiSliderFlags_ClampOnInput);
				dynamic_cast<CSphereCollider*>(pComponent)->SetSize(radius);
			}
			else if (dynamic_cast<CBoxCollider*>(pComponent))
			{
				_float3 size = dynamic_cast<CBoxCollider*>(pComponent)->GetSize();
				ImGui::DragFloat("X", &size.x, 0.1f, 0.1f, 100.f, "%.3f", ImGuiSliderFlags_ClampOnInput);
				ImGui::DragFloat("Y", &size.y, 0.1f, 0.1f, 100.f, "%.3f", ImGuiSliderFlags_ClampOnInput);
				ImGui::DragFloat("Z", &size.z, 0.1f, 0.1f, 100.f, "%.3f", ImGuiSliderFlags_ClampOnInput);
				dynamic_cast<CBoxCollider*>(pComponent)->SetSize(size);
			}
			else if (dynamic_cast<CCapsuleCollider*>(pComponent))
			{
				pair<float, float> size = dynamic_cast<CCapsuleCollider*>(pComponent)->GetSize();
				ImGui::DragFloat("X", &size.first, 0.1f, 0.1f, 100.f, "%.3f", ImGuiSliderFlags_ClampOnInput);
				ImGui::DragFloat("Y", &size.second, 0.1f, 0.1f, 100.f, "%.3f", ImGuiSliderFlags_ClampOnInput);
				dynamic_cast<CCapsuleCollider*>(pComponent)->SetSize(size);
			}

			CCollider* pCollider = dynamic_cast<CCollider*>(pComponent);
			ImGui::Checkbox("RigidBody", &pCollider->GetRigidBodyDesc().bEnabled);
			if (pCollider->GetRigidBodyDesc().bEnabled)
			{
				ImGui::Indent();

				CCollider::RIGIDBODYDESC desc = pCollider->GetRigidBodyDesc();
				ImGui::Checkbox("Use Gravity", &pCollider->GetRigidBodyDesc().bGravity);
				ImGui::Checkbox("Is Kinematic", &pCollider->GetRigidBodyDesc().bKinematic);
				ImGui::Checkbox("Character Controller", &pCollider->GetRigidBodyDesc().bCC);

				ImGui::Unindent();
			}



			ImGui::TreePop();
		}

		//if (bDelete)
		//	g_pObjFocused->RemoveComponent(TEXT("Com_Collider"));
	}
}

void CInspector::DrawTransform()
{
	CComponent* pObjTransform = nullptr;
	if (!(pObjTransform = g_pObjFocused->GetComponent("Com_Transform")))
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

void CInspector::DrawBuffer()
{
	CComponent* pVIBuffer = nullptr;
	if (pVIBuffer = g_pObjFocused->GetComponent("Com_VIBuffer"))
	{
		bool bDelete = false;

		if (dynamic_cast<CVIBuffer_Terrain*>(pVIBuffer))
		{
			bool open = ImGui::TreeNodeEx("Terrain");
			ImGui::SameLine(ImGui::GetWindowWidth() - 10);
			if (ImGui::Button("X##RemoveComponent"))
				bDelete = true;

			if (open)
			{
				string heightMapPath = dynamic_cast<CVIBuffer_Terrain*>(pVIBuffer)->GetHeightMapPath().c_str();
				heightMapPath = heightMapPath == "" ? "None" : heightMapPath;
				ImGui::Text(heightMapPath.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
					{
						const char* s = (const char*)(payload->Data);
						dynamic_cast<CVIBuffer_Terrain*>(pVIBuffer)->SetHeightMapPath(s);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::TreePop();
			}


		}

		if (bDelete)
			g_pObjFocused->RemoveComponent("Com_VIBuffer");
	}
}

void CInspector::DrawModel()
{
	CComponent* pModel = nullptr;
	if (pModel = g_pObjFocused->GetComponent("Com_Model"))
	{
		bool bDelete = false;

		if (dynamic_cast<CModel*>(pModel))
		{
			bool open = ImGui::TreeNodeEx("Model");
			ImGui::SameLine(ImGui::GetWindowWidth() - 10);
			if (ImGui::Button("X##RemoveComponent"))
				bDelete = true;

			if (open)
			{
				string meshFileName = dynamic_cast<CModel*>(pModel)->GetMeshFileName().c_str();
				meshFileName = meshFileName == "" ? "None" : meshFileName;
				ImGui::Text(meshFileName.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
					{
						const char* s = (const char*)(payload->Data);

						char	szTextureFileName[MAX_PATH] = "";
						char	szExt[MAX_PATH] = "";
						char	szDir[MAX_PATH] = "";
						_splitpath(s, nullptr, szDir, szTextureFileName, szExt);

						if (!strcmp(szExt, ".fbx") || !strcmp(szExt, ".Fbx") || !strcmp(szExt, ".FBX"))
						{
							strcat_s(szTextureFileName, szExt);
							dynamic_cast<CModel*>(pModel)->CreateBuffer(szDir, szTextureFileName);
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::TreePop();
			}
		}

		if (bDelete)
			g_pObjFocused->RemoveComponent("Com_Model");
	}
}

void CInspector::DrawRectTransform()
{
	CComponent* pObjTransform = nullptr;
	if (!(pObjTransform = g_pObjFocused->GetComponent("Com_Transform")))
		MSG_BOX("Failed to Get Transform");

	CRectTransform::RECTTRANSFORMDESC desc = dynamic_cast<CRectTransform*>(pObjTransform)->GetTransformDesc();
	
	ImGui::Separator();
	if (ImGui::TreeNodeEx("Rect Transform"))
	{
		DrawRectDesc("Position", desc.posX, desc.posY);
		DrawRectDesc("Size", desc.sizeX, desc.sizeY);

		ImGui::TreePop();
	}

	// SetTransformMat(desc)
	dynamic_cast<CRectTransform*>(pObjTransform)->SetTransformMat(desc);

	// GetTransformMat
	float _objMat[16];
	XMFLOAT4X4 objMat = dynamic_cast<CRectTransform*>(pObjTransform)->GetTransformMat();
	memcpy(_objMat, &objMat, sizeof(XMFLOAT4X4));

	m_pGizmo->SetObjMat(_objMat);
}

void CInspector::Free()
{
}
