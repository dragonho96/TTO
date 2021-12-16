#include "stdafx.h"
#include "..\Public\EquipButtonManager.h"
#include "EmptyUI.h"

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
	m_pPrimaryWeaponButton = CEngine::GetInstance()->FindGameObjectWithName("Primary");
	list<CGameObject*> children = m_pPrimaryWeaponButton->GetChildren();
	for (auto& obj : children)
	{
		CComponent* textCom = obj->GetComponent("Com_Text");
		if (textCom)
		{
			CText* text = dynamic_cast<CText*>(textCom);
			if (obj->GetName() == "ItemType")
				text->SetString("Primary Weapon");
			else if (obj->GetName() == "ItemName")
				text->SetString("M249");
			else
				text->SetString("Cost 4");
		}

	}
	m_pHoverInfo = CEngine::GetInstance()->FindGameObjectWithName("HoverInfo");

}

void CEquipButtonManager::Update(_double deltaTime)
{
	if (dynamic_cast<CEmptyUI*>(m_pPrimaryWeaponButton)->IsHovered())
	{
		m_pHoverInfo->SetActive(true);
		_float3 mouse = CEngine::GetInstance()->GetMousePosition();
		dynamic_cast<CEmptyUI*>(m_pHoverInfo)->SetPosition(mouse.x, mouse.y);
	}
	else
	{
		m_pHoverInfo->SetActive(false);
	}

	
		return;
}

void CEquipButtonManager::LapteUpdate(_double deltaTime)
{
}

void Client::CEquipButtonManager::Render()
{
}
