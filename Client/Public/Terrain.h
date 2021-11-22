#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer.h"


BEGIN(Client)
class CTerrain final : public CGameObject
{
public:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;
public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUpComponents();

private:
	CRenderer*		m_pRendererCom = nullptr;
	CVIBuffer*		m_pVIBuffer = nullptr;
	CTransform*		m_pTransform = nullptr;

};

END