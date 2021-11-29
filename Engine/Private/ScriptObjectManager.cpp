#include "../Public/ScriptObjectManager.h"


IMPLEMENT_SINGLETON(CScriptObjectManager)

CScriptObjectManager::CScriptObjectManager()
{
}

void CScriptObjectManager::Update(_double deltaTime)
{
	for (auto& obj : m_ScriptObjects)
		obj->Update(deltaTime);
}

void CScriptObjectManager::LateUpdate(_double deltaTime)
{
	for (auto& obj : m_ScriptObjects)
		obj->LapteUpdate(deltaTime);
}

void CScriptObjectManager::AddObject(IScriptObject * pObj)
{
	if (pObj)
		m_ScriptObjects.push_back(pObj);
}

void CScriptObjectManager::Free()
{
	for (auto& obj : m_ScriptObjects)
		SafeRelease(obj);
}
