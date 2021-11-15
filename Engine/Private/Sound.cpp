#include "..\Public\Sound.h"

static const FILESYSTEM::path s_Path = "../../Sound";

IMPLEMENT_SINGLETON(CSound)
USING(Engine)
CSound::CSound() 
{
	m_pSystem = nullptr;
	Initialize();
}

void CSound::Initialize()
{
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
}

void CSound::Free()
{

}

void CSound::LoadSoundFile()
{
	for (auto& iter : FILESYSTEM::directory_iterator(s_Path))
	{
		FILESYSTEM::path filePath = iter.path();
		string fileName = iter.path().filename().string();
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, filePath.string().c_str(), FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			m_mapSound.emplace(fileName, pSound);
		}
	}

	FMOD_System_Update(m_pSystem);
}

void CSound::PlaySound(string pSoundKey, CHANNELID eID)
{
	map<string, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		//return !lstrcmp(pSoundKey, iter.first);
		return iter.first == pSoundKey;
	});

	// If There is no sound registered
	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	}
	FMOD_System_Update(m_pSystem);
}

void CSound::PlayBGM(string pSoundKey)
{
	map<string, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		//return !lstrcmp(pSoundKey, iter.first);
		return iter.first == pSoundKey;
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSound::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}


