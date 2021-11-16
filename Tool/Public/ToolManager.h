#pragma once

BEGIN(Tool)
class CToolManager
{
	enum SCENE { SCENE_TOOL, SCENE_END };
public:
	explicit CToolManager();
	virtual ~CToolManager();

public:
	HRESULT Initialize();
	void Update(_double dDeltaTime);
	void Render();
	void Release();

private:
	HRESULT OpenScene(SCENE eScene);
	HRESULT ReadyPrototypeComponent();

public:
	void InitializeImGui();
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
	CRenderer*				m_pRenderer = nullptr;
};

END