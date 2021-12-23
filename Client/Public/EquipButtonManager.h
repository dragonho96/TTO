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
	void SetInventorySlot(GEAR type);
	void ClearInventorySlot(GEAR type);

	_bool AddItem(BASEEQUIPDESC* desc, EQUIPMENT type);
	void AddItemImage(BASEEQUIPDESC* desc, _uint4 itemSlotPos);
	void RemoveItemImage(list<_uint4>	itemToRemove, _bool bStoreImage = false);

	void OpenItemSelectWindow(EQUIPMENT type);
	void SetItemSelectDesc(string barName, EQUIPMENT type, _uint idx);
	void SetParameters(string barName, _float curWidthRatio, _float hoverWidthRatio, _uint idx);
	void SetParameterBar(string barName, _float widthRatio, _uint idx, CRectTransform::RECTTRANSFORMDESC defaultDesc, _float4 color = {0, 0, 0, 0});
	void ChangeEquipment(EQUIPMENT type, BASEEQUIPDESC* equipment);

	void FindHoveredSlot();
	void SetSlotIndicator(BASEEQUIPDESC* desc, _uint4 itemToPlace);
	CGameObject* GetButton(EQUIPMENT type);
private:
	class CEquipmentPool* m_pEquipmentPool = nullptr;

	vector<CGameObject*> m_vecButtons; // Equipment Button
	vector<CGameObject*> m_vecItemSelectButton;
	vector<CGameObject*> m_vecParameter;
	EQUIPMENT	m_eCurItemSelectType;
	_bool		m_bOpenItemSelectWindow = false;

	vector<vector<vector<vector<class CGameObject*>>>> m_InventorySlots;
	list<pair<_uint4, CGameObject*>> m_ImagesInSlot;

	// PlayerEquipment
	CEquipment*		m_pPlayerEquipment = nullptr;
	BASEEQUIPDESC*	m_pCopiedRemovedItem = nullptr;
private:
	class CGameObject*	m_pHoverInfo = nullptr;
	class CGameObject*	m_pMoveImage = nullptr;
	class CGameObject*	m_pItemSelectWindow = nullptr;
	class CGameObject*	m_pSlotIndicator = nullptr;
private:
	_bool	m_bHoveringSlot = false;
	_uint4	m_hoveredSlotIndex;
};

END