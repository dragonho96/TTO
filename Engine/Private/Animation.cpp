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
_bool CAnimation::Update_TransformationMatrices(_double TimeDelta)
{
	m_CurrrentTime += fmod(m_TickPerSecond * TimeDelta, m_Duration);

	if (m_CurrrentTime > m_Duration)
	{
		if (m_isLoop)
			m_CurrrentTime = fmod(m_TickPerSecond * TimeDelta, m_Duration);

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

	return m_isFinished;
}
HRESULT CAnimation::Blend_Animation(CAnimation * prevAnim, _float ratio)
{
	if (ratio >= 1.f)
		ratio = 1.f;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;
	_vector		vScale, vRotation, vPosition;

	auto curIter = m_Channels.begin();
	auto prevIter = prevAnim->m_Channels.begin();
	for (; curIter != m_Channels.end() && prevIter != prevAnim->m_Channels.end(); ++curIter, ++prevIter)
	{
		vector<KEYFRAMEDESC*>	curKeyFrames = (*curIter)->Get_KeyFrames();
		vector<KEYFRAMEDESC*>	prevKeyFrames = (*prevIter)->Get_KeyFrames();

		_uint		iCurCurrentKeyFrame = (*curIter)->Get_CurrentKeyFrame();
		_uint		iPrevCurrentKeyFrame = (*prevIter)->Get_CurrentKeyFrame();

		_matrix prevMat = (*prevIter)->Get_TransformationMatrix();

		XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourPosition, prevMat);
		vSourPosition = XMVectorSetW(vSourPosition, 1.f);

		vDestScale = XMLoadFloat3(&curKeyFrames[iCurCurrentKeyFrame]->vScale);
		vDestRotation = XMLoadFloat4(&curKeyFrames[iCurCurrentKeyFrame]->vRotation);
		vDestPosition = XMLoadFloat3(&curKeyFrames[iCurCurrentKeyFrame]->vPosition);
		vDestPosition = XMVectorSetW(vDestPosition, 1.f);

		vScale = XMVectorLerp(vSourScale, vDestScale, ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		(*curIter)->Set_TransformationMatrix(TransformationMatrix);
	}

	return S_OK;
}
HRESULT CAnimation::Blend_Animation(list<_matrix> prevAnimMatrix, _float ratio)
{
	if (ratio >= 1.f)
		ratio = 1.f;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;
	_vector		vScale, vRotation, vPosition;

	auto curIter = m_Channels.begin();
	auto prevIter = prevAnimMatrix.begin();
	for (; curIter != m_Channels.end() && prevIter != prevAnimMatrix.end(); ++curIter, ++prevIter)
	{
		vector<KEYFRAMEDESC*>	curKeyFrames = (*curIter)->Get_KeyFrames();

		_uint		iCurCurrentKeyFrame = (*curIter)->Get_CurrentKeyFrame();

		_matrix prevMat = (*prevIter);

		XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourPosition, prevMat);
		vSourPosition = XMVectorSetW(vSourPosition, 1.f);

		vDestScale = XMLoadFloat3(&curKeyFrames[iCurCurrentKeyFrame]->vScale);
		vDestRotation = XMLoadFloat4(&curKeyFrames[iCurCurrentKeyFrame]->vRotation);
		vDestPosition = XMLoadFloat3(&curKeyFrames[iCurCurrentKeyFrame]->vPosition);
		vDestPosition = XMVectorSetW(vDestPosition, 1.f);

		vScale = XMVectorLerp(vSourScale, vDestScale, ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		(*curIter)->Set_TransformationMatrix(TransformationMatrix);
	}

	return S_OK;
}
list<_matrix> CAnimation::Get_Blend_Animation_Matrix(CAnimation * prevAnim, _float ratio)
{
	list<_matrix> out;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;
	_vector		vScale, vRotation, vPosition;

	auto curIter = m_Channels.begin();
	auto prevIter = prevAnim->m_Channels.begin();
	for (; curIter != m_Channels.end() && prevIter != prevAnim->m_Channels.end(); ++curIter, ++prevIter)
	{
		vector<KEYFRAMEDESC*>	curKeyFrames = (*curIter)->Get_KeyFrames();
		vector<KEYFRAMEDESC*>	prevKeyFrames = (*prevIter)->Get_KeyFrames();

		_uint		iCurCurrentKeyFrame = (*curIter)->Get_CurrentKeyFrame();
		_uint		iPrevCurrentKeyFrame = (*prevIter)->Get_CurrentKeyFrame();

		_matrix prevMat = (*prevIter)->Get_TransformationMatrix();

		XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourPosition, prevMat);
		vSourPosition = XMVectorSetW(vSourPosition, 1.f);

		vDestScale = XMLoadFloat3(&curKeyFrames[iCurCurrentKeyFrame]->vScale);
		vDestRotation = XMLoadFloat4(&curKeyFrames[iCurCurrentKeyFrame]->vRotation);
		vDestPosition = XMLoadFloat3(&curKeyFrames[iCurCurrentKeyFrame]->vPosition);
		vDestPosition = XMVectorSetW(vDestPosition, 1.f);

		vScale = XMVectorLerp(vSourScale, vDestScale, ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		out.emplace_back(TransformationMatrix);
	}

	return out;
}
HRESULT CAnimation::ResetCurrentTime()
{
	if (!m_isLoop)
		m_CurrrentTime = 0.0;
	
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