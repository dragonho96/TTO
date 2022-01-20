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
	void SpawnEnemy();
	void SpawnEnemy_Thread(_int index);
	CRITICAL_SECTION Get_CS() { return m_CS; }

private:
	list<CGameObject*>	m_EnemySpawn;
	_double				m_dSpawnTime;
	CRITICAL_SECTION	m_CS;
	HANDLE				m_hThread = 0;

public:
	void Shake_Rifle();
	void Shake_Grenade();
	void SwitchCamera(CAMERA type);
	CAMERA GetCurrentCamera() { return m_eCurCamera; }

private:
	vector<CCamera*>	m_Camera;
	CAMERA				m_eCurCamera = CAMERA::FOLLOW;
	CVIBuffer_RectUI*	m_pSceneMask = nullptr;
	_float				m_fMastAlpha = 0.f;

public:
	class CEmptyUI*		m_pCrosshair = nullptr;
	class CEmptyUI*		m_pCursor = nullptr;

public:
	void SetShootingSightUI(_bool result, _vector hitPos = {}, _vector cursorPos = {});
private:
	class CSightIndicator*	m_pSightIndicator = nullptr;

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



/* LOBBY */
public:
	void RegisterPlayer(class CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void ChangeCameraPos(EQUIPMENT eType);
private:
	vector<CTransform*>				m_CamPos;
	class CCamera_Lobby*			m_CamLobby =nullptr;
	class CPlayer*					m_pPlayer = nullptr;
public:
	virtual void Free() override;
};

END