#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer;
END

BEGIN(Client)

class CBackground final : public CGameObject
{
public:
	explicit CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBackground(const CBackground& rhs);
	virtual ~CBackground() = default;
public:
	static CBackground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

};

END