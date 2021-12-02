#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "Engine.h"
#include "Scene_Logo.h"
#include "Scene_Loading.h"
#include "Scene_Test.h"

#include "Log.h"

CMainApp::CMainApp()
	: m_pEngine(CEngine::GetInstance())
{
	SafeAddRef(m_pEngine);
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CMainApp");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SafeRelease(m_pRenderer);

	SafeRelease(m_pEngine);
	CEngine::ReleaseEngine();
}

HRESULT CMainApp::Initialize()
{
	if (nullptr == m_pEngine)
		return E_FAIL;

	if (FAILED(m_pEngine->ReadyDevice(g_hWnd, WINCX, WINCY)))
		return E_FAIL;

	m_pDevice = m_pEngine->GetDevice();
	m_pDeviceContext = m_pEngine->GetDeviceContext();

	if (FAILED(m_pEngine->Initialize(SCENE_END, CEngine::USAGE::USAGE_CLIENT)))
		return E_FAIL;

	if (FAILED(ReadyPrototypeComponent()))
		return E_FAIL;

	if (FAILED(OpenScene(SCENE_TEST)))
		return E_FAIL;

	ImGuiInitialize();

	return S_OK;
}

_uint CMainApp::Update(_double dDeltaTime)
{
#ifdef _DEBUG
	m_TimeAcc += dDeltaTime;
#endif
	if (g_Done)
		return _uint();


	m_pEngine->Update(dDeltaTime);
	m_pEngine->UpdateScene(dDeltaTime);

	// TODO: Fix Update Order
	m_pEngine->UpdatePx(dDeltaTime);

	//if (m_pEngine->IsKeyDown('A'))
	//	ADDLOG("hihi")

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return 0;
}

HRESULT CMainApp::Render()
{
	if (g_Done)
		return _uint();

	m_pEngine->ClearBackBufferView(_float4(0.f, 0.f, 1.f, 1.f));
	m_pEngine->ClearDepthStencilView(1.f, 0);
	m_pEngine->RenderClient();
	/* 필요한 객체들을 백버퍼에 그린다. */
	m_pRenderer->DrawRenderGroup();
	m_pEngine->RenderScene();
	m_pEngine->RenderFont();
	// ImGui
	m_pEngine->UpdateImGui();
	// ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	m_pEngine->Present();

#ifdef _DEBUG
	++m_iNumDraw;

	if (m_TimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		SetWindowText(g_hWnd, m_szFPS);

		m_TimeAcc = 0.0;
		m_iNumDraw = 0;
	}
#endif

	return S_OK;
}

void CMainApp::ImGuiInitialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
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
	
	//// Log Window
	CImGuiWindow* pWindow = new CLog();
	m_pEngine->AddWindow("Log", pWindow);

	SetImGuiStyle();
}

void CMainApp::SetImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 4.0f;
}



HRESULT CMainApp::OpenScene(SCENE eScene)
{
	CScene*			pScene = nullptr;

	switch (eScene)
	{
	case SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pDevice, m_pDeviceContext, SCENE_LOGO);
		break;
	case SCENE_TEST:
		pScene = CScene_Test::Create(m_pDevice, m_pDeviceContext, SCENE_TEST);
		break;
	case SCENE_GAMEPLAY:
		pScene = CScene_Loading::Create(m_pDevice, m_pDeviceContext, eScene, SCENE_LOADING);
		break;

	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pEngine->SetUpCurrentScene(pScene)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::ReadyPrototypeComponent()
{
	if (nullptr == m_pEngine)
		return E_FAIL;

	// For.Prototype_Renderer
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, "Prototype_Renderer", m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	SafeAddRef(m_pRenderer);



	return S_OK;
}

