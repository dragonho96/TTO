#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
public:
	HRESULT NativeConstruct(const char* pAnimationName, _double Duration, _double TickPerSecond);
	HRESULT Add_Channel(class CChannel* pChannel);
	_bool Update_TransformationMatrices(_double TimeDelta);
	HRESULT Blend_Animation(CAnimation* prevAnim, _float ratio);
	HRESULT ResetCurrentTime();
	vector<class CChannel*> GetChannels() { return m_Channels; }

	char* GetName() { return m_AnimationName; }
	void	SetLoop(_bool result) { m_isLoop = result; }

private:
	char				m_AnimationName[MAX_PATH];
	_double				m_Duration = 0.0; 
	_double				m_TickPerSecond = 0.0;
	_double				m_CurrrentTime = 0.0;

	_bool				m_isFinished = false;
	_bool				m_isLoop = true;
	vector<class CChannel*>	m_Channels;
public:
	static CAnimation* Create(const char* pAnimationName, _double Duration, _double TickPerSecond);
	virtual void Free() override;
};

END