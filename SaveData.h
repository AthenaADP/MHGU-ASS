#include "Armor.h"
#include "Common.h"

ref struct SaveDataCharacter
{
	System::String^ name;
	List_t< Charm^ > charms;
};

ref struct SaveData
{
	array< SaveDataCharacter^ >^ characters;
	unsigned last_used;
	bool failed;

	SaveData( System::String^ filename );
};