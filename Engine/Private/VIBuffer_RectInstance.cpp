#include "..\Public\VIBuffer_RectInstance.h"
#include "Pipeline.h"

CVIBuffer_RectInstance::CVIBuffer_RectInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)	
{

}

CVIBuffer_RectInstance::CVIBuffer_RectInstance(const CVIBuffer_RectInstance & rhs)
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

HRESULT CVIBuffer_RectInstance::InitializePrototype(string pShaderFilePath, _uint iNumInstance)
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;	

	m_iNumInstance = iNumInstance;
	m_iNumVertexBuffers = 2;

#pragma region VERTEXBUFFER

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	
	
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

	m_iNumPrimitive = 2 * m_iNumInstance;
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

	_uint		iNumPolygons = 0;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumPolygons]._0 = 0;
		pIndices[iNumPolygons]._1 = 1;
		pIndices[iNumPolygons]._2 = 2;
		++iNumPolygons;

		pIndices[iNumPolygons]._0 = 0;
		pIndices[iNumPolygons]._1 = 2;
		pIndices[iNumPolygons]._2 = 3;
		++iNumPolygons;
	}

	m_IBSubResourceData.pSysMem = pIndices;
	
#pragma endregion

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
	SafeDeleteArray(pIndices);



#pragma endregion 
	
	//D3D11_INPUT_ELEMENT_DESC		ElmentDesc[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 

	//	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	//	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	//	{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	//	{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	//};

	//if (FAILED(__super::Compile_Shader(ElmentDesc, 6, pShaderFilePath)))
	//	return E_FAIL;

	m_shaderPath = pShaderFilePath;
	return S_OK;
}



HRESULT CVIBuffer_RectInstance::Initialize(void * pArg)
{
	m_pShader = make_unique<CShader>(m_shaderPath);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXMATRIX*		pIV = new VTXMATRIX();
		pIV->vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pIV->vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pIV->vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pIV->vPosition = _float4(0, 0, 0, 1.f);
		pIV->iStartFrame = rand() % 36;
		m_InstanceMatrices.push_back(pIV);
	}
	return S_OK;
}

HRESULT CVIBuffer_RectInstance::Update(_double TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	CPipeline*		pPipeLine = GET_INSTANCE(CPipeline);

	sort(m_InstanceMatrices.begin(), m_InstanceMatrices.end(), [&](VTXMATRIX* SourMatrix, VTXMATRIX* DestMatrix) {
		_vector		SourPosition = XMVector4Transform(XMLoadFloat4(&SourMatrix->vPosition), pPipeLine->Get_Transform(CPipeline::D3DTS_VIEW));
		_vector		DestPosition = XMVector4Transform(XMLoadFloat4(&DestMatrix->vPosition), pPipeLine->Get_Transform(CPipeline::D3DTS_VIEW));

		if (XMVectorGetZ(SourPosition) > XMVectorGetZ(DestPosition))
			return true;

		return false;
	});	

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

HRESULT CVIBuffer_RectInstance::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;



	ID3D11Buffer*	pVBBuffers[] = {
		m_pVB.Get(), 		
		m_pVBInstance
	};

	_uint	iStrides[] = {
		sizeof(VTXTEX),
		sizeof(VTXMATRIX)
	};

	_uint		iOffset[] = {
		0, 0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVBBuffers, iStrides, iOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitive);


	//m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex].pLayout);
	//if (FAILED(m_EffectDescs[iPassIndex].pPass->Apply(0, m_pDeviceContext)))
	//	return E_FAIL;
	m_pShader->Render(iPassIndex);

	/* 0 : 인스턴스하나당 인덱스버퍼의 인덱슬르 몇개 활용하여 그리는지? */
	m_pDeviceContext->DrawIndexedInstanced(6, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

CVIBuffer_RectInstance * CVIBuffer_RectInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, string pShaderFilePath, _uint iNumInstance)
{
	CVIBuffer_RectInstance*	pInstance = new CVIBuffer_RectInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(pShaderFilePath, iNumInstance)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_RectInstance::Clone(void * pArg)
{
	CVIBuffer_RectInstance*	pInstance = new CVIBuffer_RectInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CVIBuffer_RectInstance::Free()
{
	__super::Free();
	for (auto& matrix : m_InstanceMatrices)
		SafeDelete(matrix);
	m_InstanceMatrices.clear();

	SafeRelease(m_pVBInstance);
}
