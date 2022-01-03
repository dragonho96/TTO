#pragma once
#include "Base.h"
#include "IScriptObject.h"
BEGIN(Engine)

class CScriptObjectManager final : public CBase
{
	DECLARE_SINGLETON(CScriptObjectManager)
private:
	CScriptObjectManager();
	virtual ~CScriptObjectManager() = default;
	virtual void Free() override;
public:
	void Update(_double deltaTime);
	void LateUpdate(_double deltaTime);
public:
	IScriptObject * AddObject(IScriptObject* pObj);

	list<IScriptObject*>	m_ScriptObjects;


};

END