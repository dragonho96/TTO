#pragma once
#include "Engine_Defines.h"
#include "Engine.h"

BEGIN(Engine)

class CShader
{
public:
	typedef struct tagEffectDesc
	{
		ID3DX11EffectPass*			pPass = nullptr;
		ID3D11InputLayout*			pLayout = nullptr;

		D3DX11_PASS_DESC			Desc;
		D3DX11_PASS_SHADER_DESC		PassVsDesc;
		D3DX11_EFFECT_SHADER_DESC	EffectVsDesc;
		vector<D3D11_SIGNATURE_PARAMETER_DESC> vecSignatureDescs;
	}EFFECTDESC;

public:
	explicit CShader(wstring file);
	virtual ~CShader();

public:
	HRESULT Render(_uint iPassIndex = 0);

private:
	//HRESULT CompileShaderFromFile(const WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut);

	//HRESULT CreateVertexShader();
	//HRESULT CreatePixelShader();
	//HRESULT CreateInputLayout();

public:
	HRESULT Compile_Shader(wstring pShaderFilePath, _uint iTechniqueIndex = 0);
	HRESULT SetUp_ValueOnShader(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT SetUp_TextureOnShader(const char* pConstantName, class CTexture* pTextureComponent, _uint iTextureIndex = 0);
	ID3D11InputLayout* CreateInputLayout(ID3DBlob* fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params);
	ID3DX11EffectShaderResourceVariable* AsSRV(string name);


protected:
	vector<EFFECTDESC>			m_EffectDescs;
	ID3DX11Effect*				m_pEffect = nullptr;

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