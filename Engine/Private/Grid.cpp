#include "..\Public\Grid.h"

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

	return S_OK;
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
			m_Nodes[index]->SetPosition(position);
			m_Nodes[index]->SetWalkable(true);
		}
	}
}

void CGrid::Free()
{
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