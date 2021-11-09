#include "..\Public\Scene.h"

CScene::CScene(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	SafeAddRef(m_pDeviceContext);
	SafeAddRef(m_pDevice);
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

void CScene::Free()
{
	//SafeRelease(m_pDeviceContext);
	//SafeRelease(m_pDevice);
}
