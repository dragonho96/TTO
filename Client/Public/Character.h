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
	virtual void Shot(_double deltaTime) = 0;
	virtual void SetObjectTransform(CGameObject* pObj, BONEDESC* pBone);
	virtual void UpdateRifleMuzzleLightRange(_double deltaTime);
	virtual void UpdateRifleLightTransform(CGameObject* pWeapon);

protected:
	CGameObject*			m_pMuzzleLight = nullptr;
	CTransform*				m_pMuzzleLightTransform = nullptr;
	CLight*					m_pMuzzleLightCom = nullptr;
	_float					m_fMuzzleLightRange = 0.f;
	_float					m_fCurMuzzleLightRange = 0.1f;

protected:
	_double					m_ShotTime = 0.0;

protected:
	CGameObject*	m_pGameObject = nullptr;
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	CModel*			m_pModel = nullptr;
	PxController*	m_pController = nullptr;
};

END