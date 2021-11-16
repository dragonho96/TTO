#pragma once

#include "Base.h"

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

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
protected:
	unordered_map<const _tchar*, class CComponent*>			m_Components;
	typedef unordered_map<const _tchar*, class CComponent*>	COMPONENTS;

public:
	HRESULT SetUpComponents(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** pOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END