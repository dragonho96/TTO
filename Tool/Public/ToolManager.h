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

public:
	void CreateWindows();

public:
	CImGuiWindow* GetWindow(string windowName);

private:
	HWND					m_hWnd;
	CEngine*				m_pEngine;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;

	//map<string, std::shared_ptr<CImGuiWindow>>	m_mapWindows;
	map<string, CImGuiWindow*>	m_mapWindows;
};

