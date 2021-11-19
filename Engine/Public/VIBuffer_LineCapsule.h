#pragma once

#include "VIBuffer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_LineCapsule final : public CVIBuffer
{
private:
	explicit CVIBuffer_LineCapsule(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_LineCapsule(const CVIBuffer_LineCapsule& rhs);
	virtual ~CVIBuffer_LineCapsule() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	static CVIBuffer_LineCapsule* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	HRESULT CreateBuffer(void** pVertices);
	void SetSize(float fRadius, float fHeight);

private:
	_float				m_fRadius;
	_float				m_fHeight;
	UINT				m_fSliceCount;
	void*				m_pCloneVertices = nullptr;
};

END