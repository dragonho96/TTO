#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line final : public CVIBuffer
{
private:
	explicit CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;
public:
	virtual HRESULT InitializePrototype(string pShaderFilePath, _uint iNumPoint);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Update(_double TimeDelta);
	HRESULT Render(_uint iPassIndex);

public:
	void SetPoints(list<_vector> points);

private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstanceDesc;
	D3D11_SUBRESOURCE_DATA	m_VBInstanceSubResourceData;

	_uint					m_iNumPoint = 0;
	_vector					m_pointPosition[128];
	string					m_shaderPath = "";
public:
	static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, string pShaderFilePath, _uint iNumPoint = 2);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END