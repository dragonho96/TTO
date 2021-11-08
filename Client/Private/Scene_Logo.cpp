#include "stdafx.h"
#include "..\Public\Scene_Logo.h"


CScene_Logo::CScene_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CScene(pDevice, pDeviceContext)
{

}

HRESULT CScene_Logo::Initialize()
{
	__super::Initialize();


	return S_OK;
}

_uint CScene_Logo::Update(_double TimeDelta)
{
	__super::Update(TimeDelta);

	return _uint();
}

HRESULT CScene_Logo::Render()
{
	__super::Render();

	return S_OK;
}

CScene_Logo * CScene_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Logo*		pInstance = new CScene_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Creating CScene_Logo");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	__super::Free();

}
