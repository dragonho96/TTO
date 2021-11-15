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
	hr = CompileShaderFromFile(shaderFile.c_str(), "VS", "vs_4_0", &m_VSBlob);
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
	hr = CompileShaderFromFile(shaderFile.c_str(), "PS", "ps_4_0", &m_PSBlob);
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
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = CEngine::GetInstance()->GetDevice()->CreateInputLayout(layout, numElements, m_VSBlob->GetBufferPointer(),
		m_VSBlob->GetBufferSize(), &m_InputLayout);
	if (FAILED(hr))
		return hr;

	// Set the input layout
	CEngine::GetInstance()->GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());

	return S_OK;
}
