#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TEXTURETYPE { TYPE_DDS, TYPE_TGA, TYPE_WIC, TYPE_END };
private:
	explicit CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public: /* Getter */
	ID3D11ShaderResourceView* Get_ShaderResourceView(_int iIndex = 0) {
		return m_Textures[iIndex];
	}


public:
	virtual HRESULT InitializePrototype(TEXTURETYPE eType, const _tchar* pTextureFilePath, _int iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

private:
	vector<ID3D11ShaderResourceView*>			m_Textures;
	typedef vector<ID3D11ShaderResourceView*>	TEXTURES;

private:
	_uint			m_iNumTextures = 0;


public:
	/* 원형생성. */
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context, TEXTURETYPE eType, const _tchar* pTextureFilePath, _int iNumTextures = 1);

	/* 복사본생성. */
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};
END