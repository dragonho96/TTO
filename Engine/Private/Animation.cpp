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
HRESULT CAnimation::Update_TransformationMatrices(_double TimeDelta)
{
	m_CurrrentTime += fmod(m_TickPerSecond * TimeDelta, m_Duration);

	if (m_CurrrentTime > m_Duration)
	{
		m_CurrrentTime = fmod(m_TickPerSecond * TimeDelta, m_Duration);
		// m_CurrrentTime = 0.0;
		m_isFinished = true;
	}
	else
		m_isFinished = false;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;

	for (auto& pAnimChannel : m_Channels)
	{
		vector<KEYFRAMEDESC*>	KeyFrames = pAnimChannel->Get_KeyFrames();

		KEYFRAMEDESC*		pFirst = KeyFrames.front();
		KEYFRAMEDESC*		pLast = KeyFrames.back();

		_vector			vScale, vRotation, vPosition;

		vScale = XMVectorZero();
		vRotation = XMVectorZero();
		vPosition = XMVectorZero();

		_uint		iCurrentKeyFrame = pAnimChannel->Get_CurrentKeyFrame();
		_float		fRatio = 0.f;

		if (m_isFinished)
		{
			iCurrentKeyFrame = 0;
			pAnimChannel->Set_CurrentKeyFrame(iCurrentKeyFrame);
		}

		if (m_CurrrentTime <= pFirst->Time)
		{
			vScale = XMLoadFloat3(&pFirst->vScale);
			vRotation = XMLoadFloat4(&pFirst->vRotation);
			vPosition = XMLoadFloat3(&pFirst->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else if (m_CurrrentTime > pLast->Time)
		{
			vScale = XMLoadFloat3(&pLast->vScale);
			vRotation = XMLoadFloat4(&pLast->vRotation);
			vPosition = XMLoadFloat3(&pLast->vPosition);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		else
		{
			if (m_CurrrentTime > KeyFrames[iCurrentKeyFrame + 1]->Time)
				pAnimChannel->Set_CurrentKeyFrame(++iCurrentKeyFrame);


			_float		fRatio = (_float)(m_CurrrentTime - KeyFrames[iCurrentKeyFrame]->Time) /
				(_float)(KeyFrames[iCurrentKeyFrame + 1]->Time - KeyFrames[iCurrentKeyFrame]->Time);

			vSourScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vScale);
			vSourRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame]->vRotation);
			vSourPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vPosition);
			vSourPosition = XMVectorSetW(vSourPosition, 1.f);

			vDestScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vScale);
			vDestRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vPosition);
			vDestPosition = XMVectorSetW(vDestPosition, 1.f);

			vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
			vPosition = XMVectorSetW(vPosition, 1.f);
		}
		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pAnimChannel->Set_TransformationMatrix(TransformationMatrix);
	}

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