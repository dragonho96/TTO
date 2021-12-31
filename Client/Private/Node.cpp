#include "stdafx.h"
#include "..\Public\Node.h"


USING(Client)

CNode::CNode(ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context)
	: CGameObject(pDevice, pDevice_Context)
{
}

CNode::CNode(const CNode & rhs)
	: CGameObject(rhs)
{
	m_iGCost = 0;
	m_iHCost = 0;
}

HRESULT CNode::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CNode::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_uint CNode::Update(_double TimeDelta)
{
	if (__super::Update(TimeDelta))
		return E_FAIL;

	return S_OK;
}

_uint CNode::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CNode::Render()
{
	if (m_VIBuffer)
		m_VIBuffer->Render();

	return S_OK;
}

HRESULT CNode::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(0, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(0, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	m_pTransform->SetUpRotation(m_pTransform->GetState(CTransform::STATE_RIGHT), 90.f);

	AddComponent(0, "Prototype_VIBuffer_Rect", "Com_VIBuffer", m_pTransform);
	m_VIBuffer = dynamic_cast<CVIBuffer_Rect*>(GetComponent("Com_VIBuffer"));

	return S_OK;
}


_int CNode::CompareTo(CNode * nodeToCompare)
{
	_int compare = intCmp(GetFCost(), nodeToCompare->GetFCost());
	if (compare == 0)
		compare = intCmp(GetHCost(), nodeToCompare->GetHCost());

	return -compare;
}

CNode * CNode::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNode*		pInstance = new CNode(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CNode");
		SafeRelease(pInstance);
	}

	return pInstance;
}

Engine::CGameObject* CNode::Clone(void* pArg)
{
	CGameObject*		pInstance = new CNode(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CNode");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CNode::Free()
{
	__super::Free();
}
