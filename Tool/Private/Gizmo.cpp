#include "stdafx.h"
#include "..\Public\Gizmo.h"
#include "ToolManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "EmptyGameObject.h"
#include "EmptyUI.h"
#include "Log.h"

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
	_float3 vec = CEngine::GetInstance()->GetMousePosition();
	string s = "" + to_string(vec.x) + " : " + to_string(vec.y);
	dynamic_cast<CLog*>(CEngine::GetInstance()->GetWindow("Log"))->AddLog(s.c_str());
	CToolManager::SetImGuizmoStyle();
	CToolManager::SetImGuiColor();

	if (nullptr == g_pObjFocused)
		return;

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	if (CEngine::GetInstance()->IsKeyDown('W'))
		m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (CEngine::GetInstance()->IsKeyDown('E'))
		m_CurrentGizmoOperation = ImGuizmo::ROTATE;
	if (CEngine::GetInstance()->IsKeyDown('R'))
		m_CurrentGizmoOperation = ImGuizmo::SCALE;

	XMMATRIX viewMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW);
	XMFLOAT4X4 fView;
	XMStoreFloat4x4(&fView, viewMatrix);
	memcpy(_view, &fView, sizeof(XMFLOAT4X4));

	XMMATRIX projMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_PROJ);
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, projMatrix);
	memcpy(_projection, &projection, sizeof(XMFLOAT4X4));
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
	imageRect = ImGui::GetContentRegionAvail();
	winPos = ImGui::GetWindowPos();

	if (!ImGui::IsWindowCollapsed())
	{
		// TODO: Need To Change Camera proj
		// imgui화면에 랜더링하기
		ImVec2 textureRect = { imageRect.x, imageRect.y/* - ImGui::GetFontSize() * 2*/ };
		//imageRect.y -= ImGui::GetFontSize() * 2;
		CEngine::GetInstance()->ChangeProj(textureRect.x, textureRect.y);
		ImGui::Image((ImTextureID)(CEngine::GetInstance()->GetShaderResourceView()),
			imageRect, ImVec2(0, 0), ImVec2(1, 1));
		CEngine::GetInstance()->ChangeProj(imageRect.x, imageRect.y);
	}

	if (g_pObjFocused)
	{
		if (dynamic_cast<CEmptyGameObject*>(g_pObjFocused))
			ManipulateGameObject();
		else
			ManipulateUI();

	}

	ImGui::End();
	ImGui::PopStyleColor(1);

	CToolManager::SetImGuiStyle();
	CToolManager::SetImGuiColor();
}

void CGizmo::ManipulateGameObject()
{
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(winPos.x, winPos.y + ImGui::GetFontSize(), imageRect.x, imageRect.y);

	ImGuizmo::Manipulate(_view, _projection, m_CurrentGizmoOperation, mCurrentGizmoMode, _objMat, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	XMFLOAT4X4 objMat;
	memcpy(&objMat, _objMat, sizeof(XMFLOAT4X4));

	// TODO : FIX HERE
	CComponent* pObjTransform = nullptr;
	if (!(pObjTransform = g_pObjFocused->GetComponent(TEXT("Com_Transform"))))
		MSG_BOX("Failed to Get Transform");

	dynamic_cast<CTransform*>(pObjTransform)->SetMatrix(objMat);
}

void CGizmo::ManipulateUI()
{
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(winPos.x, winPos.y + ImGui::GetFontSize(), imageRect.x, imageRect.y);

	//XMMATRIX viewMatrix = XMMatrixIdentity();
	//XMFLOAT4X4 fView;
	//viewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	//XMStoreFloat4x4(&fView, viewMatrix);
	//memcpy(_view, &fView, sizeof(XMFLOAT4X4));

	/* TODO : Get WinSize */
	XMMATRIX projMatrix = XMMatrixOrthographicLH(1280, 720, 0.0f, 1.f);
	//XMMATRIX projMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_PROJ);
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, projMatrix);
	memcpy(_projection, &projection, sizeof(XMFLOAT4X4));


	ImGuizmo::Manipulate(_view, _projection, m_CurrentGizmoOperation, mCurrentGizmoMode, _objMat, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	XMFLOAT4X4 objMat;
	memcpy(&objMat, _objMat, sizeof(XMFLOAT4X4));

	CComponent* pObjTransform = nullptr;
	if (!(pObjTransform = g_pObjFocused->GetComponent(TEXT("Com_Transform"))))
		MSG_BOX("Failed to Get Transform");

	dynamic_cast<CRectTransform*>(pObjTransform)->SetTransformMat(objMat);

}

void CGizmo::SetObjMat(float* mat)
{
	memcpy(_objMat, mat, sizeof(float) * 16);

	//if (ImGui::IsWindowFocused())
	//{
	//	m_pGUIManager->AddLog("Gizmo Focused");
	//}

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
}



void CGizmo::SetNewGizmoMatrix(const GIZMOMATRIX & tMat)
{
	m_tNewGizmoMatrix = tMat;
}

void CGizmo::Free()
{
}

