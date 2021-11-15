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

	if (FAILED(m_pEngine->Initialize(SCENE_END)))
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
	if (m_pEngine->IsKeyDown('A'))
		ADDLOG("hihi")

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

	/* 필요한 객체들을 백버퍼에 그린다. */
	m_pRenderer->DrawRenderGroup();
	m_pEngine->RenderScene();

	//m_pEngine->RenderClient();
	
	// ImGui
	m_pEngine->UpdateImGui();



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
	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, TEXT("Prototype_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	SafeAddRef(m_pRenderer);

	return S_OK;
}

