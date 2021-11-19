#pragma once

#include "VIBuffer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_LineSphere final : public CVIBuffer
{
private:
	explicit CVIBuffer_LineSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_LineSphere(const CVIBuffer_LineSphere& rhs);
	virtual ~CVIBuffer_LineSphere() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	static CVIBuffer_LineSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	HRESULT CreateBuffer(void** pVertices);
	void SetSize(float fRadius);

private:
	_float				m_fRadius;
	UINT				m_fSliceCount;
	void*				m_pCloneVertices = nullptr;
};

END