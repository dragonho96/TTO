#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Line;
END


BEGIN(Client)
class CEffect_Trajectory final : public CGameObject
{
private:
	explicit CEffect_Trajectory(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Trajectory(const CEffect_Trajectory& rhs);
	virtual ~CEffect_Trajectory() = default;
public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetPoints(list<_vector> points);
private:
	CTexture*			m_pTextureCom = nullptr;

	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CVIBuffer_Line*		m_pVIBufferCom = nullptr;

	CShader*					m_pShader = nullptr;

private:
	HRESULT SetUp_Components();

public:
	static CEffect_Trajectory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END