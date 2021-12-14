#pragma once
#include "Engine.h"
#include "Transform.h"
#include "Collider.h"
#include "Model.h"
BEGIN(Client)

class CPlayer : public IScriptObject
{
private:
	explicit CPlayer();
	explicit CPlayer(CGameObject* pObj);
	virtual ~CPlayer() = default;
public:
	static CPlayer* Create(CGameObject* pObj);
	virtual void Free() override;

public:
	HRESULT Initialize();
	virtual void Update(_double deltaTime);
	virtual void LapteUpdate(_double deltaTime);

private:
	CGameObject*	m_pGameObject;
	CTransform*		m_pTransform;
	CCollider*		m_pCollider;
	CModel*			m_pModel;
	PxController*	m_pController;
	// Inherited via IScriptObject

	CModel*			m_pModel1;
};

END