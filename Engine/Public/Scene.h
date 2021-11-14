#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CScene abstract : public CBase
{
protected:
	explicit CScene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSceneIndex);
	virtual ~CScene() = default;
public:
	virtual HRESULT Initialize();
	virtual _uint Update(_double TimeDelta);
	virtual HRESULT Render();
public:
	void Clear();
private:
	_uint					m_iSceneIndex = 0;
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
public:
	virtual void Free() override;
};

END