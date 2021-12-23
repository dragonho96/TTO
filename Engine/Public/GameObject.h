#pragma once

#include "Base.h"
#include "UUID.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint	Update(_double dDeltaTime);
	virtual _uint	LateUpdate(_double dDeltaTime);
	virtual HRESULT Render();

public:
	void SetDead();
	_bool isDead() { return m_bDead; }
public:
	/* Getter Setter Name for Tool Inspector */
	string GetName() { return m_Name; }
	void SetName(string name) { m_Name = name; }
public:
	string GetLayer() { return m_Layer; }
	void SetLayer(string layer) { m_Layer = layer; }
public:
	uint64_t GetUUID() { return m_UUID(); }
public:
	void SetInfo(string name, string layer, uint64_t uuid, _bool active);
public:
	virtual void LinkTranformWithParent();
public:
	void SetParent(CGameObject* pParent);
	void AddChild(CGameObject* pChild);
	void RemoveChild(CGameObject* pChild);
	void ClearChildren();
	CGameObject* GetParent() { return m_pParent; }
	list<CGameObject*> GetChildren() { return m_listChildren; }
	CGameObject* FindChildWithName(string name);

public:
	_bool&	IsActive() { return m_bIsActive; }
	void	SetActive(_bool value);

protected:
	class CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

protected:
	string					m_Name = "Empty Object";
	string					m_Layer = "Default";
	CUUID					m_UUID;
	_bool					m_bDead = false;
	_bool					m_bIsActive = true;

protected:
	unordered_map<string, class CComponent*>			m_Components;
	typedef unordered_map<string, class CComponent*>	COMPONENTS;

	list<CGameObject*>	m_listChildren;
	CGameObject*		m_pParent = nullptr;

public:
	HRESULT SetUpComponents(_uint iSceneIndex, string pPrototypeTag, string pComponentTag, CComponent** pOut = nullptr, void* pArg = nullptr);
	HRESULT AddComponent(_uint iSceneIndex, string pPrototypeTag, string pComponentTag, void* pArg = nullptr);
	HRESULT AddModelComponent(_uint iSceneIndex, CComponent* pModel);
	CComponent* GetComponent(string pComponentTag);
	HRESULT RemoveComponent(string pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END