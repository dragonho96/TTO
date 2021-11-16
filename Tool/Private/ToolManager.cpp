#include "stdafx.h"
#include "..\Public\ToolManager.h"
#include "ImGuiWindow.h"
#include "Scene_Tool.h"

#pragma region IMGUIWINDOWS
#include "Log.h"
#include "ContentBrowser.h"
#include "Gizmo.h"
#include "Inspector.h"
#include "Hierarchy.h"
#pragma endregion
USING(Tool)
CToolManager::CToolManager()
	: m_pEngine(CEngine::GetInstance())
{
	SafeAddRef(m_pEngine);
}


CToolManager::~CToolManager()
{
	Release();
}

HRESULT CToolManager::Initialize()
{
	if (nullptr == m_pEngine)
		return E_FAIL;

	if (FAILED(m_pEngine->ReadyDevice(g_hWnd, WINCX, WINCY)))
		return E_FAIL;

	m_pDevice = m_pEngine->GetDevice();
	m_pDeviceContext = m_pEngine->GetDeviceContext();
	m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext);

	if (FAILED(m_pEngine->Initialize(SCENE_END)))
		return E_FAIL;
	if (FAILED(ReadyPrototypeComponent()))
		return E_FAIL;

	if (FAILED(OpenScene(SCENE_TOOL)))
		return E_FAIL;

	InitializeImGui();

	return S_OK;
	//m_pEngine->DeserializeScene("../../Assets/Scenes/test.yaml");
}

void CToolManager::Update(_double dDeltaTime)
{
	if (g_Done)
		return;

	m_pEngine->Update(dDeltaTime);
	m_pEngine->UpdateScene(dDeltaTime);


	SetImGuiColor();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	SetDockSpace();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);

	m_pEngine->UpdateImGui();
}

void CToolManager::Render()
{
	if (g_Done)
		return;

	m_pRenderer->DrawRenderGroup();
	m_pEngine->Render();
	//// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	m_pEngine->Present();

	return;
}

void CToolManager::Release()
{
	dynamic_cast<CLog*>(m_pEngine->GetWindow("Log"))->ClearLog();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SafeRelease(m_pRenderer);
	SafeRelease(m_pEngine);
	CEngine::ReleaseEngine();
}

HRESULT CToolManager::OpenScene(SCENE eScene)
{
	CScene*			pScene = nullptr;

	pScene = CScene_Tool::Create(m_pDevice, m_pDeviceContext, SCENE_TOOL);

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pEngine->SetUpCurrentScene(pScene)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolManager::ReadyPrototypeComponent()
{
	return S_OK;
}

void CToolManager::InitializeImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	//ImGuizmo::SetImGuiContext(ImGui::CreateContext());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

																// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);

	// Set ImGui Style
	SetImGuiStyle();
	SetImGuiColor();

	CreateWindows();
}

void CToolManager::SetImGuiStyle()
{
	ImGui::StyleColorsDark();

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(2, 2);
	style->WindowRounding = 2.0f;
	style->FramePadding = ImVec2(2, 2);
	style->WindowBorderSize = 2;
	style->ChildBorderSize = 2;
	style->PopupBorderSize = 2;
	style->FrameBorderSize = 2;
	style->TabBorderSize = 2;
	style->GrabMinSize = 2;
}

void CToolManager::SetImGuizmoStyle()
{
	ImGui::StyleColorsDark();

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(0, 0);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(0, 0);
	style->WindowBorderSize = 0;
	style->ChildBorderSize = 0;
	style->PopupBorderSize = 0;
	style->FrameBorderSize = 0;
	style->TabBorderSize = 0;
	style->GrabMinSize = 0;
	style->TouchExtraPadding = ImVec2(0, 0);
}

void CToolManager::SetImGuiColor()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.9f, 0.9f, 0.9f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.88f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.45f, 0.46f, 0.46f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.36f, 0.37f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.61f, 0.62f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.49f, 0.51f, 0.52f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.53f, 0.53f, 0.54f, 0.84f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.61f, 0.61f, 0.62f, 0.22f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.61f, 0.62f, 0.62f, 0.51f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.61f, 0.62f, 0.62f, 0.83f);
	colors[ImGuiCol_Separator] = ImVec4(0.44f, 0.44f, 0.44f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.61f, 0.61f, 0.61f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.23f, 0.23f, 0.23f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.22f, 0.22f, 0.22f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.57f, 0.57f, 0.57f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	//colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
	//colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.59f, 0.59f, 0.59f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void CToolManager::SetDockSpace()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		window_flags |= ImGuiWindowFlags_NoBackground;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
	}

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	bool p_open = true;
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	ImGui::End();
}

void CToolManager::CreateWindows()
{
	//std::shared_ptr<CImGuiWindow> pWindow = std::make_shared<CLog>(this);
	//m_mapWindows["Log"] = pWindow;

	//pWindow = std::make_shared<CContentBrowser>(this);
	//m_mapWindows["ContentBrowser"] = pWindow;

	//pWindow = std::make_shared<CGizmo>(this);
	//m_mapWindows["Gizmo"] = pWindow;

	//pWindow = std::make_shared<CInspector>(this);
	//m_mapWindows["Inspector"] = pWindow;

	CImGuiWindow* pWindow = new CContentBrowser();
	m_pEngine->AddWindow("ContentBrowser", pWindow);

	pWindow = new CLog();
	m_pEngine->AddWindow("Log", pWindow);

	pWindow = new CGizmo();
	m_pEngine->AddWindow("Gizmo", pWindow);

	pWindow = new CInspector();
	m_pEngine->AddWindow("Inspector", pWindow);

	pWindow = new CHierarchy();
	m_pEngine->AddWindow("Hierarchy", pWindow);

}

void CToolManager::AddLog(const char * log)
{
	dynamic_cast<CLog*>(m_pEngine->GetWindow("Log"))->AddLog(log);
}

