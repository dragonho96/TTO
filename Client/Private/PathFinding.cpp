#include "stdafx.h"
#include "..\Public\PathFinding.h"

USING(Client)
IMPLEMENT_SINGLETON(CPathFinding)

CPathFinding::CPathFinding()
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
	if (CEngine::GetInstance()->IsKeyDown('A'))
	{
		CEngine::GetInstance()->AddTimers("Timer_PathFinding");
		CEngine::GetInstance()->ComputeDeltaTime("Timer_PathFinding");

		_float3 startPos, targetPos;
		XMStoreFloat3(&startPos, m_pPlayerTransform->GetState(CTransform::STATE_POSITION));
		targetPos = { 0, 0, 0 };

		FindPath(startPos, targetPos);
	}
}

void CPathFinding::FindPath(_float3 startPos, _float3 targetPos)
{
	CNode* startNode = m_pGrid->NodeFromWorldPoint(startPos);
	CNode* targetNode = m_pGrid->NodeFromWorldPoint(targetPos);

	if (nullptr == startNode || nullptr == targetNode)
		return;
	list<CNode*> openSet;
	list<CNode*> closedSet;
	openSet.emplace_back(startNode);
	// make_heap(openSet.begin(), openSet.end());

	while (openSet.size() > 0)
	{
		//pop_heap(openSet.begin(), openSet.end());
		//CNode* currentNode = openSet.back();
		//openSet.pop_back();

		CNode* currentNode = openSet.front();
		openSet.pop_front();
		for (auto& openNode : openSet)
		{
			int openFCost = openNode->GetFCost();
			int openHCost = openNode->GetHCost();
			int currentFCost = currentNode->GetFCost();
			int currentHCost = currentNode->GetHCost();
			if (openFCost < currentFCost || openFCost == currentFCost && openHCost < currentHCost)
			{
				currentNode = openNode;
			}
		}

		// openSet.remove(currentNode);
		closedSet.push_back(currentNode);

		if (currentNode == targetNode)
		{
			RetracePath(startNode, targetNode);
			_double timeElapsed = CEngine::GetInstance()->ComputeDeltaTime("Timer_PathFinding");
			
			ADDLOG(to_string(timeElapsed).c_str());
			return;
		}

		list<CNode*> neighbours = m_pGrid->GetNeighbours(currentNode);
		for (auto& neighbour : neighbours)
		{
			auto findList = std::find(closedSet.begin(), closedSet.end(), neighbour);
			if (findList != closedSet.end() || !neighbour->IsWalkable())
				continue;

			_int newMovementCostToNeighbour = currentNode->GetGCost() + GetDistance(currentNode, neighbour);
			auto findVector = std::find(openSet.begin(), openSet.end(), neighbour);
			if (findVector == openSet.end() || newMovementCostToNeighbour < neighbour->GetGCost())
			{
				neighbour->SetGCost(newMovementCostToNeighbour);
				neighbour->SetHCost(GetDistance(neighbour, targetNode));
				neighbour->SetParent(currentNode);

				openSet.push_back(neighbour);
				// push_heap(openSet.begin(), openSet.end());
			}
		}
	}
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
	
	for (auto& node : path)
	{
		node->SetColor(_float4{ 1.f, 0.f, 0.f, 1.f });
	}
}

_int CPathFinding::GetDistance(CNode * nodeA, CNode * nodeB)
{
	_int dstX = abs(nodeA->GetGridX() - nodeB->GetGridX());
	_int dstZ = abs(nodeA->GetGridZ() - nodeB->GetGridZ());

	if (dstX > dstZ)
		return 14 * dstZ + 10 * (dstX - dstZ);

	return 14 * dstX + 10 * (dstZ - dstX);
}

void CPathFinding::Free()
{
}
