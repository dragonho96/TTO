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

void CCharacter::SetObjectTransform(CGameObject * pObj, BONEDESC * pBone)
{
	if (pObj)
	{
		_float4x4 pos;
		_matrix matPos;

		if (m_pModel->IsSimulatingRagdoll())
			matPos = pBone->pHierarchyNode->Get_CombinedTransformationMatrix();
		else
			matPos = pBone->pHierarchyNode->Get_CombinedTransformationMatrix() * m_pTransform->GetWorldMatrix();
		XMStoreFloat4x4(&pos, matPos);

		dynamic_cast<CTransform*>(pObj->GetComponent("Com_Transform"))->SetMatrix(pos);
	}
}

void CCharacter::UpdateRifleMuzzleLightRange(_double deltaTime)
{
	_double fLerpSpeed = deltaTime * 30.f;
	m_fCurMuzzleLightRange = Lerp(m_fCurMuzzleLightRange, m_fMuzzleLightRange, fLerpSpeed);
	if (m_fCurMuzzleLightRange >= 2.5f)
		m_fMuzzleLightRange = 0.1f;

	m_pMuzzleLightCom->SetRange(m_fCurMuzzleLightRange);
}

void CCharacter::UpdateRifleLightTransform(CGameObject * pWeapon)
{
	CTransform* pWeaponTransform = dynamic_cast<CTransform*>(pWeapon->GetComponent("Com_Transform"));
	m_pMuzzleLightTransform->SetMatrix(pWeaponTransform->GetMatrix());

	_vector up = XMVector3Normalize(pWeaponTransform->GetState(CTransform::STATE_UP));
	_vector right = XMVector3Normalize(pWeaponTransform->GetState(CTransform::STATE_RIGHT));
	_vector offsetPosition = up * 0.5f + right * -0.07f;
	_vector vPosition = pWeaponTransform->GetState(CTransform::STATE_POSITION);
	vPosition += offsetPosition;
	m_pMuzzleLightTransform->SetState(CTransform::STATE_POSITION, vPosition);

	_matrix mat = m_pMuzzleLightTransform->GetWorldMatrix();
	_matrix matRotationX = XMMatrixRotationX(XMConvertToRadians(-90.f));
	_matrix matRotationZ = XMMatrixRotationZ(XMConvertToRadians(90.f));
	// _matrix matRotation = XMMatrixRotationAxis(_vector{1.f, 0.f, 0.f}, XMConvertToRadians(-90.f));
	_matrix newMat = matRotationX * mat;
	newMat = matRotationZ * newMat;
	m_pMuzzleLightTransform->SetMatrix(newMat);
}
