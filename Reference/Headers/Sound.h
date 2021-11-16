#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)
	enum CHANNELID { BGM, PLAYER, ENEMY, EFFECT, UI, DIALOGUE, ITEM, GUN, SLOWMO, MAXCHANNEL };

class CSound : public CBase
{
	DECLARE_SINGLETON(CSound)



private:
	CSound();
	virtual ~CSound() = default;

public:
	virtual void Free() override;
	void Initialize();

	void LoadSoundFile();
	void PlaySound(string pSoundKey, CHANNELID eID);
	void PlayBGM(string pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();

private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<string, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	FMOD_SYSTEM* m_pSystem;

	float m_volume;
	FMOD_BOOL m_bool;
};

END
