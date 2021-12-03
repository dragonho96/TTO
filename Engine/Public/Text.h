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
	void SetTextInfo(string text, _float layerDepth = 0.f, _float4 color = {1.f, 1.f, 1.f, 1.f});
public:
	string& GetText() { return m_strText; }
	_float4& GetColor() { return m_vColor; }
	_float	GetLayerDepth() { return m_fLayerDepth; }
	_float2& GetScale() { return m_vScale; }

private:
	SpriteBatch*		m_pSpriteBatch;
	SpriteFont*			m_pSpriteFont;
private:
	class CRectTransform*			m_pTransform;
	string							m_strText = "";
	_float4							m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_float							m_fLayerDepth = 0.f;
	_float2							m_vScale = { 1.f, 1.f };
};

END