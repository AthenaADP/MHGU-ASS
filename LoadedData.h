#pragma once
#include <vcclr.h>

#include "Armor.h"
#include "Decoration.h"
#include "Solution.h"

ref struct SkillTag
{
	System::String^ name;
	bool disable_g, disable_b;

	SkillTag( System::String^ s );

	static List_t< SkillTag^ > tags;
	static SkillTag^ FindTag( System::String^ tag );
	static void Load( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
};

void AddToList( List_t< Decoration^ >^ list, Decoration^ item, List_t< Ability^ >^ rel_abilities, List_t< Decoration^ >^ inf_decs, const bool adv, const bool want_taunt );
void AddToList( List_t< Armor^ >^ list, Armor^ armor, List_t< Ability^ >^ rel_abilities, List_t< Armor^ >^ inf_armor, const bool adv, const bool want_taunt );

ref class LoadedData
{
public:
	Skill^ FindSkill( const unsigned skill );

	void LoadDataFiles();

	void GetRelevantData( Query^ query );
};
