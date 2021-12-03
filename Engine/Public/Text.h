#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CText final : public CComponent
{
public:
	explicit CText(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CText(const CText& rhs);
	virtual ~CText() = default;
public:
	static CText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	HRESULT		Render();
public:
	void SetTextInfo(string text, _float layerDepth = 0.f, _vector color = DirectX::Colors::White);
public:
	string& GetText() { return m_strText; }
	_vector& GetColor() { return m_vColor; }

private:
	SpriteBatch*		m_pSpriteBatch;
	SpriteFont*			m_pSpriteFont;
private:
	class CRectTransform*			m_pTransform;
	string				m_strText = "";
	_vector				m_vColor = DirectX::Colors::White;
	_float				m_fLayerDepth = 0.f;
};

END