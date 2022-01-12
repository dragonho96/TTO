#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CComponent
{
private:
	explicit CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLight(const CLight& rhs);
	virtual ~CLight() = default;
public:
	const LIGHTDESC* GetLightDesc() const {
		return &m_LightDesc;
	}

	LIGHTDESC& GetDesc() { return m_LightDesc; }

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc, class CTransform* pTransform);
	HRESULT Render_Light();

public:
	_matrix GetViewMatrix();
	_matrix GetProjMatrix();
	_vector GetPosition();
	ID3D11ShaderResourceView* GetShaderResourceView();

	string GetTargetName() { return m_targetName; };

private:
	//ID3D11Device*					m_pDevice = nullptr;
	//ID3D11DeviceContext*			m_pDeviceContext = nullptr;
	LIGHTDESC						m_LightDesc;
	class CVIBuffer_Rect_Viewport*	m_pVIBuffer = nullptr;
	class CRenderTarget*			m_pRenderTarget = nullptr;
	class CTransform*				m_pTransform = nullptr;
	string							m_targetName = "";
public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc, CTransform* pTransform);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END