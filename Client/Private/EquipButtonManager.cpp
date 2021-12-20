#include "stdafx.h"
#include "..\Public\EquipButtonManager.h"
#include "EmptyUI.h"
#include <algorithm>

USING(Client)
IMPLEMENT_SINGLETON(CEquipButtonManager)

CEquipButtonManager::CEquipButtonManager()
{
	Initialize();
}

void CEquipButtonManager::Free()
{
}

void CEquipButtonManager::Initialize()
{
	list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (objList.size() <= 0)
		return;

	CGameObject* m_pGameObject = objList.front();
	m_pPlayerEquipment = dynamic_cast<CEquipment*>(m_pGameObject->GetComponent("Com_Equipment"));
	m_pPrimaryWeaponButton = CEngine::GetInstance()->FindGameObjectWithName("Primary");
	m_pTorsoButton = CEngine::GetInstance()->FindGameObjectWithName("Torso");

	/* Slot GameObject�� ���� ���� */
	m_InventorySlots.resize((size_t)GEAR::NONE);

	SetButtonText(m_pPrimaryWeaponButton, m_pPlayerEquipment->m_pWeapons.primary);
	SetButtonText(m_pTorsoButton, m_pPlayerEquipment->m_Gears[(_uint)GEAR::VEST]);

	SetInventorySlot(m_pTorsoButton, GEAR::VEST);


	m_pHoverInfo = CEngine::GetInstance()->FindGameObjectWithName("HoverInfo");
}

void CEquipButtonManager::Update(_double deltaTime)
{
	if (dynamic_cast<CEmptyUI*>(m_pPrimaryWeaponButton)->IsHovered())
	{
		if (CEngine::GetInstance()->IsMouseDown(0))
		{
			// Add Magazine
			BASEEQUIPDESC*	itemToPlace = nullptr;
			_uint4			itemSlotPos;
			if (m_pPlayerEquipment->AddItem(m_pPlayerEquipment->m_pWeapons.primaryMag, EQUIPMENT::PRIMARYMAG, itemToPlace, itemSlotPos))
				AddItemImage(itemToPlace, itemSlotPos);
		}
		if (CEngine::GetInstance()->IsMouseDown(1))
		{
			// Add Magazine
			BASEEQUIPDESC*	itemToPlace = nullptr;
			_uint4			itemSlotPos;
			if (m_pPlayerEquipment->AddItem(m_pPlayerEquipment->m_pWeapons.secondaryMag, EQUIPMENT::SECONDARYMAG, itemToPlace, itemSlotPos))
				AddItemImage(itemToPlace, itemSlotPos);
		}
	}
	if (m_bHoveringSlot)
	{
		if (CEngine::GetInstance()->IsMouseDown(1))
		{
			list<_uint4>	itemToRemove;
			if (m_pPlayerEquipment->RemoveItem(m_hoveredSlotIndex, itemToRemove))
				RemoveItemImage(itemToRemove);
		}
	}

	// iterate m_InventorySlots and check hoverstate? click state? drag state?
	FindHoveredSlot();
	return;
}

void CEquipButtonManager::LapteUpdate(_double deltaTime)
{
}

void Client::CEquipButtonManager::Render()
{
}

void CEquipButtonManager::SetButtonText(CGameObject * button, BASEEQUIPDESC* desc)
{
	list<CGameObject*> children = button->GetChildren();
	for (auto& obj : children)
	{
		CComponent* textCom = obj->GetComponent("Com_Text");
		if (textCom)
		{
			CText* text = dynamic_cast<CText*>(textCom);
			if (obj->GetName() == "Name")
				text->SetString(desc->name);
			else
				text->SetString(to_string(desc->cost));
		}
	}
}

/* Slot�� ȭ�鿡 �������ش� */
void CEquipButtonManager::SetInventorySlot(CGameObject * button, GEAR type)
{
	ClearInventorySlot(type);

	CRectTransform* buttonTransform = dynamic_cast<CRectTransform*>(button->GetComponent("Com_Transform"));
	
	CRectTransform::RECTTRANSFORMDESC buttonDesc = buttonTransform->GetTransformDesc();
	// get rightmost pos
	_float2 startPos = { buttonDesc.posX + (buttonDesc.sizeX / 2.f), buttonDesc.posY - (buttonDesc.sizeY / 2.f) };
	startPos.x += (buttonDesc.sizeX / 25.f); // Adding Space between slots

	list<_uint2> slots = m_pPlayerEquipment->m_Gears[(size_t)type]->inventories;
	slots.sort([](_uint2 first, _uint2 second) {
		return second.y < first.y;
	});

	CRectTransform::RECTTRANSFORMDESC slotDesc;
	vector<vector<vector<class CGameObject*>>> outerVecSlots;
	for (auto& slot : slots)
	{
		_float2 slotPos = startPos;
		vector<vector<class CGameObject*>> rowVecSlots;
		rowVecSlots.reserve(slot.y);
		for (int i = 0; i < slot.y; ++i)
		{
			vector<CGameObject*> colVecItem;
			colVecItem.reserve(slot.x);
			for (int j = 0; j < slot.x; ++j)
			{
				_float2 finalPos = slotPos;
				CGameObject* pSlot = CEngine::GetInstance()->SpawnPrefab("Slot");
				CRectTransform* slotTransform = dynamic_cast<CRectTransform*>(pSlot->GetComponent("Com_Transform"));
				slotDesc = slotTransform->GetTransformDesc();
				
				finalPos.x += (slotDesc.sizeX * j) + slotDesc.sizeX / 2.f;
				finalPos.y += (slotDesc.sizeY * i) + slotDesc.sizeY / 2.f;

				slotTransform->SetPosition(finalPos.x, finalPos.y);

				colVecItem.emplace_back(pSlot);
			}
			rowVecSlots.emplace_back(colVecItem);
		}
		outerVecSlots.emplace_back(rowVecSlots);

		startPos.x += (slotDesc.sizeX * slot.x) + (buttonDesc.sizeX / 40.f);
	}

	m_InventorySlots[(_uint)type] = outerVecSlots;
}

void CEquipButtonManager::ClearInventorySlot(GEAR type)
{
	// Call Shink_to_fit if changed to vector
	for (auto& innerVec : m_InventorySlots[(_uint)type])
	{
		for (auto& row : innerVec)
		{
			for (auto& col : row)
			{
				col->SetDead();
			}
			row.clear();
		}
		innerVec.clear();
		innerVec.shrink_to_fit();
	}
	m_InventorySlots[(_uint)type].clear();
	m_InventorySlots[(_uint)type].shrink_to_fit();
}

void CEquipButtonManager::AddItemImage(BASEEQUIPDESC * desc, _uint4 itemSlotPos)
{
	_uint2 itemSize = desc->slotSize;
	for (_uint row = 0; row < itemSize.y; ++row)
	{
		for (_uint col = 0; col < itemSize.x; ++col)
		{
			CGameObject* obj = m_InventorySlots[itemSlotPos.x][itemSlotPos.y][itemSlotPos.z + row][itemSlotPos.w + col];
			dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent("Com_VIBuffer"))->SetColor(_float4( 1.f, 0.f, 0.f, 1.f));
		}
	}
}

void CEquipButtonManager::RemoveItemImage(list<_uint4> itemToRemove)
{
	for (auto& item : itemToRemove)
	{
		CGameObject* obj = m_InventorySlots[item.x][item.y][item.z][item.w];
		dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent("Com_VIBuffer"))->SetColor(_float4(0.f, 0.f, 0.f, 1.f));
	}
}

void CEquipButtonManager::FindHoveredSlot()
{
	for (_uint i = 0; i < (size_t)GEAR::NONE; ++i)
	{
		//for (auto& outerIter = m_InventorySlots[i].begin(); outerIter != m_InventorySlots[i].end(); ++outerIter)
		for (_uint j = 0; j < m_InventorySlots[i].size(); ++j)
		{
			for (_uint k = 0; k < m_InventorySlots[i][j].size(); ++k)
			{
				for (_uint l = 0; l < m_InventorySlots[i][j][k].size(); ++l)
				{
					if (dynamic_cast<CEmptyUI*>(m_InventorySlots[i][j][k][l])->IsHovered())
					{
						m_hoveredSlotIndex = { i, j, k, l };
						m_bHoveringSlot = true;
						//hoveredSlotIndex.x = i;
						//hoveredSlotIndex.y = std::distance(m_InventorySlots[i].begin(), outerIter);
						//hoveredSlotIndex.z = std::distance(outerIter->begin(), innerIter);
						//std::distance(m_InventorySlots[i].begin(), outerIter);
						return;
					}
				}
			}
		}
	}
	m_bHoveringSlot = false;
}
