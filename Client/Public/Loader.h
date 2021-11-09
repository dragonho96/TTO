#pragma once

/* 다음레벨에 대한 리소스 원형, 객체 우넌형을 준비하낟. */
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader();
	virtual ~CLoader() = default;
public:
	CRITICAL_SECTION Get_CS() {
		return m_CS;
	}

	SCENE Get_NextScene() const {
		return m_eScene;
	}

	_bool Get_Finish() const {
		return m_isFinish;
	}
public:
	HRESULT Initialize(SCENE eScene);
	HRESULT GamePlayLoader();

private:
	SCENE				m_eScene = SCENE_END;
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CS;
	_bool				m_isFinish;

private:



public:
	static CLoader* Create(SCENE eScene);
	virtual void Free() override;
};

END