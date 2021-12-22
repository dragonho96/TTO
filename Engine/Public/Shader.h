#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CShader
{
public:
	explicit CShader(string file);
	virtual ~CShader();

public:
	HRESULT Render(_uint iPassIndex = 0);
	HRESULT SetInputLayout(_uint iPassIndex = 0);
	HRESULT Apply(_uint iPassIndex = 0);
private:
	//HRESULT CompileShaderFromFile(const WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut);

	//HRESULT CreateVertexShader();
	//HRESULT CreatePixelShader();
	//HRESULT CreateInputLayout();

public:
	HRESULT Compile_Shader(string pShaderFilePath, _uint iTechniqueIndex = 0);
	HRESULT SetUp_ValueOnShader(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT SetUp_TextureOnShader(const char* pConstantName, class CTexture* pTextureComponent, _uint iTextureIndex = 0);
	HRESULT SetUp_TextureOnShader(const char* pConstantName, vector<MODELTEXTURES*> modelTextures, _uint iMaterialIndex, aiTextureType eTextureType);
	ID3D11InputLayout* CreateInputLayout(ID3DBlob* fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params);
	ID3DX11EffectShaderResourceVariable* AsSRV(string name);


protected:
	vector<EFFECTDESC>			m_EffectDescs;
	ID3DX11Effect*				m_pEffect = nullptr;

private:

	string shaderFile;
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