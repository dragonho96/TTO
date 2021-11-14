#pragma once

#include "Base.h"

/* 객체들을 개발자가 원하는 기준으로 나누어서 보관한다.  */

BEGIN(Engine)
class CGameObject;
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
	HRESULT	AddPrototype(const _tchar* sPrototypeTag, CGameObject* pPrototype);
	HRESULT AddGameObject(_uint iSceneIndex, const _tchar* sPrototypeTag, const _tchar* pLayerTag, void* pArg);

public:
	CGameObject*	FindPrototype(const _tchar* pPrototypeTag);
	CLayer*			FindLayer(_uint iSceneIndex, const _tchar* pLayerTag);

private:
	_uint		m_iNumScenes = 0;

private:
	unordered_map<const _tchar*, CGameObject*>			m_pPrototypes;

	typedef unordered_map<const _tchar*, CLayer*>		GAMEOBJECTS;
	GAMEOBJECTS*								m_pGameObjects = nullptr;
};

END