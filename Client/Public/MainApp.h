#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CEngine;
class CRenderer;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	_uint	Update(_double dDeltaTime);
	HRESULT Render();
public:
	void ImGuiInitialize();

private:
	HRESULT OpenScene(SCENE eScene);
	HRESULT ReadyPrototypeComponent();

private:
	CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CRenderer*				m_pRenderer = nullptr;

#ifdef _DEBUG
private:
	_tchar		m_szFPS[MAX_PATH] = TEXT("");
	_uint		m_iNumDraw = 0;
	_double		m_TimeAcc = 0.0;
#endif
private:
	string		m_sLog;
public:
	static CMainApp* Create();
	virtual void Free() override;
};

END