#include "stdafx.h"
#include "..\Public\Gizmo.h"
#include "ToolManager.h"
static ImGuizmo::OPERATION m_CurrentGizmoOperation(ImGuizmo::TRANSLATE);
static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
0.f, 1.f, 0.f, 0.f,
0.f, 0.f, 1.f, 0.f,
0.f, 0.f, 0.f, 1.f };

float objectMatrix[4][16] = {
	{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f },

	{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	2.f, 0.f, 0.f, 1.f },

	{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	2.f, 0.f, 2.f, 1.f },

	{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 2.f, 1.f }
};
USING(Tool)
CGizmo::CGizmo()
{
	Initialize();
}

void CGizmo::Initialize()
{
	useWindow = true;
	gizmoCount = 1;
	camDistance = 8.f;
	m_tNewGizmoMatrix.matScale[0] = 1;
	m_tNewGizmoMatrix.matScale[1] = 1;
	m_tNewGizmoMatrix.matScale[2] = 1;
}

void CGizmo::Update()
{
	CToolManager::SetImGuizmoStyle();
	CToolManager::SetImGuiColor();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	XMMATRIX viewMatrix = CEngine::GetInstance()->GetViewMatrix();
	XMFLOAT4X4 fView;
	XMStoreFloat4x4(&fView, viewMatrix);
	memcpy(_view, &fView, sizeof(XMFLOAT4X4));

	XMMATRIX projMatrix = CEngine::GetInstance()->GetProjectionMatrix();
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, projMatrix);
	memcpy(_projection, &projection, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 objMat = CEngine::GetInstance()->GetObjectMatrix();
	memcpy(_objMat, &objMat, sizeof(XMFLOAT4X4));

	ImGuizmo::SetID(0);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	ImGuizmo::DecomposeMatrixToComponents(_objMat, matrixTranslation, matrixRotation, matrixScale);

	memcpy(m_tGizmoMatrix.matTranslation, matrixTranslation, sizeof(float) * 3);
	memcpy(m_tGizmoMatrix.matRotation, matrixRotation, sizeof(float) * 3);
	memcpy(m_tGizmoMatrix.matScale, matrixScale, sizeof(float) * 3);
}

void CGizmo::LateUpdate()
{
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	ImGuizmo::RecomposeMatrixFromComponents(
		m_tNewGizmoMatrix.matTranslation,
		m_tNewGizmoMatrix.matRotation,
		m_tNewGizmoMatrix.matScale, _objMat);


	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	if (useWindow)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		bool pOpen = false;
		ImGui::Begin("Gizmo", &pOpen, window_flags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImVec2 imageRect = ImGui::GetContentRegionAvail();

		//ImGui::SetWindowSize(ImVec2{ windowWidth, windowHeight });
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFontSize(), imageRect.x, imageRect.y);

		if (!ImGui::IsWindowCollapsed())
		{
			// TODO: Need To Change Camera proj
			// imgui화면에 랜더링하기
			ImVec2 textureRect = { imageRect.x, imageRect.y/* - ImGui::GetFontSize() * 2*/ };
			//imageRect.y -= ImGui::GetFontSize() * 2;
			CEngine::GetInstance()->ChangeProj(textureRect.x, textureRect.y);
			ImGui::Image((ImTextureID)(CEngine::GetInstance()->GetShaderResourceView()),
				imageRect, ImVec2(0, 0), ImVec2(1, 1));
		}

		//if (ImGui::IsWindowFocused())
		//{
		//	m_pGUIManager->AddLog("Gizmo Focused");
		//}
	}
	else
	{
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}

	//ImGuizmo::DrawGrid(_view, _projection, identityMatrix, 100.f);
	ImGuizmo::Manipulate(_view, _projection, m_CurrentGizmoOperation, mCurrentGizmoMode, _objMat, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	XMFLOAT4X4 objMat;
	memcpy(&objMat, _objMat, sizeof(XMFLOAT4X4));
	//objMat._11 = _objMat[0];
	//objMat._12 = _objMat[1];
	//objMat._13 = _objMat[2];
	//objMat._14 = _objMat[3];
	//objMat._21 = _objMat[4];
	//objMat._22 = _objMat[5];
	//objMat._23 = _objMat[6];
	//objMat._24 = _objMat[7];
	//objMat._31 = _objMat[8];
	//objMat._32 = _objMat[9];
	//objMat._33 = _objMat[10];
	//objMat._34 = _objMat[11];
	//objMat._41 = _objMat[12];
	//objMat._42 = _objMat[13];
	//objMat._43 = _objMat[14];
	//objMat._44 = _objMat[15];
	
	CEngine::GetInstance()->SetObjectMatrix(objMat);

	//if (ImGui::BeginDragDropTarget())
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
	//	{
	//		//string s = *(static_cast<std::string *>(payload->Data));
	//		//const int s = *(const int*)(payload->Data);
	//		const char* s = (const char*)(payload->Data);
	//		//string sdf = "ss" + to_string(s);

	//		m_pGUIManager->AddLog(s);
	//	}
	//	ImGui::EndDragDropTarget();
	//}


	if (useWindow)
	{
		ImGui::End();
		ImGui::PopStyleColor(1);
	}

	//ImGuizmo::IsUsing();

	CToolManager::SetImGuiStyle();
	CToolManager::SetImGuiColor();
}



void CGizmo::SetNewGizmoMatrix(const GIZMOMATRIX & tMat)
{
	m_tNewGizmoMatrix = tMat;
}

void CGizmo::Free()
{
}

