#pragma once
namespace Client
{


	typedef struct tagBaseEquipDesc
	{
		string		name;
		string		background;
		_float		weight;
		_uint		cost;
		_uint		slotX, slotY;
		Ref<CModel> model;
	}BASEEQUIPDESC;

	typedef struct tagWeaponDesc : BASEEQUIPDESC
	{
		_float			rpm;
		_float			accuracy;
	}WEAPONDESC;

	typedef struct tagMagazineDesc : BASEEQUIPDESC
	{
		_uint			magCapacity;
		_uint			magRound;
	}MAGAZINEDESC;

	typedef struct tagGearDesc : BASEEQUIPDESC
	{
		_float			mobility;
		_float			dexterity;
		list<_float2>	inventories;
	}GEARDESC;

}