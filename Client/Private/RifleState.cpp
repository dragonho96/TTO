#include "stdafx.h"
#include "..\Public\RifleState.h"
#include "GrenadeState.h"

USING(Client)

void CRifleState::HandleInput(CStateMachine ** pState, CPlayer& pPlayer)
{
	if (m_bEquipping && pPlayer.m_pModel->IsUpperFinished())
	{
		pPlayer.EquipWeapon(EQUIPMENT::PRIMARY);
		m_bEquipping = false;
	}

	if (!m_bUnEquipping && !m_bEquipping && CEngine::GetInstance()->IsKeyDown('3'))
	{
		pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::UNEQUIP_RIFLE, ANIM_TYPE::UPPER);
		pPlayer.UnEquipWeapon(EQUIPMENT::PRIMARY);
		m_bUnEquipping = true;
	}
	if (m_bUnEquipping && pPlayer.m_pModel->IsUpperFinished())
	{
		m_bUnEquipping = false;
		*pState = CStateMachine::grenade;
		(*pState)->Enter(pState, pPlayer);
		return;
	}

	if (m_bEquipping || m_bUnEquipping)
		return;

	// 각도에 따라 달라지는 anim
	CModel::UPPERBLENDDESC desc;


	// Up Down
	_float yDegree = XMConvertToDegrees(pPlayer.m_targetUpperRotation.x);
	// Right Left
	_float xDegree = XMConvertToDegrees(pPlayer.m_targetUpperRotation.y);

	// + looking down, - looking up
	if (yDegree < 0.f)
		desc.iAnimY = (_uint)ANIM_UPPER::AIM_FU;
	else
		desc.iAnimY = (_uint)ANIM_UPPER::AIM_FD;
	desc.fRatioY = abs(yDegree) / 90.f;

	if (xDegree < 0.f)
		desc.iAnimX = (_uint)ANIM_UPPER::AIM_L;
	else
		desc.iAnimX = (_uint)ANIM_UPPER::AIM_R;
	desc.fRatioX = abs(xDegree) / 90.f;

	if (desc.iAnimY == (_uint)ANIM_UPPER::AIM_FU)
	{
		if (desc.iAnimX == (_uint)ANIM_UPPER::AIM_L)
			desc.iCornerBlendIdx = (_uint)ANIM_UPPER::AIM_LU;
		else
			desc.iCornerBlendIdx = (_uint)ANIM_UPPER::AIM_RU;
	}
	else
	{
		if (desc.iAnimX == (_uint)ANIM_UPPER::AIM_L)
			desc.iCornerBlendIdx = (_uint)ANIM_UPPER::AIM_LD;
		else
			desc.iCornerBlendIdx = (_uint)ANIM_UPPER::AIM_RD;
	}
	//desc.fRatioX = 0.f;

	pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::IDLE_RIFLE, ANIM_TYPE::UPPER, true, desc);
}

void CRifleState::Update(CStateMachine ** pState, CPlayer& pPlayer)
{
}

void CRifleState::Enter(CStateMachine ** pState, CPlayer& pPlayer)
{
	pPlayer.m_pModel->SetUp_AnimationIndex((_uint)ANIM_UPPER::EQUIP_RIFLE, ANIM_TYPE::UPPER);
	m_bEquipping = true;
	m_bUnEquipping = false;
	pPlayer.m_pUI_Primary->SetColor(_float4{ 1.f, 0.5f, 0.f, 1.f });
	pPlayer.m_pUI_Grenade->SetColor(_float4{ 1.f, 1.f, 1.f, 1.f });
}
