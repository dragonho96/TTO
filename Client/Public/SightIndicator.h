#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Line;
END


BEGIN(Client)
class CSightIndicator final : public CGameObject
{
private:
	explicit CSightIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSightIndicator(const CSightIndicator& rhs);
	virtual ~CSightIndicator() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render(_uint iPassIndex) override;

public:
	void SetPoints(_bool active, list<_vector> points = {});
private:
	CGameObject*	m_pNoSightCross = nullptr;
	CGameObject*	m_pNoSightEye = nullptr;
private:
	CTexture*			m_pTextureCom = nullptr;

	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Line*				m_pVIBufferCom = nullptr;
	CShader*					m_pShader = nullptr;
private:
	HRESULT SetUp_Components();

public:
	static CSightIndicator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END