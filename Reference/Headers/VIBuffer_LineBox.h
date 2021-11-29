#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_LineBox final : public CVIBuffer
{
private:
	explicit CVIBuffer_LineBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_LineBox(const CVIBuffer_LineBox& rhs);
	virtual ~CVIBuffer_LineBox() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	static CVIBuffer_LineBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	HRESULT CreateBuffer(void** pVertices);
	void SetSize(_float3 m_Size);

private:
	void*				m_pCloneVertices = nullptr;
	_float3				m_Size;
};

END