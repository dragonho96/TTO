#include "..\Public\Channel.h"



CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const char * pChannelName)
{
	strcpy_s(m_szChannelName, pChannelName);

	return S_OK;
}

HRESULT CChannel::Add_KeyFrameDesc(KEYFRAMEDESC * pKeyFrameDesc)
{
	if (nullptr == pKeyFrameDesc)
		return E_FAIL;

	m_KeyFrames.push_back(pKeyFrameDesc);

	return S_OK;
}

CChannel * CChannel::Create(const char * pChannelName)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannelName)))
	{
		MSG_BOX("Failed to Creating CChannel");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		SafeDelete(pKeyFrame);

	m_KeyFrames.clear();
}
