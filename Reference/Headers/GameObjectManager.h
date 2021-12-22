#pragma once

#include "Base.h"
#include "EmptyGameObject.h"
#include "EmptyUI.h"
/* 객체들을 개발자가 원하는 기준으로 나누어서 보관한다.  */

BEGIN(Engine)
class CLayer;
class CGameObjectManager final : public CBase
{
	DECLARE_SINGLETON(CGameObjectManager)
private:
	CGameObjectManager();
	virtual ~CGameObjectManager() = default;
	virtual void Free() override;

public:
	_int	Update(_double dDeltaTime);
	_int	LateUpdate(_double dDeltaTime);
	void	Clear(_uint iSceneIndex);

public:
	// allocate the size of list
	HRESULT ReserveManager(_uint iNumScenes);
	HRESULT	AddPrototype(string sPrototypeTag, CGameObject* pPrototype);
	CGameObject* AddGameObject(_uint iSceneIndex, string sPrototypeTag, string pLayerTag, void* pArg);
	list<class CGameObject*> GetGameObjectInLayer(_uint iSceneIndex, string pLayerTag);
	unordered_map<string, CLayer*>* GetLayers() { return m_pGameObjects; }
	void AddGameObjectWithUUID(uint64_t uuid, CGameObject* pObj) { m_pUUIDObjects.insert({ uuid, pObj }); }
	void AddGameObjectWithName(string name, CGameObject* pObj) { m_pNameObjects.insert({ name, pObj }); }
public:
	CGameObject*	FindPrototype(string pPrototypeTag);
	CLayer*			FindLayer(_uint iSceneIndex, string pLayerTag);
	CGameObject*	FindGameObjectWithUUID(uint64_t uuid);
	CGameObject*	FindGameObjectWithName(string name);

public:
	void			AddPrefab(string name, YAML::Node node);
	CGameObject*	SpawnPrefab(string name);
private:
	_uint		m_iNumScenes = 0;

private:
	unordered_map<string, CGameObject*>			m_pPrototypes;
	unordered_map<string, YAML::Node>			m_pPrefabs;

	typedef unordered_map<string, CLayer*>		GAMEOBJECTS;
	GAMEOBJECTS*										m_pGameObjects = nullptr;

	unordered_map<uint64_t, CGameObject*>		m_pUUIDObjects;
	unordered_map<string, CGameObject*>			m_pNameObjects;
};

END