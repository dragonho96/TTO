#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_RectInstance;
END


BEGIN(Client)

class CEffect_Fire final : public CGameObject
{
public:
	explicit CEffect_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Fire(const CEffect_Fire& rhs);
	virtual ~CEffect_Fire() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render(_uint iPassIndex) override;

public:
	void SetPosition(_vector vPos);

private:
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pTextureGreyCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CVIBuffer_RectInstance*		m_pVIBufferCom = nullptr;

	CShader*					m_pShader = nullptr;
private:
	_float						m_fFrame = 0.f;

private:
	HRESULT SetUp_Components();

public:
	static CEffect_Fire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END