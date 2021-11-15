#include "stdafx.h"
#include "..\Public\Scene_Test.h"

#include "Engine.h"
#include "Background.h"

USING(Client)

CScene_Test::CScene_Test(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
	: CScene(pDevice, pDeviceContext, iSceneIndex)
{
}

CScene_Test * CScene_Test::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
{
	CScene_Test*		pInstance = new CScene_Test(pDevice, pDeviceContext, iSceneIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Test");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Test::Free()
{
	__super::Free();
}

HRESULT CScene_Test::Initialize()
{
	__super::Initialize();
	m_pEngine->PlaySoundW("CrashMan.mp3", CHANNELID::DIALOGUE);
	return S_OK;
}

_uint CScene_Test::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	return _uint();
}

HRESULT CScene_Test::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Test::ReadyPrototypeGameObject()
{

	/* Prototype_BackGround */
	//if (FAILED(m_pEngine->AddPrototype(TEXT("Prototype_BackGround"), CBackground::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CScene_Test::ReadyLayerBackGround(const _tchar * pLayerTag)
{
	// Add GameObject
	return S_OK;
}

