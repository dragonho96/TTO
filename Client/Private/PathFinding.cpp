#include "stdafx.h"
#include "..\Public\PathFinding.h"

USING(Client)
IMPLEMENT_SINGLETON(CPathFinding)

CPathFinding::CPathFinding()
	: m_pGrid(nullptr), m_pPlayerTransform(nullptr)
{
}

void CPathFinding::Initialize()
{
	list<class CGameObject*> list = CEngine::GetInstance()->GetGameObjectInLayer(0, "LAYER_GRID");
	if (list.size() <= 0)
		return;

	m_pGrid = dynamic_cast<CGrid*>(list.front());

	list = CEngine::GetInstance()->GetGameObjectInLayer(0, "Player");
	if (list.size() <= 0)
		return ;

	m_pPlayerTransform = dynamic_cast<CTransform*>(list.front()->GetComponent("Com_Transform"));
}

void CPathFinding::Update()
{
	if (!m_pGrid)
		return;
	if (CEngine::GetInstance()->IsKeyDown('F'))
	{
		CEngine::GetInstance()->AddTimers("Timer_PathFinding");
		CEngine::GetInstance()->ComputeDeltaTime("Timer_PathFinding");

		_float3 startPos, targetPos;
		XMStoreFloat3(&startPos, m_pPlayerTransform->GetState(CTransform::STATE_POSITION));
		targetPos = { 2, 0, 2 };

		FindPath(startPos, targetPos);
	}
}

void CPathFinding::FindPath(_float3 startPos, _float3 targetPos)
{
	CNode* startNode = m_pGrid->NodeFromWorldPoint(startPos);
	CNode* targetNode = m_pGrid->NodeFromWorldPoint(targetPos);

	if (nullptr == startNode || nullptr == targetNode)
		return;

	openSet.clear();
	closedSet.clear();

	openSet.emplace_back(startNode);
	// make_heap(openSet.begin(), openSet.end());

	while (openSet.size() > 0)
	{
		//pop_heap(openSet.begin(), openSet.end());
		//CNode* currentNode = openSet.back();
		//openSet.pop_back();

		CNode* currentNode = openSet.front();
		//openSet.pop_front();
		auto iter = openSet.begin(); ++iter;
		for (iter; iter != openSet.end(); ++iter)
		{
			int openFCost = (*iter)->GetFCost();
			int openHCost = (*iter)->GetHCost();
			int currentFCost = currentNode->GetFCost();
			int currentHCost = currentNode->GetHCost();
			if (openFCost <= currentFCost && openHCost < currentHCost)
			{
				currentNode = (*iter);
			}
		}

		openSet.remove(currentNode);
		closedSet.push_back(currentNode);

		if (currentNode == targetNode)
		{
			RetracePath(startNode, targetNode);
			_double timeElapsed = CEngine::GetInstance()->ComputeDeltaTime("Timer_PathFinding");
			
			ADDLOG(to_string(timeElapsed).c_str());
			return;
		}
		
		_int curNodeX = currentNode->GetGridX();
		_int curNodeZ = currentNode->GetGridZ();

		//for (_int x = -1; x <= 1; x++)
		//{
		//	for (_int z = -1; z <= 1; z++)
		//	{
		//		if (x == 0 && z == 0)
		//			continue;

		//		_int checkX = curNodeX + x;
		//		_int checkZ = curNodeZ + z;

		//		AddOpenSet(checkX, checkZ, currentNode, targetNode);
		//	}
		//}


		list<CNode*> neighbours = m_pGrid->GetNeighbours(currentNode);
		for (auto& neighbour : neighbours)
		{
			auto findList = std::find(closedSet.begin(), closedSet.end(), neighbour);
			if (findList != closedSet.end()/* || !neighbour->IsWalkable()*/)
				continue;

			_int newMovementCostToNeighbour = currentNode->GetGCost() + GetMoveCost(currentNode, neighbour);
			auto findVector = std::find(openSet.begin(), openSet.end(), neighbour);
			if (findVector == openSet.end() || newMovementCostToNeighbour < neighbour->GetGCost())
			{
				neighbour->SetGCost(newMovementCostToNeighbour);
				neighbour->SetHCost(GetHCost(neighbour, targetNode));
				neighbour->SetParent(currentNode);

				openSet.push_back(neighbour);
				// push_heap(openSet.begin(), openSet.end());
			}
		}
	}
}
_bool CPathFinding::AddOpenSet(_int checkX, _int checkZ, CNode* pCurNode, CNode* pTargetNode)
{
	CNode* node = m_pGrid->GetNode(checkX, checkZ);
	if (nullptr == node)
		return false;

	_int curNodeX = pCurNode->GetGridX();
	_int curNodeZ = pCurNode->GetGridZ();

	auto find = std::find(closedSet.begin(), closedSet.end(), node);


	if (m_pGrid->ValidateNode(checkX, checkZ) && find == closedSet.end())
	{
		// �� ���̷� ��� �ȵ�
		if ((!m_pGrid->ValidateNode(curNodeX, checkZ)) && (!m_pGrid->ValidateNode(checkX, curNodeZ)))
			return false;

		// �������� ��ֹ� �̵� �Ұ�
		if ((!m_pGrid->ValidateNode(curNodeX, checkZ)) || (!m_pGrid->ValidateNode(checkX, curNodeZ)))
			return false;

		_int newMovementCostToNeighbour = pCurNode->GetGCost() + GetMoveCost(pCurNode, node);
		auto findVector = std::find(openSet.begin(), openSet.end(), node);
		if (findVector == openSet.end() || newMovementCostToNeighbour < node->GetGCost())
		{
			node->SetGCost(newMovementCostToNeighbour);
			node->SetHCost(GetHCost(node, pTargetNode));
			node->SetParent(pCurNode);

			openSet.push_back(node);
			// push_heap(openSet.begin(), openSet.end());
		}
	}
	return _bool();
}

void CPathFinding::RetracePath(CNode * startNode, CNode * endNode)
{
	list<CNode*> path;
	CNode* currentNode = endNode;
	while (currentNode != startNode)
	{
		path.emplace_back(currentNode);
		currentNode = currentNode->GetParent();
	}

	m_pGrid->ResetColor();
	
	for (auto& node : path)
		node->SetColor(_float4{ 0.f, 1.f, 0.f, 1.f });

}

_int CPathFinding::GetMoveCost(CNode * nodeA, CNode * nodeB)
{
	_int dstX = abs(nodeA->GetGridX() - nodeB->GetGridX());
	_int dstZ = abs(nodeA->GetGridZ() - nodeB->GetGridZ());

	return (dstX == 0 || dstZ == 0) ? 10 : 14;
	//if (dstX > dstZ)
	//	return 14 * dstZ + 10 * (dstX - dstZ);

	//return 14 * dstX + 10 * (dstZ - dstX);
}

_int CPathFinding::GetHCost(CNode * nodeA, CNode * nodeB)
{
	_int dstX = abs(nodeA->GetGridX() - nodeB->GetGridX());
	_int dstZ = abs(nodeA->GetGridZ() - nodeB->GetGridZ());
	
	return (dstX + dstZ) * 10;
}



void CPathFinding::Free()
{
}
