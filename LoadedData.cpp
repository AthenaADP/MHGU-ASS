#include "stdafx.h"

#include "Skill.h"
#include "Common.h"
#include "LoadedData.h"
#include "CharmDatabase.h"

using namespace System;

Skill^ LoadedData::FindSkill( const unsigned skill )
{
	Assert( int( skill ) < Skill::static_skills.Count, L"Skill index out of range" );
	return Skill::static_skills[ skill ];
}

SkillTag^ SkillTag::FindTag( String^ tag )
{
	for each( SkillTag^ st in tags )
	{
		if( st->name == tag )
			return st;
	}
	return nullptr;
}

void DumpFamilies( String^ filename )
{
	Map_t< int, List_t< Armor^ >^ > fammap;

	for each( List_t< Armor^ >^ l in Armor::static_armors )
	{
		for each( Armor^ a in l )
		{
			if( !fammap.ContainsKey( a->family ) )
				fammap.Add( a->family, gcnew List_t< Armor^ >() );
			
			fammap[ a->family ]->Add( a );
		}
	}

	IO::StreamWriter fout( filename );
	Map_t< int, List_t< Armor^ >^ >::Enumerator e = fammap.GetEnumerator();
	int last_family = -1;
	while( e.MoveNext() )
	{
		if( e.Current.Key != last_family )
		{
			last_family = e.Current.Key;
			fout.WriteLine( L"\nFamily " + last_family  );
		}
		for each( Armor^ a in e.Current.Value )
		{
			fout.WriteLine( a->name );
		}
	}
}

void DumpSkills( String^ filename )
{
	IO::StreamWriter fout( filename );

	for each( Ability^ ab in Ability::static_abilities )
	{
		fout.WriteLine( ab->jap_name );
	}

	fout.WriteLine();

	for each( Skill^ sk in Skill::static_skills )
	{
		fout.WriteLine( sk->name );
	}
}

void LoadedData::LoadDataFiles()
{
	Armor::static_armors = gcnew array< List_t< Armor^ >^ >( int( Armor::ArmorType::NumArmorTypes ) );
	Armor::charm_up_armors_b = gcnew array< Armor^ >( int( Armor::ArmorType::NumArmorTypes ) );
	Armor::charm_up_armors_g = gcnew array< Armor^ >( int( Armor::ArmorType::NumArmorTypes ) );
	for( int i = 0; i < Armor::static_armors->Length; ++i )
		Armor::static_armors[ i ] = gcnew List_t< Armor^ >;

	Material::LoadMaterials( L"Data/components.txt" );

	SkillTag::Load( L"Data/tags.txt" );
	Skill::Load( L"Data/skills.txt" );
	Skill::LoadCompound( L"Data/compound_skills.txt" );

	//DumpSkills( L"Data/Languages/Japanese/skills.txt" );

	Skill::LoadSaveData( L"Data/savedata_format.txt");
	Armor::Load( L"Data/head.txt",  Armor::ArmorType::HEAD );
	Armor::Load( L"Data/body.txt",  Armor::ArmorType::BODY );
	Armor::Load( L"Data/arms.txt",  Armor::ArmorType::ARMS );
	Armor::Load( L"Data/waist.txt", Armor::ArmorType::WAIST );
	Armor::Load( L"Data/legs.txt",  Armor::ArmorType::LEGS );
	Decoration::Load( L"Data/decorations.txt" );

	//DumpFamilies( L"Data/families.txt" );
	
}

void AddToList( List_t< Decoration^ >^ list, Decoration^ item, List_t< Ability^ >^ rel_abilities, List_t< Decoration^ >^ inf_decs, const bool adv, const bool want_taunt )
{
	if( adv && item->force_disable )
		return;
	const bool may_remove_self = !adv || !item->force_enable;
	for( int i = 0; i < list->Count; ++i )
	{
		if( item->IsBetterThan( list[ i ], rel_abilities ) )
		{
			const bool may_remove = !adv || !list[ i ]->force_enable;
			if( may_remove && !list[ i ]->IsBetterThan( item, rel_abilities ) )
				list->Remove( list[ i-- ] );
		}
		else if( may_remove_self && list[ i ]->IsBetterThan( item, rel_abilities ) )
				return;
	}
	list->Add( item );
	inf_decs->Add( item );
}

void AddToList( List_t< Armor^ >^ list, Armor^ armor, List_t< Ability^ >^ rel_abilities, List_t< Armor^ >^ inf_armor, const bool adv, const bool want_taunt )
{
	if( adv && armor->force_disable )
		return;

	const bool may_remove_self = !adv || !armor->force_enable;
	for( int i = 0; i < list->Count; ++i )
	{
		if( armor->IsBetterThan( list[ i ], rel_abilities, want_taunt) )
		{
			const bool may_remove = !adv || !list[ i ]->force_enable;
			if( may_remove && ( !armor->danger || armor->danger == list[ i ]->danger ) && !list[ i ]->IsBetterThan( armor, rel_abilities, want_taunt ) )
			{
				list->Remove( list[ i-- ] );
			}
		}
		else if( may_remove_self && ( !list[ i ]->danger || list[ i ]->danger == armor->danger ) && list[ i ]->IsBetterThan( armor, rel_abilities, want_taunt ) )
		{
			if( inf_armor )
			{
				inf_armor->Add( armor );
			}
			return;
		}
	}
	list->Add( armor );
	if( inf_armor )
		inf_armor->Add( armor );
}

void GetRelevantArmors( Query^ query, List_t< Armor^ >^ rel_armor, List_t< Armor^ >^ list, List_t< Armor^ >^ inf_armor, List_t< Ability^ >^ danger_skills, unsigned % max_slots )
{
	bool need_slot_only = true;
	for each( Armor^ armor in list )
	{
		if( armor->MatchesQuery( query, danger_skills, max_slots ) )
		{
			if( armor->num_slots > max_slots )
			{
				max_slots = armor->num_slots;
				need_slot_only = armor->no_skills;
			}
			else if( armor->num_slots == max_slots && !armor->no_skills )
				need_slot_only = false;

			AddToList( rel_armor, armor, %query->rel_abilities, inf_armor, false, query->want_taunt );
		}
	}

	for( int i = 0; i < inf_armor->Count; ++i )
	{
		Armor^ a = inf_armor[ i ];
		if( a->no_skills )
		{
			if( a->num_slots < max_slots || 
				a->num_slots == max_slots && !need_slot_only )
				inf_armor->RemoveAt( i-- );
		}
	}
}

void GetRelevantDecorations( Query^ query )
{
	for each( Skill^ skill in query->skills )
	{
		if( !Decoration::static_decoration_ability_map.ContainsKey( skill->ability ) ||
			skill->is_taunt )
			continue;

		for each( Decoration^ decoration in Decoration::static_decoration_ability_map[ skill->ability ] )
		{
			if( decoration->MatchesQuery( query ) )
			{
				AddToList( %query->rel_decorations, decoration, %query->rel_abilities, %query->inf_decorations, false, query->want_taunt );
			}
		}
	}

	if( query->want_taunt )
	{
		for each( Decoration^ decoration in Decoration::static_decorations )
		{
			if( decoration->slots_required == 1 &&
				decoration->abilities.Count == 2 &&
				decoration->abilities[1]->ability == SpecificAbility::sense &&
				decoration->abilities[1]->amount < 0 )
			{
				if( decoration->hr > query->hr && decoration->elder_star > query->elder_star ||
					decoration->is_event && !query->allow_event )
					continue;

				query->taunt_decorations.Add( decoration );
			}				
		}
	}

	//for each( Decoration^ decoration in query->rel_decorations )
	//	decoration->dangerous = decoration->abilities.Count == 2 && Utility::Contains( %query->rel_abilities, decoration->abilities[ 1 ]->ability );
}

int NeededPoints( List_t< Skill^ >^ skills, Ability^ ability )
{
	for each( Skill^ skill in skills )
	{
		if( skill->ability == ability )
			return skill->points_required;
	}
	return 0;
}

void LoadedData::GetRelevantData( Query^ query )
{
	for each( Ability^ ab in Ability::static_abilities )
	{
		ab->relevant = false;
	}
	for each( Skill^ skill in query->skills )
	{
		skill->autodisabled = false;
	}
	List_t< Ability^ > danger_skills;
	//get relevant abilities
	for each( Skill^ skill in query->skills )
	{
		if( !skill->is_taunt )
		{
			query->rel_abilities.Add( skill->ability );
			skill->ability->relevant = true;
		}

		if( Skill::compound_skill_map.ContainsKey( skill ) )
		{
			for each( Skill^ s in Skill::compound_skill_map[ skill ] )
			{
				s->autodisabled = true;
			}
		}
	}
	query->rel_abilities.TrimExcess();

	//get relevant decorations
	Decoration::RecheckEfficientDecorations( query );
	GetRelevantDecorations( query );
	query->rel_decorations.TrimExcess();

	//get danger skills
	/*if( query->danger )
		for each( Decoration^ decoration in query->rel_decorations )
		{
			Ability^ bad_ability = nullptr;
			bool points20 = false;
			for each( AbilityPair^ apair in decoration->abilities )
			{
				if( apair->amount < 0 )
				{
					if( !Utility::Contains( %danger_skills, apair->ability ) )
						bad_ability = apair->ability;
				}
				else if( NeededPoints( %query->skills, apair->ability ) == 20 )
					points20 = true;
			}
			if( points20 && bad_ability )
				danger_skills.Add( bad_ability );
		}*/

	//get relevant armors
	for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
	{
		unsigned max_slots = 0;
		GetRelevantArmors( query, query->rel_armor[ i ], Armor::static_armors[ i ], query->inf_armor[ i ], %danger_skills, max_slots );
		
		query->rel_armor[ i ]->TrimExcess();
	}
	query->rel_armor.TrimExcess();
}

void SkillTag::Load( System::String^ filename )
{
	tags.Clear();
	tags.Add( gcnew SkillTag( L"All" ) );
	tags.Add( gcnew SkillTag( L"Misc" ) );
	tags.Add( gcnew SkillTag( L"Related" ) );
	IO::StreamReader fin( filename );
	while( !fin.EndOfStream )
	{
		String^ tag = fin.ReadLine();
		if( tag != L"" )
			tags.Add( gcnew SkillTag( tag ) );
	}
}

void SkillTag::LoadLanguage( String^ filename )
{
	IO::StreamReader fin( filename );
	for( int i = 0; i < tags.Count; )
	{
		String^ line = fin.ReadLine();
		if( line == L"" || line[ 0 ] == L'#' )
			continue;
		tags[ i ]->name = line;
		i++;
	}
}

SkillTag::SkillTag( System::String^ s )
	: name( s ), disable_g( s == "Blademaster" ), disable_b( s == "Bow/Gunner" )
{

}
