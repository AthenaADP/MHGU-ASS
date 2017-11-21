#pragma once
#include "Common.h"

ref struct Charm;
ref struct Query;
ref struct Armor;
ref struct Weapon;

ref struct TableSlotDatum
{
	array< signed char, 2 >^ max_single;
	array< array< List_t< Charm^ >^, 2 >^ >^ two_skill_data;
};

ref struct CharmLocationDatum
{
	array< unsigned, 2 >^ table;
	System::String^ example;
	array< System::String^ >^ examples;
};

ref struct CharmDatabase
{
	static List_t< Charm^ > mycharms;
	//static array< TableSlotDatum^, 2 >^ min_max;
	//static Map_t< unsigned, unsigned >^ hash_to_table;
	static Map_t< System::String^, CharmLocationDatum^ >^ location_cache;
	static array< int >^ table_seeds = { 1, 15, 5, 13, 4, 3, 9, 12, 26, 18, 163, 401, 6, 2, 489, 802, 1203 };
	static array< List_t< unsigned short >^ >^ table_seed_list;
	static unsigned NumCharmTypes = 4;

	static List_t< Charm^ >^ LoadCharms( System::String^ filename );
	static bool LoadCustom();
	static void SaveCustom();
	static bool CreateNewCustom();

	static void GenerateCharmTable();
	static bool CharmIsLegal( Charm^ charm );
	static List_t< Charm^ >^ GetCharms( Query^ query, const bool use_two_skill_charms );

	static CharmLocationDatum^ FindCharmLocations( Charm^ charm );
};
