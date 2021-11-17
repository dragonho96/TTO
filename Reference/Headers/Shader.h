#pragma once
#include "Engine_Defines.h"
#include "Engine.h"

BEGIN(Engine)

class CShader
{
public:
	explicit CShader(wstring file, string vsName = "VS", string psName = "PS");
	virtual ~CShader();

public:
	void Render();

private:
	HRESULT CompileShaderFromFile(const WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut);
	HRESULT CreateVertexShader();
	HRESULT CreatePixelShader();
	HRESULT CreateInputLayout();

private:
	wstring shaderFile;
	string vsName;
	string psName;

	ComRef<ID3D11VertexShader> m_VS;
	ComRef<ID3D11PixelShader> m_PS;

	ComRef<ID3D11InputLayout> m_InputLayout;

	ComRef<ID3DBlob> m_VSBlob;
	ComRef<ID3DBlob> m_PSBlob;

	ComRef<ID3D11ShaderReflection> m_Reflection;
};

END