#include "..\Private\Grid.h"


void CGrid::CreateGrid()
{
	//m_Nodes = new CNode[m_iSizeX * m_iSizeZ];
	//for (int i = 0; i < m_iSizeX; ++i)
	//{
	//	for (int j = 0; j < m_iSizeZ; ++j)
	//	{
	//		int index = i * m_iSizeZ + j;
	//		_float3 position = { j * m_iSizeInterval, 0.f, i * m_iSizeInterval };
	//		m_Nodes[index].SetPosition(position);
	//		m_Nodes[index].SetWalkable(true);
	//	}
	//}
}

void CGrid::Free()
{
	//SafeDeleteArray(m_Nodes);
}
