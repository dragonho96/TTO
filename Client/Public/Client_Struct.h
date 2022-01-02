#pragma once
namespace Client
{
	typedef struct tagBaseEquipDesc
	{
		tagBaseEquipDesc() {}
		tagBaseEquipDesc(const tagBaseEquipDesc& rhs)
			: type(rhs.type)
			, name(rhs.name), background(rhs.background)
			, weight(rhs.weight), cost(rhs.cost)
			, slotSize(rhs.slotSize), texturePath(rhs.texturePath)
			, model(nullptr), mesh(nullptr) {}

		EQUIPMENT					type;
		string						name;
		string						texturePath;
		string						background;
		_float						weight;
		_uint						cost;
		_uint2						slotSize;
		CGameObject*				model;		// for weapons and tools
		class CMeshContainer*		mesh;		// for gears
	}BASEEQUIPDESC;

	typedef struct tagWeaponDesc : BASEEQUIPDESC
	{
		tagWeaponDesc() {}
		tagWeaponDesc(const tagWeaponDesc& rhs)
			:tagBaseEquipDesc(rhs)
			,rpm(rhs.rpm), accuracy(rhs.accuracy) {}

		_float			rpm;
		_float			muzzleVelocity;
		_float			recoil;
		_float			accuracy;
		_float			handling;
	}WEAPONDESC;

	typedef struct tagMagazineDesc : BASEEQUIPDESC
	{
		tagMagazineDesc() {}
		tagMagazineDesc(const tagMagazineDesc& rhs)
			: tagBaseEquipDesc(rhs)
			, magRound(rhs.magRound) {}

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