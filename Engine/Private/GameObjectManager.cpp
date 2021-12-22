#include "..\public\GameObjectManager.h"
#include "Layer.h"
#include "GameObject.h"
#include "SceneSerializer.h"

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

HRESULT CGameObjectManager::AddPrototype(string pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype ||
		nullptr != FindPrototype(pPrototypeTag))
		return E_FAIL;

	m_pPrototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CGameObject* CGameObjectManager::AddGameObject(_uint iSceneIndex, string pPrototypeTag, string pLayerTag, void * pArg)
{
	CGameObject*	pPrototype = FindPrototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	CLayer*		pLayer = FindLayer(iSceneIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		CLayer*		pNewLayer = CLayer::Create();
		if (nullptr == pNewLayer)
			return nullptr;

		pNewLayer->AddGameObjectInLayer(pGameObject);

		m_pGameObjects[iSceneIndex].emplace(pLayerTag, pNewLayer);
	}
	else
		pLayer->AddGameObjectInLayer(pGameObject);


	return pGameObject;
}

list<class CGameObject*> CGameObjectManager::GetGameObjectInLayer(_uint iSceneIndex, string pLayerTag)
{
	CLayer* pLayer = FindLayer(iSceneIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		//MSG_BOX("Failed to GetGameObjectInLayer");
		return list<class CGameObject*>();
	}

	return pLayer->GetGameObjectList();
}

CGameObject * CGameObjectManager::FindPrototype(string pPrototypeTag)
{
	auto iter = find_if(m_pPrototypes.begin(), m_pPrototypes.end(), STagFinder(pPrototypeTag));
	
	if (iter == m_pPrototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CGameObjectManager::FindLayer(_uint iSceneIndex, string pLayerTag)
{
	auto iter = find_if(m_pGameObjects[iSceneIndex].begin(), m_pGameObjects[iSceneIndex].end(), STagFinder(pLayerTag));

	if (iter == m_pGameObjects[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

CGameObject * CGameObjectManager::FindGameObjectWithUUID(uint64_t uuid)
{
	auto find = m_pUUIDObjects.find(uuid);
	if (find != m_pUUIDObjects.end())
		return find->second;

	return nullptr;
}

CGameObject * CGameObjectManager::FindGameObjectWithName(string name)
{
	auto find = m_pNameObjects.find(name);
	if (find != m_pNameObjects.end())
		return find->second;

	return nullptr;
}

void CGameObjectManager::AddPrefab(string name, YAML::Node node)
{
	m_pPrefabs.emplace(name, node);
}

CGameObject * CGameObjectManager::SpawnPrefab(string name)
{
	CSceneSerializer serializer;
	if (m_pPrefabs[name])
		return serializer.SpawnPrefab(m_pPrefabs[name]);

	return nullptr;
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
	{
		//SafeDeleteArray(Pair.first);
		SafeRelease(Pair.second);
	}
	m_pGameObjects[iSceneIndex].clear();
}
