#pragma once
#include "Engine.h"
#include "Transform.h"
#include "Collider.h"
#include "Model.h"
#include "StateMachine.h"
BEGIN(Client)

class CEnemy : public IScriptObject
{
public:
	explicit CEnemy();
	explicit CEnemy(CGameObject* pObj);
	virtual ~CEnemy() = default;
public:
	static CEnemy* Create(CGameObject* pObj);
	virtual void Free() override;
public:
	HRESULT Initialize();
	virtual void Update(_double deltaTime);
	virtual void LapteUpdate(_double deltaTime);
	virtual void Render();
public:
	CRITICAL_SECTION Get_CS() { return m_CS; }

	void FindPath();
	void FollowPlayer(_double deltaTime);
	void GetShot();
	void SetPathPos(list<_vector> pos) { m_pathPosition = pos; }
public:
private:
	CGameObject*	m_pGameObject = nullptr;
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	CModel*			m_pModel = nullptr;
	PxController*	m_pController = nullptr;

	CTransform*		m_pTargetTransform = nullptr;
private:
	string			m_Timer = "";
	_float			m_fPathFinding = 0.f;
	list<_vector>	m_pathPosition;

private:
	CRITICAL_SECTION	m_CS;
	HANDLE				m_hThread = 0;

public:
	_vector			m_velocity = XMVectorZero();
	_vector			m_curVelocity = XMVectorZero();
private:
	CStateMachine*	m_pState = nullptr;
};

END