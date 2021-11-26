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

void CGameObject::SetInfo(string name, string layer, uint64_t uuid)
{
	m_Name = name;
	m_Layer = layer;
	m_UUID = uuid;
}

void CGameObject::AddChild(CGameObject* pChild)
{
	if (pChild->GetParent())
		pChild->GetParent()->RemoveChild(pChild);
	m_listChildren.push_back(pChild);
	pChild->SetParent(this);
}

void CGameObject::RemoveChild(CGameObject * pChild)
{
	if (m_listChildren.size() <= 0)
		return;

	uint64_t uuid = pChild->GetUUID();

	for (auto& iter = m_listChildren.begin(); iter != m_listChildren.end();)
	{
		if (uuid == (*iter)->GetUUID())
		{
			(*iter)->SetParent(nullptr);
			iter = m_listChildren.erase(iter);
			return;
		}
		else
			++iter;
	}
}

void CGameObject::ClearChildren()
{
	for (auto& child : m_listChildren)
		child->SetParent(nullptr);

	m_listChildren.clear();
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
		if (pOut)
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

HRESULT CGameObject::RemoveComponent(const _tchar * pComponentTag)
{
	auto& iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (m_Components.end() == iter)
		return E_FAIL;

	SafeRelease((*iter).second);
	m_Components.erase(iter);
	return S_OK;
}



void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		SafeRelease(Pair.second);

	m_Components.clear();

	m_listChildren.clear();
	m_pParent = nullptr;
}
