#pragma once
#include "Base.h"
#include "Engine.h"

BEGIN(Client)

class CEquipButtonManager : public IScriptObject
{
	DECLARE_SINGLETON(CEquipButtonManager)
private:
	CEquipButtonManager();
	virtual ~CEquipButtonManager() = default;
public:
	virtual void Free() override;
public:
	void Initialize();
	virtual void Update(_double deltaTime);
	virtual void LapteUpdate(_double deltaTime);
	virtual void Render();

private:
	class CGameObject* m_pPrimaryWeaponButton = nullptr;
	class CGameObject* m_pHoverInfo = nullptr;

};

END