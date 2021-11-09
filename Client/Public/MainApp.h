#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CEngine;
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

private:
	HRESULT OpenScene(SCENE eScene);

private:
	CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

#ifdef _DEBUG
private:
	_tchar		m_szFPS[MAX_PATH] = TEXT("");
	_uint		m_iNumDraw = 0;
	_double		m_TimeAcc = 0.0;
#endif

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END