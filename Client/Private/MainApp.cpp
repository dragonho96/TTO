#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "Engine.h"
#include "Scene_Logo.h"
#include "Scene_Loading.h"

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
	SafeRelease(m_pDeviceContext);
	SafeRelease(m_pDevice);

	SafeRelease(m_pEngine);
	CEngine::ReleaseEngine();
}

HRESULT CMainApp::Initialize()
{
	if (nullptr == m_pEngine)
		return E_FAIL;

	if (FAILED(m_pEngine->ReadyDevice(g_hWnd, g_iWinCX, g_iWinCY)))
		return E_FAIL;
	m_pDevice = m_pEngine->GetDevice();
	m_pDeviceContext = m_pEngine->GetDeviceContext();

	if (FAILED(OpenScene(SCENE_GAMEPLAY)))
		return E_FAIL;



	return S_OK;
}

_uint CMainApp::Update(_double dDeltaTime)
{
	m_TimeAcc += dDeltaTime;
	m_pEngine->UpdateScene(dDeltaTime);

	return 0;
}

HRESULT CMainApp::Render()
{
	m_pEngine->ClearBackBufferView(_float4(0.f, 0.f, 1.f, 1.f));
	m_pEngine->ClearDepthStencilView(1.f, 0);

	/* 필요한 객체들을 백버퍼에 그린다. */

	m_pEngine->Present();

	++m_iNumDraw;

	if (m_TimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		SetWindowText(g_hWnd, m_szFPS);

		m_TimeAcc = 0.0;
		m_iNumDraw = 0;
	}

	return S_OK;
}

HRESULT CMainApp::OpenScene(SCENE eScene)
{
	CScene*			pScene = nullptr;

	switch (eScene)
	{
	case SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pDevice, m_pDeviceContext);
		break;
	case SCENE_GAMEPLAY:
		pScene = CScene_Loading::Create(m_pDevice, m_pDeviceContext, eScene);
		break;
	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pEngine->SetUpCurrentScene(pScene)))
		return E_FAIL;

	return S_OK;
}

