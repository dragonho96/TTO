#pragma once

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTimerManager final : public CBase
{
	DECLARE_SINGLETON(CTimerManager)
private:
	explicit CTimerManager();
	virtual ~CTimerManager() = default;
public:
	HRESULT AddTimers(string pTimerTag);
	_double ComputeDeltaTime(string pTimerTag);
private:
	unordered_map<string, CTimer*>			m_Timers;
	typedef unordered_map<string, CTimer*>	TIMERS;
private:
	CTimer* FindTimers(string pTimerTag);
public:
	virtual void Free();
};

END