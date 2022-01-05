#include "..\Public\Shader.h"
#include "Texture.h"
#include "Engine.h"

CShader::CShader(string file)
	: shaderFile(file)
{
	Compile_Shader(file);
}

CShader::~CShader()
{
	for (auto& EffectDesc : m_EffectDescs)
	{
		SafeRelease(EffectDesc.pPass);
		SafeRelease(EffectDesc.pLayout);
	}
	m_EffectDescs.clear();

	SafeRelease(m_pEffect);
}



HRESULT CShader::Render(_uint iPassIndex)
{
	CEngine::GetInstance()->GetDeviceContext()->IASetInputLayout(m_EffectDescs[iPassIndex].pLayout);
	if (FAILED(m_EffectDescs[iPassIndex].pPass->Apply(0, CEngine::GetInstance()->GetDeviceContext())))
		return E_FAIL;

	return S_OK;
	//CEngine::GetInstance()->GetDeviceContext()->VSSetShader(m_VS.Get(), NULL, 0);
	//CEngine::GetInstance()->GetDeviceContext()->PSSetShader(m_PS.Get(), NULL, 0);
}

HRESULT CShader::SetInputLayout(_uint iPassIndex)
{
	CEngine::GetInstance()->GetDeviceContext()->IASetInputLayout(m_EffectDescs[iPassIndex].pLayout);
	return S_OK;
}

HRESULT CShader::Apply(_uint iPassIndex)
{
	if (FAILED(m_EffectDescs[iPassIndex].pPass->Apply(0, CEngine::GetInstance()->GetDeviceContext())))
		return E_FAIL;

	return S_OK;
}

#pragma region VSPSSHADER
//HRESULT CShader::CompileShaderFromFile(const WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
//{
//	HRESULT hr = S_OK;
//
//	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.c
//	// Setting this flag improves the shader debugging experience, but still allows 
//	// the shaders to be optimized and to run exactly the way they will run in 
//	// the release configuration of this program.
//	dwShaderFlags |= D3DCOMPILE_DEBUG;
//#endif
//
//	ComRef<ID3DBlob> pErrorBlob;
//	if (FAILED(D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob)))
//		return E_FAIL;
//
//	//hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
//	//	dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
//
//	if (FAILED(hr))
//	{
//		if (pErrorBlob != NULL)
//			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
//		return hr;
//	}
//
//	return S_OK;
//}
//
//HRESULT CShader::CreateVertexShader()
//{
//	HRESULT hr;
//	hr = CompileShaderFromFile(shaderFile.c_str(), "VS", "vs_5_0", &m_VSBlob);
//	if (FAILED(hr))
//	{
//		MessageBox(NULL,
//			L"The shader file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
//		return hr;
//	}
//
//	hr = CEngine::GetInstance()->GetDevice()->CreateVertexShader(
//		m_VSBlob->GetBufferPointer(),
//		m_VSBlob->GetBufferSize(),
//		NULL,
//		&m_VS
//	);
//	if (FAILED(hr))
//		return hr;
//
//	return S_OK;
//}
//
//HRESULT CShader::CreatePixelShader()
//{
//	HRESULT hr;
//	hr = CompileShaderFromFile(shaderFile.c_str(), "PS", "ps_5_0", &m_PSBlob);
//	if (FAILED(hr))
//	{
//		MessageBox(NULL,
//			L"The shader file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
//		return hr;
//	}
//
//	hr = CEngine::GetInstance()->GetDevice()->CreatePixelShader(
//		m_PSBlob->GetBufferPointer(),
//		m_PSBlob->GetBufferSize(),
//		NULL,
//		&m_PS
//	);
//	if (FAILED(hr))
//		return hr;
//
//	return S_OK;
//}
//
//
//HRESULT CShader::CreateInputLayout()
//{
//	HRESULT hr;
//	hr = D3DReflect
//	(
//		m_VSBlob->GetBufferPointer()
//		, m_VSBlob->GetBufferSize()
//		, IID_ID3D11ShaderReflection
//		, (void**)&m_Reflection
//	);
//	assert(SUCCEEDED(hr));
//
//
//	D3D11_SHADER_DESC shaderDesc;
//	m_Reflection->GetDesc(&shaderDesc);
//
//	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
//	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
//	{
//		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
//		m_Reflection->GetInputParameterDesc(i, &paramDesc);
//
//		D3D11_INPUT_ELEMENT_DESC elementDesc;
//		elementDesc.SemanticName = paramDesc.SemanticName;
//		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
//		elementDesc.InputSlot = 0;
//		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
//		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//		elementDesc.InstanceDataStepRate = 0;
//
//		if (paramDesc.Mask == 1)
//		{
//			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//				elementDesc.Format = DXGI_FORMAT_R32_UINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//				elementDesc.Format = DXGI_FORMAT_R32_SINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
//		}
//		else if (paramDesc.Mask <= 3)
//		{
//			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
//		}
//		else if (paramDesc.Mask <= 7)
//		{
//			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
//		}
//		else if (paramDesc.Mask <= 15)
//		{
//			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
//			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//		}
//
//		string temp = paramDesc.SemanticName;
//		if (temp == "POSITION")
//			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
//
//		inputLayoutDesc.push_back(elementDesc);
//	}
//
//	hr = CEngine::GetInstance()->GetDevice()->CreateInputLayout
//	(
//		&inputLayoutDesc[0]
//		, inputLayoutDesc.size()
//		, m_VSBlob->GetBufferPointer()
//		, m_VSBlob->GetBufferSize()
//		, &m_InputLayout
//	);
//	return S_OK;
//}
#pragma endregion VSPSSHADER

HRESULT CShader::Compile_Shader(string pShaderFilePath, _uint iTechniqueIndex)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob*		pCompileShader = nullptr;
	ID3DBlob*		pCompileShaderErrorMessage = nullptr;

	/* 외부에서 가져온 쉐이더 파일을 번역하여 바이너리화하였고 메모리영역안에 ㄷ마았다. */
	if (FAILED(D3DCompileFromFile(StringToWString(pShaderFilePath).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pCompileShader, &pCompileShaderErrorMessage)))
		return E_FAIL;

	if (FAILED(D3DX11CreateEffectFromMemory(pCompileShader->GetBufferPointer(), pCompileShader->GetBufferSize(), 0, CEngine::GetInstance()->GetDevice(), &m_pEffect)))
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(iTechniqueIndex);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	m_EffectDescs.reserve(TechniqueDesc.Passes);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		EFFECTDESC			EffectDesc;

		EffectDesc.pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == EffectDesc.pPass)
			return E_FAIL;

		if (FAILED(EffectDesc.pPass->GetDesc(&EffectDesc.Desc)))
			return E_FAIL;

		if (FAILED(EffectDesc.pPass->GetVertexShaderDesc(&EffectDesc.PassVsDesc)))
			return E_FAIL;

		if (FAILED(EffectDesc.PassVsDesc.pShaderVariable->GetShaderDesc(EffectDesc.PassVsDesc.ShaderIndex, &EffectDesc.EffectVsDesc)))
			return E_FAIL;

		for (UINT i = 0; i < EffectDesc.EffectVsDesc.NumInputSignatureEntries; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC desc;
			HRESULT hr = EffectDesc.PassVsDesc.pShaderVariable->GetInputSignatureElementDesc(EffectDesc.PassVsDesc.ShaderIndex, i, &desc);
			assert(SUCCEEDED(hr));

			EffectDesc.vecSignatureDescs.push_back(desc);
		}

		EffectDesc.pLayout = CreateInputLayout(pCompileShader, &EffectDesc.EffectVsDesc, EffectDesc.vecSignatureDescs);

		m_EffectDescs.push_back(EffectDesc);
	}

	return S_OK;
}

HRESULT CShader::SetUp_ValueOnShader(const char * pConstantName, void * pData, _uint iByteSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	if (FAILED(pVariable->SetRawValue(pData, 0, iByteSize)))
		return E_FAIL;
	//pVariable->AsShaderResource
	return S_OK;
}

HRESULT CShader::SetUp_TextureOnShader(const char * pConstantName, CTexture * pTextureComponent, _uint iTextureIndex)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	ID3D11ShaderResourceView*		pShaderResourceView = pTextureComponent->Get_ShaderResourceView(iTextureIndex);
	if (nullptr == pShaderResourceView)
		return E_FAIL;

	if (FAILED(pVariable->SetResource(pShaderResourceView)))
		return E_FAIL;
	return S_OK;
}

HRESULT CShader::SetUp_TextureOnShader(const char * pConstantName, vector<MODELTEXTURES*> modelTextures, _uint iMaterialIndex, aiTextureType eTextureType)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	if (nullptr != modelTextures[iMaterialIndex]->pModelTexture[eTextureType])
	{
		ID3D11ShaderResourceView*		pShaderResourceView =
			modelTextures[iMaterialIndex]->pModelTexture[eTextureType]->Get_ShaderResourceView(0);
		if (nullptr == pShaderResourceView)
			return E_FAIL;
		if (FAILED(pVariable->SetResource(pShaderResourceView)))
			return E_FAIL;
	}

	return S_OK;
}

ID3D11InputLayout * CShader::CreateInputLayout(ID3DBlob * fxBlob, D3DX11_EFFECT_SHADER_DESC * effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (D3D11_SIGNATURE_PARAMETER_DESC& paramDesc : params)
	{
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string name = paramDesc.SemanticName;
		transform(name.begin(), name.end(), name.begin(), ::toupper);

		if (name == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		if (name == "INSTANCE")
		{
			elementDesc.InputSlot = 1;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		inputLayoutDesc.push_back(elementDesc);
	}


	const void* pCode = effectVsDesc->pBytecode;
	UINT pCodeSize = effectVsDesc->BytecodeLength;

	if (inputLayoutDesc.size() > 0)
	{
		ID3D11InputLayout* inputLayout = NULL;
		HRESULT hr = CEngine::GetInstance()->GetDevice()->CreateInputLayout
		(
			&inputLayoutDesc[0]
			, (UINT)inputLayoutDesc.size()
			, pCode
			, (size_t)pCodeSize
			, &inputLayout
		);
		assert(SUCCEEDED(hr));

		return inputLayout;
	}

	return NULL;
}

ID3DX11EffectShaderResourceVariable * CShader::AsSRV(string name)
{
	return m_pEffect->GetVariableByName(name.c_str())->AsShaderResource();
}
