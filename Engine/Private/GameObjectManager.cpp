#include "..\public\GameObjectManager.h"
#include "Layer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameObjectManager)

CGameObjectManager::CGameObjectManager()
{
}

HRESULT CGameObjectManager::ReserveManager(_uint iNumScenes)
{
	if (nullptr != m_pGameObjects)
		return E_FAIL;

	m_pGameObjects = new GAMEOBJECTS[iNumScenes];

	m_iNumScenes = iNumScenes;

	return S_OK;
}

_int CGameObjectManager::Update(_double dDeltaTime)
{
	_int		iProgress = 0;

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pGameObjects[i])
		{
			iProgress = Pair.second->Update(dDeltaTime);
			if (0 > iProgress)
				return -1;
		}
	}

	return S_OK;
}

_int CGameObjectManager::LateUpdate(_double dDeltaTime)
{
	_int		iProgress = 0;

	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pGameObjects[i])
		{
			iProgress = Pair.second->LateUpdate(dDeltaTime);
			if (0 > iProgress)
				return -1;
		}
	}

	return S_OK;
}

HRESULT CGameObjectManager::AddPrototype(const string sPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype ||
		nullptr != FindPrototype(sPrototypeTag))
		return E_FAIL;

	m_pPrototypes.emplace(sPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CGameObjectManager::AddGameObject(_uint iLevelIndex, const string sPrototypeTag, const string pLayerTag, void * pArg)
{
	CGameObject*	pPrototype = FindPrototype(sPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = FindLayer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		CLayer*		pNewLayer = CLayer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;

		pNewLayer->AddGameObjectInLayer(pGameObject);

		m_pGameObjects[iLevelIndex].emplace(pLayerTag, pNewLayer);
	}
	else
		pLayer->AddGameObjectInLayer(pGameObject);


	return S_OK;
}

CGameObject * CGameObjectManager::FindPrototype(const string sPrototypeTag)
{
	auto iter = find_if(m_pPrototypes.begin(), m_pPrototypes.end(), STagFinder(sPrototypeTag));
	
	if (iter == m_pPrototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CGameObjectManager::FindLayer(_uint iSceneIndex, const string sLayerTag)
{
	auto iter = find_if(m_pGameObjects[iSceneIndex].begin(), m_pGameObjects[iSceneIndex].end(), STagFinder(sLayerTag));

	if (iter == m_pGameObjects[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

void CGameObjectManager::Free()
{
	for (_uint i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pGameObjects[i])
			SafeRelease(Pair.second);

		m_pGameObjects[i].clear();
	}

	SafeDeleteArray(m_pGameObjects);

	for (auto& Pair : m_pPrototypes)
		SafeRelease(Pair.second);

	m_pPrototypes.clear();
}



void CGameObjectManager::Clear(_uint iSceneIndex)
{
	if (iSceneIndex >= m_iNumScenes)
		return;

	for (auto& Pair : m_pGameObjects[iSceneIndex])
		SafeRelease(Pair.second);
	m_pGameObjects[iSceneIndex].clear();
}
