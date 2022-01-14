#pragma once

#include "GameObject.h"
#include "Engine.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CNode : public CGameObject
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
	virtual HRESULT Render(_uint iPassIndex = 0) ;
private:
	HRESULT SetUpComponents();

public:
	void SetPosition(_float3 _position, _int _gridX, _int _gridZ){
		m_pTransform->SetState(CTransform::STATE_POSITION, XMLoadFloat3(&_position));
		m_vPosition = _position;
		m_iGridX = _gridX;
		m_iGridZ = _gridZ;
	}
	void SetWalkable(_bool _walkable){
		m_bWalkable = _walkable;
	}
	void SetColor(_float4 color) {
		m_VIBuffer->SetColor(color);
		// m_VIBuffer->SetColor({0, 0, 0, 0.f});
	}
	const _float3 GetPosition() { return m_vPosition; }
	_bool	IsWalkable() { return m_bWalkable; }

	_int GetGridX() { return m_iGridX; }
	_int GetGridZ() { return m_iGridZ; }

	_int GetFCost() { return m_iGCost + m_iHCost; }
	_int GetGCost() { return m_iGCost; }
	_int GetHCost() { return m_iHCost; }

	void SetGCost(_int gCost) { m_iGCost = gCost; }
	void SetHCost(_int hCost) { m_iHCost = hCost; }

	CNode* GetParent() { return m_pParent; }
	void SetParent(CNode* parent) { m_pParent = parent; }

public:
	_int				CompareTo(CNode* nodeToCompare);

private:
	_bool				m_bWalkable;
	_float3				m_vPosition;
	_int				m_iGridX;
	_int				m_iGridZ;
	_int				m_iGCost;
	_int				m_iHCost;
	CNode*				m_pParent;
	CVIBuffer_Rect*		m_VIBuffer = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransform = nullptr;
public:
	_int	heapIndex;
public:
	static CNode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END