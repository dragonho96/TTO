#pragma once

#include "VIBuffer.h"
#include "RectTransform.h"
#include "Texture.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectUI final : public CVIBuffer
{
private:
	explicit CVIBuffer_RectUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_RectUI(const CVIBuffer_RectUI& rhs);
	virtual ~CVIBuffer_RectUI() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual HRESULT Render() override;
public:
	static CVIBuffer_RectUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	string GetTextureFilePath();
	void UpdateTexture(string texturePath);

private:
	CRectTransform* m_pRectTransform = nullptr;
	CTexture*		m_pTexture = nullptr;
};

END