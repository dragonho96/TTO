#pragma once
#include "Base.h"
#include "Engine.h"

BEGIN(Client)
class CGameManager : public CBase
{
public:
	enum class CAMERA { FOLLOW, FLY, NONE };
	DECLARE_SINGLETON(CGameManager)
private:
	CGameManager();
	virtual ~CGameManager() = default;
public:
	HRESULT Initialize();
	void Update(_double TimeDelta);

public:
	void SwitchCamera(CAMERA type);
	CAMERA GetCurrentCamera() { return m_eCurCamera; }
private:
	vector<CCamera*>	m_Camera;
	CAMERA				m_eCurCamera = CAMERA::FOLLOW;
	CVIBuffer_RectUI*	m_pSceneMask = nullptr;
	_float				m_fMastAlpha = 0.f;
public:
	virtual void Free() override;
};

END