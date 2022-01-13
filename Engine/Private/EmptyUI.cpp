#include "..\Public\EmptyUI.h"
#include "Engine.h"
USING(Engine)

CEmptyUI::CEmptyUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
	m_Layer = "UI";
}

CEmptyUI::CEmptyUI(const CEmptyUI & rhs)
	: CGameObject(rhs)
{
}

CEmptyUI * CEmptyUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEmptyUI*		pInstance = new CEmptyUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create CEmptyUI");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEmptyUI::Clone(void * pArg)
{
	CEmptyUI*		pInstance = new CEmptyUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create CEmptyUI");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEmptyUI::Free()
{
	__super::Free();
}

HRESULT CEmptyUI::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CEmptyUI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_uint CEmptyUI::Update(_double TimeDelta)
{
	if (__super::Update(TimeDelta))
		return E_FAIL;

	if (m_pParent || dynamic_cast<CEmptyUI*>(m_pParent))
	{
		CRectTransform* pParentTransform = dynamic_cast<CRectTransform*>(m_pParent->GetComponent("Com_Transform"));
		CRectTransform::RECTTRANSFORMDESC parentDesc = pParentTransform->GetTransformDesc();
		CRectTransform::RECTTRANSFORMDESC desc =  m_pRectTransformCom->GetTransformDesc();
		CRectTransform::RECTTRANSFORMDESC newDesc = {
			parentDesc.posX + m_vTransformOffSet.x,
			parentDesc.posY + m_vTransformOffSet.y,
			desc.sizeX, desc.sizeY
		};

		m_pRectTransformCom->SetTransformMat(newDesc);
	}
	InteractMouse();

	return S_OK;
}

_uint CEmptyUI::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (!m_bIsActive)
		return 1;

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEmptyUI::Render(_uint iPassIndex)
{
	CComponent* buffer = GetComponent("Com_VIBuffer");
	if (buffer)
		dynamic_cast<CVIBuffer*>(buffer)->Render();

	CComponent* text = GetComponent("Com_Text");
	if (text)
		dynamic_cast<CText*>(text)->Render();

	return S_OK;
}

void CEmptyUI::SetRectTransform(CRectTransform::RECTTRANSFORMDESC _desc)
{
	m_pRectTransformCom->SetTransformMat(_desc);
}

void CEmptyUI::SetPosition(_float x, _float y)
{
	m_pRectTransformCom->SetPosition(x, y);
}

void CEmptyUI::SetClientPosition(_float x, _float y)
{
	m_pRectTransformCom->SetClientPosition(x, y);
}

void CEmptyUI::LinkTranformWithParent()
{
	if (m_pParent || dynamic_cast<CEmptyUI*>(m_pParent))
	{
		CRectTransform* pParentTransform = dynamic_cast<CRectTransform*>(m_pParent->GetComponent("Com_Transform"));
		CRectTransform::RECTTRANSFORMDESC parentDesc = pParentTransform->GetTransformDesc();
		CRectTransform::RECTTRANSFORMDESC desc = m_pRectTransformCom->GetTransformDesc();

		m_vTransformOffSet.x = desc.posX - parentDesc.posX;
		m_vTransformOffSet.y = desc.posY - parentDesc.posY;
	}
}

void CEmptyUI::InteractMouse()
{
	if (m_pRectTransformCom->IsMouseInRect())
	{
		m_bHover = true;
		if (m_pEngine->IsMouseUp(0))
			m_bSelect = !m_bSelect;
	}
	else
	{
		m_bHover = false;
	}
}

HRESULT CEmptyUI::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(0, "Prototype_RectTransform", "Com_Transform", (CComponent**)&m_pRectTransformCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(0, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}
