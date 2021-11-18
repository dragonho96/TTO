#include "stdafx.h"
#include "..\Public\Scene_Test.h"

#include "Engine.h"
#include "Background.h"
#include "VIBuffer.h"
#include "VIBuffer_LineSphere.h"

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
	//m_pEngine->PlaySoundW("CrashMan.mp3", CHANNELID::DIALOGUE);

	if (FAILED(ReadyPrototypeGameObject()))
		return E_FAIL;

	if (FAILED(ReadyLayerLineCircle(TEXT("LAYER_LINE"))))
		return E_FAIL;
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
	if (FAILED(m_pEngine->AddPrototype(TEXT("Prototype_LineCircle"), CBackground::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pEngine->AddPrototype(SCENE_STATIC, TEXT("Prototype_VIBuffer_LineSphere"), CVIBuffer_LineSphere::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Test::ReadyLayerLineCircle(const _tchar * pLayerTag)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	/* For.GameObject_BackGround */
	if (FAILED(pEngine->AddGameObject(SCENE_TEST, TEXT("Prototype_LineCircle"), pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

