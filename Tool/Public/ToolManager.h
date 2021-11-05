#pragma once

USING(Engine)
class CImGuiWindow;
class CToolManager
{
public:
	CToolManager();
	~CToolManager();

public:
	void Initialize();
	void Update();
	void Release();

public:
	void SetImGuiStyle();
	void SetImGuiColor();
	void SetDockSpace();

	void SetGizmo();

public:
	void CreateWindows();

	void ContentBrowser();

public:
	CImGuiWindow* GetWindow(string windowName);

private:
	HWND					m_hWnd;
	CEngine*				m_pEngine;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	IDXGISwapChain*         m_pSwapChain = NULL;
	ID3D11RenderTargetView* m_pRenderTargetView = NULL;
	ID3D11DepthStencilView* m_pDepthStencil = NULL;

	map<string, class CImGuiWindow*>	m_mapWindows;
};

