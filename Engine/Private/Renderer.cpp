#include "..\public\Renderer.h"
#include "GameObject.h"
#include "EmptyUI.h"
#include "TargetManager.h"
#include "LightManager.h"
#include "VIBuffer_Rect_Viewport.h"
#include "Engine.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTargetManager(CTargetManager::GetInstance())
{

}

CRenderer::CRenderer(const CRenderer & rhs)
	: CComponent(rhs)
{

}

HRESULT CRenderer::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	_uint	iNumViewports = 1;

	D3D11_VIEWPORT			ViewportDesc;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* Target_Diffuse */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pDeviceContext, "Target_Diffuse", ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Normal*/
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pDeviceContext, "Target_Normal", ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pDeviceContext, "Target_Shade", ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* MRT_Deferred */
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Add_MRT("MRT_Deferred", "Target_Normal")))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pTargetManager->Add_MRT("MRT_LightAcc", "Target_Shade")))
		return E_FAIL;

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_TOOL)
	{
		if (FAILED(m_pTargetManager->Add_RenderTarget(m_pDevice, m_pDeviceContext, "Target_EditorWindow", ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Add_MRT("MRT_EditorWindow", "Target_EditorWindow")))
			return E_FAIL;
	}




	m_pVIBuffer = CVIBuffer_Rect_Viewport::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, "../../Assets/Shader/Shader_Rect_Viewport.fx");
	if (nullptr == m_pVIBuffer)
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(m_pTargetManager->Ready_DebugBuffer("Target_Diffuse", 0.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_DebugBuffer("Target_Normal", 0.f, 200.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTargetManager->Ready_DebugBuffer("Target_Shade", 200.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::AddRenderGroup(RENDER eRenderID, CGameObject * pRenderObject)
{
	if (eRenderID >= RENDER_END ||
		nullptr == pRenderObject)
		return E_FAIL;

	m_RenderGroups[eRenderID].push_back(pRenderObject);

	SafeAddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::DrawRenderGroup()
{
	m_pTargetManager->Initialize(m_pDeviceContext);

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_TOOL)
	{
		m_pTargetManager->Clear_MRT(m_pDeviceContext, "MRT_EditorWindow");
		m_pTargetManager->Set_MRT(m_pDeviceContext, "MRT_EditorWindow");
	}

	//if (FAILED(m_pTargetManager->Begin_MRT(m_pDeviceContext, "MRT_Deferred")))
	//	return E_FAIL;

	//for (auto& pGameObject : m_RenderGroups[RENDER_NONALPHA])
	//{
	//	if (nullptr != pGameObject)
	//	{
	//		if (FAILED(pGameObject->Render()))
	//			return E_FAIL;

	//		SafeRelease(pGameObject);
	//	}
	//}
	//m_RenderGroups[RENDER_NONALPHA].clear();

	//if (FAILED(m_pTargetManager->Set_MRT(m_pDeviceContext, "MRT_EditorWindow")))
	//	return E_FAIL;

	//if (FAILED(m_pTargetManager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;




	if (FAILED(RenderPriority()))
		return E_FAIL;

	if (FAILED(RenderNonAlpha()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(RenderAlpha()))
		return E_FAIL;

	if (FAILED(RenderUI()))
		return E_FAIL;

	if (FAILED(RenderText()))
		return E_FAIL;

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_CLIENT)
	{
		if (FAILED(m_pTargetManager->Render_DebugBuffers("MRT_Deferred")))
			return E_FAIL;
		if (FAILED(m_pTargetManager->Render_DebugBuffers("MRT_LightAcc")))
			return E_FAIL;
	}

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_TOOL)
	{
		if (FAILED(m_pTargetManager->End_MRT(m_pDeviceContext)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::RenderPriority()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::RenderNonAlpha()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pDeviceContext, "MRT_Deferred")))
		return E_FAIL;


	for (auto& pGameObject : m_RenderGroups[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_NONALPHA].clear();

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_TOOL)
	{
		if (FAILED(m_pTargetManager->Set_MRT(m_pDeviceContext, "MRT_EditorWindow")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTargetManager->End_MRT(m_pDeviceContext)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	CLightManager*		pLights = GET_INSTANCE(CLightManager);

	m_pTargetManager->Begin_MRT(m_pDeviceContext, "MRT_LightAcc");

	pLights->Render_Lights();

	if (CEngine::GetInstance()->GetCurrentUsage() == CEngine::USAGE::USAGE_TOOL)
	{
		if (FAILED(m_pTargetManager->Set_MRT(m_pDeviceContext, "MRT_EditorWindow")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTargetManager->End_MRT(m_pDeviceContext)))
			return E_FAIL;
	}
	// m_pTargetManager->End_MRT(m_pDeviceContext);

	RELEASE_INSTANCE(CLightManager);

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	ID3D11ShaderResourceView*	pDiffuseSRV = m_pTargetManager->GetShaderResourceView("Target_Diffuse");
	if (nullptr == pDiffuseSRV)
		return E_FAIL;
	ID3D11ShaderResourceView*	pShadeSRV = m_pTargetManager->GetShaderResourceView("Target_Shade");
	if (nullptr == pShadeSRV)
		return E_FAIL;

	m_pVIBuffer->GetShader()->SetUp_TextureOnShader("g_DiffuseTexture", pDiffuseSRV);
	m_pVIBuffer->GetShader()->SetUp_TextureOnShader("g_ShadeTexture", pShadeSRV);

	m_pVIBuffer->Render(3);

	return S_OK;
}

HRESULT CRenderer::RenderAlpha()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUI()
{
	//ComRef<ID3D11BlendState> d3dBlendState;
	//D3D11_BLEND_DESC omDesc;
	//ZeroMemory(&omDesc,

	//	sizeof(D3D11_BLEND_DESC));
	//omDesc.RenderTarget[0].BlendEnable = true;
	//omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	//if (FAILED(m_pDevice->CreateBlendState(&omDesc, &d3dBlendState)))
	//	return E_FAIL;

	//m_pDeviceContext->OMSetBlendState(d3dBlendState.Get(), 0, 0xffffffff);

	m_RenderGroups[RENDER_UI].sort([](CGameObject* pFirst, CGameObject* pSecond) {
		return dynamic_cast<CEmptyUI*>(pFirst)->GetSortingOrder() < dynamic_cast<CEmptyUI*>(pSecond)->GetSortingOrder();
	});

	for (auto& pGameObject : m_RenderGroups[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::RenderText()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_TEXT])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			SafeRelease(pGameObject);
		}
	}
	m_RenderGroups[RENDER_TEXT].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CRenderer");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	SafeRelease(m_pVIBuffer);
	__super::Free();
}