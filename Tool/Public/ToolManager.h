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
	void SetImGuizmoStyle();
	void SetImGuiColor();
	void SetDockSpace();

public:
	void CreateWindows();

public:
	CImGuiWindow* GetWindow(string windowName);

public:
	void AddLog(const char* log);

private:
	HWND					m_hWnd;
	CEngine*				m_pEngine;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;

	//map<string, std::shared_ptr<CImGuiWindow>>	m_mapWindows;
	map<string, CImGuiWindow*>	m_mapWindows;
};

