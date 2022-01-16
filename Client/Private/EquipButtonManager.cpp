#include "stdafx.h"
#include "..\Public\EquipButtonManager.h"
#include "EmptyUI.h"
#include "EquipmentPool.h"
#include "GameManager.h"
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

	m_vecButtons.resize((size_t)EQUIPMENT::NONE);
	m_vecButtons[(size_t)EQUIPMENT::PRIMARY] = CEngine::GetInstance()->FindGameObjectWithName("PrimaryButton");
	m_vecButtons[(size_t)EQUIPMENT::SECONDARY] = CEngine::GetInstance()->FindGameObjectWithName("SecondaryButton");
	m_vecButtons[(size_t)EQUIPMENT::GRENADE] = CEngine::GetInstance()->FindGameObjectWithName("GrenadeButton");
	m_vecButtons[(size_t)EQUIPMENT::TOOL] = CEngine::GetInstance()->FindGameObjectWithName("ToolButton");
	m_vecButtons[(size_t)EQUIPMENT::HEADGEAR] = CEngine::GetInstance()->FindGameObjectWithName("HeadgearButton");
	m_vecButtons[(size_t)EQUIPMENT::TORSO] = CEngine::GetInstance()->FindGameObjectWithName("TorsoButton");
	m_vecButtons[(size_t)EQUIPMENT::LEGS] = CEngine::GetInstance()->FindGameObjectWithName("LegsButton");
	m_vecButtons[(size_t)EQUIPMENT::VEST] = CEngine::GetInstance()->FindGameObjectWithName("VestButton");
	m_vecButtons[(size_t)EQUIPMENT::BACKPACK] = CEngine::GetInstance()->FindGameObjectWithName("BackpackButton");

	m_vecAddItemButtons.resize((size_t)EQUIPMENT::TOOL + 1);
	m_vecAddItemButtons[(size_t)EQUIPMENT::PRIMARY] = m_vecButtons[(size_t)EQUIPMENT::PRIMARY]->FindChildWithName("AddButton");
	m_vecAddItemButtons[(size_t)EQUIPMENT::SECONDARY] = m_vecButtons[(size_t)EQUIPMENT::SECONDARY]->FindChildWithName("AddButton");
	m_vecAddItemButtons[(size_t)EQUIPMENT::GRENADE] = m_vecButtons[(size_t)EQUIPMENT::GRENADE]->FindChildWithName("AddButton");
	m_vecAddItemButtons[(size_t)EQUIPMENT::TOOL] = m_vecButtons[(size_t)EQUIPMENT::TOOL]->FindChildWithName("AddButton");

	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::PRIMARY], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::PRIMARY]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::SECONDARY], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::SECONDARY]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::GRENADE], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::GRENADE]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::TOOL], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::TOOL]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::HEADGEAR], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::HEADGEAR]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::TORSO], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::TORSO]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::LEGS], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::LEGS]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::VEST], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::VEST]);
	SetButtonText(m_vecButtons[(size_t)EQUIPMENT::BACKPACK], m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::BACKPACK]);

	///* Slot GameObject를 담을 공간 */
	m_InventorySlots.resize((size_t)GEAR::NONE);
	SetInventorySlot(GEAR::TORSO);
	SetInventorySlot(GEAR::LEGS);
	SetInventorySlot(GEAR::VEST);
	SetInventorySlot(GEAR::BACKPACK);


	m_pHoverInfo = CEngine::GetInstance()->FindGameObjectWithName("HoverInfo");
	m_pItemSelectWindow = CEngine::GetInstance()->FindGameObjectWithName("ItemSelectWindow");
	m_pSlotIndicator = CEngine::GetInstance()->SpawnPrefab("SlotIndicator");

	m_pItemSelectWindow->SetActive(false);
	m_pSlotIndicator->SetActive(false);
	m_pItemWindowCloseButton = m_pItemSelectWindow->FindChildWithName("CloseButton");

	// Get ItemSelectButton
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

	// Get Parameters
	m_vecParameter.reserve(5);
	for (int i = 0; i < 5; ++i)
	{
		string childName = "Parameter" + to_string(i);
		auto iter = find_if(children.begin(), children.end(), [&](CGameObject* child) {
			return child->GetName() == childName;
		});

		if (iter != children.end())
			m_vecParameter.emplace_back(*iter);
	}
}

void CEquipButtonManager::Update(_double deltaTime)
{
	if (m_bOpenItemSelectWindow)
	{
		for (int i = 0; i < m_vecItemSelectButton.size(); ++i)
		{
			if (dynamic_cast<CEmptyUI*>(m_vecItemSelectButton[i])->IsHovered())
			{
				SetItemSelectDesc("HoverBar", m_eCurItemSelectType, i);
				if (CEngine::GetInstance()->IsMouseDown(0))
				{
					// 내가 들고있는 것과 다르다면?
					if (m_pPlayerEquipment->GetCurrentEquipment(m_eCurItemSelectType) !=
						m_pEquipmentPool->GetEquipment(m_eCurItemSelectType, i))
					{
						// player equipment를 바꾼다
						// slot 을 clear하고 바뀐것으로 다시 생성한다

						// ChangeEquipment(m_eCurItemSelectType, m_pEquipmentPool->GetEquipment(m_eCurItemSelectType, i));
						m_bOpenItemSelectWindow = false;
						m_pItemSelectWindow->SetActive(false);
						return;
					}
				}
				// SET WHITE
				SetButtonColor(m_vecItemSelectButton[i], _float4{ 1.f, 1.f, 1.f, 1.f });
			}
			else
			{
				// SET GREY
				SetButtonColor(m_vecItemSelectButton[i], _float4{ 0.5f, 0.5f, 0.5f, 1.f });
			}

			if (m_pEquipmentPool->GetEquipment(m_eCurItemSelectType, i) == m_pPlayerEquipment->m_Equipments[(size_t)m_eCurItemSelectType])
				SetButtonColor(m_vecItemSelectButton[i], _float4{ 0.9f, 0.5f, 0.0f, 1.f });
		}
	}

	/* Add Item*/
	if (dynamic_cast<CEmptyUI*>(m_pItemWindowCloseButton)->IsHovered() &&
		CEngine::GetInstance()->IsMouseDown(0))
	{
		m_bOpenItemSelectWindow = false;
		m_pItemSelectWindow->SetActive(false);
	}

	for (int i = 0; i < m_vecAddItemButtons.size(); ++i)
	{
		if (m_vecAddItemButtons[i])
		{
			if (dynamic_cast<CEmptyUI*>(m_vecAddItemButtons[i])->IsHovered() &&
				CEngine::GetInstance()->IsMouseDown(0))
			{
				AddItem(m_pPlayerEquipment->m_Equipments[i], EQUIPMENT(i));
				return;
			}
		}
	}
	/* ItemSelectWindow */
	for (int i = 0; i < m_vecButtons.size(); ++i)
	{
		if (m_vecButtons[i])
		{
			if (dynamic_cast<CEmptyUI*>(m_vecButtons[i])->IsHovered() &&
				CEngine::GetInstance()->IsMouseDown(0))
			{
				OpenItemSelectWindow(EQUIPMENT(i));
				CGameManager::GetInstance()->ChangeCameraPos(EQUIPMENT(i));
			}

			if (i == (size_t)m_eCurItemSelectType && m_bOpenItemSelectWindow)
				SetButtonColor(m_vecButtons[i], _float4{ 0.9f, 0.5f, 0.0f, 1.f });
			else
				SetButtonColor(m_vecButtons[i], _float4{ 1.f, 1.f, 1.f, 1.f });
		}
	}


	//if (dynamic_cast<CEmptyUI*>(m_pPrimaryWeaponButton)->IsHovered())
	//{
	//	if (CEngine::GetInstance()->IsMouseDown(0))
	//	{
	//		// Add Magazine
	//		//AddItem(m_pPlayerEquipment->m_pWeapons.primaryMag, EQUIPMENT::SECONDARYMAG);
	//		OpenItemSelectWindow(EQUIPMENT::PRIMARY);
	//	}
	//	else if (CEngine::GetInstance()->IsMouseDown(1))
	//	{
	//		AddItem(m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::PRIMARYMAG], EQUIPMENT::PRIMARYMAG);
	//	}
	//}
	//if (dynamic_cast<CEmptyUI*>(m_pSecondaryWeaponButton)->IsHovered())
	//{
	//	if (CEngine::GetInstance()->IsMouseDown(0))
	//	{
	//		// Add Magazine
	//		//AddItem(m_pPlayerEquipment->m_pWeapons.primaryMag, EQUIPMENT::SECONDARYMAG);
	//		OpenItemSelectWindow(EQUIPMENT::SECONDARY);
	//	}
	//	else if (CEngine::GetInstance()->IsMouseDown(1))
	//	{
	//		AddItem(m_pPlayerEquipment->m_Equipments[(size_t)EQUIPMENT::SECONDARYMAG], EQUIPMENT::SECONDARYMAG);
	//	}
	//}




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
				RemoveItemImage(itemToRemove, true);

			// m_pCopiedRemovedItem = copiedRemovedItem;
		}
	}
	if (m_pMoveImage)
	{
		if (m_bHoveringSlot)
		{
			_uint4			itemToPlace;
			if (m_pPlayerEquipment->ValidateSlot(m_pCopiedRemovedItem, itemToPlace, m_hoveredSlotIndex))
				SetSlotIndicator(m_pCopiedRemovedItem, itemToPlace);
			else
				m_pSlotIndicator->SetActive(false);
		}

		if (CEngine::GetInstance()->IsMouseUp(0))
		{
			m_pMoveImage->SetDead();
			m_pMoveImage = nullptr;
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

			m_pSlotIndicator->SetActive(false);
		}
		else if (CEngine::GetInstance()->IsMousePressed(0))
		{
			_float3 mousePos = CEngine::GetInstance()->GetMousePosition();
			CRectTransform* pTransform = dynamic_cast<CRectTransform*>(m_pMoveImage->GetComponent("Com_Transform"));
			pTransform->SetClientPosition(mousePos.x, mousePos.y);
		}
	}

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

void CEquipButtonManager::SetButtonColor(CGameObject * button, _float4 color)
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
					text->SetColor(color);
				else if (obj->GetName() == "Cost")
					text->SetColor(color);
				else if (obj->GetName() == "Weight")
					text->SetColor(color);
			}
		}
		else if (obj->GetComponent("Com_VIBuffer"))
		{
			if (obj->GetName() == "Image")
			{
				CComponent* viBuffer = obj->GetComponent("Com_VIBuffer");
				dynamic_cast<CVIBuffer_RectUI*>(viBuffer)->SetColor(color);
			}
		}
	}
}

/* Slot을 화면에 생성해준다 */
void CEquipButtonManager::SetInventorySlot(GEAR type)
{
	ClearInventorySlot(type);

	EQUIPMENT idx = (EQUIPMENT((size_t)EQUIPMENT::HEADGEAR + (size_t)type));
	CGameObject* button = GetButton(idx);

	CRectTransform* buttonTransform = dynamic_cast<CRectTransform*>(button->GetComponent("Com_Transform"));

	CRectTransform::RECTTRANSFORMDESC buttonDesc = buttonTransform->GetTransformDesc();
	// get rightmost pos
	_float2 startPos = { buttonDesc.posX + (buttonDesc.sizeX / 2.f), buttonDesc.posY - (buttonDesc.sizeY / 2.f) };
	startPos.x += (buttonDesc.sizeX / 25.f); // Adding Space between slots

	_uint iGearIdx = (size_t)EQUIPMENT::HEADGEAR + (size_t)type;
	list<_uint2> slots = ((GEARDESC*)m_pPlayerEquipment->m_Equipments[iGearIdx])->inventories;
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
	list<_uint4> itemToRemove;

	// Call Shink_to_fit if changed to vector
	for (_uint i = 0; i < m_InventorySlots[(_uint)type].size(); ++i)
	{
		for (_uint j = 0; j < m_InventorySlots[(_uint)type][i].size(); ++j)
		{
			for (_uint k = 0; k < m_InventorySlots[(_uint)type][i][j].size(); ++k)
			{
				m_InventorySlots[(_uint)type][i][j][k]->SetDead();
				itemToRemove.emplace_back((_uint)type, i, j, k);
			}
			m_InventorySlots[(_uint)type][i][j].clear();
		}
		m_InventorySlots[(_uint)type][i].clear();
		m_InventorySlots[(_uint)type][i].shrink_to_fit();
	}
	m_InventorySlots[(_uint)type].clear();
	m_InventorySlots[(_uint)type].shrink_to_fit();


	for (auto& item : itemToRemove)
	{
		for (auto iter = m_ImagesInSlot.begin(); iter != m_ImagesInSlot.end();)
		{
			if (iter->first.x == item.x && iter->first.y == item.y && iter->first.z == item.z && iter->first.w == item.w)
			{
				iter->second->SetDead();
				iter = m_ImagesInSlot.erase(iter);
			}
			else
				++iter;
		}
	}
}

_bool CEquipButtonManager::AddItem(BASEEQUIPDESC * desc, EQUIPMENT type)
{
	BASEEQUIPDESC*	itemToPlace = nullptr;
	_uint4			itemSlotPos;
	if (m_pPlayerEquipment->AddItem(desc, type, itemToPlace, itemSlotPos))
	{
		AddItemImage(itemToPlace, itemSlotPos);
		return true;
	}
	return false;
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

void CEquipButtonManager::RemoveItemImage(list<_uint4> itemToRemove, _bool bStoreImage)
{
	_bool bDelete = false;
	for (auto& item : itemToRemove)
	{
		CGameObject* obj = m_InventorySlots[item.x][item.y][item.z][item.w];
		dynamic_cast<CVIBuffer_RectUI*>(obj->GetComponent("Com_VIBuffer"))->SetColor(_float4(0.f, 0.f, 0.f, 1.f));

		list<CGameObject*> children = obj->GetChildren();
		for (auto& child : children)
			dynamic_cast<CText*>(child->GetComponent("Com_Text"))->SetString("");
	}

	for (auto& item : itemToRemove)
	{
		for (auto iter = m_ImagesInSlot.begin(); iter != m_ImagesInSlot.end();)
		{
			if (iter->first.x == item.x && iter->first.y == item.y && iter->first.z == item.z && iter->first.w == item.w)
			{
				// Hold the image when dragging
				if (bStoreImage)
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
	// 위치 변경
	CRectTransform* buttonTransform = dynamic_cast<CRectTransform*>(m_vecButtons[(size_t)type]->GetComponent("Com_Transform"));
	CRectTransform::RECTTRANSFORMDESC buttonDesc = buttonTransform->GetTransformDesc();
	// get rightmost pos
	_float2 startPos = { buttonDesc.posX + (buttonDesc.sizeX / 2.f), buttonDesc.posY - (buttonDesc.sizeY / 2.f) };
	startPos.x += (buttonDesc.sizeX / 25.f); // Adding Space between slots

	CRectTransform* windowTransform = dynamic_cast<CRectTransform*>(m_pItemSelectWindow->GetComponent("Com_Transform"));
	windowTransform->SetPosition(startPos.x, startPos.y);
	m_pItemSelectWindow->SetActive(true);


	m_bOpenItemSelectWindow = true;
	m_eCurItemSelectType = type;
	CEquipmentPool* pEquipmentPool = CEquipmentPool::GetInstance();

	list<CGameObject*> children = m_pItemSelectWindow->GetChildren();
	for (int i = 0; i < 3; ++i)
	{
		BASEEQUIPDESC* equipment = (WEAPONDESC*)pEquipmentPool->GetEquipment(type, i);
		SetButtonText(m_vecItemSelectButton[i], equipment);

		if (m_pPlayerEquipment->m_Equipments[(size_t)type]->name == equipment->name)
			SetItemSelectDesc("EquipBar",type, i);
	}
}

void CEquipButtonManager::SetItemSelectDesc(string barName, EQUIPMENT type, _uint idx)
{
	// type과 idx로 hover아이템이 뭔지 알수있음
	// player type의 아이템과 비교
	BASEEQUIPDESC* curEquipment = m_pPlayerEquipment->m_Equipments[(size_t)type];
	BASEEQUIPDESC* hoverEquipment = m_pEquipmentPool->GetEquipment(type, idx);

	CGameObject* textBackground = m_pItemSelectWindow->FindChildWithName("Background");
	if (textBackground)
	{
		dynamic_cast<CText*>(textBackground->GetComponent("Com_Text"))->SetString(hoverEquipment->background);
		// SetParameters(barName,type, idx);
	}

	if (type == EQUIPMENT::PRIMARY || type == EQUIPMENT::SECONDARY)
	{
		m_pItemSelectWindow->FindChildWithName("TextParameter")->SetActive(true);

		_float curWidthRatio = ((WEAPONDESC*)curEquipment)->rpm / 100.f;
		_float hoverWidthRatio = ((WEAPONDESC*)hoverEquipment)->rpm / 100.f;
		SetParameters("FIRERATE", curWidthRatio, hoverWidthRatio, 0);

		curWidthRatio = ((WEAPONDESC*)curEquipment)->muzzleVelocity / 100.f;
		hoverWidthRatio = ((WEAPONDESC*)hoverEquipment)->muzzleVelocity / 100.f;
		SetParameters("MUZZLE VELOCITY", curWidthRatio, hoverWidthRatio, 1);

		curWidthRatio = ((WEAPONDESC*)curEquipment)->recoil / 100.f;
		hoverWidthRatio = ((WEAPONDESC*)hoverEquipment)->recoil / 100.f;
		SetParameters("RECOIL", curWidthRatio, hoverWidthRatio, 2);

		curWidthRatio = ((WEAPONDESC*)curEquipment)->accuracy / 100.f;
		hoverWidthRatio = ((WEAPONDESC*)hoverEquipment)->accuracy / 100.f;
		SetParameters("ACCURACY", curWidthRatio, hoverWidthRatio, 3);

		curWidthRatio = ((WEAPONDESC*)curEquipment)->handling / 100.f;
		hoverWidthRatio = ((WEAPONDESC*)hoverEquipment)->handling / 100.f;
		SetParameters("HANDLING", curWidthRatio, hoverWidthRatio, 4);

		for (int i = 0; i < m_vecParameter.size(); ++i)
			m_vecParameter[i]->SetActive(true);
	}
	else if ((size_t)type >= (size_t)EQUIPMENT::HEADGEAR)
	{
		m_pItemSelectWindow->FindChildWithName("TextParameter")->SetActive(true);

		_float curWidthRatio = ((GEARDESC*)curEquipment)->mobility / 100.f;
		_float hoverWidthRatio = ((GEARDESC*)hoverEquipment)->mobility / 100.f;
		SetParameters("MOBILITY", curWidthRatio, hoverWidthRatio, 0);

		curWidthRatio = ((GEARDESC*)curEquipment)->dexterity / 100.f;
		hoverWidthRatio = ((GEARDESC*)hoverEquipment)->dexterity / 100.f;
		SetParameters("DEXTERITY", curWidthRatio, hoverWidthRatio, 1);

		for (int i = 0; i < 2; ++i)
			m_vecParameter[i]->SetActive(true);
		for (int i = 2; i < 5; ++i)
			m_vecParameter[i]->SetActive(false);
	}
	else
	{
		m_pItemSelectWindow->FindChildWithName("TextParameter")->SetActive(false);

		for (int i = 0; i < m_vecParameter.size(); ++i)
			m_vecParameter[i]->SetActive(false);
	}
}

void CEquipButtonManager::SetParameters(string barName, _float curWidthRatio, _float hoverWidthRatio, _uint idx)
{
	CGameObject* name = m_vecParameter[idx]->FindChildWithName("Name");
	CComponent* textName = name->GetComponent("Com_Text");
	dynamic_cast<CText*>(textName)->SetString(barName);

	CGameObject* defaultBar = m_vecParameter[idx]->FindChildWithName("DefaultBar");
	CComponent* defaultTransform = defaultBar->GetComponent("Com_Transform");
	CRectTransform::RECTTRANSFORMDESC defaultDesc = dynamic_cast<CRectTransform*>(defaultTransform)->GetTransformDesc();

	if (curWidthRatio <= hoverWidthRatio)
	{
		SetParameterBar("HoverBar", hoverWidthRatio, idx, defaultDesc, _float4(0.f, 1.f, 0.f, 1.f));
		SetParameterBar("EquipBar", curWidthRatio, idx, defaultDesc);
	}
	else
	{
		SetParameterBar("HoverBar", curWidthRatio, idx, defaultDesc, _float4(1.f, 0.f, 0.f, 1.f));
		SetParameterBar("EquipBar", hoverWidthRatio, idx, defaultDesc);
	}
}

void CEquipButtonManager::SetParameterBar(string barName, _float widthRatio, _uint idx, CRectTransform::RECTTRANSFORMDESC defaultDesc, _float4 color)
{
	CGameObject* hoverBar = m_vecParameter[idx]->FindChildWithName(barName);
	CComponent* hoverTransform = hoverBar->GetComponent("Com_Transform");
	CComponent* hoverVIBuffer = hoverBar->GetComponent("Com_VIBuffer");
	CRectTransform::RECTTRANSFORMDESC newHoverDesc = defaultDesc;
	newHoverDesc.sizeX = defaultDesc.sizeX * widthRatio;
	newHoverDesc.posX = newHoverDesc.posX - ((defaultDesc.sizeX - newHoverDesc.sizeX) / 2.f);
	dynamic_cast<CRectTransform*>(hoverTransform)->SetTransformMat(newHoverDesc);
	if ("HoverBar" == barName)
		dynamic_cast<CVIBuffer_RectUI*>(hoverVIBuffer)->SetColor(color);

	hoverBar->LinkTranformWithParent();
}

void CEquipButtonManager::ChangeEquipment(EQUIPMENT type, BASEEQUIPDESC * equipment)
{
	list<pair<BASEEQUIPDESC*, EQUIPMENT>> itemToPutIn = m_pPlayerEquipment->SetCurrentEquipment(m_eCurItemSelectType, equipment);

	if ((size_t)type >= (size_t)EQUIPMENT::HEADGEAR)
	{
		GEAR idx = GEAR((size_t)type - (size_t)EQUIPMENT::HEADGEAR);
		SetInventorySlot(idx);
	}

	SetButtonText(GetButton(type), equipment);

	// 빈 공간에 넣는다
	for (auto iter = itemToPutIn.begin(); iter != itemToPutIn.end(); ++iter)
	{
		AddItem(iter->first, iter->second);
		SafeDelete(iter->first);
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

void CEquipButtonManager::SetSlotIndicator(BASEEQUIPDESC * desc, _uint4 itemToPlace)
{
	if (desc == nullptr)
		return;

	CGameObject* slot = m_InventorySlots[itemToPlace.x][itemToPlace.y][itemToPlace.z][itemToPlace.w];
	CRectTransform::RECTTRANSFORMDESC rectDesc = dynamic_cast<CRectTransform*>(slot->GetComponent("Com_Transform"))->GetTransformDesc();
	
	_float2 indicatorSize = { rectDesc.sizeX * desc->slotSize.x, rectDesc.sizeY * desc->slotSize.y };
	_float2 indicatorPos = {rectDesc.posX - (rectDesc.sizeX / 2) + ((rectDesc.sizeX * desc->slotSize.x) / 2),
		rectDesc.posY - (rectDesc.sizeY / 2) + ((rectDesc.sizeY * desc->slotSize.y) / 2) };

	CRectTransform::RECTTRANSFORMDESC slotTransform = { indicatorPos.x, indicatorPos.y, indicatorSize.x, indicatorSize.y };
	dynamic_cast<CRectTransform*>(m_pSlotIndicator->GetComponent("Com_Transform"))->SetTransformMat(slotTransform);
	m_pSlotIndicator->SetActive(true);
}

CGameObject * CEquipButtonManager::GetButton(EQUIPMENT type)
{
	return m_vecButtons[(size_t)type];
}
