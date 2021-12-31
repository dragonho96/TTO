#pragma once

#include "Base.h"
#include "Engine.h"
#include "Grid.h"
BEGIN(Client)

class CEnemy;
class CPathFinding : public CBase
{
public:
	typedef struct _tagStartEndPos
	{
		_tagStartEndPos(CEnemy* _obj, _float3 _start, _float3 _end)
			: obj(_obj), start(_start), end(_end){}

		CEnemy* obj;
		_float3 start;
		_float3 end;
	}PATHPOS;

	DECLARE_SINGLETON(CPathFinding)
private:
	CPathFinding();
	virtual ~CPathFinding() = default;
public:
	void Initialize();
	void Update();
public:
	CRITICAL_SECTION& Get_CS() { return m_CS; }

public:
	HRESULT		FindPath_Thread(CEnemy* obj, _float3 startPos, _float3 targetPos);
	list<_vector> FindPath(_float3 startPos, _float3 targetPos);
	list<_vector> RetracePath(CNode* startNode, CNode* endNode);
	_int GetMoveCost(CNode* nodeA, CNode* nodeB);
	_int GetHCost(CNode* nodeA, CNode* nodeB);
	_bool AddOpenSet(_int checkX, _int checkZ, CNode* pCurNode, CNode* pTargetNode);

public:
	virtual void Free() override;
private:
	CGrid*			m_pGrid = nullptr;
	CTransform*		m_pPlayerTransform = nullptr;
	_double			m_dTimeElapsed = 0;
	list<CNode*> openSet;
	list<CNode*> closedSet;
	CRITICAL_SECTION	m_CS;
	HANDLE				m_hThread = 0;
};

END