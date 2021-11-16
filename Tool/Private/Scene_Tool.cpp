#include "stdafx.h"
#include "..\Public\Scene_Tool.h"

USING(Tool)

CScene_Tool::CScene_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
	: CScene(pDevice, pDeviceContext, iLevelIndex)
{
}


HRESULT CScene_Tool::Initialize()
{
	__super::Initialize();

	if (FAILED(ReadyPrototypeGameObject()))
		return E_FAIL;

	//if (FAILED(ReadyLayerBackGround(TEXT("LAYER_BACKGROUND"))))
	//	return E_FAIL;

	return S_OK;
}

_uint CScene_Tool::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	return _uint();
}

HRESULT CScene_Tool::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScene_Tool::ReadyPrototypeGameObject()
{
	//CEngine*	pEngine = GET_INSTANCE(CEngine);

	///* Prototype_BackGround */
	//if (FAILED(pEngine->AddPrototype(TEXT("Prototype_BackGround"), CBackground::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	//RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CScene_Tool::ReadyLayerBackGround(const _tchar * pLayerTag)
{
	return S_OK;
}

CScene_Tool * CScene_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
{
	CScene_Tool*		pInstance = new CScene_Tool(pDevice, pDeviceContext, iLevelIndex);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Tool");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Tool::Free()
{
	__super::Free();
}
