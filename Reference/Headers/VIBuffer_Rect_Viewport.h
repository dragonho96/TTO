#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Viewport final : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect_Viewport(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Rect_Viewport(const CVIBuffer_Rect_Viewport& rhs);
	virtual ~CVIBuffer_Rect_Viewport() = default;
public:
	virtual HRESULT InitializePrototype(_float fX, _float fY, _float fWidth, _float fHeight, string pShaderFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	string		m_shaderPath = "";
public:
	static CVIBuffer_Rect_Viewport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fX, _float fY, _float fWidth, _float fHeight, string pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END