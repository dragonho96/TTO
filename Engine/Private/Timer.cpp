
#include "..\public\Timer.h"

CTimer::CTimer()
{
}

HRESULT CTimer::ReadyTimer()
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);

	// 1초에 cpu가 얼마나 카운팅할 수 있는지. 
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

_double CTimer::ComputeDeltaTime()
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		m_FixTime = m_FrameTime;
		QueryPerformanceFrequency(&m_CpuTick);
	}


	m_DeltaTime = (_double(m_FrameTime.QuadPart) - m_LastTime.QuadPart) / m_CpuTick.QuadPart;

	m_LastTime = m_FrameTime;

	return _double(m_DeltaTime);
}

CTimer * CTimer::Create()
{
	CTimer*	pInstance = new CTimer();

	if (FAILED(pInstance->ReadyTimer()))
	{
		MSG_BOX("Failed To Creating CTimer");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CTimer::Free()
{
}
