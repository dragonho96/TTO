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
	HRESULT Render_Lights();
private:
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;
public:
	virtual void Free() override;
};

END