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
	_vector GetPickingDir();

private:
	CGameObject*	m_pGameObject;
	CTransform*		m_pTransform;
	CCollider*		m_pCollider;
	CModel*			m_pModel;
	PxController*	m_pController;

	CEquipment*		m_pEquipment;
	// Inherited via IScriptObject


	CTransform*		m_pCameraTransform;
	CModel*			m_pModel1;

};

END