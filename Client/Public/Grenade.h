#pragma once
#include "Engine.h"
#include "Transform.h"
#include "Collider.h"
#include "Model.h"
BEGIN(Client)

class CGrenade : public IScriptObject
{
private:
	explicit CGrenade();
	explicit CGrenade(CGameObject* pObj);
	virtual ~CGrenade() = default;
public:
	static CGrenade* Create(CGameObject* pObj);
	virtual void Free() override;

public:
	HRESULT Initialize();
	virtual void Update(_double deltaTime);
	virtual void LapteUpdate(_double deltaTime);
	virtual void Render();
public:
	void OnThrow(_vector dir, _float forcePower);
	void Explode();
private:
	CGameObject*	m_pGameObject = nullptr;
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	CModel*			m_pModel = nullptr;
private:
	_bool			m_bThrown = false;

	string			m_Timer = "";
	_float			m_TimeExplosion = 0.0f;
};

END