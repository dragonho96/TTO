#pragma once

#include "GameObject.h"
#include "Renderer.h"
#include "RectTransform.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CEmptyUI final : public CGameObject
{
private:
	explicit CEmptyUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEmptyUI(const CEmptyUI& rhs);
	virtual ~CEmptyUI() = default;
public:
	static CEmptyUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Update(_double TimeDelta) override;
	virtual _uint LateUpdate(_double TimeDelta) override;
	virtual HRESULT Render(_uint iPassIndex = 0) override;

public:
	void SetRectTransform(CRectTransform::RECTTRANSFORMDESC _desc);
	void SetPosition(_float x, _float y);
	void SetClientPosition(_float x, _float y);
	virtual void LinkTranformWithParent();

public:
	void InteractMouse();
	bool IsHovered() { return m_bHover; }
	bool IsSelected() { return m_bSelect; }

public:
	_int& GetSortingOrder() { return m_iSortingOrder; }
	void SetSortingOrder(_int order) { m_iSortingOrder = order; }

	_int& GetRenderIndex() { return m_iRenderIndex; }
	void SetRenderIndex(_int idx) { m_iRenderIndex = idx; }

protected:
	_bool m_bHover = false;
	_bool m_bSelect = false;
	_int	m_iSortingOrder = 0;
	_float2	m_vTransformOffSet = { 0.f, 0.f };
private:
	HRESULT SetUpComponents();

private:
	CRectTransform*		m_pRectTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	_int				m_iRenderIndex = 0;
};

END