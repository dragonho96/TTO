#pragma once

#include "GameObject.h"
#include "VIBuffer_Rect.h"

BEGIN(Engine)

class ENGINE_DLL CNode : public CGameObject
{
protected:
	explicit CNode(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit CNode(const CNode& rhs);
	virtual ~CNode() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Update(_double TimeDelta);
	virtual _uint LateUpdate(_double TimeDelta);
	virtual HRESULT Render();
private:
	HRESULT SetUpComponents();

public:
	void SetPosition(_float3 _position)
	{
		m_pTransform->SetState(CTransform::STATE_POSITION, XMLoadFloat3(&_position));
	}
	void SetWalkable(_bool _walkable)
	{
		m_bWalkable = _walkable;
	}

private:
	_bool				m_bWalkable;
	_float3				m_vPosition;
	CVIBuffer_Rect*		m_VIBuffer = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransform = nullptr;

public:
	static CNode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END