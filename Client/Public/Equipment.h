#pragma once
#include "Component.h"


BEGIN(Client)
class CEquipment : public CComponent
{
public:
	typedef struct tagWeapon
	{
		WEAPONDESC*			primary;
		WEAPONDESC*			secondary;
		BASEEQUIPDESC*		grenade;
		BASEEQUIPDESC*		tool;
	} WEAPON;

	typedef struct tagGear
	{
		GEARDESC* head;
		GEARDESC* shirt;
		GEARDESC* assult;
		GEARDESC* backpack;
	} GEAR;

private:
	explicit CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEquipment(const CEquipment& rhs);
	virtual ~CEquipment() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
public:
	static CEquipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END