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
	, m_ePrimitive(rhs.m_ePrimitive)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
{
	//SafeAddRef(m_pIB);
	//SafeAddRef(m_pVB);
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

	m_pObjTransform = (CTransform*)pArg;

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	_uint		iOffset = 0;

	ConstantBuffer cb1;
	//cb1.mWorld = XMMatrixTranspose(XMMatrixIdentity());
	cb1.mWorld = XMMatrixTranspose( XMLoadFloat4x4( &m_pObjTransform->GetMatrix() ) );
	cb1.mView = XMMatrixTranspose(CEngine::GetInstance()->GetViewMatrix());
	cb1.mProjection = XMMatrixTranspose(CEngine::GetInstance()->GetProjectionMatrix());
	CEngine::GetInstance()->GetDeviceContext()->UpdateSubresource(
		CEngine::GetInstance()->GetConstantBuffer(), 0, NULL, &cb1, 0, 0);

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, m_pVB.GetAddressOf(), &m_iStride, &iOffset);
	
	if (m_IBSubResourceData.pSysMem)
		m_pDeviceContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);

	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitive);
	// m_pDeviceContext->IASetInputLayout();

	m_pShader->Render();

	//if (m_IBSubResourceData.pSysMem)
	//	m_pDeviceContext->DrawIndexed();
	CEngine::GetInstance()->GetDeviceContext()->Draw(m_iNumVertices, 0);
	return S_OK;
}

HRESULT CVIBuffer::Create_Buffers()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB)))
		return E_FAIL;

	if (m_IBSubResourceData.pSysMem) 
	{
		if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubResourceData, &m_pIB)))
			return E_FAIL;
	}

	return S_OK;

}

HRESULT CVIBuffer::Compile_Shader(const _tchar* pShaderFilePath, _uint iTechniqueIndex)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
	
	ID3DBlob*		pCompileShader = nullptr;
	ID3DBlob*		pCompileShaderErrorMessage = nullptr;
	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, nullptr, nullptr, "fx_5_0", iFlag, 0, &pCompileShader, &pCompileShaderErrorMessage)))
		return E_FAIL;

	if (FAILED(D3DX11CreateEffectFromMemory(pCompileShader->GetBufferPointer(), pCompileShader->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(iTechniqueIndex);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	pTechnique->GetDesc(&TechniqueDesc);

	//for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	//{
	//	D3DX11_PASS_DESC			PassDesc;
	//	m_pDevice->CreateInputLayout();


	//}






	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
		SafeDeleteArray(m_pVertices);


	//SafeRelease(m_pIB);
	//SafeRelease(m_pVB);
}
