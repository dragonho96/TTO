#pragma once

#include "Renderer.h"

BEGIN(Engine)

class CComponentManager final : public CBase
{
	DECLARE_SINGLETON(CComponentManager)
private:
	CComponentManager();
	virtual ~CComponentManager() = default;
	virtual void Free() override;

public:
	HRESULT ReserveManager(_uint iNumScenes);
	HRESULT AddPrototype(_uint iSceneIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* CloneComponent(_uint iSceneIndex, const _tchar* pPrototypeTag, void* pArg);
	void Clear(_uint iSceneIndex);

private:
	unordered_map<const _tchar*, CComponent*>*			m_pPrototypes = nullptr;
	typedef unordered_map<const _tchar*, CComponent*>	PROTOTYPES;
private:
	_uint		m_iNumScenes = 0;
public:
	CComponent* FindPrototype(_uint iSceneIndex, const _tchar* pPrototypeTag);
};

END