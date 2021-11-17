#pragma once

#include "Component.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

protected: /* For.Vertex Buffer */
	ID3D11Buffer*				m_pVB = nullptr;
	D3D11_BUFFER_DESC			m_VBDesc;	
	D3D11_SUBRESOURCE_DATA		m_VBSubResourceData;
	_uint						m_iStride; /* 정점하나의 바이트 크기. */
	_uint						m_iNumVertices = 0;
	_uint						m_iNumVertexBuffers = 0;


protected: /* For.Index Buffer */
	ID3D11Buffer*				m_pIB = nullptr;
	D3D11_BUFFER_DESC			m_IBDesc;
	D3D11_SUBRESOURCE_DATA		m_IBSubResourceData;
	_uint						m_iNumPrimitive = 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitive;
	Scope<CShader>	m_pShader;

protected:
	void*				m_pVertices = nullptr;

protected:
	HRESULT Create_Buffers();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END