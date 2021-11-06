#include "stdafx.h"
#include "..\Public\Gizmo.h"
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

CGizmo::CGizmo(CToolManager* pToolManager)
	: CImGuiWindow(pToolManager)
{
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
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	XMMATRIX viewMatrix = m_pEngine->GetViewMatrix();
	XMFLOAT4X4 fView;
	XMStoreFloat4x4(&fView, viewMatrix);
	_view[0] = fView._11;
	_view[1] = fView._12;
	_view[2] = fView._13;
	_view[3] = fView._14;
	_view[4] = fView._21;
	_view[5] = fView._22;
	_view[6] = fView._23;
	_view[7] = fView._24;
	_view[8] = fView._31;
	_view[9] = fView._32;
	_view[10] = fView._33;
	_view[11] = fView._34;
	_view[12] = fView._41;
	_view[13] = fView._42;
	_view[14] = fView._43;
	_view[15] = fView._44;

	XMMATRIX projMatrix = m_pEngine->GetProjectionMatrix();
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, projMatrix);
	_projection[0] = projection._11;
	_projection[1] = projection._12;
	_projection[2] = projection._13;
	_projection[3] = projection._14;
	_projection[4] = projection._21;
	_projection[5] = projection._22;
	_projection[6] = projection._23;
	_projection[7] = projection._24;
	_projection[8] = projection._31;
	_projection[9] = projection._32;
	_projection[10] = projection._33;
	_projection[11] = projection._34;
	_projection[12] = projection._41;
	_projection[13] = projection._42;
	_projection[14] = projection._43;
	_projection[15] = projection._44;

	XMMATRIX objMatrix = m_pEngine->GetObjectMatrix();
	XMFLOAT4X4 objMat;
	XMStoreFloat4x4(&objMat, objMatrix);
	_objMat[0] = objMat._11;
	_objMat[1] = objMat._12;
	_objMat[2] = objMat._13;
	_objMat[3] = objMat._14;
	_objMat[4] = objMat._21;
	_objMat[5] = objMat._22;
	_objMat[6] = objMat._23;
	_objMat[7] = objMat._24;
	_objMat[8] = objMat._31;
	_objMat[9] = objMat._32;
	_objMat[10] = objMat._33;
	_objMat[11] = objMat._34;
	_objMat[12] = objMat._41;
	_objMat[13] = objMat._42;
	_objMat[14] = objMat._43;
	_objMat[15] = objMat._44;

	ImGuizmo::SetID(0);
	//EditTransform(_view, _projection, _objMat, true);

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
		ImGui::SetWindowSize(ImVec2{ windowWidth, windowHeight });
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		if (!ImGui::IsWindowCollapsed())
		{
			// imgui화면에 랜더링하기
			ImVec2 imageRect = { ImGui::GetWindowSize().x , ImGui::GetWindowSize().y - (ImGui::GetFontSize() * 2) };
			ImTextureID texture = m_pEngine->GetShaderResourceView();
			
			ImGui::Image(texture, imageRect, ImVec2(0, 0), ImVec2(1, 1));
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
	objMat._11 = _objMat[0];
	objMat._12 = _objMat[1];
	objMat._13 = _objMat[2];
	objMat._14 = _objMat[3];
	objMat._21 = _objMat[4];
	objMat._22 = _objMat[5];
	objMat._23 = _objMat[6];
	objMat._24 = _objMat[7];
	objMat._31 = _objMat[8];
	objMat._32 = _objMat[9];
	objMat._33 = _objMat[10];
	objMat._34 = _objMat[11];
	objMat._41 = _objMat[12];
	objMat._42 = _objMat[13];
	objMat._43 = _objMat[14];
	objMat._44 = _objMat[15];
	
	m_pEngine->SetObjectMatrix(XMLoadFloat4x4(&objMat));

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

}



void CGizmo::SetNewGizmoMatrix(const GIZMOMATRIX & tMat)
{
	m_tNewGizmoMatrix = tMat;
}

