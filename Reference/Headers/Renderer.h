#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRenderer(const CRenderer& rhs);
	virtual ~CRenderer() = default;
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	HRESULT AddRenderGroup(RENDER eRenderID, class CGameObject* pRenderObject);
	HRESULT DrawRenderGroup();
private:
	list<class CGameObject*>			m_RenderGroups[RENDER_END];
	typedef list<class CGameObject*>	RENDERGROUPS;
private:
	HRESULT RenderPriority();
	HRESULT RenderNonAlpha();
	HRESULT RenderAlpha();
	HRESULT RenderUI();

};

END