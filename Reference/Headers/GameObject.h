#pragma once

#include "Base.h"
#include "Engine.h"
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

protected:
	CEngine*				m_pEngine = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

protected:
	string					m_Name = "Empty Object";

protected:
	unordered_map<const _tchar*, class CComponent*>			m_Components;
	typedef unordered_map<const _tchar*, class CComponent*>	COMPONENTS;

public:
	HRESULT SetUpComponents(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** pOut, void* pArg = nullptr);
	HRESULT AddComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar* pComponentTag, void* pArg = nullptr);
	CComponent* GetComponent(const _tchar * pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END