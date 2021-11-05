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
	HRESULT AddTimers(const _tchar* pTimerTag);
	_double ComputeDeltaTime(const _tchar* pTimerTag);
private:
	unordered_map<const _tchar*, CTimer*>			m_Timers;
	typedef unordered_map<const _tchar*, CTimer*>	TIMERS;
private:
	CTimer* FindTimers(const _tchar* pTimerTag);
public:
	virtual void Free();
};

END