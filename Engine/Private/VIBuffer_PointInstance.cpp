#include "..\Public\VIBuffer_PointInstance.h"
#include "Pipeline.h"

CVIBuffer_PointInstance::CVIBuffer_PointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_PointInstance::CVIBuffer_PointInstance(const CVIBuffer_PointInstance & rhs)
	: CVIBuffer(rhs)
	, m_VBInstanceDesc(rhs.m_VBInstanceDesc)
	, m_VBInstanceSubResourceData(rhs.m_VBInstanceSubResourceData)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_InstanceMatrices(rhs.m_InstanceMatrices)
	, m_shaderPath(rhs.m_shaderPath)
{
	SafeAddRef(m_pVBInstance);

}

HRESULT CVIBuffer_PointInstance::InitializePrototype(string pShaderFilePath, _uint iNumInstance)
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	m_iNumInstance = iNumInstance;
	m_iNumVertexBuffers = 2;

#pragma region VERTEXBUFFER

	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = m_iNumInstance;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		((VTXPOINT*)m_pVertices)[i].vPosition = _float3(0.0f, 0.0f, 0.f);
		((VTXPOINT*)m_pVertices)[i].vSize = _float2(1.0f, 1.0f);
	}

	/* For.D3D11_SUBRESOURCE_DATA */
	m_VBSubResourceData.pSysMem = m_pVertices;

#pragma endregion VERTEXBUFFER

	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	if (FAILED(__super::Create_Buffers()))
	return E_FAIL;	

#pragma region VERTEXINSTANCEBUFFER

	ZeroMemory(&m_VBInstanceDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstanceDesc.ByteWidth = m_iNumInstance * sizeof(VTXMATRIX);
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = sizeof(VTXMATRIX);

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	m_VBInstanceSubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &m_VBInstanceSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	SafeDeleteArray(pInstanceVertices);
#pragma endregion 

	m_shaderPath = pShaderFilePath;

	return S_OK;
}



HRESULT CVIBuffer_PointInstance::Initialize(void * pArg)
{
	m_pShader = make_unique<CShader>(m_shaderPath);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXMATRIX*		pIV = new VTXMATRIX();
		pIV->vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pIV->vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pIV->vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pIV->vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		pIV->iStartFrame = 0;
		m_InstanceMatrices.push_back(pIV);
	}

	return S_OK;
}

HRESULT CVIBuffer_PointInstance::Update(_double TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	CPipeline*		pPipeLine = GET_INSTANCE(CPipeline);

	//sort(m_InstanceMatrices.begin(), m_InstanceMatrices.end(), [&](VTXMATRIX* SourMatrix, VTXMATRIX* DestMatrix) {
	//	_vector		SourPosition = XMVector4Transform(XMLoadFloat4(&SourMatrix->vPosition), pPipeLine->Get_Transform(CPipeline::D3DTS_VIEW));
	//	_vector		DestPosition = XMVector4Transform(XMLoadFloat4(&DestMatrix->vPosition), pPipeLine->Get_Transform(CPipeline::D3DTS_VIEW));

	//	if (XMVectorGetZ(SourPosition) > XMVectorGetZ(DestPosition))
	//		return true;

	//	return false;
	//});

	if (FAILED(m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i] = *m_InstanceMatrices[i];
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);

	RELEASE_INSTANCE(CPipeline);

	return S_OK;
}

HRESULT CVIBuffer_PointInstance::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVBBuffers[] = {
		m_pVB.Get(),
		m_pVBInstance
	};

	_uint	iStrides[] = {
		sizeof(VTXPOINT),
		sizeof(VTXMATRIX)
	};

	_uint		iOffset[] = {
		0, 0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVBBuffers, iStrides, iOffset);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitive);
	m_pShader->Render(iPassIndex);
	m_pDeviceContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	return S_OK;

	return S_OK;
}

CVIBuffer_PointInstance * CVIBuffer_PointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, string pShaderFilePath, _uint iNumInstance)
{
	CVIBuffer_PointInstance*	pInstance = new CVIBuffer_PointInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(pShaderFilePath, iNumInstance)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_PointInstance::Clone(void * pArg)
{
	CVIBuffer_PointInstance*	pInstance = new CVIBuffer_PointInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_PointInstance::Free()
{
	__super::Free();

	SafeRelease(m_pVBInstance);
}