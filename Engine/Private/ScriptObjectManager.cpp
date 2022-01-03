#include "../Public/ScriptObjectManager.h"


IMPLEMENT_SINGLETON(CScriptObjectManager)

CScriptObjectManager::CScriptObjectManager()
{
}

void CScriptObjectManager::Update(_double deltaTime)
{
	for (auto& iter = m_ScriptObjects.begin(); iter != m_ScriptObjects.end();)
	{
		if ((*iter)->IsDead())
		{
			SafeRelease(*iter);
			iter = m_ScriptObjects.erase(iter);
		}
		else
		{
			(*iter)->Update(deltaTime);
			++iter;
		}
	}
	//for (auto& obj : m_ScriptObjects)
	//	obj->Update(deltaTime);
}

void CScriptObjectManager::LateUpdate(_double deltaTime)
{
	for (auto& obj : m_ScriptObjects)
		obj->LapteUpdate(deltaTime);
}

IScriptObject * CScriptObjectManager::AddObject(IScriptObject * pObj)
{
	if (pObj)
	{
		m_ScriptObjects.push_back(pObj);
		return pObj;
	}
}

void CScriptObjectManager::Free()
{
	for (auto& obj : m_ScriptObjects)
		SafeRelease(obj);
}
