#include "..\public\GameObjectManager.h"

IMPLEMENT_SINGLETON(CGameObjectManager)

CGameObjectManager::CGameObjectManager()
{
}

HRESULT CGameObjectManager::ReserveManager(_uint iNumLevels)
{
	//if (nullptr != m_pGameObjects)
	//	return E_FAIL;

	//m_pGameObjects = new GAMEOBJECTS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void CGameObjectManager::Free()
{
	//for (_uint i = 0; i < m_iNumLevels; ++i)
	//{
	//	for (auto& Pair : m_pGameObjects[i])
	//		SafeRelease(Pair.second);

	//	m_pGameObjects[i].clear();
	//}

	//SafeDeleteArray(m_pGameObjects);
}
