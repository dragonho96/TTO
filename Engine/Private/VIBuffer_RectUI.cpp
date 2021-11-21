#include "..\Public\VIBuffer_RectUI.h"

CVIBuffer_RectUI::CVIBuffer_RectUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)	
{
}

CVIBuffer_RectUI::CVIBuffer_RectUI(const CVIBuffer_RectUI & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_RectUI::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;	

#pragma region VERTEXBUFFER

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iNumVertexBuffers = 1;
	
	/* For.D3D11_BUFFER_DESC */
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXTEX) * m_iNumVertices);

	((VTXTEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.f, 0.f);

	((VTXTEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.f, 0.f);

	((VTXTEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.f, 1.f);

	((VTXTEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices)[3].vTexUV = _float2(0.f, 1.f);

	/* For.D3D11_SUBRESOURCE_DATA */
	m_VBSubResourceData.pSysMem = m_pVertices;

#pragma endregion VERTEXBUFFER

#pragma region INDEXBUFFER

	m_iNumPrimitive = 2;
	m_iNumVerticesPerPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/* For.D3D11_BUFFER_DESC */
	m_IBDesc.ByteWidth = sizeof(POLYGONINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	POLYGONINDICES16*		pIndices = new POLYGONINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(POLYGONINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_IBSubResourceData.pSysMem = pIndices;
	
#pragma endregion INDEXBUFFER

	if (FAILED(__super::Create_Buffers()))
		return E_FAIL;

	//D3D11_INPUT_ELEMENT_DESC		ElmentDesc[] = {
	//	,

	//};

	SafeDeleteArray(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_RectUI::Initialize(void * pArg)
{
	if (pArg)
		m_pRectTransform = (CRectTransform*)pArg;

	m_pShader = make_unique<CShader>(L"../../Assets/Shader/Shader_RectImage.fx");
	m_pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TEXTURETYPE::TYPE_WIC, L"../../Assets/Texture/Folder.png");

	return S_OK;
}

HRESULT CVIBuffer_RectUI::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;


	_uint		iOffset = 0;

	m_pShader->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_pRectTransform->GetTransformMat())), sizeof(_matrix));
	m_pShader->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_matrix));
	m_pShader->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_pRectTransform->GetProjMat())), sizeof(_matrix));

	ID3DX11EffectShaderResourceVariable* map;
	map = m_pShader->AsSRV("Map");
	map->SetResource(m_pTexture->Get_ShaderResourceView());

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, m_pVB.GetAddressOf(), &m_iStride, &iOffset);
	if (m_IBSubResourceData.pSysMem)
		m_pDeviceContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitive);

	m_pShader->Render();

	if (m_IBSubResourceData.pSysMem)
		m_pDeviceContext->DrawIndexed(m_iNumPrimitive * m_iNumVerticesPerPrimitive, 0, 0);
	else
		m_pDeviceContext->Draw(m_iNumVertices, 0);
	return S_OK;
}

CVIBuffer_RectUI * CVIBuffer_RectUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_RectUI*	pInstance = new CVIBuffer_RectUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CVIBuffer_RectUI");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_RectUI::Clone(void * pArg)
{
	CVIBuffer_RectUI*	pInstance = new CVIBuffer_RectUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_RectUI");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_RectUI::Free()
{
	__super::Free();
	SafeRelease(m_pTexture);
}


