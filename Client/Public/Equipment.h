#pragma once
#include "Component.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEquipment : public CComponent
{
	friend class CEquipButtonManager;
public:
	typedef struct tagWeapon
	{
		WEAPONDESC*			primary;
		WEAPONDESC*			secondary;
		MAGAZINEDESC*		primaryMag;
		MAGAZINEDESC*		secondaryMag;
		BASEEQUIPDESC*		grenade;
		BASEEQUIPDESC*		tool;
	} WEAPONS;

private:
	explicit CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEquipment(const CEquipment& rhs);
	virtual ~CEquipment() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void					SetMyInventory(GEAR type);
	list<BASEEQUIPDESC*>	ClearMyInventory(GEAR type);

	_bool			AddItem(BASEEQUIPDESC* desc, EQUIPMENT type, BASEEQUIPDESC*& outputItem, _uint4& outputSlotPos);
	_bool			AddItemAtPos(BASEEQUIPDESC* desc, _uint4 slotPosToValidate, BASEEQUIPDESC*& outputItem, _uint4& outputSlotPos);
	_bool			RemoveItem(_uint4 itemSlotPos, list<_uint4>& outputItemSize, BASEEQUIPDESC** outCopiedRemovedItem = nullptr, _uint mouseInput = 1);
	BASEEQUIPDESC*	AddMagazine(BASEEQUIPDESC* desc, EQUIPMENT type);
	//_bool	AddItem(/* slot 위치 */);
	//_bool	RemoveItem(/* slot 위치 */);

	/* Iterate all possible inventories*/
	_bool FindSlot(BASEEQUIPDESC* desc, _uint4& slotStartPos);
	/* Check if given inventory has validate slot */
	_bool ValidateSlot(BASEEQUIPDESC* desc, _uint4& slotStartPos, _uint4 slotToValidate);
	void PlaceItemInSlot(BASEEQUIPDESC* desc, _uint4 slotStarPos);

	const BASEEQUIPDESC* GetCurrentEquipment(EQUIPMENT type);
private:
	WEAPONS	m_pWeapons;
	vector<GEARDESC*> m_Gears;

	// 전체 인벤토리 관리
	// Type->Inventories->InventoryRow->InventoryColumn
	vector<vector<vector<vector<BASEEQUIPDESC*>>>> m_MyInventories;

public:
	static CEquipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END