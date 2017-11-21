#pragma once
#include "Common.h"
#include <cliext/utility>

ref struct Skill;
ref struct Ability;
ref struct Armor;
ref struct Charm;
ref struct ArmorEquivalence;
ref struct Decoration;
ref struct AbilityPair;

typedef System::Collections::Generic::Dictionary< Ability^, int > AbilityMap;

ref struct Query : public System::Object
{
	List_t< Skill^ > skills;
	HunterType hunter_type;
	Gender gender;
	unsigned hr, elder_star, weapon_slots_allowed;
	bool include_arena, allow_bad, allow_event, allow_lower_tier, allow_japs, add_extra, want_taunt, allow_gunner_helms;

	List_t< List_t< Armor^ >^ > rel_armor, inf_armor;
	List_t< Ability^ > rel_abilities;
	List_t< Decoration^ > rel_decorations, inf_decorations, taunt_decorations;

	template< class T >
	static bool HRMatches( T^ t, const unsigned hr, const unsigned ve )
	{
		if( t->hr_or )
			return hr >= t->hr || ve >= t->elder_star;
		else
			return hr >= t->hr && ve >= t->elder_star;
	}
};


ref struct ThreadSearchData
{
	Query^ query;
	Charm^ charm_template;
};

ref struct ThreadSearchData2
{
	Query^ query;
	List_t< Charm^ >^ charm_templates;
};

ref struct CalculationData;

ref struct Solution
{
	List_t< Armor^ > armors;
	List_t< Decoration^ > decorations, body_decorations, non_body_decorations, charm_decorations;
	List_t< Skill^ > extra_skills, bad_skills, potential_extra_skills;
	AbilityMap abilities;
	Charm^ charm;
	System::Drawing::Bitmap^ preview_image;

	unsigned torso_slots_spare, charm_slots_spare, total_slots_spare;
	int fire_res, ice_res, water_res, thunder_res, dragon_res, extra_skill_score, torso_multiplier;
	unsigned defence, max_defence, rarity, difficulty;
	array< unsigned >^ slots_spare, ^family_score;
	bool doubled, reordered_to_fix, charm_up, skill_plus2;

	bool MatchesQuery( Query^ query );
	void CalculateData( const unsigned hr, const unsigned elder_star );
	void CalculateFamilyScore();
	void CalculateExtraSkillScore( const unsigned hr, const unsigned elder_star );
	void CalculateSkillModifiers();
	void AddAllSkillsPlus2();

private:
	CalculationData^ data;

	bool FixBadSkill( Skill^ skill );
	bool FixBadSkills( Query^ query );
	bool CheckBadSkills( Query^ query );
	bool ImpossibleNoDecorations( Query^ query );
	Decoration^ Count1SocketGems( Ability^ ability, int& num );
	int GetReplacable( Ability^ ability, unsigned slots );
	bool ReorderGems( Query^ query );
	void Replace( const unsigned index, Decoration^ decoration );
	void CalculateExtraSkills( Query^ query );
	void ReduceSlots();
	void ReduceSkills();
	void ReduceCharm();
	void RearrangeDecorations();
	void AddExtraSkills();
};
