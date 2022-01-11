#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_PointInstance;
END


BEGIN(Client)

class CEffect_Impact final : public CGameObject
{
public:
	typedef struct _tagInstanceForce
	{
		_vector vForceDir;
		_float	fForcePower;
	}INSTNACEFORCE;
public:
	explicit CEffect_Impact(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Impact(const CEffect_Impact& rhs);
	virtual ~CEffect_Impact() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render(_uint iPassIndex) override;

public:
	void Play(_vector vPos);
	void Reset(_vector vPos);

private:
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pTextureGreyCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CVIBuffer_PointInstance*		m_pVIBufferCom = nullptr;
	vector<INSTNACEFORCE>		m_InstanceForce;
	_vector						m_vNormalDir;
	
	CShader*					m_pShader = nullptr;
private:
	_float						m_fFrame = 0.f;
	_bool						m_bPlaying = false;
private:
	HRESULT SetUp_Components();

public:
	static CEffect_Impact* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END