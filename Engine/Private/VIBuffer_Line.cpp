#include "..\Public\VIBuffer_Line.h"

#include "Pipeline.h"

CVIBuffer_Line::CVIBuffer_Line(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Line::CVIBuffer_Line(const CVIBuffer_Line & rhs)
	: CVIBuffer(rhs)
	, m_shaderPath(rhs.m_shaderPath)
{
	SafeAddRef(m_pVBInstance);

}

HRESULT CVIBuffer_Line::InitializePrototype(string pShaderFilePath, _uint iNumPoint)
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;
	if (iNumPoint <= 1)
		return E_FAIL;

	m_iNumPoint = iNumPoint;
	m_iNumVertexBuffers = 1;

#pragma region VERTEXBUFFER

	m_iStride = sizeof(VTXLINEPOINT);
	m_iNumVertices = iNumPoint;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXLINEPOINT[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXLINEPOINT) * m_iNumVertices);

	for (int i = 0; i < m_iNumVertices; ++i)
	{
		((VTXLINEPOINT*)m_pVertices)[i].vPosition = _float3(0.0f, 0.0f, 0.f);
		((VTXLINEPOINT*)m_pVertices)[i].vSize = _float2(1.0f, 1.0f);
		((VTXLINEPOINT*)m_pVertices)[i].iIndex = i;
	}

	/* For.D3D11_SUBRESOURCE_DATA */
	m_VBSubResourceData.pSysMem = m_pVertices;

#pragma endregion VERTEXBUFFER

	// m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region INDEXBUFFER

	m_iNumPrimitive = m_iNumVertices - 1;
	m_iNumVerticesPerPrimitive = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	/* For.D3D11_BUFFER_DESC */
	m_IBDesc.ByteWidth = sizeof(LINEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	LINEINDICES16*		pIndices = new LINEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(LINEINDICES16) * m_iNumPrimitive);

	unsigned short count = 0;
	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = count;
		pIndices[i]._1 = count + 1;
		count++;
	}

	m_IBSubResourceData.pSysMem = pIndices;

#pragma endregion

	if (FAILED(__super::Create_Buffers()))
		return E_FAIL;

	//if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB)))
	//	return E_FAIL;


	SafeDeleteArray(pIndices);
	m_shaderPath = pShaderFilePath;

	return S_OK;
}



HRESULT CVIBuffer_Line::Initialize(void * pArg)
{
	m_pShader = make_unique<CShader>(m_shaderPath);
	ZeroMemory(m_pointPosition, sizeof(_vector) * 128);

	return S_OK;
}

HRESULT CVIBuffer_Line::Update(_double TimeDelta)
{
	return S_OK;
}

HRESULT CVIBuffer_Line::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;



	m_pShader->SetUp_ValueOnShader("g_PointWorldPosition", m_pointPosition, sizeof(_vector) * 128);

	__super::Render(iPassIndex);
	//_uint		iOffset = 0;

	//m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, m_pVB.GetAddressOf(), &m_iStride, &iOffset);
	//m_pDeviceContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	//m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitive);

	//m_pShader->Render();

	//m_pDeviceContext->DrawIndexed(m_iNumPrimitive * m_iNumVerticesPerPrimitive, 0, 0);

	return S_OK;
}

void CVIBuffer_Line::SetPoints(list<_vector> points)
{
	if (points.size() > m_iNumVertices)
		return;

	int count = 0;
	for (auto& point : points)
	{
		m_pointPosition[count] = point;
		count++;
	}
}

CVIBuffer_Line * CVIBuffer_Line::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, string pShaderFilePath, _uint iNumPoint)
{
	CVIBuffer_Line*	pInstance = new CVIBuffer_Line(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(pShaderFilePath, iNumPoint)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Line::Clone(void * pArg)
{
	CVIBuffer_Line*	pInstance = new CVIBuffer_Line(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Line::Free()
{
	__super::Free();

	SafeRelease(m_pVBInstance);
}
