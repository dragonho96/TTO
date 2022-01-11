#pragma once

#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CEmptyGameObject final : public CGameObject
{
public:
	explicit CEmptyGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEmptyGameObject(const CEmptyGameObject& rhs);
	virtual ~CEmptyGameObject() = default;
public:
	static CEmptyGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render(_uint iPassIndex = 0) override;

public:
	void SetRenderGroup(CRenderer::RENDER type) { m_eRenderGroup = type; }
private:
	HRESULT SetUpComponents();
	virtual void LinkTranformWithParent();


private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
private:
	CRenderer::RENDER m_eRenderGroup = CRenderer::RENDER_NONALPHA;
};

END