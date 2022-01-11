#pragma once

#include "Base.h"


BEGIN(Engine)

class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)
public:
	CLightManager();
	virtual ~CLightManager() = default;
public:
	const LIGHTDESC* GetLightDesc(_uint iIndex = 0) const;
public:
	HRESULT AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	HRESULT AddLight(class CLight* pLight);
	HRESULT Render_Lights();

	_matrix GetViewMatrix(_uint index);
	_matrix GetProjMatrix(_uint index);
	_vector GetPosition(_uint index);

	void SetCurrentIndex(_uint index) { m_iCurrentIndex = index; };
	_uint GetCurrentIndex() { return m_iCurrentIndex; };
	_uint	GetNumLights() { return m_Lights.size(); }
private:
	vector<class CLight*>				m_Lights;
	typedef vector<class CLight*>		LIGHTS;
private:
	_uint							m_iCurrentIndex = 0;
public:
	virtual void Free() override;
};

END