#pragma once
#include "Base.h"
#include "Engine.h"
#include "Equipment.h"

BEGIN(Client)

class CEquipButtonManager : public IScriptObject
{
	DECLARE_SINGLETON(CEquipButtonManager)
private:
	CEquipButtonManager();
	virtual ~CEquipButtonManager() = default;
public:
	virtual void Free() override;
public:
	void Initialize();
	virtual void Update(_double deltaTime);
	virtual void LapteUpdate(_double deltaTime);
	virtual void Render();

public:
	void SetButtonText(CGameObject* button, BASEEQUIPDESC* desc);
	void SetInventorySlot(CGameObject* button, GEAR type);
	void ClearInventorySlot(GEAR type);

	void AddItem(BASEEQUIPDESC* desc, EQUIPMENT type);
	void AddItemImage(BASEEQUIPDESC* desc, _uint4 itemSlotPos);
	void RemoveItemImage(list<_uint4>	itemToRemove);

	void OpenItemSelectWindow(EQUIPMENT type);

	void FindHoveredSlot();
private:
	// Buttons
	class CGameObject* m_pPrimaryWeaponButton = nullptr;
	class CGameObject* m_pSecondaryWeaponButton = nullptr;
	class CGameObject* m_pGrenadeButton = nullptr;
	class CGameObject* m_pToolButton = nullptr;
	class CGameObject* m_pTorsoButton = nullptr;
	class CGameObject* m_pLegsButton = nullptr;
	class CGameObject* m_pVestButton = nullptr;
	class CGameObject* m_pBackpackButton = nullptr;


	class CGameObject* m_pHoverInfo = nullptr;
	class CGameObject* m_pItemSelectWindow = nullptr;
	vector<CGameObject*> m_vecItemSelectButton;
	EQUIPMENT	m_eCurItemSelectType;
	_bool		m_bOpenItemSelectWindow = false;

	vector<vector<vector<vector<class CGameObject*>>>> m_InventorySlots;
	list<pair<_uint4, CGameObject*>> m_ImagesInSlot;

	class CEquipmentPool* m_pEquipmentPool = nullptr;
	// PlayerEquipment
	CEquipment*		m_pPlayerEquipment = nullptr;
	CGameObject*	m_pMoveImage = nullptr;
	BASEEQUIPDESC*	m_pCopiedRemovedItem = nullptr;
private:
	_bool	m_bHoveringSlot = false;
	_uint4	m_hoveredSlotIndex;
};

END