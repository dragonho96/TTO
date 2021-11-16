#include "..\Public\ImGuiManager.h"

IMPLEMENT_SINGLETON(CImGuiManager)

CImGuiManager::CImGuiManager()
{

}

void CImGuiManager::Free()
{
	for (auto& window : m_mapWindows)
		SafeDelete(window.second);

	m_mapWindows.clear();


	//ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();

	//ImGui::DestroyContext();
}

void CImGuiManager::Initialize(HWND hWnd, ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui_ImplWin32_Init(hWnd);
	//ImGui_ImplDX11_Init(device, deviceContext);
}

void CImGuiManager::Update()
{
	for (auto& window : m_mapWindows)
		window.second->Update();
	for (auto& window : m_mapWindows)
		window.second->LateUpdate();
}

void CImGuiManager::AddWindow(string name, CImGuiWindow * window)
{
	auto iter_find = m_mapWindows.find(name);
	if (m_mapWindows.end() != iter_find)
		return;

	m_mapWindows[name] = window;
}

CImGuiWindow * CImGuiManager::GetWindow(string name)
{
	auto iter_find = m_mapWindows.find(name);
	if (m_mapWindows.end() == iter_find)
		return nullptr;

	return iter_find->second;
}
