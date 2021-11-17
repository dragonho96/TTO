#pragma once

#include "VIBuffer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_LineCircle final : public CVIBuffer
{
private:
	explicit CVIBuffer_LineCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_LineCircle(const CVIBuffer_LineCircle& rhs);
	virtual ~CVIBuffer_LineCircle() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	static CVIBuffer_LineCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_float	m_fRadius;
	UINT	m_fSliceCount;
};

END