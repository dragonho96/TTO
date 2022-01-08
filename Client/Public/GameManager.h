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
	void PlayMuzzleEffect();
	void PlayImpactSmokeEffect(_vector vPos, _vector vNormal);
	void PlayImpactEffect(_vector vPos);
	void PlayExplosion(_vector vPos);
private:
	class CEffect_Muzzle* m_pMuzzleEffect = nullptr;
	class CEffect_ImpactSmoke* m_pImpactSmokeEffect = nullptr;
	class CEffect_Impact* m_pImpactEffect = nullptr;
	class CEffect_Explosion* m_pExplosion = nullptr;
public:
	virtual void Free() override;
};

END