#include "..\public\ComponentManager.h"

IMPLEMENT_SINGLETON(CComponentManager)

CComponentManager::CComponentManager()
{

}

HRESULT CComponentManager::ReserveManager(_uint iNumScenes)
{

	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumScenes];

	m_iNumScenes = iNumScenes;

	return S_OK;
}

HRESULT CComponentManager::AddPrototype(_uint iSceneIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == pPrototype ||
		iSceneIndex >= m_iNumScenes ||
		nullptr != FindPrototype(iSceneIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iSceneIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponentManager::CloneComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, void* pArg)
{
	if (iSceneIndex >= m_iNumScenes)
		return nullptr;

	CComponent*	pPrototype = FindPrototype(iSceneIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);
}

void CComponentManager::Clear(_uint iSceneIndex)
{
	if (iSceneIndex >= m_iNumScenes)
		return;

	for (auto& Pair : m_pPrototypes[iSceneIndex])
		SafeRelease(Pair.second);

	m_pPrototypes[iSceneIndex].clear();
}

CComponent * CComponentManager::FindPrototype(_uint iSceneIndex, const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iSceneIndex].begin(), m_pPrototypes[iSceneIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iSceneIndex].end())
		return nullptr;

	return iter->second;
}

void CComponentManager::Free()
{
	for (size_t i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			SafeRelease(Pair.second);

		m_pPrototypes[i].clear();
	}
	SafeDeleteArray(m_pPrototypes);
}