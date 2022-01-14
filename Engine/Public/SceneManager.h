#pragma once

#include "Base.h"
#include "Scene.h"

/* ���� �Ҵ�Ǿ��ִ� ������ ���������Ѵ�. */
/* ƽ, ������ ȣ�����ش�. */
/* ���� ��ü�� �ÿ��� ���� ������ �����ϰ� ��ü�Ͽ� �����Ѵ�. */

BEGIN(Engine)

class CSceneManager final : public CBase
{
	DECLARE_SINGLETON(CSceneManager)
public:
	CSceneManager();
	virtual ~CSceneManager() = default;

public:
	HRESULT		SetUpCurrentScene(CScene* pCurrentScene);
	_uint		UpdateScene(_double TimeDelta);
	HRESULT		RenderScene();

private:
	CScene*				m_pCurrentScene = nullptr;
public:
	virtual void Free() override;
};

END