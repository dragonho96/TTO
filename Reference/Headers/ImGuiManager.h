#pragma once
#include "Base.h"
#include "ImGuiWindow.h"
BEGIN(Engine)

class CImGuiManager : public CBase
{
	DECLARE_SINGLETON(CImGuiManager)
private:
	CImGuiManager();
	virtual ~CImGuiManager() = default;
	virtual void Free() override;

public:
	void Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update();

public:
	void AddWindow(string name, CImGuiWindow* window);
	CImGuiWindow* GetWindow(string name);

private:
	map<string, CImGuiWindow*>	m_mapWindows;
};

END