#include "..\public\TimerManager.h"

IMPLEMENT_SINGLETON(CTimerManager)

CTimerManager::CTimerManager()
{

}

HRESULT CTimerManager::AddTimers(const string pTimerTag)
{
	if (nullptr != FindTimers(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.insert(TIMERS::value_type(pTimerTag, pTimer));

	return S_OK;
}

_double CTimerManager::ComputeDeltaTime(string pTimerTag)
{
	CTimer*	pTimer = FindTimers(pTimerTag);	
	if (nullptr == pTimer)
		return E_FAIL;

	return pTimer->ComputeDeltaTime();
}

CTimer * CTimerManager::FindTimers(string pTimerTag)
{
	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), STagFinder(pTimerTag));
	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimerManager::Free()
{
	for (auto& Pair : m_Timers)
		SafeRelease(Pair.second);

	m_Timers.clear();
}
