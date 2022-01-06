#pragma once

#include "Character.h"
#include "StateMachine.h"
BEGIN(Client)

class CEnemy : public CCharacter
{
private:
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
	void CheckVisibility();
	void SetVisibility(VISIBILITY eType) { m_eCurVisibility = eType; }

public:
	CRITICAL_SECTION Get_CS() { return m_CS; }

	void FindPath();
	void FollowPlayer(_double deltaTime);
	virtual void GetDamage(_vector sourceLocation);
	void SetPathPos(list<_vector> pos) { m_pathPosition = pos; }
public:
private:
	CTransform*		m_pTargetTransform = nullptr;

private:
	VISIBILITY		m_eCurVisibility = VISIBILITY::VISIBLE;
	VISIBILITY		m_ePreVisibility = VISIBILITY::VISIBLE;
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