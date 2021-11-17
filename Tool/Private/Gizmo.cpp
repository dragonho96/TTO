#include "stdafx.h"
#include "..\Public\Gizmo.h"
#include "ToolManager.h"
#include "GameObject.h"
#include "Transform.h"

CGameObject* g_pObjFocused = nullptr;

static ImGuizmo::OPERATION m_CurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
static bool useSnap = false;
static float snap[3] = { 1.f, 1.f, 1.f };
static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
static bool boundSizing = false;
static bool boundSizingSnap = false;

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
	ZeroMemory(&m_tGizmoMatrix, sizeof(GIZMOMATRIX));
	ZeroMemory(&m_tNewGizmoMatrix, sizeof(GIZMOMATRIX));
	m_tGizmoMatrix.matScale[0] = 1;
	m_tGizmoMatrix.matScale[1] = 1;
	m_tGizmoMatrix.matScale[2] = 1;
	m_tNewGizmoMatrix.matScale[0] = 1;
	m_tNewGizmoMatrix.matScale[1] = 1;
	m_tNewGizmoMatrix.matScale[2] = 1;
	XMFLOAT4X4 fMatIdentity;
	XMStoreFloat4x4(&fMatIdentity, XMMatrixIdentity());
	memcpy(_objMat, &fMatIdentity, sizeof(XMFLOAT4X4));
}

void CGizmo::Update()
{
	CToolManager::SetImGuizmoStyle();
	CToolManager::SetImGuiColor();

	if (nullptr == g_pObjFocused)
		return;

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

	/* NEED TO BE FIXED */
	CComponent* pObjTransform = nullptr;
	if (!(pObjTransform = g_pObjFocused->GetComponent(TEXT("Com_Transform"))))
		MSG_BOX("Failed to Get Transform");

	XMFLOAT4X4 objMat = dynamic_cast<CTransform*>(pObjTransform)->GetMatrix();
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
	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoScrollbar;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
	bool pOpen = false;
	ImGui::Begin("Gizmo", &pOpen, window_flags);
	//ImGuizmo::SetDrawlist();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	ImVec2 imageRect = ImGui::GetContentRegionAvail();

	//ImGui::SetWindowSize(ImVec2{ windowWidth, windowHeight });
	//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFontSize(), imageRect.x, imageRect.y);

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





	if (nullptr != g_pObjFocused)
	{
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFontSize(), imageRect.x, imageRect.y);

		ImGuizmo::RecomposeMatrixFromComponents(
			m_tNewGizmoMatrix.matTranslation,
			m_tNewGizmoMatrix.matRotation,
			m_tNewGizmoMatrix.matScale, _objMat);

		//ImGuizmo::DrawGrid(_view, _projection, identityMatrix, 100.f);
		ImGuizmo::Manipulate(_view, _projection, m_CurrentGizmoOperation, mCurrentGizmoMode, _objMat, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
		XMFLOAT4X4 objMat;
		memcpy(&objMat, _objMat, sizeof(XMFLOAT4X4));

		// FIX HERE
		CComponent* pObjTransform = nullptr;
		if (!(pObjTransform = g_pObjFocused->GetComponent(TEXT("Com_Transform"))))
			MSG_BOX("Failed to Get Transform");

		dynamic_cast<CTransform*>(pObjTransform)->SetMatrix(objMat);
	}

	//CEngine::GetInstance()->SetObjectMatrix(objMat);


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

	ImGui::End();
	ImGui::PopStyleColor(1);

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

