#include "..\Public\VIBuffer_LineCapsule.h"


CVIBuffer_LineCapsule::CVIBuffer_LineCapsule(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
	, m_fSliceCount(36)
{
}

CVIBuffer_LineCapsule::CVIBuffer_LineCapsule(const CVIBuffer_LineCapsule & rhs)
	: CVIBuffer(rhs)
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
	, m_fSliceCount(36)
{
}


HRESULT CVIBuffer_LineCapsule::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	CreateBuffer(&m_pVertices);
}

HRESULT CVIBuffer_LineCapsule::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pShader = make_unique<CShader>(L"../../Assets/Shader/Tutorial05.fx");
	return S_OK;
}

CVIBuffer_LineCapsule * CVIBuffer_LineCapsule::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_LineCapsule*	pInstance = new CVIBuffer_LineCapsule(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CVIBuffer_LineCapsule");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_LineCapsule::Clone(void * pArg)
{
	CVIBuffer_LineCapsule*	pInstance = new CVIBuffer_LineCapsule(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_LineCapsule");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_LineCapsule::Free()
{
	__super::Free();
	SafeDeleteArray(m_pCloneVertices);
}

HRESULT CVIBuffer_LineCapsule::CreateBuffer(void** pVertices)
{
	SafeDeleteArray(*pVertices);

#pragma region VERTEXBUFFER
	m_iStride = sizeof(VTXCOLOR);
	m_iNumVertices = (m_fSliceCount * 4 + 4) * 2;
	m_iNumVertexBuffers = 1;

	/* For.D3D11_BUFFER_DESC */
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;



	float phiStep = 2.0f * 3.14f / (float)m_fSliceCount;

	int vertexCount = m_fSliceCount * 4 + 4;
	_float3* vertices = new _float3[vertexCount];

	_int index = 0;
	float phi = 0.f;
	// x = 0
	for (_int i = 0; i < m_fSliceCount / 2; i++) {
		phi = i * phiStep;
		vertices[index] = _float3(0, (m_fRadius * sinf(phi)) + m_fHeight / 2.f, (m_fRadius * cosf(phi)));
		index++;
	}
	vertices[index] = _float3(0, (m_fRadius * sinf(phi)) - m_fHeight / 2.f, (m_fRadius * cosf(phi)));
	index++;
	// x = 0
	phi = 0.f;
	for (_int i = m_fSliceCount / 2; i < m_fSliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3(0, (m_fRadius * sinf(phi)) - m_fHeight / 2.f, (m_fRadius * cosf(phi)));
		index++;
	}
	vertices[index] = _float3(0, (m_fRadius * sinf(phi)) + m_fHeight / 2.f, (m_fRadius * cosf(phi)));
	index++;


	// y = 0
	phi = 0.f;
	for (_int i = 0; i < m_fSliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), m_fHeight / 2.f, (m_fRadius * sinf(phi)));
		index++;
	}
	// y = 0
	phi = 0.f;
	for (_int i = 0; i < m_fSliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), -m_fHeight / 2.f, (m_fRadius * sinf(phi)));
		index++;
	}

	// z = 0
	phi = 0.f;
	for (_int i = 0; i < m_fSliceCount / 2; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) + m_fHeight / 2.f, 0);
		index++;
	}
	vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) - m_fHeight / 2.f, 0);
	index++;
	// z = 0
	phi = 0.f;
	for (_int i = m_fSliceCount / 2; i < m_fSliceCount; i++) {
		phi = i * phiStep;
		vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) - m_fHeight / 2.f, 0);
		index++;
	}
	vertices[index] = _float3((m_fRadius * cosf(phi)), (m_fRadius * sinf(phi)) + m_fHeight / 2.f, 0);
	index++;

	*pVertices = new VTXCOLOR[m_iNumVertices];
	VTXCOLOR* m_pLines = new VTXCOLOR[m_iNumVertices];
	ZeroMemory(*pVertices, sizeof(VTXCOLOR) * m_iNumVertices);
	ZeroMemory(m_pLines, sizeof(VTXCOLOR) * m_iNumVertices);

	index = 0;
	for (_int i = 0; i < m_fSliceCount + 2; i++) {
		m_pLines[index++].vPos = vertices[i];
		m_pLines[index++].vPos = i == (m_fSliceCount + 2) - 1 ? vertices[0] : vertices[i + 1];
	}

	for (_int i = m_fSliceCount + 2; i < m_fSliceCount * 2 + 2; i++) {
		m_pLines[index++].vPos = vertices[i];
		m_pLines[index++].vPos = i == (m_fSliceCount * 2 + 2) - 1 ? vertices[m_fSliceCount + 2] : vertices[i + 1];
	}

	for (_int i = m_fSliceCount * 2 + 2; i < m_fSliceCount * 3 + 2; i++) {
		m_pLines[index++].vPos = vertices[i];
		m_pLines[index++].vPos = i == (m_fSliceCount * 3 + 2) - 1 ? vertices[m_fSliceCount * 2 + 2] : vertices[i + 1];
	}

	for (_int i = m_fSliceCount * 3 + 2; i < m_fSliceCount * 4 + 4; i++) {
		m_pLines[index++].vPos = vertices[i];
		m_pLines[index++].vPos = i == (m_fSliceCount * 4 + 4) - 1 ? vertices[m_fSliceCount * 3 + 2] : vertices[i + 1];
	}
	for (_int i = 0; i < m_iNumVertices; ++i)
		m_pLines[i].vColor = { 0.f, 1.f, 0.f, 1.f };

	SafeDeleteArray(vertices);

	memcpy(*pVertices, m_pLines, sizeof(VTXCOLOR) * m_iNumVertices);

	SafeDeleteArray(vertices);
	SafeDeleteArray(m_pLines);

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

void CVIBuffer_LineCapsule::SetSize(float fRadius, float fHeight)
{
	m_fRadius = fRadius;
	m_fHeight = fHeight;
	CreateBuffer(&m_pCloneVertices);
}

