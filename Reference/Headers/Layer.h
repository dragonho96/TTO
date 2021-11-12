#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;
	static CLayer* Create();
	virtual void Free() override;

public:
	HRESULT AddGameObjectInLayer(CGameObject* pGameObject);
	_uint	Update(_double dDeltaTime);
	_uint	LateUpdate(_double dDeltaTime);

private:
	typedef list<class CGameObject*>	OBJECTLIST;
	OBJECTLIST							m_ObjectList;
};

END