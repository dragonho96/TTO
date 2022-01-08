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

class CEffect_Muzzle final : public CGameObject
{
public:
	typedef struct _tagInstanceForce
	{
		_vector vForceDir;
		_float	fForcePower;
	}INSTNACEFORCE;
public:
	explicit CEffect_Muzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Muzzle(const CEffect_Muzzle& rhs);
	virtual ~CEffect_Muzzle() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play(_float fCamAngle);
	void Reset(_float fCamAngle);

private:
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pTextureGreyCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTransform*					m_pTargetTransform = nullptr;
	CVIBuffer_RectInstance*		m_pVIBufferCom = nullptr;
	vector<INSTNACEFORCE>		m_InstanceForce;
	CShader*					m_pShader = nullptr;

private:
	_bool						m_bPlaying = false;
private:
	_float						m_fFrame = 0.f;

private:
	HRESULT SetUp_Components();

public:
	static CEffect_Muzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END