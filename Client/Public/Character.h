#pragma once
#include "Engine.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Collider.h"
#include "Model.h"

BEGIN(Client)
class CCharacter : public IScriptObject
{
protected:
	explicit CCharacter();
	explicit CCharacter(CGameObject* pObj);
	virtual ~CCharacter() = default;
public:
	static CCharacter* Create(CGameObject* pObj);
	virtual void Free();
public:
	virtual void GetDamage(_vector sourceLocation) = 0;

protected:
	CGameObject*	m_pGameObject = nullptr;
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	CModel*			m_pModel = nullptr;
	PxController*	m_pController = nullptr;
};

END