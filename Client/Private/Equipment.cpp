#include "stdafx.h"
#include "..\Public\Equipment.h"
#include "EquipmentPool.h"

USING(Client)

CEquipment::CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CEquipment::CEquipment(const CEquipment & rhs)
	: CComponent(rhs)
	, m_pWeapons(rhs.m_pWeapons)
	, m_Gears(rhs.m_Gears)
{
}

HRESULT CEquipment::InitializePrototype()
{
	CEquipmentPool* pEquipmentPool = CEquipmentPool::GetInstance();

	m_pWeapons.primary = (WEAPONDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::PRIMARY);
	m_pWeapons.secondary = (WEAPONDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::SECONDARY);
	m_pWeapons.primaryMag = (MAGAZINEDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::PRIMARYMAG);
	m_pWeapons.secondaryMag = (MAGAZINEDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::SECONDARYMAG);
	m_pWeapons.grenade = pEquipmentPool->GetEquipment(EQUIPMENT::GRENADE);
	m_pWeapons.tool = pEquipmentPool->GetEquipment(EQUIPMENT::TOOL);

	m_Gears.resize((size_t)GEAR::NONE);
	m_Gears[(size_t)GEAR::HEADGEAR] = (GEARDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::HEADGEAR);
	m_Gears[(size_t)GEAR::TORSO] = (GEARDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::TORSO);
	m_Gears[(size_t)GEAR::LEGS] = (GEARDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::LEGS);
	m_Gears[(size_t)GEAR::VEST] = (GEARDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::VEST);
	m_Gears[(size_t)GEAR::BACKPACK] = (GEARDESC*)pEquipmentPool->GetEquipment(EQUIPMENT::BACKPACK);
	return S_OK;
}

HRESULT CEquipment::Initialize(void * pArg)
{
	m_MyInventories.resize((size_t)GEAR::NONE);

	SetMyInventory(GEAR::TORSO);
	SetMyInventory(GEAR::LEGS);
	SetMyInventory(GEAR::VEST);
	SetMyInventory(GEAR::BACKPACK);
	// ClearMyInventory(GEAR::VEST);

	return S_OK;
}

void CEquipment::SetMyInventory(GEAR type)
{

	list<_uint2> slots = m_Gears[(size_t)type]->inventories;
	slots.sort([](_uint2 first, _uint2 second) {
		return second.y < first.y;
	});

	vector<vector<vector<BASEEQUIPDESC*>>> outerVecItem;
	outerVecItem.reserve(slots.size());
	for (auto& slot : slots)
	{
		vector<vector<BASEEQUIPDESC*>> rowVecItem;
		rowVecItem.reserve(slot.y);
		for (int i = 0; i < slot.y; ++i)
		{
			vector<BASEEQUIPDESC*> colVecItem;
			colVecItem.reserve(slot.x);
			for (int j = 0; j < slot.x; ++j)
			{
				// innerVecItem[i * slot.x + j] = nullptr;
				colVecItem.emplace_back(nullptr);
			}
			rowVecItem.emplace_back(colVecItem);
		}
		outerVecItem.emplace_back(rowVecItem);
	}
	m_MyInventories[(_uint)type] = outerVecItem;
}

/* Return items in the inventory*/
list<BASEEQUIPDESC*> CEquipment::ClearMyInventory(GEAR type)
{
	// Inventory clear시 원래 들어있던 아이템을 저장해놓고 새롭게 생성된 inventory에 넣는다
	list<BASEEQUIPDESC*> listItems;

	for (auto& innerVec : m_MyInventories[(_uint)type])
	{
		for (auto& row : innerVec)
		{
			for (auto& col : row)
			{
				if (col)
					listItems.emplace_back(col);
			}
		}
		innerVec.clear();
		innerVec.shrink_to_fit();
	}
	m_MyInventories[(_uint)type].clear();
	m_MyInventories[(_uint)type].shrink_to_fit();

	return listItems;
}

_bool CEquipment::AddItem(BASEEQUIPDESC* desc, EQUIPMENT type, BASEEQUIPDESC*& outputItem, _uint4& outputSlotPos)
{
	_uint4 slotPos;
	if (!FindSlot(desc, slotPos))
		return false;


	BASEEQUIPDESC* itemToPlace = nullptr;
	switch (type)
	{
	case Client::EQUIPMENT::GRENADE:
	case Client::EQUIPMENT::TOOL:
		itemToPlace = new BASEEQUIPDESC(*m_pWeapons.tool);
		break;
	case Client::EQUIPMENT::PRIMARYMAG:
		itemToPlace = AddMagazine(desc, EQUIPMENT::PRIMARYMAG);
		break;
	case Client::EQUIPMENT::SECONDARYMAG:
		itemToPlace = AddMagazine(desc, EQUIPMENT::SECONDARYMAG);
		break;
	case Client::EQUIPMENT::PRIMARY:
	case Client::EQUIPMENT::SECONDARY:
		break;
	case Client::EQUIPMENT::HEADGEAR:
	case Client::EQUIPMENT::TORSO:
	case Client::EQUIPMENT::LEGS:
	case Client::EQUIPMENT::VEST:
	case Client::EQUIPMENT::BACKPACK:
		break;
	default:
		break;
	}

	if (!itemToPlace)
		return false;

	PlaceItemInSlot(itemToPlace, slotPos);
	outputItem = itemToPlace;
	outputSlotPos = slotPos;
	return true;
}

_bool CEquipment::AddItemAtPos(BASEEQUIPDESC * desc, _uint4 slotPosToValidate, BASEEQUIPDESC *& outputItem, _uint4 & outputSlotPos)
{
	//_uint4 slotPos;
	//if (!FindSlot(desc, slotPos))
	//	return false;

	if (!ValidateSlot(desc, outputSlotPos, slotPosToValidate))
		return false;

	BASEEQUIPDESC* itemToPlace = nullptr;
	switch (desc->type)
	{
	case Client::EQUIPMENT::GRENADE:
	case Client::EQUIPMENT::TOOL:
		itemToPlace = new BASEEQUIPDESC(*desc);
		break;
	case Client::EQUIPMENT::PRIMARYMAG:
		itemToPlace = AddMagazine(desc, EQUIPMENT::PRIMARYMAG);
		break;
	case Client::EQUIPMENT::SECONDARYMAG:
		itemToPlace = AddMagazine(desc, EQUIPMENT::SECONDARYMAG);
		break;
	case Client::EQUIPMENT::PRIMARY:
	case Client::EQUIPMENT::SECONDARY:
		break;
	case Client::EQUIPMENT::HEADGEAR:
	case Client::EQUIPMENT::TORSO:
	case Client::EQUIPMENT::LEGS:
	case Client::EQUIPMENT::VEST:
	case Client::EQUIPMENT::BACKPACK:
		break;
	default:
		break;
	}

	if (!itemToPlace)
		return false;

	PlaceItemInSlot(itemToPlace, outputSlotPos);
	outputItem = itemToPlace;
	return true;
}

_bool CEquipment::RemoveItem(_uint4 itemSlotPos, list<_uint4>& outputItemSize, BASEEQUIPDESC** outCopiedRemovedItem)
{
	BASEEQUIPDESC* item = nullptr;
	item = m_MyInventories[itemSlotPos.x][itemSlotPos.y][itemSlotPos.z][itemSlotPos.w];
	if (!item)
		return false;

	for (_uint i = 0; i < (size_t)GEAR::NONE; ++i)
	{
		//for (auto& outerIter = m_InventorySlots[i].begin(); outerIter != m_InventorySlots[i].end(); ++outerIter)
		for (_uint j = 0; j < m_MyInventories[i].size(); ++j)
		{
			for (_uint k = 0; k < m_MyInventories[i][j].size(); ++k)
			{
				for (_uint l = 0; l < m_MyInventories[i][j][k].size(); ++l)
				{
					if (item == m_MyInventories[i][j][k][l])
					{
						m_MyInventories[i][j][k][l] = nullptr;
						outputItemSize.emplace_back(_uint4(i, j, k, l));
					}
				}
			}
		}
	}

	if (outCopiedRemovedItem)
	{
		switch (item->type)
		{
		case Client::EQUIPMENT::GRENADE:
		case Client::EQUIPMENT::TOOL:
			*outCopiedRemovedItem = new BASEEQUIPDESC(*item);
			break;
		case Client::EQUIPMENT::PRIMARYMAG:
		case Client::EQUIPMENT::SECONDARYMAG:
			*outCopiedRemovedItem = new MAGAZINEDESC(*(MAGAZINEDESC*)item);
			break;
		case Client::EQUIPMENT::PRIMARY:
		case Client::EQUIPMENT::SECONDARY:
			*outCopiedRemovedItem = new WEAPONDESC(*(WEAPONDESC*)item);
			break;
		case Client::EQUIPMENT::HEADGEAR:
		case Client::EQUIPMENT::TORSO:
		case Client::EQUIPMENT::LEGS:
		case Client::EQUIPMENT::VEST:
		case Client::EQUIPMENT::BACKPACK:
			*outCopiedRemovedItem = new GEARDESC(*(GEARDESC*)item);
			break;
		default:
			break;
		}
	}


	
	SafeDelete(item);
	return true;
}

BASEEQUIPDESC * CEquipment::AddMagazine(BASEEQUIPDESC* desc, EQUIPMENT type)
{
	// Find if space available for magazine

	MAGAZINEDESC* magDesc = nullptr;
	magDesc = new MAGAZINEDESC(*(MAGAZINEDESC*)desc);

	//if (type == EQUIPMENT::PRIMARYMAG)
	//	magDesc = new MAGAZINEDESC(*m_pWeapons.primaryMag);
	//else
	//	magDesc = new MAGAZINEDESC(*m_pWeapons.secondaryMag);
	return magDesc;
}

_bool CEquipment::FindSlot(BASEEQUIPDESC * desc, _uint4& slotStartPos)
{
	_uint2 itemSize = desc->slotSize;

	for (_uint i = 0; i < m_MyInventories.size(); ++i)
	{
		list<_uint2> inventories = m_Gears[i]->inventories;
		if (inventories.size() <= 0)
			continue;

		inventories.sort([](_uint2 first, _uint2 second) {
			return second.y < first.y;
		});

		auto& inventory = inventories.begin();
		for (_uint j = 0; j < m_MyInventories[i].size(); ++j)
		{
			_uint inventorySizeX = inventory->x;
			_uint inventorySizeY = inventory->y;
			if (itemSize.x <= inventorySizeX && itemSize.y <= inventorySizeY)
			{
				/*
				inventorySizeY - (inventorySizeY - itemSize.y)
				range 밖으로 나갈 일이 없음
				*/
				for (_uint row = 0; row < inventorySizeY - itemSize.y + 1; ++row)
				{
					for (_uint col = 0; col < inventorySizeX - itemSize.x + 1; ++col)
					{
						if (m_MyInventories[i][j][row][col] == nullptr)
						{
							// iterate empty slots
							_uint stack = 0;
							for (int nextRow = row; nextRow < itemSize.y + row; ++nextRow)
							{
								for (int nextCol = col; nextCol < itemSize.x + col; ++nextCol)
								{
									if (m_MyInventories[i][j][nextRow][nextCol] == nullptr)
										++stack;
								}
							}
							if (stack == (itemSize.y * itemSize.x))
							{
								// space found;
								slotStartPos = { i, j, row, col };
								return true;
							}
						}
					}
				}
			}
			++inventory;
		}
	}
	return false;
}

_bool CEquipment::ValidateSlot(BASEEQUIPDESC * desc, _uint4 & slotStartPos, _uint4 slotToValidate)
{
	_uint2 itemSize = desc->slotSize;

	_uint inventorySizeY = m_MyInventories[slotToValidate.x][slotToValidate.y].size();
	_uint inventorySizeX = m_MyInventories[slotToValidate.x][slotToValidate.y][slotToValidate.z].size();

	if (itemSize.x <= inventorySizeX && itemSize.y <= inventorySizeY)
	{
		for (_uint row = slotToValidate.z; row < inventorySizeY - itemSize.y + 1; ++row)
		{
			for (_uint col = slotToValidate.w; col < inventorySizeX - itemSize.x + 1; ++col)
			{
				if (m_MyInventories[slotToValidate.x][slotToValidate.y][row][col] == nullptr)
				{
					// iterate empty slots
					_uint stack = 0;
					for (int nextRow = row; nextRow < itemSize.y + row; ++nextRow)
					{
						for (int nextCol = col; nextCol < itemSize.x + col; ++nextCol)
						{
							if (m_MyInventories[slotToValidate.x][slotToValidate.y][nextRow][nextCol] == nullptr)
								++stack;
						}
					}
					if (stack == (itemSize.y * itemSize.x))
					{
						// space found;
						slotStartPos = { slotToValidate.x, slotToValidate.y, row, col };
						return true;
					}
				}
			}
		}
	}

	return false;
}

void CEquipment::PlaceItemInSlot(BASEEQUIPDESC * desc, _uint4 slotStarPos)
{
	_uint2 itemSize = desc->slotSize;
	for (int row = 0; row < itemSize.y; ++row)
	{
		for (int col = 0; col < itemSize.x; ++col)
		{
			m_MyInventories[slotStarPos.x][slotStarPos.y][slotStarPos.z + row][slotStarPos.w + col] = desc;
		}
	}
}


CEquipment * CEquipment::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEquipment*	pInstance = new CEquipment(pDevice, pDeviceContext);
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Create CEquipment");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CEquipment::Clone(void * pArg)
{
	CEquipment*	pInstance = new CEquipment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create CEquipment");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CEquipment::Free()
{
	__super::Free();
}
