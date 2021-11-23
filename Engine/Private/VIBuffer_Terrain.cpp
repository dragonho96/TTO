#include "..\Public\VIBuffer_Terrain.h"

USING(Engine)

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
{
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Terrain");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Terrain");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}

HRESULT CVIBuffer_Terrain::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	_uint	sizeX = 100;
	_uint	sizeZ = 100;
	float	fInterval = 0.2f;
#pragma region VERTEXBUFFER
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = (sizeX) * (sizeZ);
	m_iNumVertexBuffers = 1;
	m_iNumVerticesPerPrimitive = 3;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXTEX) * m_iNumVertices);

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	for (_uint i = 0; i < sizeZ; ++i)
	{
		for (_uint j = 0; j < sizeX; ++j)
		{
			_uint index = i * sizeX + j;
			float y = sin(j * 0.1f);
			pVertices[index].vPosition = _float3(j * fInterval, y, i * fInterval);
			pVertices[index].vTexUV = _float2(0.f, 0.f);
		}
	}

	memcpy(m_pVertices, pVertices, sizeof(VTXTEX) * m_iNumVertices);
	SafeDeleteArray(pVertices);

	m_VBSubResourceData.pSysMem = m_pVertices;

#pragma endregion VERTEXBUFFER

#pragma region INDEXBUFFER
	m_iNumPrimitive = (sizeZ - 1) * (sizeX - 1) * 2;
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

#pragma endregion INDEXBUFFER
	int iPrimitiveCount = 0;
	for (int i = 0; i < sizeZ - 1; ++i)
	{
		for (int j = 0; j < sizeX - 1; ++j)
		{
			int index = i * sizeX + j;
			pIndices[iPrimitiveCount]._0 = index + sizeZ;
			pIndices[iPrimitiveCount]._1 = index + sizeZ + 1;
			pIndices[iPrimitiveCount]._2 = index + 1;
			++iPrimitiveCount;
			pIndices[iPrimitiveCount]._0 = index + sizeZ;
			pIndices[iPrimitiveCount]._1 = index + 1;
			pIndices[iPrimitiveCount]._2 = index;
			++iPrimitiveCount;
		}
	}
	m_IBSubResourceData.pSysMem = pIndices;
	if (FAILED(__super::Create_Buffers()))
		return E_FAIL;

	SafeDeleteArray(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pShader = make_unique<CShader>(L"../../Assets/Shader/Shader_Rect.fx");
	return S_OK;
}
