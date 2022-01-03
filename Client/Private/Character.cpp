#include "stdafx.h"
#include "..\Public\Character.h"

USING(Client)

CCharacter::CCharacter()
{
}

CCharacter::CCharacter(CGameObject * pObj)
	: m_pGameObject(pObj)
{
}

CCharacter * CCharacter::Create(CGameObject * pObj)
{
	return nullptr;
}

void CCharacter::Free()
{
}
