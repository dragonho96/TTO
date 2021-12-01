#include "stdafx.h"
#include "..\Public\Grid.h"

USING(Client)

CGrid::CGrid(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGrid::CGrid(const CGrid & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrid::InitializePrototype()
{
	if (FAILED(__super::InitializePrototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGrid::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Desc = *(GRIDDESC*)(pArg);

	SetUpGrid();

	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (list.size() <= 0)
		return E_FAIL;
	m_pPlayerTransform = dynamic_cast<CTransform*>(list.front()->GetComponent("Com_Transform"));

	return S_OK;
}

_uint CGrid::Update(_double dDeltaTime)
{
	//if (!m_pPlayerTransform)
	//	return _uint();
	//_float  fScaleFactor = m_Desc.iSizeInterval / 2.f;
	//_float3 fNodePos, fPlayerPos;
	//XMStoreFloat3(&fPlayerPos, m_pPlayerTransform->GetState(CTransform::STATE_POSITION));
	//for (auto node : m_Nodes)
	//{
	//	fNodePos = node->GetPosition();
	//	if (fPlayerPos.x > (fNodePos.x - fScaleFactor) && fPlayerPos.x < (fNodePos.x + fScaleFactor) &&
	//		fPlayerPos.z >(fNodePos.z - fScaleFactor) && fPlayerPos.z < (fNodePos.z + fScaleFactor))
	//		node->SetColor(_float4{ 0.f, 1.f, 0.f, 1.f });
	//	else
	//		node->SetColor(_float4{ 1.f, 1.f, 0.f, 1.f });
	//}
	return _uint();
}

CNode * CGrid::NodeFromWorldPoint(_float3 worldPosition)
{
	_float3 fNodePos;
	_float  fScaleFactor = m_Desc.iSizeInterval / 2.f;

	for (auto node : m_Nodes)
	{
		fNodePos = node->GetPosition();
		if (worldPosition.x > (fNodePos.x - fScaleFactor) && worldPosition.x < (fNodePos.x + fScaleFactor) &&
			worldPosition.z >(fNodePos.z - fScaleFactor) && worldPosition.z < (fNodePos.z + fScaleFactor))
			return node;

		node->SetColor(_float4{ 1.f, 1.f, 0.f, 1.f });
	}
	return nullptr;
}

void CGrid::SetUpGrid()
{
	m_Nodes.resize(m_Desc.iSizeX * m_Desc.iSizeZ);

	for (int i = 0; i < m_Desc.iSizeX; ++i)
	{
		for (int j = 0; j < m_Desc.iSizeZ; ++j)
		{
			int index = i * m_Desc.iSizeZ + j;
			_float3 position = { j * m_Desc.iSizeInterval, 1.f, i * m_Desc.iSizeInterval };
			m_Nodes[index] = dynamic_cast<CNode*>(m_pEngine->AddGameObject(0, "GameObject_Node", "LAYER_NODE"));
			m_Nodes[index]->SetPosition(position, i, j);
			m_Nodes[index]->SetWalkable(true);
		}
	}
}

list<CNode*> CGrid::GetNeighbours(CNode * node)
{
	list<CNode*> neighbours;

	for (_int x = -1; x <= 1; ++x)
	{
		for (_int z = -1; z <= 1; ++z)
		{
			if (x == 0 && z == 0)
				continue;

			_int checkX = node->GetGridX() + x;
			_int checkZ = node->GetGridZ() + z;

			if (checkX >= 0 && checkX < m_Desc.iSizeX &&
				checkZ >= 0 && checkZ < m_Desc.iSizeZ)
				neighbours.emplace_back(m_Nodes[checkX * m_Desc.iSizeZ + checkZ]);
		}
	}
	return neighbours;
}





CGrid * CGrid::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGrid*		pInstance = new CGrid(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create CGrid");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject* CGrid::Clone(void* pArg)
{
	CGrid*		pInstance = new CGrid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create CGrid");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CGrid::Free()
{
}