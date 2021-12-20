#pragma once
namespace Client
{
	enum class  EQUIPMENT : int { PRIMARY, PRIMARYMAG, SECONDARY, SECONDARYMAG, GRENADE, TOOL, HEADGEAR, TORSO, LEGS, VEST, BACKPACK, NONE };
	enum class  GEAR : int { HEADGEAR, TORSO, LEGS, VEST, BACKPACK, NONE };


	typedef struct tagBaseEquipDesc
	{
		tagBaseEquipDesc() {}
		tagBaseEquipDesc(const tagBaseEquipDesc& rhs)
			: name(rhs.name), background(rhs.background)
			, weight(rhs.weight), cost(rhs.cost)
			, slotSize(rhs.slotSize) {}

		string		name;
		string		background;
		_float		weight;
		_uint		cost;
		_uint2		slotSize;
		Ref<CModel> model;
	}BASEEQUIPDESC;

	typedef struct tagWeaponDesc : BASEEQUIPDESC
	{
		tagWeaponDesc() {}
		tagWeaponDesc(const tagWeaponDesc& rhs)
			:tagBaseEquipDesc(rhs)
			,rpm(rhs.rpm), accuracy(rhs.accuracy) {}

		_float			rpm;
		_float			accuracy;
	}WEAPONDESC;

	typedef struct tagMagazineDesc : BASEEQUIPDESC
	{
		tagMagazineDesc() {}
		tagMagazineDesc(const tagMagazineDesc& rhs)
			: tagBaseEquipDesc(rhs)
			, magType(rhs.magType), magRound(rhs.magRound) {}

		EQUIPMENT		magType;
		_uint			magRound;
	}MAGAZINEDESC;

	typedef struct tagGearDesc : BASEEQUIPDESC
	{
		tagGearDesc() {}
		tagGearDesc(const tagGearDesc& rhs)
			: tagBaseEquipDesc(rhs)
			, mobility(rhs.mobility), dexterity(rhs.dexterity)
			, inventories(rhs.inventories) {}

		_float			mobility;
		_float			dexterity;
		list<_uint2>	inventories;
	}GEARDESC;

	typedef struct tagInventory
	{
		_uint sizeX, sizeY;

	}INVENTORY;
}