#pragma once

#include "Base.h"
#include "Engine.h"
#include "Grid.h"

BEGIN(Client)

class CPathFinding : public CBase
{
	DECLARE_SINGLETON(CPathFinding)
private:
	CPathFinding();
	virtual ~CPathFinding() = default;
public:
	void Initialize();
	void Update();
public:
	void FindPath(_float3 startPos, _float3 targetPos);
	void RetracePath(CNode* startNode, CNode* endNode);
	_int GetDistance(CNode* nodeA, CNode* nodeB);
	_bool AddOpenSet(_int checkX, _int checkZ, CNode* pCurNode, CNode* pTargetNode);

public:
	virtual void Free() override;
private:
	CGrid*			m_pGrid;
	CTransform*		m_pPlayerTransform;
	_double			m_dTimeElapsed = 0;
	list<CNode*> openSet;
	list<CNode*> closedSet;
};

END