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
	/* Getter Setter Name for Tool Inspector */
	string GetName() { return m_Name; }
	void SetName(string name) { m_Name = name; }
public:
	string GetLayer() { return m_Layer; }
	void SetLayer(string layer) { m_Layer = layer; }
public:
	uint64_t GetUUID() { return m_UUID(); }
public:
	void SetInfo(string name, string layer, uint64_t uuid);
public:
	void SetParent(CGameObject* pParent) { m_pParent = pParent; }
	void AddChild(CGameObject* pChild);
	void RemoveChild(CGameObject* pChild);
	void ClearChildren();
	CGameObject* GetParent() { return m_pParent; }
	list<CGameObject*> GetChildren() { return m_listChildren; }

protected:
	class CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

protected:
	string					m_Name = "Empty Object";
	string					m_Layer = "Default";
	CUUID					m_UUID;
	bool					m_bDead = false;

protected:
	unordered_map<const _tchar*, class CComponent*>			m_Components;
	typedef unordered_map<const _tchar*, class CComponent*>	COMPONENTS;

	list<CGameObject*>	m_listChildren;
	CGameObject*		m_pParent = nullptr;

public:
	HRESULT SetUpComponents(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** pOut = nullptr, void* pArg = nullptr);
	HRESULT AddComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar* pComponentTag, void* pArg = nullptr);
	CComponent* GetComponent(const _tchar * pComponentTag);
	HRESULT RemoveComponent(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
	void SetDead() { m_bDead = true; }
	_bool isDead() { return m_bDead; }
};

END