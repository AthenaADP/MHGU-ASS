#include "stdafx.h"
#include "Solution.h"
#include "Armor.h"
#include "Decoration.h"
#include "Skill.h"

using namespace System;

ref struct CalculationData
{
	Query^ query;
	Solution^ solution;
	AbilityMap need;
	Map_t< Skill^, int > relevant;
	List_t< List_t< Decoration^ >^ > rel_decoration_map;
	List_t< Ability^ > at_risk;
};

bool Detrimental( Decoration^ dec, CalculationData^ data, const int multiplier )
{
	if( dec->abilities.Count < 2 || !dec->abilities[ 1 ]->ability->relevant )
		return false;
	
	const int have = data->solution->abilities[ dec->abilities[ 1 ]->ability ];
	const int need = data->need[ dec->abilities[ 1 ]->ability ];
	
	return have + dec->abilities[ 1 ]->amount * multiplier < need;
}

void AddAbilities( AbilityMap% total, List_t< AbilityPair^ >% abilities, const int multiplier )
{
	for each( AbilityPair^ ap in abilities )
	{
		if( ap->ability == SpecificAbility::torso_inc )
			continue;

		if( total.ContainsKey( ap->ability ) )
			total[ ap->ability ] += ap->amount * multiplier;
		else
			total.Add( ap->ability, ap->amount * multiplier );
	}
}

bool AddDecoration( Solution^ solution, Decoration^ deco, const int multiplier, List_t< Decoration^ >% deco_list )
{
	solution->decorations.Add( deco );
	deco_list.Add( deco );
	AddAbilities( solution->abilities, deco->abilities, multiplier );
	solution->total_slots_spare -= deco->slots_required;
	return solution->abilities[ deco->abilities[ 0 ]->ability ] > -10;
}

Decoration^ GetBestAvailableDecoration( CalculationData^ data, Ability^ ability, const unsigned slots_spare )
{
	return ability->relevant
		? Decoration::GetBestDecoration( ability, slots_spare, data->rel_decoration_map )
		: Decoration::GetBestDecoration( ability, slots_spare, data->query->hr, data->query->elder_star );
}

bool Solution::FixBadSkill( Skill^ skill )
{
	while( torso_slots_spare > 0 )
	{
		Decoration^ dec = GetBestAvailableDecoration( data, skill->ability, torso_slots_spare );
		if( !dec || Detrimental( dec, data, torso_multiplier ) )
			return false;
		torso_slots_spare -= dec->slots_required;
		if( AddDecoration( data->solution, dec, torso_multiplier, body_decorations ) )
			return true;
	}

	int max_slots = 3;
	while( max_slots > 0 && !slots_spare[ max_slots ] )
		max_slots--;

	if( max_slots == 0 )
		return false;

	const int have = abilities[ skill->ability ];
	const unsigned points_required = -9 - have;

	Decoration^ dec;
	for( int slot_size = 1; slot_size <= max_slots; ++slot_size )
	{
		if( dec = GetBestAvailableDecoration( data, skill->ability, slot_size ) )
		{
			if( dec->abilities[ 0 ]->amount >= (int)points_required )
				break;
		}
	}

	if( !dec )
		return false;

	while( max_slots > 0 )
	{
		if( Detrimental( dec, data, 1 ) )
			return false;

		if( slots_spare[ dec->slots_required ] == 0 )
		{
			if( slots_spare[ dec->slots_required + 1 ] == 0 )
			{
				Assert( slots_spare[ 3 ] > 0, L"Not enough slots spare" );
				slots_spare[ 3 ]--;
				slots_spare[ 2 ]++;
			}
			else
			{
				Assert( data->solution->slots_spare[ dec->slots_required + 1 ] > 0, L"Not enough slots spare" );
				slots_spare[ dec->slots_required + 1 ]--;
				slots_spare[ 1 ]++;
			}
		}
		else
		{
			Assert( slots_spare[ dec->slots_required ] > 0, L"Not enough slots spare" );
			slots_spare[ dec->slots_required ]--;
		}

		if( AddDecoration( data->solution, dec, 1, non_body_decorations ) )
			return true;

		if( slots_spare[ max_slots ] > 0 )
			continue;

		while( max_slots > 0 && !slots_spare[ max_slots ] )
			dec = GetBestAvailableDecoration( data, skill->ability, --max_slots );
	}
	return false;
}


bool Solution::FixBadSkills( Query^ query )
{
	for each( Skill^ skill in bad_skills )
	{
		if( !FixBadSkill( skill ) )
			return false;
	}
	CalculateExtraSkills( query );
	return bad_skills.Count == 0;
}

bool Solution::CheckBadSkills( Query^ query )
{
	return bad_skills.Count == 0 || ( total_slots_spare > 0 && FixBadSkills( query ) ) || ReorderGems( query );
}

Decoration^ Solution::Count1SocketGems( Ability^ ability, int& num )
{
	num = 0;
	Map_t< Decoration^, unsigned > deco_count;
	Decoration^ best = nullptr;
	for each( Decoration^ dec in decorations )
	{
		if( dec->slots_required == 1 && dec->abilities.Count == 2 && dec->abilities[ 1 ]->ability == ability )
		{
			if( deco_count.ContainsKey( dec ) )
			{
				const int n = ++deco_count[ dec ];
				if( n > num )
				{
					num = n;
					best = dec;
				}
			}
			else
			{
				deco_count.Add( dec, 1 );
				if( num == 0 )
				{
					num = 1;
					best = dec;
				}
			}
		}
	}
	return best;
}

int Solution::GetReplacable( Ability^ ability, unsigned slots )
{
	for( int i = 0; i < decorations.Count; ++i )
		if( decorations[ i ]->slots_required == slots && decorations[ i ]->abilities.Count > 1 && decorations[ i ]->abilities[ 1 ]->ability != ability )
			return i;
	return -1;
}

bool Solution::ReorderGems( Query^ query )
{
	//return false;
	for each( Skill^ skill in bad_skills )
	{
		if( abilities[ skill->ability ] < -9 )
		{
			int num_1_socketed;
			Decoration^ one_slot = Count1SocketGems( skill->ability, num_1_socketed );
			if( num_1_socketed == 0 )
				return false;
			
			Decoration^ bigger_slotted = GetBestAvailableDecoration( data, one_slot->abilities[ 0 ]->ability, 3 );
			const int num_required = bigger_slotted->abilities[ 0 ]->amount;
			if( num_1_socketed < num_required )
				return false;
			
			const int other = GetReplacable( skill->ability, bigger_slotted->slots_required );
			Assert( other == -1 || decorations[ other ]->abilities.Count == 2, L"Failed to replace gems" );
			if( other != -1 && num_1_socketed >= decorations[ other ]->abilities[ 0 ]->amount && abilities[ decorations[ other ]->abilities[ 1 ]->ability ] > -8 )
			{
				Decoration^ other_one_slot = Decoration::static_decoration_ability_map[ decorations[ other ]->abilities[ 0 ]->ability ][ 0 ];
				Replace( other, bigger_slotted );
				int num_replaced = 0;
				int num_to_replace = decorations[ other ]->abilities[ 0 ]->amount > num_required ? decorations[ other ]->abilities[ 0 ]->amount : num_required;
				for( int i = 0; i < decorations.Count; ++i )
				{
					if( decorations[ i ] == one_slot && num_replaced++ < num_to_replace )
						Replace( i, other_one_slot );
				}
				if( other_one_slot->abilities.Count > 1 )
				{
					const int other_have = abilities.ContainsKey( other_one_slot->abilities[ 1 ]->ability ) ? abilities[ other_one_slot->abilities[ 1 ]->ability ] : 0;
					const int other_need = data->need.ContainsKey( other_one_slot->abilities[ 1 ]->ability ) ? data->need[ other_one_slot->abilities[ 1 ]->ability ] : 0;
					if( other_have < other_need )
						return false;
				}
				if( abilities[ skill->ability ] < -9 )
					return false;
			}
			else return false;
		}
	}
	CalculateExtraSkills( query );
	reordered_to_fix = true;
	return true;
}

void Solution::Replace( const unsigned index, Decoration^ decoration )
{
	for each( AbilityPair^ apair in decorations[ index ]->abilities )
	{
		abilities[ apair->ability ] -= apair->amount;
	}
	decorations[ index ] = decoration;
	for each( AbilityPair^ apair in decoration->abilities )
	{
		if( abilities.ContainsKey( apair->ability ) )
			abilities[ apair->ability ] += apair->amount;
		else abilities.Add( apair->ability, apair->amount );
	}
}

void AddInitialArmor( Armor^ armor, Solution^ solution, const int multiplier )
{
	if( armor )
	{
		solution->slots_spare[ armor->num_slots ] += ( armor != solution->armors[ 1 ] ) * !!armor->num_slots;
		solution->defence += armor->defence;
		solution->max_defence += armor->max_defence;
		solution->ice_res += armor->ice_res;
		solution->fire_res += armor->fire_res;
		solution->water_res += armor->water_res;
		solution->dragon_res += armor->dragon_res;
		solution->thunder_res += armor->thunder_res;
		if( armor->rarity != 99 )
			solution->rarity += armor->rarity;
		solution->difficulty += armor->difficulty;
		AddAbilities( solution->abilities, armor->abilities, multiplier );
		solution->torso_multiplier += armor->torso_inc;
		solution->total_slots_spare += armor->num_slots;
	}
}

void GetInitialData( CalculationData^ data )
{
	data->solution->skill_plus2 = data->solution->charm_up = false;

	for each( Skill^ skill in data->query->skills )
	{
		if( !skill->is_taunt )
		{
			data->need.Add( skill->ability, skill->points_required );
			data->relevant.Add( skill, 0 );
		}

		if( skill->ability == SpecificAbility::skill_point_plus2 || skill->ability == SpecificAbility::charm_up )
			data->solution->charm_up = data->solution->skill_plus2 = true;
		
		data->solution->abilities.Add( skill->ability, 0 );
	}
	if( data->solution->charm )
	{
		for each( AbilityPair^ ap in data->solution->charm->abilities )
		{
			if( !data->need.ContainsKey( ap->ability ) )
				data->need.Add( ap->ability, 0 );
		}
	}

	data->solution->total_slots_spare = data->query->weapon_slots_allowed;
	data->solution->torso_multiplier = 1;
	data->solution->slots_spare = gcnew array< unsigned >( 4 );
	for( int i = 0; i < 4; ++i )
		data->solution->slots_spare[ i ] = 0;
	
	data->solution->slots_spare[ data->query->weapon_slots_allowed ] += !!data->query->weapon_slots_allowed;
	data->solution->torso_slots_spare = data->solution->armors[ 1 ] ? data->solution->armors[ 1 ]->num_slots : 0;
	data->solution->charm_slots_spare = data->solution->charm ? data->solution->charm->num_slots : 0;

	data->solution->defence = data->solution->max_defence = data->solution->fire_res = data->solution->ice_res = data->solution->water_res
		= data->solution->thunder_res = data->solution->dragon_res = data->solution->rarity = data->solution->difficulty = 0;

	AddInitialArmor( data->solution->armors[ 0 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 2 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 3 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 4 ], data->solution, 1 );
	AddInitialArmor( data->solution->armors[ 1 ], data->solution, data->solution->torso_multiplier );

	if( !data->solution->skill_plus2 && !data->solution->charm_up )
	{
		bool neset = true;
		for each( Armor^ armor in data->solution->armors )
		{
			if( !armor || !armor->charm_up )
			{
				neset = false;
				break;
			}
		}
		if( neset )
			data->solution->charm_up = data->solution->skill_plus2 = true;
	}

	if( data->solution->charm )
	{
		data->solution->total_slots_spare += data->solution->charm->num_slots;
		AddAbilities( data->solution->abilities, data->solution->charm->abilities, data->solution->charm_up ? 2 : 1 );
	}

	if( data->solution->skill_plus2 )
	{
		bool taunt_was_relevant = SpecificAbility::sense->relevant;
		if( data->query->want_taunt )
			SpecificAbility::sense->relevant = true;

		data->solution->AddAllSkillsPlus2();

		if( data->query->want_taunt )
			SpecificAbility::sense->relevant = taunt_was_relevant;
	}

	for each( Decoration^ deco in data->query->rel_decorations )
	{
		if( deco->abilities.Count > 1 &&
			deco->abilities[1]->amount < 0 &&
			deco->abilities[1]->ability->relevant /*&&
			data->solution->abilities[ deco->abilities[1]->ability ] >= data->need[ deco->abilities[1]->ability ]*/ )
			data->at_risk.Add( deco->abilities[1]->ability );
	}
}

void Solution::AddAllSkillsPlus2()
{
	List_t< Ability^ > positive_abilities;
	for each( Collections::Generic::KeyValuePair< Ability^, int >^ ap in abilities )
	{
		if( ap->Value != 0 || ap->Key->relevant )
			positive_abilities.Add( ap->Key );
	}

	for each( Ability^ ab in positive_abilities )
		abilities[ ab ] += 2;
}

ref class DecorationComparer : Collections::Generic::IComparer< Decoration^ >
{
	CalculationData^ data;
public:
	DecorationComparer( CalculationData^ data ) : data( data ) {}

	virtual int Compare( Decoration^ d1, Decoration^ d2 ) sealed
	{
		Ability^ a1 = d1->abilities[ 0 ]->ability;
		Ability^ a2 = d2->abilities[ 0 ]->ability;
		int need1 = data->need[ a1 ] - data->solution->abilities[ a1 ];
		int need2 = data->need[ a2 ] - data->solution->abilities[ a2 ];
		if( d1->abilities.Count > 1 && d1->abilities[1]->ability->relevant )
			need1 += 100;
		if( d2->abilities.Count > 1 && d2->abilities[1]->ability->relevant )
			need2 += 100;

		//sort decoration by most-needed first
		return need2 - need1;
	}
};

template< bool allow_bad, bool allow_overkill, bool allow_detrimental >
void AddDecorations1( CalculationData^ data, unsigned% num_slots, const int multiplier, List_t< Decoration^ >% deco_list )
{
	List_t< Decoration^ >^ rel_decorations = data->rel_decoration_map[ 1 ];
	if( num_slots == 0 || rel_decorations->Count == 0 )
		return;

	rel_decorations = gcnew List_t< Decoration^ >( data->rel_decoration_map[ 1 ] );
	DecorationComparer comp( data );
	rel_decorations->Sort( %comp );

	bool loop = true;
	int i = 0;
	while( loop )
	{
		Decoration^ decoration = rel_decorations[ i ];
		if( allow_detrimental || !Detrimental( decoration, data, multiplier ) )
		{
			AbilityPair^ apair = decoration->abilities[ 0 ];
			Assert( apair->amount > 0, L"Decoration's first skill is negative?" );

			if( allow_bad || decoration->abilities.Count < 2 || !data->solution->abilities.ContainsKey( decoration->abilities[ 1 ]->ability ) ||
				data->solution->abilities[ decoration->abilities[ 1 ]->ability ] + decoration->abilities[ 1 ]->amount * multiplier > -10 )
			{
				const int current_amount = data->solution->abilities[ apair->ability ];
				const int decoration_amount = apair->amount * multiplier;
				const int needed_amount = data->need[ apair->ability ];

				if( decoration_amount > 0 && current_amount < needed_amount && ( allow_overkill || current_amount + decoration_amount <= needed_amount + apair->amount - 1 ) )
				{
					loop = true;
					AddDecoration( data->solution, decoration, multiplier, deco_list );
					if( --num_slots == 0 )
						return;
					--i;
				}
			}
		}
		
		if( ++i == rel_decorations->Count )
		{
			loop = false;
			i = 0;
		}
	}
}

ref struct PossibleDecoration
{
	Decoration^ decoration;
	int points_given;
};

generic< typename T >
T GetLast( List_t< T >% l )
{
	return l[ l.Count - 1 ];
}

//pair( good, bad )
cliext::pair< int, int > GetScore( Decoration^ decoration, CalculationData^ data, const unsigned actual_good )
{
	static int point_score3[ 6 ] = { 0, 1, 2, 3, 5, 4 };
	int good = decoration->slots_required == 3 ? point_score3[ actual_good ] : actual_good;
	if( !decoration->abilities[0]->ability->has_1slot_decorations )
		good += 100;
	if( decoration->abilities.Count > 1 && decoration->abilities[1]->ability->relevant )
		good += 10;

	const int bad = decoration->abilities.Count == 1 ? 0 : ( data->solution->abilities.ContainsKey( GetLast( decoration->abilities )->ability ) ? data->solution->abilities[ GetLast( decoration->abilities )->ability ] : 0 );
	return cliext::make_pair( good , bad );
}

PossibleDecoration^ GetBestDecoration( List_t< PossibleDecoration^ >% decorations, CalculationData^ data )
{
	unsigned best = 0;
	cliext::pair< int, int > score = GetScore( decorations[ best ]->decoration, data, decorations[ best ]->points_given );
	for( int i = 1; i < decorations.Count; ++i )
	{
		cliext::pair< int, int > curr_score = GetScore( decorations[ i ]->decoration, data, decorations[ i ]->points_given );
		if( curr_score.first >= score.first || curr_score.first == score.first && curr_score.second > score.second )
		{
			score = curr_score;
			best = i;
		}
	}
	return decorations[ best ];
}

bool IsCharmAbility( Charm^ charm, Ability^ ab )
{
	return charm && charm->abilities.Count > 0 && charm->abilities[ 0 ]->ability == ab;
}

template< bool allow_bad, bool allow_detrimental >
void AddDecorations23( CalculationData^ data, const unsigned size, unsigned& num_slots, const int multiplier, List_t< Decoration^ >% deco_list )
{
	List_t< Decoration^ >^ rel_decorations = data->rel_decoration_map[ size ];
	if( num_slots == 0 || rel_decorations->Count == 0 ) return;
	bool loop = true;
	int i = 0;
	List_t< PossibleDecoration^ > possible_decorations;
	while( loop )
	{
		Decoration^ deco = rel_decorations[ i ];

		if( allow_detrimental || !Detrimental( deco, data, multiplier ) )
		{
			AbilityPair^ apair = deco->abilities[ 0 ];
			Assert( apair->amount > 0, L"Decoration's first skill is negative!?" );

			if( allow_bad || deco->abilities.Count < 2 || !data->solution->abilities.ContainsKey( deco->abilities[ 1 ]->ability ) ||
				data->solution->abilities[ deco->abilities[ 1 ]->ability ] + deco->abilities[ 1 ]->amount * multiplier > -10 )
			{
				const int current_amount = data->solution->abilities[ apair->ability ];
				const int decoration_amount = apair->amount * multiplier;
				const int needed_amount = data->need[ apair->ability ];

				if( decoration_amount > 0 && current_amount < needed_amount && current_amount + decoration_amount <= needed_amount + !apair->ability->has_1slot_decorations + Utility::Contains( %data->at_risk, deco->abilities[ 0 ]->ability ) )
				{
					loop = true;
					PossibleDecoration^ possible = gcnew PossibleDecoration;
					possible->decoration = deco;
					if( IsCharmAbility( data->solution->charm, apair->ability ) )
						possible->points_given = decoration_amount;
					else
						possible->points_given = Min( decoration_amount, needed_amount - current_amount );
					possible_decorations.Add( possible );
				}
			}
		}
		
		if( ++i == rel_decorations->Count )
		{
			loop = false;
			i = 0;
		}
	}
	if( possible_decorations.Count > 0 )
	{
		PossibleDecoration^ best = GetBestDecoration( possible_decorations, data );
		const bool good_deco = allow_bad || !Detrimental( best->decoration, data, multiplier );
		Assert( good_deco, L"Best decoration isn't good?" );
		AddDecoration( data->solution, best->decoration, multiplier, deco_list );
		if( --num_slots > 0 )
			AddDecorations23< allow_bad, allow_detrimental >( data, size, num_slots, multiplier, deco_list );
	}
}

template< bool allow_bad, bool allow_detrimental >
void CalculateDecorations( CalculationData^ data )
{
	unsigned multi_slots[ 4 ] = { 0, 0, 0, 0 }; // slots with a multiplier
	unsigned other_slots[ 4 ] = { 0, 0, 0, 0 }; // slots with no multiplier
	//get initial slot configuration and sort decorations by size
	for( int i = 1; i < 4; ++i )
	{
		other_slots[ i ] = data->solution->slots_spare[ i ];
	}

	//Currently Torso Up and Charm Up are mutually exclusive to keep things simpler

	unsigned% excluded_slots = data->solution->charm_up ? data->solution->torso_slots_spare : data->solution->charm_slots_spare;
	other_slots[ excluded_slots ] += !!excluded_slots;
	data->solution->slots_spare[ excluded_slots ] += !!excluded_slots;
	excluded_slots = 0;

	const int multiplier = data->solution->charm_up ? 2 : data->solution->torso_multiplier;
	unsigned% multi_slots_spare = data->solution->charm_up ? data->solution->charm_slots_spare : data->solution->torso_slots_spare;
	List_t< Decoration^ >% multi_decorations = data->solution->charm_up ? data->solution->charm_decorations : data->solution->body_decorations;
	
	multi_slots[ multi_slots_spare ] += !!multi_slots_spare;
	
	//try adding multi decorations first
	AddDecorations23< allow_bad, allow_detrimental >( data, 3, multi_slots[ 3 ], multiplier, multi_decorations );
	multi_slots[ 1 ] += multi_slots[ 3 ];
	multi_slots[ 2 ] += multi_slots[ 3 ];
	multi_slots[ 3 ] = 0;

	if( multiplier >= 3 )
	{
		AddDecorations23< allow_bad, allow_detrimental >( data, 2, multi_slots[ 2 ], multiplier, multi_decorations );
		multi_slots[ 1 ] += 2 * multi_slots[ 2 ];
		multi_slots[ 2 ] = 0;

		AddDecorations1< allow_bad, false, allow_detrimental >( data, multi_slots[ 1 ], multiplier, multi_decorations );
	}

	AddDecorations23< allow_bad, allow_detrimental >( data, 3, other_slots[ 3 ], 1, data->solution->non_body_decorations );
	data->solution->slots_spare[ 3 ] -= data->solution->slots_spare[ 3 ] - other_slots[ 3 ];
	other_slots[ 1 ] += other_slots[ 3 ];
	other_slots[ 2 ] += other_slots[ 3 ];
	other_slots[ 3 ] = 0;

	if( multiplier < 4 )
	{
		AddDecorations23< allow_bad, allow_detrimental >( data, 2, multi_slots[ 2 ], multiplier, multi_decorations );
		multi_slots[ 1 ] += 2 * multi_slots[ 2 ];
		multi_slots[ 2 ] = 0;

		if( multiplier == 3 )
		{
			AddDecorations1< allow_bad, false, allow_detrimental >( data, multi_slots[ 1 ], multiplier, multi_decorations );
		}
	}

	{
		const unsigned temp = other_slots[ 2 ];
		AddDecorations23< allow_bad, allow_detrimental >( data, 2, other_slots[ 2 ], 1, data->solution->non_body_decorations );
		const unsigned num_2slot_decorations = temp - other_slots[ 2 ];
		if( num_2slot_decorations > data->solution->slots_spare[ 2 ] )
		{
			const unsigned to_convert = num_2slot_decorations - data->solution->slots_spare[ 2 ];
			data->solution->slots_spare[ 3 ] -= to_convert;
			data->solution->slots_spare[ 2 ] = 0;
			data->solution->slots_spare[ 1 ] += to_convert;
		}
		else data->solution->slots_spare[ 2 ] -= num_2slot_decorations;
	}
	other_slots[ 1 ] += 2 * other_slots[ 2 ];
	other_slots[ 2 ] = 0;

	if( multiplier < 3 )
	{
		AddDecorations1< allow_bad, false, allow_detrimental >( data, multi_slots[ 1 ], multiplier, multi_decorations );
	}

	const unsigned temp = other_slots[ 1 ];
	AddDecorations1< allow_bad, true, allow_detrimental >( data, other_slots[ 1 ], 1, data->solution->non_body_decorations );
	unsigned num_1slot_decorations = temp - other_slots[ 1 ];
	if( num_1slot_decorations > data->solution->slots_spare[ 1 ] )
	{
		unsigned extra = num_1slot_decorations - data->solution->slots_spare[ 1 ];
		if( extra > data->solution->slots_spare[ 2 ] * 2 )
		{
			const unsigned extra3 = extra - data->solution->slots_spare[ 2 ] * 2;
			const unsigned num_3_convert = extra3 / 3 + !!( extra3 % 3 );
			data->solution->slots_spare[ 3 ] -= num_3_convert;
			if( num_3_convert * 3 > extra )
			{
				const unsigned super_extra = num_3_convert * 3 - extra;
				data->solution->slots_spare[ 2 ] += super_extra / 2;
				data->solution->slots_spare[ 1 ] += super_extra % 2;
				num_1slot_decorations -= extra;
				extra = 0;
			}
			else
			{
				extra -= num_3_convert * 3;
				num_1slot_decorations -= num_3_convert * 3;
			}
		}
		const unsigned num_2_slots = extra / 2 + extra % 2;
		data->solution->slots_spare[ 2 ] -= num_2_slots;
		data->solution->slots_spare[ 1 ] -= num_1slot_decorations - num_2_slots * 2;
	}
	else data->solution->slots_spare[ 1 ] -= num_1slot_decorations;
	
	if( multi_slots[ 1 ] )
		AddDecorations1< allow_bad, true, allow_detrimental >( data, multi_slots[ 1 ], multiplier, multi_decorations );

	multi_slots_spare = multi_slots[ 1 ];
}

unsigned Num1SlotDecorations( Solution^ sol, Ability^ ab )
{
	unsigned num = 0;
	for each( Decoration^ deco in sol->non_body_decorations )
	{
		if( deco->slots_required == 1 && deco->abilities[ 0 ]->ability == ab )
			num++;
	}
	if( sol->torso_multiplier == 1 )
	{
		for each( Decoration^ deco in sol->body_decorations )
		{
			if( deco->slots_required == 1 && deco->abilities[ 0 ]->ability == ab )
				num++;
		}
	}
	if( !sol->charm_up )
	{
		for each( Decoration^ deco in sol->charm_decorations )
		{
			if( deco->slots_required == 1 && deco->abilities[ 0 ]->ability == ab )
				num++;
		}
	}
	return num;
}

int PotentialSpareSlots( CalculationData^ data, const unsigned slot_size )
{
	int count = 0;
	for each( Armor^ arm in data->solution->armors )
	{
		if( arm )
			count += arm->num_slots == slot_size;
	}
	count += data->query->weapon_slots_allowed == slot_size;
	count += data->solution->charm->num_slots == slot_size;
	for each( Decoration^ deco in data->solution->decorations )
	{
		count -= deco->slots_required == slot_size;
	}
	return count;
}

bool RoomFor3Slot( CalculationData^ data )
{
	const int free3 = PotentialSpareSlots( data, 3 );
	Assert( free3 >= 0, L"Not enough room for 3-slot gem(1)" );
	return free3 > 0 && free3 + PotentialSpareSlots( data, 2 ) > 0;
}

bool RoomFor2Slot( CalculationData^ data )
{
	const int free3 = PotentialSpareSlots( data, 3 );
	Assert( free3 >= 0, L"Not enough room for 3-slot gem(2)" );
	return free3 + PotentialSpareSlots( data, 2 ) > 0;
}

void SwapOut( Solution^ sol, Decoration^ deco )
{
	unsigned count = 0;
	for( int i = 0; count < deco->slots_required; ++i )
	{
		Assert( i < sol->decorations.Count, L"Overrunning decoration count" );
		if( sol->decorations[ i ]->slots_required == 1 && sol->decorations[ i ]->abilities[ 0 ]->ability == deco->abilities[ 0 ]->ability )
		{
			for each( AbilityPair^ ap in sol->decorations[ i ]->abilities )
			{
				sol->abilities[ ap->ability ] -= ap->amount; 
			}
			sol->decorations.RemoveAt( i-- );
			count++;
		}
	}
	for each( AbilityPair^ ap in deco->abilities )
	{
		sol->abilities[ ap->ability ] += ap->amount; 
	}
	sol->decorations.Add( deco );
}

void Solution::ReduceSlots()
{
	if( total_slots_spare > 0 && !charm_up )
	{
		unsigned i = charm->num_slots;
		while( i <= 3 && !slots_spare[ i ] ) ++i;
		if( i <= 3 )
		{
			//if( i > charm->num_slots )
			Assert( slots_spare[ i ] > 0, L"Not enough slots spare" );

			slots_spare[ i - charm->num_slots ]++;
			slots_spare[ i ]--;
			total_slots_spare -= charm->num_slots;
			charm->num_slots = 0;
		}
	}
}

void Solution::ReduceSkills()
{
	if( total_slots_spare > 0 )
	{
		const int multiplier = charm_up ? 2 : 1;
		for each( AbilityPair^ ap in charm->abilities )
		{
			if( !charm_up || ap->ability->efficient )
				data->need[ ap->ability ] += ap->amount * multiplier;
		}
		CalculateDecorations< false, false >( data );
		for each( AbilityPair^ ap in charm->abilities )
		{
			if( !charm_up || ap->ability->efficient )
				data->need[ ap->ability ] -= ap->amount * multiplier;
		}
	}
}

void Solution::RearrangeDecorations()
{
	for each( AbilityPair^ ap in charm->abilities )
	{
		unsigned num1 = Num1SlotDecorations( this, ap->ability );
		Decoration^ best3 = Decoration::GetBestDecoration( ap->ability, 3, data->rel_decoration_map );
		Decoration^ best2 = Decoration::GetBestDecoration( ap->ability, 2, data->rel_decoration_map );
		bool replaced = false;
		do 
		{
			replaced = false;
			if( num1 >= 3 )
			{
				if( best3 && best3->slots_required == 3 && RoomFor3Slot( data ) )
				{
					replaced = true;
					SwapOut( this, best3 );
					num1 -= 3;
				}
				else if( best2 && best2->slots_required == 2 && RoomFor2Slot( data ) )
				{
					replaced = true;
					SwapOut( this, best2 );
					num1 -= 2;
				}
			}
			else if( num1 == 2 )
			{
				if( best2 && best2->slots_required == 2 && RoomFor2Slot( data ) )
				{
					replaced = true;
					SwapOut( this, best2 );
					num1 -= 2;
				}
			}
		}
		while ( replaced );
	}
}

void Solution::ReduceCharm()
{
	const int charm_mult = charm_up ? 2 : 1;
	for each( AbilityPair^ ap in charm->abilities )
	{
		const int need = data->need[ ap->ability ];
		if( need > 0 )
		{
			const int have = abilities[ ap->ability ];
			const int extra = ( have - need ) / charm_mult;
			Assert( extra >= 0, L"Reducing charm has caused solution to fail!" );
			const int take_off = extra >= ap->amount ? ap->amount : extra;
			ap->amount -= take_off;
			abilities[ ap->ability ] -= take_off * charm_mult;
		}
	}

	for( int i = 0; i < charm->abilities.Count; ++i )
	{
		if( charm->abilities[ i ]->amount == 0 )
		{
			charm->abilities.RemoveAt( i-- );
		}
	}
	if( charm->num_slots == 0 && charm->abilities.Count == 0 )
		charm = nullptr;
}

void Solution::AddExtraSkills()
{
	if( total_slots_spare == 0 )
		return;
	
	//first try to upgrade skills

	for each( Collections::Generic::KeyValuePair< Ability^, int >^ ap in abilities )
	{
		Ability^ ab = ap->Key;
		if( !ab->has_decorations )
			continue;
		
		int val = ap->Value, bad = 0;
		Skill^ current_skill = ab->GetSkill( val );
		if( current_skill && current_skill->best )
			continue;

		array< Decoration^ >^ decs =
		{
			nullptr,
			Decoration::GetBestDecoration( ab, 1, data->query->hr, data->query->elder_star ),
			Decoration::GetBestDecoration( ab, 2, data->query->hr, data->query->elder_star ),
			Decoration::GetBestDecoration( ab, 3, data->query->hr, data->query->elder_star )
		};

		//for now Torso Up and Charm Up are mutually exclusive to make things simpler

		const int multiplier = charm_up ? 2 : torso_multiplier;
		const unsigned multi_slots_spare = charm_up ? charm_slots_spare : torso_slots_spare;

		List_t< Decoration^ > multi_deco;
		for( unsigned multi_spare = multi_slots_spare; multi_spare > 0; )
		{
			Decoration^ d = decs[multi_spare];
			if( !d || Detrimental( d, data, multiplier ) )
				break;
			
			if( d->abilities.Count > 1 && d->abilities[1]->ability->has_bad )
			{
				if( bad + d->abilities[1]->amount * multiplier <= -10 )
					break;
				bad += d->abilities[1]->amount * multiplier;
			}

			multi_deco.Add( d );
			val += d->abilities[0]->amount * multiplier;
			multi_spare -= d->slots_required;
		}

		List_t< Decoration^ > non_body_deco;
		unsigned spare[4] = { 0, slots_spare[1], slots_spare[2], slots_spare[3] };
		for( unsigned size = 4; size --> 1; )
		{
			Assert( (int)slots_spare[ size ] >= 0, L"Negative slots spare?" );
			if( (int)slots_spare[ size ] < 0 )
				continue;

			for( unsigned i = 0; i < slots_spare[size]; ++i )
			{
				Assert( spare[ size] > 0, L"Not enough slots spare" );
				spare[size]--;
				unsigned s = size;
				while( s > 0 )
				{
					Decoration^ d = decs[s];
					if( !d || Detrimental( d, data, 1 ) )
						break;

					if( d->abilities.Count > 1 && d->abilities[1]->ability->has_bad )
					{
						if( bad + d->abilities[1]->amount <= -10 )
							break;
						bad += d->abilities[1]->amount;
					}

					non_body_deco.Add( d );
					val += d->abilities[0]->amount;
					s -= d->slots_required;
				}
				spare[s]++;
			}
			if( size == 3 && spare[3] )
			{
				spare[1] += spare[3];
				spare[2] += spare[3];
				spare[3] = 0;
			}
			if( size == 2 && spare[2] )
			{
				spare[1] += spare[2] * 2;
				spare[2] = 0;
			}
		}

		Skill^ skill = ab->GetSkill( val );
		if( !skill || skill == current_skill )
			continue;

		potential_extra_skills.Add( skill );
	}
}

bool Solution::ImpossibleNoDecorations( Query^ query )
{
	for each( Skill^ skill in query->skills )
	{
		if( skill->is_taunt || skill->ability == SpecificAbility::charm_up )
			continue;

		const int have = abilities[ skill->ability ],
				  need = data->need[ skill->ability ];
		if( have < need )
		{
			if( !skill->ability->has_decorations )
				return true;

			bool found = false;
			for each( Decoration^ deco in query->rel_decorations )
			{
				if( deco->abilities.Count > 0 && deco->abilities[0]->ability == skill->ability )
				{
					found = true;
					break;
				}
			}
			if( !found )
				return true;
		}
	}
	return false;
}

bool CalculateTaunt( CalculationData^ data )
{
	const int have = data->solution->abilities[ SpecificAbility::sense ];
	if( have <= -10 )
		return true;

	if( data->query->taunt_decorations.Count == 0 )
		return false;

	const int effective_total =
		data->solution->torso_slots_spare * data->solution->torso_multiplier +
		data->solution->charm_slots_spare * ( data->solution->charm_up ? 1 : 0 ) +
		( data->solution->total_slots_spare - data->solution->torso_slots_spare );
	int need = have + 10;
	if( need > effective_total )
		return false;

	Decoration^ best = nullptr;
	int best_dist = 999;
	for each( Decoration^ d in data->query->taunt_decorations )
	{
		Assert( d->abilities.Count == 2, L"Bad taunt decoration!?" );
		const int have = data->solution->abilities.ContainsKey( d->abilities[0]->ability ) ? data->solution->abilities[ d->abilities[0]->ability ] : 0;
		const int dist = ( have >= 10 ) ? 15 - have : 10 - have;
		if( dist <= 0 )
		{
			if( !best )
				best = d;
		}
		else
		{
			if( dist < best_dist )
			{
				best_dist = dist;
				best = d;
			}
		}
	}

	while( data->solution->torso_slots_spare > 0 && need > 0 )
	{
		AddDecoration( data->solution, best, data->solution->torso_multiplier, data->solution->body_decorations );
		need -= data->solution->torso_multiplier;
		data->solution->torso_slots_spare--;
	}

	while( data->solution->charm_slots_spare > 0 && need > 0 )
	{
		AddDecoration( data->solution, best, data->solution->torso_multiplier, data->solution->charm_decorations );
		need -= data->solution->charm_up ? 2 : 1;
		data->solution->charm_slots_spare--;
	}

	while( data->solution->total_slots_spare > 0 && need > 0 )
	{
		AddDecoration( data->solution, best, 1, data->solution->non_body_decorations );
		need--;
		if( data->solution->slots_spare[1] == 0 )
		{
			if( data->solution->slots_spare[2] == 0 )
			{
				Assert( data->solution->slots_spare[ 3 ] > 0, L"Not enough slots spare" );
				data->solution->slots_spare[3]--;
				data->solution->slots_spare[2]++;
			}
			else
			{
				Assert( data->solution->slots_spare[ 2 ] > 0, L"Not enough slots spare" );
				data->solution->slots_spare[2]--;
				data->solution->slots_spare[1]++;
			}
		}
		else
		{
			Assert( data->solution->slots_spare[ 1 ] > 0, L"Not enough slots spare" );
			data->solution->slots_spare[ 1 ]--;
		}
	}
	
	return true;
}

bool Solution::MatchesQuery( Query^ query )
{
	data = gcnew CalculationData;
	data->query = query;
	data->solution = this;

	for( int i = 0; i < 4; ++i )
		data->rel_decoration_map.Add( gcnew List_t< Decoration^ > );
	for each( Decoration^ deco in query->rel_decorations )
	{
		data->rel_decoration_map[ deco->slots_required ]->Add( deco );
	}
	
	GetInitialData( data );
	if( ImpossibleNoDecorations( query ) )
		return false;

	CalculateDecorations< true, true >( data );
	
	for each( Skill^ skill in query->skills )
	{
		if( !skill->is_taunt )
		{
			const int have = abilities[ skill->ability ],
					  need = data->need[ skill->ability ];
			if( have < need )
				return false;
		}
	}
	if( query->want_taunt && !CalculateTaunt( data ) )
		return false;

	CalculateExtraSkills( query );

	reordered_to_fix = false;
	if( !query->allow_bad && !CheckBadSkills( query ) )
		return false;
	
	//reduce charms
	if( charm )
	{
		if( charm->custom )
		{
			if( !reordered_to_fix )
				RearrangeDecorations();
		}
		else
		{
			ReduceSlots();
			ReduceSkills();

			RearrangeDecorations();

			ReduceCharm();

			CalculateExtraSkills( query );
			Assert( query->allow_bad || bad_skills.Count == 0, L"A bad skill crept in?" );

#ifdef _DEBUG
			for each( Skill^ skill in query->skills )
			{
				if( skill->is_taunt )
				{
					const int have = abilities[ skill->ability ];
					if( have > -10 )
						return false;
				}
				else
				{
					const int have = abilities[ skill->ability ],
							  need = data->need[ skill->ability ];
					if( have < need )
					{
						System::Windows::Forms::MessageBox::Show( L"Not enough skill points!!!" );
					}
				}
			}
#endif
		}
	}

	if( query->add_extra )
		AddExtraSkills();

	CalculateFamilyScore();
	CalculateExtraSkillScore( query->hr, query->elder_star );
	CalculateSkillModifiers();
	
	if( charm_up )
		slots_spare[ charm_slots_spare ]++;
	else
		slots_spare[ torso_slots_spare ]++;
	Assert( total_slots_spare < 1000, L"Total slots spare is negative" );
	for each( Decoration^ deco in decorations )
	{
		difficulty += deco->difficulty;
	}
	return true;
}

void Solution::CalculateExtraSkills( Query^ query )
{
	extra_skills.Clear();
	bad_skills.Clear();
	for each( Collections::Generic::KeyValuePair< Ability^, int >^ ap in abilities )
	{
		Ability^ ab = ap->Key;
		if( query->want_taunt && ab == SpecificAbility::sense )
			continue;

		Skill^ sk = ab->GetSkill( ap->Value );
		if( sk && !sk->autodisabled && !data->relevant.ContainsKey( sk ) )
		{
			if( ap->Value < 0 && !data->query->allow_bad )
				bad_skills.Add( sk );
			else if( ab != SpecificAbility::torso_inc )
				extra_skills.Add( sk );
		}
	}
}

void Solution::CalculateData( const unsigned hr, const unsigned elder_star )
{
	abilities.Clear();
	fire_res = ice_res = thunder_res = water_res = dragon_res = defence = rarity = difficulty = max_defence = 0;
	torso_multiplier = 1;
	charm_up = skill_plus2 = true;
	for each( Armor^ armor in armors )
	{
		if( !armor )
		{
			charm_up = skill_plus2 = false;
			continue;
		}
		
		fire_res += armor->fire_res;
		ice_res += armor->ice_res;
		thunder_res += armor->thunder_res;
		water_res += armor->water_res;
		dragon_res += armor->dragon_res;
		defence += armor->defence;
		if( armor->rarity != 99 )
			rarity += armor->rarity;
		difficulty += armor->difficulty;
		max_defence += armor->max_defence;

		if( !armor->charm_up )
			charm_up = skill_plus2 = false;
	
		if( armor->torso_inc )
			torso_multiplier++;
		else Utility::AddAbilitiesToMap( armor->abilities, abilities );
	}
	if( armors[ 1 ] && torso_multiplier > 1 )
		Utility::AddAbilitiesToMap( armors[ 1 ]->abilities, abilities, torso_multiplier - 1 );

	if( charm )
		Utility::AddAbilitiesToMap( charm->abilities, abilities, charm_up ? 2 : 1 );

	for each( Decoration^ dec in non_body_decorations )
		Utility::AddAbilitiesToMap( dec->abilities, abilities );

	for each( Decoration^ dec in body_decorations )
		Utility::AddAbilitiesToMap( dec->abilities, abilities, torso_multiplier );

	for each( Decoration^ dec in charm_decorations )
		Utility::AddAbilitiesToMap( dec->abilities, abilities, charm_up ? 2 : 1 );

	if( skill_plus2 )
	{
		AddAllSkillsPlus2();
	}

	CalculateFamilyScore();
	CalculateExtraSkillScore( hr, elder_star );
	CalculateSkillModifiers();
}

void Solution::CalculateSkillModifiers()
{
#define CHECK_RES(X) else if( e.Current.Key == SpecificAbility::X ) \
	{ \
		if( e.Current.Value >= 15 ) \
			X += 20; \
		else if( e.Current.Value >= 10 ) \
			X += 15; \
		else if( e.Current.Value <= -10 ) \
			X -= 20; \
	}

	AbilityMap::Enumerator e = abilities.GetEnumerator();
	while( e.MoveNext() )
	{
		if( e.Current.Key == SpecificAbility::defence )
		{
			if( e.Current.Value >= 25 )
				defence = int( defence * 1.08 ) + 30; //DUXL

			else if( e.Current.Value >= 20 )
				defence = int( defence * 1.06 ) + 25; //DUL

			else if( e.Current.Value >= 15 )
				defence = int( defence * 1.03 ) + 20; //DUM

			else if( e.Current.Value >= 10 )
				defence = defence + 15;        //DUS

			else if( e.Current.Value <= -20 )
				defence = int( defence * 0.9 ) - 10;  //DDL

			else if( e.Current.Value <= -15 )
				defence = int( defence * 0.95 ) - 10; //DDM

			else if( e.Current.Value <= -10 )
				defence = defence - 10;        //DDS

			if( defence < 1 )
				defence = 1;
		}
		CHECK_RES( fire_res )
		CHECK_RES( water_res )
		CHECK_RES( thunder_res )
		CHECK_RES( ice_res )
		CHECK_RES( dragon_res )
	}
}

unsigned GetFamilyScore( List_t< Armor^ >% armors, const unsigned family )
{
	array< unsigned >^ rare_count = gcnew array< unsigned >( 11 );
	for each( Armor^ armor in armors )
	{
		if( armor && armor->family == family && (int)armor->rarity < rare_count->Length )
			rare_count[ armor->rarity ]++;
	}
	unsigned res = 0;
	for( int i = 0; i < rare_count->Length; ++i )
		if( rare_count[ i ] > res )
			res = rare_count[ i ];
	return res;
}

void Solution::CalculateFamilyScore()
{
	family_score = gcnew array< unsigned >( 3 );
	array< unsigned >^ count = gcnew array< unsigned >( Family::count );
	for each( Armor^ armor in armors )
	{
		if( armor )
			count[ armor->family ]++;
	}
	array< unsigned >^ count2 = gcnew array< unsigned >( 6 );
	for( int i = 0; i < count2->Length; ++i ) count2[ i ] = Family::count;
	for( int i = 0; i < count->Length; ++i )
		if( count[ i ] > 0 )
			count2[ count[ i ] ] = i;
	array< unsigned >::Sort( count );

	switch( count[ Family::count - 1 ] )
	{
	case 2:
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 2 ] );
		{
			const unsigned max_val = -1;
			array< unsigned >^ other_armor_families = { max_val, max_val, max_val };
			int i = 0;
			for each( Armor^ armor in armors )
			{
				if( armor && armor->family != count2[ 2 ] )
					other_armor_families[ i++ ] = armor->family;
			}
			if( other_armor_families[ 0 ] == other_armor_families[ 1 ] )
			{
				family_score[ 0 ] = 2;
				family_score[ 2 ] = GetFamilyScore( armors, other_armor_families[ 0 ] );
			}
			else if( other_armor_families[ 0 ] == other_armor_families[ 2 ] )
			{
				family_score[ 0 ] = 2;
				family_score[ 2 ] = GetFamilyScore( armors, other_armor_families[ 0 ] );
			}
			else if( other_armor_families[ 1 ] == other_armor_families[ 2 ] )
			{
				family_score[ 0 ] = 2;
				family_score[ 2 ] = GetFamilyScore( armors, other_armor_families[ 1 ] );
			}
			else family_score[ 0 ] = 1;
		}
		break;
	case 3:
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 3 ] );
		if( count2[ 2 ] != Family::count )
		{
			family_score[ 0 ] = 4;
			family_score[ 2 ] = GetFamilyScore( armors, count2[ 2 ] );
		}
		else
		{
			family_score[ 0 ] = 3;
		}
		break;
	case 4:
		family_score[ 0 ] = 5;
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 4 ] );
		break;
	case 5:
		family_score[ 0 ] = 6;
		family_score[ 1 ] = GetFamilyScore( armors, count2[ 5 ] );
		break;
	}
}

int CalculateBonusExtraSkillPoints( Ability^ ability, int spare_slots, const int mult, const unsigned hr, const unsigned elder_star )
{
	int total = 0;
	while( spare_slots > 0 )
	{
		Decoration^ deco = Decoration::GetBestDecoration( ability, spare_slots, hr, elder_star );
		if( deco == nullptr )
			break;
		total += deco->abilities [ 0 ]->amount * mult;
		spare_slots -= deco->slots_required;
	}
	return total;
}

void Solution::CalculateExtraSkillScore( const unsigned hr, const unsigned elder_star )
{
	/////////////////////// 3  4   5   6    7     8      9      10+
	const int weights[] = { 1, 5, 20, 50, 100, 1000, 10000, 100000 };
	extra_skill_score = 0;

	AbilityMap::Enumerator e = abilities.GetEnumerator();
	while( e.MoveNext() )
	{
		if( e.Current.Value < 10 &&
			e.Current.Value > 3 )
		{
			Skill^ s = e.Current.Key->GetWorstGoodSkill();
			if( !s || s->autodisabled )
				continue;

			const int effective_skill_points = e.Current.Value
				+ CalculateBonusExtraSkillPoints( e.Current.Key, torso_slots_spare, torso_multiplier, hr, elder_star )
				+ CalculateBonusExtraSkillPoints( e.Current.Key, slots_spare[ 3 ] * 3, 1, hr, elder_star )
				+ CalculateBonusExtraSkillPoints( e.Current.Key, slots_spare[ 2 ] * 2, 1, hr, elder_star )
				+ CalculateBonusExtraSkillPoints( e.Current.Key, slots_spare[ 1 ], 1, hr, elder_star );

			extra_skill_score += weights[ Math::Min( effective_skill_points - 3, 10 ) ];
		}
	}

	extra_skill_score += potential_extra_skills.Count * weights[10 - 3];
}
