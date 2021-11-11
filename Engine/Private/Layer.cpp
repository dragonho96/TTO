#include "..\Public\Layer.h"
#include "GameObject.h"


CLayer::CLayer()
{
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer();
	return		pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
		SafeRelease(pGameObject);

	m_ObjectList.clear();
}

HRESULT CLayer::AddGameObjectInLayer(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return S_OK;
}

_uint CLayer::Update(_double dDeltaTime)
{
	_int		iProgress = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		iProgress = pGameObject->Update(dDeltaTime);
		if (0 > iProgress)
			return -1;
	}
	return _int();
}

_uint CLayer::LateUpdate(_double dDeltaTime)
{
	_int		iProgress = 0;

	for (auto& pGameObject : m_ObjectList)
	{
		iProgress = pGameObject->LateUpdate(dDeltaTime);
		if (0 > iProgress)
			return -1;
	}
	return _int();
}
