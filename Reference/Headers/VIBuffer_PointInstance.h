#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstance final : public CVIBuffer
{
private:
	explicit CVIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_PointInstance(const CVIBuffer_PointInstance& rhs);
	virtual ~CVIBuffer_PointInstance() = default;
public:
	virtual HRESULT InitializePrototype(string pShaderFilePath, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Update(_double TimeDelta);
	HRESULT Render(_uint iPassIndex);

private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstanceDesc;
	D3D11_SUBRESOURCE_DATA	m_VBInstanceSubResourceData;

	_uint			m_iNumInstance = 0;
	vector<VTXMATRIX>		m_InstanceMatrices;
	string					m_shaderPath = "";
public:
	static CVIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string pShaderFilePath, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

