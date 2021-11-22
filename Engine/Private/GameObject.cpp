#include "..\public\GameObject.h"
#include "Engine.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
{
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pEngine(CEngine::GetInstance())
	, m_UUID(CUUID())
{
}

HRESULT CGameObject::InitializePrototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

_uint CGameObject::Update(_double TimeDelta)
{
	return _uint();
}

_uint CGameObject::LateUpdate(_double dDeltaTime)
{
	return _uint();
}

HRESULT CGameObject::Render()
{

	return S_OK;
}

HRESULT CGameObject::SetUpComponents(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** pOut, void * pArg)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	CComponent*		pComponent = pEngine->CloneComponent(iSceneIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	if (m_Components.end() != find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag)))
		return E_FAIL;
	else
	{
		m_Components.emplace(pComponentTag, pComponent);
		*pOut = pComponent;
		//SafeAddRef(pComponent);
	}

	RELEASE_INSTANCE(CEngine);

	return S_OK;
}

HRESULT CGameObject::AddComponent(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar* pComponentTag, void* pArg)
{
	if (GetComponent(pComponentTag))
	{
		MSG_BOX("Component already exist");
		return E_FAIL;
	}

	CComponent*		pComponent = m_pEngine->CloneComponent(iSceneIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr == GetComponent(pComponentTag))
		m_Components.emplace(pComponentTag, pComponent);
	else
		return E_FAIL;

	return S_OK;
}

CComponent* CGameObject::GetComponent(const _tchar * pComponentTag)
{
	auto& iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (m_Components.end() == iter)
		return nullptr;
	else
		return (*iter).second;
}



void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		SafeRelease(Pair.second);

	m_Components.clear();
}
