#include "stdafx.h"
#include "..\public\Scene_Loading.h"

#include "Engine.h"
#include "Loader.h"
#include "Scene_GamePlay.h"

CScene_Loading::CScene_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{

}

HRESULT CScene_Loading::Initialize(SCENE eScene)
{
	__super::Initialize();

	m_pLoader = CLoader::Create(eScene);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_eNextSCENE = eScene;

	return S_OK;
}

_uint CScene_Loading::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	if (true == m_pLoader->Get_Finish())
	{
		CEngine*		pEngine = GET_INSTANCE(CEngine);

		SetWindowText(g_hWnd, TEXT("·Îµù ³¡!!!!!!!!!!!"));
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CScene*		pScene = nullptr;

			switch (m_eNextSCENE)
			{
			case SCENE_GAMEPLAY:
				pScene = CScene_GamePlay::Create(m_pDevice, m_pDeviceContext);
				break;

				/*case SCENE_GAMEPLAY1:
				break;*/
			}
			if (FAILED(pEngine->SetUpCurrentScene(pScene)))
				return E_FAIL;
		}

		RELEASE_INSTANCE(CEngine);
	}

	return _uint();
}

HRESULT CScene_Loading::Render()
{
	__super::Render();

	return S_OK;
}

CScene_Loading * CScene_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, SCENE eScene)
{
	CScene_Loading*		pInstance = new CScene_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(eScene)))
	{
		MSG_BOX("Failed to Creating CScene_Loading");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Loading::Free()
{
	__super::Free();

	SafeRelease(m_pLoader);
}
