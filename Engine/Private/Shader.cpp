#include "Engine_Defines.h"
#include "..\Public\Shader.h"

CShader::CShader(wstring file, string vsName, string psName)
	: shaderFile(file), vsName(vsName), psName(psName)
{
	CreateVertexShader();
	CreatePixelShader();
	CreateInputLayout();
}

CShader::~CShader()
{
}



void CShader::Render()
{
	CEngine::GetInstance()->GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
	CEngine::GetInstance()->GetDeviceContext()->VSSetShader(m_VS.Get(), NULL, 0);
	CEngine::GetInstance()->GetDeviceContext()->PSSetShader(m_PS.Get(), NULL, 0);
}

HRESULT CShader::CompileShaderFromFile(const WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.c
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ComRef<ID3DBlob> pErrorBlob;
	hr = D3DX11CompileFromFileW(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		return hr;
	}

	return S_OK;
}

HRESULT CShader::CreateVertexShader()
{
	HRESULT hr;
	hr = CompileShaderFromFile(shaderFile.c_str(), "VS", "vs_5_0", &m_VSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The shader file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = CEngine::GetInstance()->GetDevice()->CreateVertexShader(
		m_VSBlob->GetBufferPointer(),
		m_VSBlob->GetBufferSize(),
		NULL,
		&m_VS
	);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT CShader::CreatePixelShader()
{
	HRESULT hr;
	hr = CompileShaderFromFile(shaderFile.c_str(), "PS", "ps_5_0", &m_PSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The shader file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = CEngine::GetInstance()->GetDevice()->CreatePixelShader(
		m_PSBlob->GetBufferPointer(),
		m_PSBlob->GetBufferSize(),
		NULL,
		&m_PS
	);
	if (FAILED(hr))
		return hr;

	return S_OK;
}


HRESULT CShader::CreateInputLayout()
{
	HRESULT hr;
	hr = D3DReflect
	(
		m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, IID_ID3D11ShaderReflection
		, (void**)&m_Reflection
	);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_DESC shaderDesc;
	m_Reflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		m_Reflection->GetInputParameterDesc(i, &paramDesc);

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

		string temp = paramDesc.SemanticName;
		if (temp == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		inputLayoutDesc.push_back(elementDesc);
	}

	hr = CEngine::GetInstance()->GetDevice()->CreateInputLayout
	(
		&inputLayoutDesc[0]
		, inputLayoutDesc.size()
		, m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, &m_InputLayout
	);
	return S_OK;
}
