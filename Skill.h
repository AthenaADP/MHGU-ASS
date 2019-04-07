#pragma once
#include "Common.h"

ref struct Skill;
ref struct Query;
ref struct SkillTag;

ref struct Ability
{
	typedef System::Collections::Generic::Dictionary< int, Skill^ > SkillMap_t;
	System::String^ name;
	System::String^ jap_name;
	List_t< SkillTag^ > tags;
	bool relevant, related, efficient, has_bad, has_decorations, has_1slot_decorations;
	unsigned static_index, ping_index;
	array< unsigned char >^ max_vals1;
	int order, num_good_skills;
	SkillMap_t skills;

	Skill^ GetSkill( const int amount );
	Skill^ GetWorstGoodSkill();

	static List_t< Ability^ > static_abilities, ordered_abilities, savedata_abilities; //for sorting alphabetically
	static Map_t< System::String^, Ability^ > static_ability_map;
	static Map_t< System::String^, Ability^ > charm_ability_map; //Japanese-only, for importing from MasaxMH
	static Ability^ FindAbility( System::String^ name );
	static Ability^ FindCharmAbility( System::String^ name ); //Japanese-only, for importing from MasaxMH
	static void UpdateOrdering();
};

ref struct SpecificAbility
{
	static System::String^ torso_inc_desc;
	static Ability^ torso_inc;
	static Ability^ fire_res;
	static Ability^ water_res;
	static Ability^ thunder_res;
	static Ability^ ice_res;
	static Ability^ dragon_res;
	static Ability^ defence;
	static Ability^ sense;
	static Ability^ gathering;
	static Ability^ charm_up;
	static Ability^ skill_point_plus2;
};

int CompareAbilitiesByName( Ability^ a1, Ability^ a2 );

ref struct Skill
{
	System::String^ name;
	int points_required;
	unsigned order, static_index, ping_index;
	bool best, is_taunt, autodisabled, impossible;
	Ability^ ability;

	System::String^ description;

	static void Load( System::String^ filename );
	static void LoadCompound( System::String^ filename );
	static void LoadLanguage( System::String^ filename );
	static void LoadSaveData( System::String^ filename );
	static void LoadDescriptions( System::String^ filename );
	static Map_t< Skill^, array< Skill^ >^ > compound_skill_map;
	static List_t< Skill^ > static_skills, ordered_skills; //for sorting alphabetically
	static Map_t< System::String^, Skill^ > static_skill_map;
	static Skill^ FindSkill( System::String^ name );
	static void UpdateOrdering();
	static bool CompoundSkillOverrides( Skill^ compound, Skill^ other );
	static bool UpdateImpossibleSkills( const unsigned hr, const unsigned ve );
};

ref struct AbilityPair
{
	Ability^ ability;
	int amount;

	AbilityPair( Ability^ ab, const int am )
		: ability( ab ), amount( am ) {}
};

void FindRelatedSkills( List_t< System::Windows::Forms::ComboBox^ >% skills, List_t< System::Windows::Forms::ComboBox^ >% skill_filters, List_t< Map_t< unsigned, unsigned >^ >% index_maps );