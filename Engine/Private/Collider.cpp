#include "..\Public\Collider.h"

USING(Engine)

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_RigidBodyDesc(rhs.m_RigidBodyDesc)
{
}

void CCollider::Free()
{
	__super::Free();
	if (m_isCloned)
		SafeRelease(m_pDebugLine);
}

HRESULT CCollider::InitializePrototype()
{
	ZeroMemory(&m_RigidBodyDesc, sizeof(m_RigidBodyDesc));
	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	if (pArg)
		m_pObjTransform = (CTransform*)pArg;

	//SetUpPhysX();

	return S_OK;
}

HRESULT CCollider::Render()
{



	if (m_pDebugLine)
		dynamic_cast<CVIBuffer*>(m_pDebugLine)->Render();
	return S_OK;
}

