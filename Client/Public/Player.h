#pragma once
#include "Character.h"
#include "StateMachine.h"
#include "Equipment.h"

BEGIN(Client)
class CPlayer : public CCharacter
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
	virtual void GetDamage(_vector sourceLocation) {}
public:
	void UpdateWeaponTransform();
	// void SetObjectTransform(CGameObject* pObj, BONEDESC* pBone);
	virtual void UpdateRifleLightTransform(CGameObject* pWeapon) override;
	// void UpdateRifleMuzzleLightRange(_double deltaTime);

	void EquipWeapon(EQUIPMENT eType);
	void UnEquipWeapon(EQUIPMENT eType);
	void ChangeGear(EQUIPMENT eType, _uint iIndex);

public:
	void	RotateBody(_double deltaTime);

public:
	virtual void	Shot(_double deltaTime);
	void	ThrowGrenade();

	void	SetGrenadeTrajectory(_bool result);
	void	VisualizeTrajectory(_vector origin, _vector initialVelocity, _float time);
	_vector	CalculatePosInTime(_vector origin, _vector initialVelocity, _float time);
	_float	GetXAxisAngle(_vector hitPos);
	_float	GetYAxisAngle(_vector hitPos);

	_vector GetPickingDir();

	void	CheckEnemyInSight();
	void	RaycastRifleToHitPos();

	void AssignMeshContainter();
	void FindBones();
	_vector GetBonePos(EQUIPMENT eType);
private:
	CTransform*				m_pCameraTransform = nullptr;
	CGameObject*			m_pGrenadeTrajectory = nullptr;
	class CGrenade*			m_pGrenadeInHand = nullptr;
	_vector					m_ThrowingVelocity;

	CGameObject*			m_pRifleLight = nullptr;
	CTransform*				m_pRifleLightTransform = nullptr;


	class CEquipment*		m_pEquipment = nullptr;
	CGameObject*			m_pWeaponInHand = nullptr;
	CGameObject*			m_pPrimaryWeapon = nullptr;
	CGameObject*			m_pSecondaryWeapon = nullptr;
	CGameObject*			m_pGrenade = nullptr;
	CGameObject*			m_pTool = nullptr;
	BONEDESC*				m_pHandBone = nullptr;
	BONEDESC*				m_pRThighBone = nullptr;
	BONEDESC*				m_pGrenadeBone = nullptr;
	BONEDESC*				m_pToolBone = nullptr;
	BONEDESC*				m_pSpineBone = nullptr;
	BONEDESC*				m_pSlingBone = nullptr;
	BONEDESC*				m_pHeadBone = nullptr;

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

	_vector					m_cursorHitPos = { 0, 0, 0, 0 };
	_vector					m_rifleHitPos = { 0, 0, 0, 0 };
	_vector					m_rifleHitNormal = { 0, 0, 0, 0 };
	PxRigidActor*			m_pHitActor = nullptr;

	_double					TimeRaycast = 0.0;
	_double					TimeCheckEnemyInSight = 0.0;
};

END