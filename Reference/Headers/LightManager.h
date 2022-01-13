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
	HRESULT RemoveLight(class CLight* pLight);
	HRESULT SortLight();
	HRESULT Render_Lights();
	HRESULT Render_DebugBuffer();

	_matrix						GetViewMatrix(_uint index);
	_matrix						GetProjMatrix(_uint index);
	_vector						GetPosition(_uint index);
	_vector						GetDirection(_uint index);
	_float						GetAngle(_uint index);
	ID3D11ShaderResourceView*	GetShaderResourceView(_uint index);
	string						GetTargetName(_uint index);

	void	SetCurrentIndex(_uint index) { m_iCurrentIndex = index; };
	_uint	GetCurrentIndex() { return m_iCurrentIndex; };
	_uint	GetNumRenderLights() { return (_uint)m_RenderLights.size(); }
	_uint	GetNumLights() { return (_uint)m_Lights.size(); }
private:
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;
private:
	list<class CLight*>				m_RenderLights;
private:
	_uint							m_iCurrentIndex = 0;
public:
	virtual void Free() override;
};

END