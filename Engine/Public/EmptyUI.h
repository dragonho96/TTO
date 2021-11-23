#pragma once

#include "GameObject.h"
#include "Renderer.h"
#include "RectTransform.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CEmptyUI final : public CGameObject
{
public:
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
	virtual HRESULT Render() override;

public:
	void SetRectTransform(CRectTransform::RECTTRANSFORMDESC _desc);

public:
	void InteractMouse();
	bool IsHovered() { return m_bHover; }
	bool IsSelected() { return m_bSelect; }

protected:
	_bool m_bHover = false;
	_bool m_bSelect = false;

private:
	HRESULT SetUpComponents();

private:
	CRectTransform*		m_pRectTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

};

END