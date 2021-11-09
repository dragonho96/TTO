#pragma once

#include "Base.h"

/* 객체들을 개발자가 원하는 기준으로 나누어서 보관한다.  */

BEGIN(Engine)

class CGameObjectManager final : public CBase
{
	DECLARE_SINGLETON(CGameObjectManager)
private:
	CGameObjectManager();
	virtual ~CGameObjectManager() = default;
public:
	HRESULT ReserveManager(_uint iNumLevels);

private:
	_uint		m_iNumLevels = 0;

private:
	unordered_map<const _tchar*, class CGameObject*>			m_Prototypes;


	//typedef unordered_map<const _tchar*, class CLayer*>		GAMEOBJECTS;
	//GAMEOBJECTS*		m_pGameObjects = nullptr;

public:
	virtual void Free() override;
};

END