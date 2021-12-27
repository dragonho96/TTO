#pragma once
#include "Engine.h"
#include "Transform.h"
#include "Collider.h"
#include "Model.h"
#include "Equipment.h"

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
	virtual void Render();

public:
	void UpdateWeaponTransform();
	void SetObjectTransform(CGameObject* pObj, BONEDESC* pBone);

	void ChangeWeapon(EQUIPMENT eType, _uint iIndex);
	void ChangeGear(EQUIPMENT eType, _uint iIndex);

	_vector GetPickingDir();
	void AssignMeshContainter();
	void FindBones();
private:
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

	CTransform*		m_pCameraTransform = nullptr;
	CModel*			m_pModel1 = nullptr;
};

END