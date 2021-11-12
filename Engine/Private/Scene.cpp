#include "..\Public\Scene.h"
#include "Engine.h"

CScene::CScene(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iLevelIndex)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_iLevelIndex(iLevelIndex)
{
	//SafeAddRef(m_pDeviceContext);
	//SafeAddRef(m_pDevice);
}

HRESULT CScene::Initialize()
{
	return S_OK;
}

_uint CScene::Update(_double TimeDelta)
{
	return _uint();
}

HRESULT CScene::Render()
{
	return S_OK;
}

void CScene::Clear()
{
	CEngine*		pEngine = GET_INSTANCE(CEngine);

	pEngine->Clear(m_iLevelIndex);

	RELEASE_INSTANCE(CEngine);
}

void CScene::Free()
{
	//SafeRelease(m_pDeviceContext);
	//SafeRelease(m_pDevice);
}
