#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRectTransform final : public CComponent
{
public:
	typedef struct tagRectTransformDesc
	{
		_float	posX;
		_float	posY;
		_float	sizeX;
		_float	sizeY;
	}RECTTRANSFORMDESC;

private:
	explicit CRectTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRectTransform(const CRectTransform& rhs);
	virtual ~CRectTransform() = default;
public:
	static CRectTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);

public:
	bool IsMouseInRect();

public:
	_float4x4				GetTransformMat() { return m_TransformMatrix; }
	_float4x4				GetProjMat() { return m_ProjMatrix; }
	RECTTRANSFORMDESC		GetTransformDesc() { return m_TransformDesc; }
public:
	void					SetTransformMat(RECTTRANSFORMDESC _desc);
	void					SetTransformMat(_float4x4 _mat);
	void					SetPosition(_float x, _float y);
	void					SetClientPosition(_float x, _float y);

private:
	class CEngine*				m_pEngine = nullptr;
	_float4x4				m_ProjMatrix;
	_float4x4				m_TransformMatrix;
	RECTTRANSFORMDESC		m_TransformDesc;
};

END