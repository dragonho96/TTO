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

void CGameObject::SetDead()
{
	m_bDead = true;
	list<CGameObject*> childrenToDelete = m_listChildren;
	
	if (m_pParent)
		m_pParent->RemoveChild(this);


	if (0 < m_listChildren.size())
	{
		for (auto& iter = m_listChildren.begin(); iter != m_listChildren.end();)
		{
			(*iter)->SetDead();
			// Child에서 deparent하기때문에 iter를 다시 begin으로 잡아준다
			iter = m_listChildren.begin();
		}
	}
	m_listChildren.clear();


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

void CGameObject::SetInfo(string name, string layer, uint64_t uuid, _bool active)
{
	// if UUID is not 0, it means it has uuid to inherit
	if (uuid)
		m_UUID = uuid;

	m_Name = name;
	m_Layer = layer;
	m_bIsActive = active;
	m_pEngine->AddGameObjectWithName(name, this);
	m_pEngine->AddGameObjectWithUUID(uuid, this);
}

void CGameObject::LinkTranformWithParent()
{
}

void CGameObject::SetActive(_bool value)
{
	m_bIsActive = value;
	for (auto& child : m_listChildren)
		child->SetActive(value);
}

void CGameObject::SetParent(CGameObject* pParent)
{
	m_pParent = pParent;
	LinkTranformWithParent();
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

CGameObject * CGameObject::FindChildWithName(string name)
{
	auto iter = find_if(m_listChildren.begin(), m_listChildren.end(), [&](CGameObject* child) {
		return child->GetName() == name;
	});
	if (iter != m_listChildren.end())
		return (*iter);

	return nullptr;
}

HRESULT CGameObject::SetUpComponents(_uint iSceneIndex, string pPrototypeTag, string pComponentTag, CComponent ** pOut, void * pArg)
{
	CEngine*	pEngine = GET_INSTANCE(CEngine);

	CComponent*		pComponent = pEngine->CloneComponent(iSceneIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	if (m_Components.end() != find_if(m_Components.begin(), m_Components.end(), STagFinder(pComponentTag)))
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

HRESULT CGameObject::AddComponent(_uint iSceneIndex, string pPrototypeTag, string pComponentTag, void* pArg)
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

HRESULT CGameObject::AddModelComponent(_uint iSceneIndex, CComponent * pModel)
{
	if (nullptr != GetComponent("Com_Model"))
		return E_FAIL;

	m_Components.emplace("Com_Model", pModel);
	return S_OK;
}

CComponent* CGameObject::GetComponent(string pComponentTag)
{
	auto& iter = find_if(m_Components.begin(), m_Components.end(), STagFinder(pComponentTag));
	if (m_Components.end() == iter)
		return nullptr;
	else
		return (*iter).second;
}

HRESULT CGameObject::RemoveComponent(string pComponentTag)
{
	auto& iter = find_if(m_Components.begin(), m_Components.end(), STagFinder(pComponentTag));
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



	m_pParent = nullptr;
}
