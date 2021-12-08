#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(const char * pAnimationName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_AnimationName, pAnimationName);
	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;

	return S_OK;
}

HRESULT CAnimation::Add_Channel(CChannel * pChannel)
{
	if (nullptr == pChannel)
		return E_FAIL;

	m_Channels.push_back(pChannel);

	return S_OK;
}

CAnimation * CAnimation::Create(const char * pAnimationName, _double Duration, _double TickPerSecond)
{

	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pAnimationName, Duration, TickPerSecond)))
	{
		MSG_BOX("Failed to Creating CHierarchyNode");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& channel : m_Channels)
		SafeRelease(channel);
	m_Channels.clear();
}