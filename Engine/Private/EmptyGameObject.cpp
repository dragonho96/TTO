#include "..\Public\EmptyGameObject.h"

USING(Engine)

CEmptyGameObject::CEmptyGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEmptyGameObject::CEmptyGameObject(const CEmptyGameObject & rhs)
	: CGameObject(rhs)
{
}

CEmptyGameObject * CEmptyGameObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEmptyGameObject*		pInstance = new CEmptyGameObject(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Creating CEmptyGameObject");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CEmptyGameObject::Clone(void * pArg)
{
	CEmptyGameObject*		pInstance = new CEmptyGameObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Creating CEmptyGameObject");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CEmptyGameObject::Free()
{
	__super::Free();
}

HRESULT CEmptyGameObject::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CEmptyGameObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUpComponents()))
		return E_FAIL;

	return S_OK;
}

_uint CEmptyGameObject::Update(_double TimeDelta)
{
	if (__super::Update(TimeDelta))
		return E_FAIL;

	return S_OK;
}

_uint CEmptyGameObject::LateUpdate(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	return m_pRendererCom->AddRenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CEmptyGameObject::Render()
{
	CComponent* buffer = GetComponent(TEXT("Com_VIBuffer"));
	if (buffer)
		dynamic_cast<CVIBuffer*>(buffer)->Render();
		
	CComponent* collider = GetComponent(TEXT("Com_Collider"));
	if (collider)
		dynamic_cast<CSphereCollider*>(collider)->Render();

	return S_OK;
}

HRESULT CEmptyGameObject::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(0, TEXT("Prototype_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(0, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}
