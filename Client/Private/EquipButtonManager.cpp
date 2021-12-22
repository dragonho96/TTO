#include "stdafx.h"
#include "..\Public\EquipButtonManager.h"
#include "EmptyUI.h"
#include "EquipmentPool.h"

#include <algorithm>

USING(Client)
IMPLEMENT_SINGLETON(CEquipButtonManager)

CEquipButtonManager::CEquipButtonManager()
{
	Initialize();
}

void CEquipButtonManager::Free()
{
	if (m_pCopiedRemovedItem)
		SafeDelete(m_pCopiedRemovedItem);
}

void CEquipButtonManager::Initialize()
{
	m_pEquipmentPool = CEquipmentPool::GetInstance();

	list<class CGameObject*> objList = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (objList.size() <= 0)
		return;

	CGameObject* m_pGameObject = objList.front();
	m_pPlayerEquipment = dynamic_cast<CEquipment*>(m_pGameObject->GetComponent("Com_Equipment"));

	m_pPrimaryWeaponButton = CEngine::GetInstance()->FindGameObjectWithName("PrimaryButton");
	m_pSecondaryWeaponButton = CEngine::GetInstance()->FindGameObjectWithName("SecondaryButton");
	//m_pGrenadeButton = CEngine::GetInstance()->FindGameObjectWithName("GrenadeButton");
	//m_pToolButton = CEngine::GetInstance()->FindGameObjectWithName("ToolButton");
	// m_pHeadgearButton = CEngine::GetInstance()->FindGameObjectWithName("HeadgearButton");
	m_pTorsoButton = CEngine::GetInstance()->FindGameObjectWithName("TorsoButton");
	//m_pLegsButton = CEngine::GetInstance()->FindGameObjectWithName("LegsButton");
	//m_pVestButton = CEngine::GetInstance()->FindGameObjectWithName("VestButton");
	//m_pBackpackButton = CEngine::GetInstance()->FindGameObjectWithName("BackpackButton");

	SetButtonText(m_pPrimaryWeaponButton, m_pPlayerEquipment->m_pWeapons.primary);
	SetButtonText(m_pSecondaryWeaponButton, m_pPlayerEquipment->m_pWeapons.secondary);
	// SetButtonText(m_pGrenadeButton, m_pPlayerEquipment->m_pWeapons.grenade);
	// SetButtonText(m_pToolButton, m_pPlayerEquipment->m_pWeapons.tool);


	//m_pTorsoButton = CEngine::GetInstance()->FindGameObjectWithName("Torso");
	//m_pLegsButton = CEngine::GetInstance()->FindGameObjectWithName("Legs");
	//m_pVestButton = CEngine::GetInstance()->FindGameObjectWithName("Vest");
	//m_pBackpackButton = CEngine::GetInstance()->FindGameObjectWithName("Backpack");


	//SetButtonText(m_pTorsoButton, m_pPlayerEquipment->m_Gears[(_uint)GEAR::TORSO]);
	//SetButtonText(m_pLegsButton, m_pPlayerEquipment->m_Gears[(_uint)GEAR::LEGS]);
	//SetButtonText(m_pLegsButton, m_pPlayerEquipment->m_Gears[(_uint)GEAR::VEST]);
	//SetButtonText(m_pLegsButton, m_pPlayerEquipment->m_Gears[(_uint)GEAR::BACKPACK]);

	///* Slot GameObject를 담을 공간 */
	m_InventorySlots.resize((size_t)GEAR::NONE);
	SetInventorySlot(m_pTorsoButton, GEAR::TORSO);
	// SetInventorySlot(m_pLegsButton, GEAR::LEGS);
	// SetInventorySlot(m_pVestButton, GEAR::VEST);
	// SetInventorySlot(m_pBackpackButton, GEAR::BACKPACK);


	m_pHoverInfo = CEngine::GetInstance()->FindGameObjectWithName("HoverInfo");
	m_pItemSelectWindow = CEngine::GetInstance()->FindGameObjectWithName("ItemSelectWindow");
	m_vecItemSelectButton.reserve(3);

	list<CGameObject*> children = m_pItemSelectWindow->GetChildren();
	for (int i = 0; i < 3; ++i)
	{
		string childName = "ItemSelectButton" + to_string(i);
		auto iter = find_if(children.begin(), children.end(), [&](CGameObject* child) {
			return child->GetName() == childName;
		});

		if (iter != children.end())
			m_vecItemSelectButton.emplace_back(*iter);
	}
}

void CEquipButtonManager::Update(_double deltaTime)
{
	/* ItemSelectWindow */
	if (dynamic_cast<CEmptyUI*>(m_pPrimaryWeaponButton)->IsHovered())
	{
		if (CEngine::GetInstance()->IsMouseDown(0))
		{
			// Add Magazine
			//AddItem(m_pPlayerEquipment->m_pWeapons.primaryMag, EQUIPMENT::SECONDARYMAG);
			OpenItemSelectWindow(EQUIPMENT::PRIMARY);
		}
	}
	if (dynamic_cast<CEmptyUI*>(m_pSecondaryWeaponButton)->IsHovered())
	{
		if (CEngine::GetInstance()->IsMouseDown(0))
		{
			// Add Magazine
			//AddItem(m_pPlayerEquipment->m_pWeapons.primaryMag, EQUIPMENT::SECONDARYMAG);
			OpenItemSelectWindow(EQUIPMENT::SECONDARY);
		}
	}
	if (m_bOpenItemSelectWindow)
	{
		for (int i = 0; i < m_vecItemSelectButton.size(); ++i)
		{
			if (dynamic_cast<CEmptyUI*>(m_vecItemSelectButton[i])->IsHovered() && 
				CEngine::GetInstance()->IsMouseDown(0))
			{
				// 내가 들고있는 것과 다르다면?
				if (m_pPlayerEquipment->GetCurrentEquipment(m_eCurItemSelectType) != 
					m_pEquipmentPool->GetEquipment(m_eCurItemSelectType, i))
				{
					// player equipment를 바꾼다
					// slot 을 clear하고 바뀐것으로 다시 생성한다

					int i = 0;
				}


			}
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
		if (CEngine::GetInstance()->IsMouseDown(0))
		{
			list<_uint4>	itemToRemove;
			// BASEEQUIPDESC*	copiedRemovedItem;
			if (m_pPlayerEquipment->RemoveItem(m_hoveredSlotIndex, itemToRemove, &m_pCopiedRemovedItem, 0))
				RemoveItemImage(itemToRemove);

			// m_pCopiedRemovedItem = copiedRemovedItem;
		}
	}
	if (m_pMoveImage)
	{
		if (CEngine::GetInstance()->IsMouseUp(0))
		{
			m_pMoveImage->SetDead();

			// 아이템이 place가능한 slot에 있다면 place
			if (m_bHoveringSlot)
			{
				BASEEQUIPDESC*	itemToPlace = nullptr;
				_uint4			itemSlotPos;
				if (m_pPlayerEquipment->AddItemAtPos(m_pCopiedRemovedItem, m_hoveredSlotIndex, itemToPlace, itemSlotPos))
					// itemSlotPos여기에 item 넣기
					AddItemImage(itemToPlace, itemSlotPos);
			}
			SafeDelete(m_pCopiedRemovedItem);
		}
		else if (CEngine::GetInstance()->IsMousePressed(0))
		{
			_float3 mousePos = CEngine::GetInstance()->GetMousePosition();
			CRectTransform* pTransform = dynamic_cast<CRectTransform*>(m_pMoveImage->GetComponent("Com_Transform"));
			pTransform->SetClientPosition(mousePos.x, mousePos.y);
		}
	}

	// FindHoveredSlot();
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
		if (obj->GetComponent("Com_Text"))
		{
			CComponent* textCom = obj->GetComponent("Com_Text");
			if (textCom)
			{
				CText* text = dynamic_cast<CText*>(textCom);
				if (obj->GetName() == "Name")
					text->SetString(desc->name);
				else if (obj->GetName() == "Cost")
					text->SetString("Cost " + to_string(desc->cost));
				else if (obj->GetName() == "Weight")
				{
					string str = to_string(desc->weight);
					size_t pos = str.find(".");
					string newStr = str.substr(0, pos + 2);
					text->SetString(newStr + "KG");
				}
			}
		}
		else if (obj->GetComponent("Com_VIBuffer"))
		{
			if (obj->GetName() == "Image")
			{
				CComponent* viBuffer = obj->GetComponent("Com_VIBuffer");
				dynamic_cast<CVIBuffer_RectUI*>(viBuffer)->UpdateTexture(desc->texturePath);
			}
		}
	}
}

/* Slot을 화면에 생성해준다 */
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

void CEquipButtonManager::AddItem(BASEEQUIPDESC * desc, EQUIPMENT type)
{
	BASEEQUIPDESC*	itemToPlace = nullptr;
	_uint4			itemSlotPos;
	if (m_pPlayerEquipment->AddItem(desc, type, itemToPlace, itemSlotPos))
		AddItemImage(itemToPlace, itemSlotPos);
}

void CEquipButtonManager::AddItemImage(BASEEQUIPDESC * desc, _uint4 itemSlotPos)
{
	CGameObject*	pTopLeftSlot = nullptr;
	CGameObject*	pBottomLeftSlot = nullptr;
	list<CGameObject*> slotsInImage;
	_uint2 itemSize = desc->slotSize;
	for (_uint row = 0; row < itemSize.y; ++row)
	{
		for (_uint col = 0; col < itemSize.x; ++col)
		{
			CGameObject* obj = m_InventorySlots[itemSlotPos.x][itemSlotPos.y][itemSlotPos.z + row][itemSlotPos.w + col];
			dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent("Com_VIBuffer"))->SetColor(_float4(1.f, 0.f, 0.f, 1.f));

			slotsInImage.emplace_back(obj);

			// Top Right
			if (row == 0 && col == 0)
			{
				CGameObject* pChild = obj->GetChildren().front();
				dynamic_cast<CText*>(pChild->GetComponent("Com_Text"))->SetString(desc->name);
			}
			// Bottom Right
			else if (row == itemSize.y - 1 && col == 0)
			{
				CGameObject* pChild = obj->GetChildren().back();
				dynamic_cast<CText*>(pChild->GetComponent("Com_Text"))->SetString(to_string(desc->cost));
			}
		}
	}


	// Find middle pos of slots
	_float midX, midY;
	_float totalX = 0, totalY = 0;
	for (auto& slot : slotsInImage)
	{
		CRectTransform::RECTTRANSFORMDESC desc = dynamic_cast<CRectTransform*>(slot->GetComponent("Com_Transform"))->GetTransformDesc();
		totalX += desc.posX;
		totalY += desc.posY;
	}

	midX = totalX / slotsInImage.size();
	midY = totalY / slotsInImage.size();

	if (desc->texturePath == "")
		return;

	CGameObject* pImage = CEngine::GetInstance()->SpawnPrefab("ItemImage");
	CVIBuffer_RectUI* pVIBuffer = dynamic_cast<CVIBuffer_RectUI*>(pImage->GetComponent("Com_VIBuffer"));
	pVIBuffer->UpdateTexture(desc->texturePath);

	CRectTransform* pTransform = dynamic_cast<CRectTransform*>(pImage->GetComponent("Com_Transform"));
	pTransform->SetPosition(midX, midY);

	m_ImagesInSlot.emplace_back(itemSlotPos, pImage);
}

void CEquipButtonManager::RemoveItemImage(list<_uint4> itemToRemove)
{
	_bool bDelete = false;
	for (auto& item : itemToRemove)
	{
		CGameObject* obj = m_InventorySlots[item.x][item.y][item.z][item.w];
		dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent("Com_VIBuffer"))->SetColor(_float4(0.f, 0.f, 0.f, 1.f));

		list<CGameObject*> children = obj->GetChildren();
		for (auto& child : children)
			dynamic_cast<CText*>(child->GetComponent("Com_Text"))->SetString("");


		//if (!bDelete)
		//{
		//	for (auto iter = m_ImagesInSlot.begin(); iter != m_ImagesInSlot.end();)
		//	{
		//		if (iter->first.x == item.x && iter->first.y == item.y && iter->first.z == item.z && iter->first.w == item.w)
		//		{
		//			iter->second->SetDead();
		//			iter = m_ImagesInSlot.erase(iter);
		//			bDelete = true;
		//		}
		//		else
		//			++iter;
		//	}
		//}
	}

	for (auto& item : itemToRemove)
	{
		for (auto iter = m_ImagesInSlot.begin(); iter != m_ImagesInSlot.end();)
		{
			if (iter->first.x == item.x && iter->first.y == item.y && iter->first.z == item.z && iter->first.w == item.w)
			{
				// Hold the image when dragging
				if (CEngine::GetInstance()->IsMouseDown(0))
					m_pMoveImage = iter->second;
				else
					iter->second->SetDead();
				iter = m_ImagesInSlot.erase(iter);
				return;
			}
			else
				++iter;
		}
	}
}

void CEquipButtonManager::OpenItemSelectWindow(EQUIPMENT type)
{
	m_bOpenItemSelectWindow = true;
	m_eCurItemSelectType = type;
	CEquipmentPool* pEquipmentPool = CEquipmentPool::GetInstance();

	list<CGameObject*> children = m_pItemSelectWindow->GetChildren();
	for (int i = 0; i < 3; ++i)
	{
		BASEEQUIPDESC* equipment = (WEAPONDESC*)pEquipmentPool->GetEquipment(type, i);
		SetButtonText(m_vecItemSelectButton[i], equipment);
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
						return;
					}
				}
			}
		}
	}
	m_bHoveringSlot = false;
}
