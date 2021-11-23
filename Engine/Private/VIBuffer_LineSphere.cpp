#include "..\Public\VIBuffer_LineSphere.h"


CVIBuffer_LineSphere::CVIBuffer_LineSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
	, m_fRadius(0.5f)
	, m_fSliceCount(36)
{
}

CVIBuffer_LineSphere::CVIBuffer_LineSphere(const CVIBuffer_LineSphere & rhs)
	: CVIBuffer(rhs)
	, m_fRadius(0.5f)
	, m_fSliceCount(36)
{
}


HRESULT CVIBuffer_LineSphere::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	CreateBuffer(&m_pVertices);
}

HRESULT CVIBuffer_LineSphere::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pShader = make_unique<CShader>(L"../../Assets/Shader/DebugLine.fx");
	return S_OK;
}

CVIBuffer_LineSphere * CVIBuffer_LineSphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_LineSphere*	pInstance = new CVIBuffer_LineSphere(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CVIBuffer_LineSphere");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_LineSphere::Clone(void * pArg)
{
	CComponent*	pInstance = new CVIBuffer_LineSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_LineSphere");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_LineSphere::Free()
{
	__super::Free();
	SafeDeleteArray(m_pCloneVertices);
}

HRESULT CVIBuffer_LineSphere::CreateBuffer(void** pVertices)
{
	SafeDeleteArray(*pVertices);

#pragma region VERTEXBUFFER
	m_iStride = sizeof(VTXCOLOR);
	m_iNumVertices = m_fSliceCount * 6; // (m_fSliceCount * 3) * 2 
	m_iNumVertexBuffers = 1;

	/* For.D3D11_BUFFER_DESC */
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	float phiStep = 2.0f * 3.14f / (float)m_fSliceCount;

	// Create Vertex
	int vertexCount = m_fSliceCount * 3;
	_float3* vertices = new _float3[vertexCount];

	UINT index = 0;
	// x = 0
	for (UINT i = 0; i < m_fSliceCount; i++) {
		float phi = i * phiStep;
		vertices[index] = _float3(0, (m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)));
		index++;
	}
	// y = 0
	for (UINT i = 0; i < m_fSliceCount; i++) {
		float phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), 0, (m_fRadius * sinf(phi)));
		index++;
	}
	// z = 0
	for (UINT i = 0; i < m_fSliceCount; i++) {
		float phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)), 0);
		index++;
	}

	*pVertices = new VTXCOLOR[m_iNumVertices];
	VTXCOLOR* lines = new VTXCOLOR[m_iNumVertices];
	ZeroMemory(*pVertices, sizeof(VTXCOLOR) * m_iNumVertices);
	ZeroMemory(lines, sizeof(VTXCOLOR) * m_iNumVertices);


	index = 0;
	for (UINT i = 0; i < m_fSliceCount; i++) {
		lines[index++].vPos = vertices[i];
		lines[index++].vPos = i == m_fSliceCount - 1 ? vertices[0] : vertices[i + 1];
	}
	for (UINT i = m_fSliceCount; i < m_fSliceCount * 2; i++) {
		lines[index++].vPos = vertices[i];
		lines[index++].vPos = i == m_fSliceCount * 2 - 1 ? vertices[m_fSliceCount] : vertices[i + 1];
	}
	for (UINT i = m_fSliceCount * 2; i < m_fSliceCount * 3; i++) {
		lines[index++].vPos = vertices[i];
		lines[index++].vPos = i == m_fSliceCount * 3 - 1 ? vertices[m_fSliceCount * 2] : vertices[i + 1];
	}

	for (UINT i = 0; i < m_iNumVertices; ++i) {
		lines[i].vColor = { 0.f, 1.f, 0.f, 1.f };
	}

	memcpy(*pVertices, lines, sizeof(VTXCOLOR) * m_iNumVertices);

	SafeDeleteArray(vertices);
	SafeDeleteArray(lines);

	/* For.D3D11_SUBRESOURCE_DATA */
	m_VBSubResourceData.pSysMem = *pVertices;
#pragma endregion


#pragma region INDEXBUFFER
	/* No Index Buffer */
#pragma endregion INDEXBUFFER

	if (FAILED(__super::Create_Buffers()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_LineSphere::SetSize(float fRadius)
{
	m_fRadius = fRadius;
	CreateBuffer(&m_pCloneVertices);
}
