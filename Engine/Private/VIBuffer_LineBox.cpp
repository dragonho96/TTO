#include "..\Public\VIBuffer_LineBox.h"


CVIBuffer_LineBox::CVIBuffer_LineBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
	, m_Size(1.f, 1.f, 1.f)
{
}

CVIBuffer_LineBox::CVIBuffer_LineBox(const CVIBuffer_LineBox & rhs)
	: CVIBuffer(rhs)
	, m_Size(1.f, 1.f, 1.f)
{
}


HRESULT CVIBuffer_LineBox::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	CreateBuffer(&m_pVertices);
}

HRESULT CVIBuffer_LineBox::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pShader = make_unique<CShader>(L"../../Assets/Shader/Tutorial05.fx");
	return S_OK;
}

CVIBuffer_LineBox * CVIBuffer_LineBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_LineBox*	pInstance = new CVIBuffer_LineBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CVIBuffer_LineBox");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_LineBox::Clone(void * pArg)
{
	CComponent*	pInstance = new CVIBuffer_LineBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_LineBox");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_LineBox::Free()
{
	__super::Free();
	SafeDeleteArray(m_pCloneVertices);
}

HRESULT CVIBuffer_LineBox::CreateBuffer(void** pVertices)
{
	SafeDeleteArray(*pVertices);

#pragma region VERTEXBUFFER
	m_iStride = sizeof(VTXCOLOR);
	m_iNumVertices = 24; // (m_fSliceCount * 3) * 2 
	m_iNumVertexBuffers = 1;

	/* For.D3D11_BUFFER_DESC */
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	_float3 max{ m_Size.x / 2.f, m_Size.y / 2.f , m_Size.z / 2.f };
	_float3 min{ -m_Size.x / 2.f, -m_Size.y / 2.f , -m_Size.z / 2.f };

	VTXCOLOR vertices[8];

	// ¾Õ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	vertices[0].vPos = _float3(min.x, min.y, min.z);
	vertices[1].vPos = _float3(min.x, max.y, min.z);
	vertices[2].vPos = _float3(max.x, min.y, min.z);
	vertices[3].vPos = _float3(max.x, max.y, min.z);

	// µÚ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	vertices[4].vPos = _float3(min.x, min.y, max.z);
	vertices[5].vPos = _float3(min.x, max.y, max.z);
	vertices[6].vPos = _float3(max.x, min.y, max.z);
	vertices[7].vPos = _float3(max.x, max.y, max.z);

	VTXCOLOR temp[] =
	{
		// ¾Õ¸é
		vertices[0],vertices[1], vertices[1],vertices[3],
		vertices[3],vertices[2], vertices[2],vertices[0],

		// µÞ¸é
		vertices[4],vertices[5], vertices[5],vertices[7],
		vertices[7],vertices[6], vertices[6],vertices[4],

		// ¿ÞÂÊ ¿·
		vertices[0],vertices[4], vertices[1],vertices[5],
		// ¿À¸¥ÂÊ ¿·
		vertices[2],vertices[6], vertices[3],vertices[7]
	};

	*pVertices = new VTXCOLOR[m_iNumVertices];
	VTXCOLOR* lines = new VTXCOLOR[m_iNumVertices];
	ZeroMemory(*pVertices, sizeof(VTXCOLOR) * m_iNumVertices);
	ZeroMemory(lines, sizeof(VTXCOLOR) * m_iNumVertices);

	for (int i = 0; i < 24; i++)
	{
		lines[i] = temp[i];
		lines[i].vColor = { 0.f, 1.f, 0.f, 1.f };
	}

	memcpy(*pVertices, lines, sizeof(VTXCOLOR) * m_iNumVertices);

	//SafeDeleteArray(vertices);
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

void CVIBuffer_LineBox::SetSize(_float3 vSize)
{
	m_Size = vSize;
	CreateBuffer(&m_pCloneVertices);
}
