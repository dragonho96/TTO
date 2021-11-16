#include "..\public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_IBDesc(rhs.m_IBDesc)
	, m_VBSubResourceData(rhs.m_VBSubResourceData)
	, m_IBSubResourceData(rhs.m_IBSubResourceData)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_pVertices(rhs.m_pVertices)
{
	SafeAddRef(m_pIB);
	SafeAddRef(m_pVB);
}

HRESULT CVIBuffer::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	_uint		iOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, &m_pVB, &m_iStride, &iOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitive);
	// m_pDeviceContext->IASetInputLayout();

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffers()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubResourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;

}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)	
		SafeDeleteArray(m_pVertices);	

	SafeRelease(m_pIB);
	SafeRelease(m_pVB);
}
