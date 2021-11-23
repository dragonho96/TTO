#include "..\Public\EmptyUI.h"

USING(Engine)

CEmptyUI::CEmptyUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
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

	InteractMouse();

	return S_OK;
}

_uint CEmptyUI::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEmptyUI::Render()
{
	CComponent* buffer = GetComponent(TEXT("Com_VIBuffer"));
	if (buffer)
		dynamic_cast<CVIBuffer*>(buffer)->Render();

	return S_OK;
}

void CEmptyUI::SetRectTransform(CRectTransform::RECTTRANSFORMDESC _desc)
{
	m_pRectTransformCom->SetTransformMat(_desc);
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
	if (FAILED(__super::SetUpComponents(0, TEXT("Prototype_RectTransform"), TEXT("Com_Transform"), (CComponent**)&m_pRectTransformCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(0, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}
