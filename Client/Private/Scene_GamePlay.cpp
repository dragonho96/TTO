#include "stdafx.h"
#include "..\public\Scene_GamePlay.h"

CScene_GamePlay::CScene_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
	: CScene(pDevice, pDeviceContext, iLevelIndex)
{

}

HRESULT CScene_GamePlay::Initialize()
{
	__super::Initialize();


	return S_OK;
}

_uint CScene_GamePlay::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	return _uint();
}

HRESULT CScene_GamePlay::Render()
{
	__super::Render();

	return S_OK;
}

CScene_GamePlay * CScene_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
{
	CScene_GamePlay*		pInstance = new CScene_GamePlay(pDevice, pDeviceContext, iLevelIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_GamePlay");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_GamePlay::Free()
{
	__super::Free();

}