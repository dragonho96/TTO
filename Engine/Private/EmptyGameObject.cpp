#include "..\Public\EmptyGameObject.h"
#include "Engine.h"

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

	CComponent* collider = GetComponent("Com_Collider");
	if (collider)
		dynamic_cast<CCollider*>(collider)->LateUpdate(TimeDelta);

	//if (GetName() == "Player")
	//	m_pRendererCom->AddRenderGroup(CRenderer::RENDER_EXTRA, this);

	return m_pRendererCom->AddRenderGroup(m_eRenderGroup, this);
}

HRESULT CEmptyGameObject::Render(_uint iPassIndex)
{
	if (!m_bIsActive)
		return S_OK;

	CComponent* buffer = GetComponent("Com_VIBuffer");
	if (buffer)
		dynamic_cast<CVIBuffer*>(buffer)->Render(iPassIndex);

	CComponent* modelCom = GetComponent("Com_Model");
	if (modelCom)
	{
		CModel* pModel = dynamic_cast<CModel*>(modelCom);
		pModel->Bind_Buffers(iPassIndex);
		
		_uint		iNumMaterials = pModel->Get_NumMaterials();

		//for (_uint i = 0; i < iNumMaterials; ++i)
		//{
		//	pModel->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		//	// TODO: Handle passIndex
		//	pModel->Render(i, 3);
		//}
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			pModel->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			// TODO: Handle passIndex
			pModel->Render(i, iPassIndex);
		}

	}


	if (buffer && modelCom)
		int i = 0;
		
	CComponent* collider = GetComponent("Com_Collider");
	if (collider)
		dynamic_cast<CCollider*>(collider)->Render();

	return S_OK;
}

HRESULT CEmptyGameObject::SetUpComponents()
{
	if (FAILED(__super::SetUpComponents(0, "Prototype_Transform", "Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::SetUpComponents(0, "Prototype_Renderer", "Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

void CEmptyGameObject::LinkTranformWithParent()
{
}
