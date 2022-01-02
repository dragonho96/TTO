#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectInstance final : public CVIBuffer
{
private:
	explicit CVIBuffer_RectInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_RectInstance(const CVIBuffer_RectInstance& rhs);
	virtual ~CVIBuffer_RectInstance() = default;
public:
	virtual HRESULT InitializePrototype(string pShaderFilePath, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Update(_double TimeDelta);
	HRESULT Render(_uint iPassIndex);

public:
	vector<VTXMATRIX*>&		GetInstanceMatrices() {
		return m_InstanceMatrices;
	};

private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstanceDesc;
	D3D11_SUBRESOURCE_DATA	m_VBInstanceSubResourceData;

	_uint					m_iNumInstance = 0;
	vector<VTXMATRIX*>		m_InstanceMatrices;
	string					m_shaderPath = "";
public:
	static CVIBuffer_RectInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string pShaderFilePath, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END