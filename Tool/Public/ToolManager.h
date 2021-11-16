#pragma once

BEGIN(Tool)
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
	static void SetImGuiStyle();
	static void SetImGuizmoStyle();
	static void SetImGuiColor();
	void SetDockSpace();

public:
	void CreateWindows();

public:
	void AddLog(const char* log);

private:
	HWND					m_hWnd;
	CEngine*				m_pEngine;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
};

END