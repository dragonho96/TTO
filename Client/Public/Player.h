#pragma once
#include "Engine.h"
#include "Transform.h"
#include "Collider.h"
#include "Model.h"
#include "Equipment.h"
#include "StateMachine.h"

BEGIN(Client)
class CPlayer : public IScriptObject
{
public:
	friend class CIdleState;
	friend class CWalkState;
	friend class CRunState;
	friend class CCrouchState;
	friend class CRifleState;
	friend class CGrenadeState;

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
	virtual void Render();

public:
	void UpdateWeaponTransform();
	void SetObjectTransform(CGameObject* pObj, BONEDESC* pBone);

	void ChangeWeapon(EQUIPMENT eType, _uint iIndex);
	void ChangeGear(EQUIPMENT eType, _uint iIndex);

	_float	GetXAxisAngle(_vector hitPos);
	_float	GetYAxisAngle(_vector hitPos);

	_vector GetPickingDir();
	void	CheckEnemyInSight();

	void AssignMeshContainter();
	void FindBones();
private:
	CTransform*		m_pCameraTransform = nullptr;

	CGameObject*	m_pGameObject = nullptr;
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	CModel*			m_pModel = nullptr;
	PxController*	m_pController = nullptr;

	CEquipment*		m_pEquipment = nullptr;
	CGameObject*	m_pWeaponInHand = nullptr;
	CGameObject*	m_pPrimaryWeapon = nullptr;
	CGameObject*	m_pSecondaryWeapon = nullptr;
	CGameObject*	m_pGrenade = nullptr;
	CGameObject*	m_pTool = nullptr;
	BONEDESC*		m_pHandBone = nullptr;
	BONEDESC*		m_pRThighBone = nullptr;
	BONEDESC*		m_pGrenadeBone = nullptr;
	BONEDESC*		m_pToolBone = nullptr;
	BONEDESC*		m_pSpineBone = nullptr;

	class CStateMachine*	m_pLowerState = nullptr;
	class CStateMachine*	m_pUpperState = nullptr;

	_vector					m_velocity = XMVectorZero();
	_vector					m_curVelocity = XMVectorZero();
	_float2					m_curUpperRotation = { 0.f, 0.f };
	_float2					m_targetUpperRotation = { 0.f, 0.f };
	_float					m_curBodyRotation = 0.f;
	_float					m_targetBodyRotation = 0.f;
	_float					m_angleBetweenCamPlayer = 0;

	_float					m_fSpeedFactor = 30.f;
	pair<_bool, _bool>		m_turn90 = {false, false};

	_vector XZDirPrev = {0.f, 0.f, 1.f};

	_double					TimeRaycast = 0.0;
	_double					TimeCheckEnemyInSight = 0.0;

};

END