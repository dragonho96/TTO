#include "..\Public\VIBuffer_Rect_Viewport.h"

CVIBuffer_Rect_Viewport::CVIBuffer_Rect_Viewport(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)	
{
}

CVIBuffer_Rect_Viewport::CVIBuffer_Rect_Viewport(const CVIBuffer_Rect_Viewport & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect_Viewport::InitializePrototype(_float fX, _float fY, _float fWidth, _float fHeight, string pShaderFilePath)
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;	

	m_shaderPath = pShaderFilePath;

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


	_uint	iNumViewports = 1;

	D3D11_VIEWPORT			ViewportDesc;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	((VTXTEX*)m_pVertices)[0].vPosition = _float3(fX / (ViewportDesc.Width * 0.5f) - 1.f, fY / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.f, 0.f);

	((VTXTEX*)m_pVertices)[1].vPosition = _float3((fX + fWidth) / (ViewportDesc.Width * 0.5f) - 1.f, fY / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.f, 0.f);

	((VTXTEX*)m_pVertices)[2].vPosition = _float3((fX + fWidth) / (ViewportDesc.Width * 0.5f) - 1.f, (fY + fHeight) / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.f, 1.f);

	((VTXTEX*)m_pVertices)[3].vPosition = _float3(fX / (ViewportDesc.Width * 0.5f) - 1.f, (fY + fHeight) / (ViewportDesc.Height * -0.5f) + 1.f, 0.f);
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


	SafeDeleteArray(pIndices);


	return S_OK;
}



HRESULT CVIBuffer_Rect_Viewport::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pShader = make_unique<CShader>(m_shaderPath);

	return S_OK;
}

CVIBuffer_Rect_Viewport * CVIBuffer_Rect_Viewport::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float fX, _float fY, _float fWidth, _float fHeight, string pShaderFilePath)
{
	CVIBuffer_Rect_Viewport*	pInstance = new CVIBuffer_Rect_Viewport(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(fX, fY, fWidth, fHeight, pShaderFilePath)))
	{
		MSG_BOX("Failed To Create CVIBuffer_Rect_Viewport");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Rect_Viewport::Clone(void * pArg)
{
	CVIBuffer_Rect_Viewport*	pInstance = new CVIBuffer_Rect_Viewport(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect_Viewport");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Rect_Viewport::Free()
{
	__super::Free();
}
