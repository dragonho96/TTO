#include "..\Public\Scene.h"
#include "Engine.h"

CScene::CScene(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iSceneIndex)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_iSceneIndex(iSceneIndex)
{
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

	pEngine->ClearGameObjectManager(m_iSceneIndex);
	pEngine->ClearComponentManager(m_iSceneIndex);

	RELEASE_INSTANCE(CEngine);
}

void CScene::Free()
{

}
