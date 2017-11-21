#include "stdafx.h"
#include "SaveData.h"

using namespace System;

//Convert 4 bytes into 32-bit unsigned
unsigned ExtractUnsigned( array< Byte >^ data, const unsigned offset )
{
	unsigned result = data[ offset ];
	for( unsigned i = 1; i < 4; ++i )
		result += ((unsigned)(data[offset+i])) << (i * 8);
	return result;
}

//Convert 36 bytes into charm
Charm^ ExtractCharm( array< Byte >^ data, const unsigned offset )
{
	//Format (36 bytes):
	//Byte 0: Equipment type (6=charm)
	//Byte 1: Rarity
	//...
	//Byte 12: Skill 1
	//Byte 13: Skill 2
	//Byte 14: Points 1
	//Byte 15: Points 2
	//Byte 16: Num slots

	if( data[ offset ] == 0x6 )
	{
		Charm^ c = gcnew Charm();
		c->num_slots = data[ offset + 16 ];
		
		if( const int skill1_index = data[ offset + 12 ] )
		{
			if( skill1_index <= Ability::static_abilities.Count )
				c->abilities.Add( gcnew AbilityPair( Ability::static_abilities[ skill1_index - 1 ], data[ offset + 14 ] ) );
			else
				return nullptr;
		}

		if( const int skill2_index = data[ offset + 13 ] )
		{
			if( skill2_index > 0 && skill2_index <= Ability::static_abilities.Count )
				c->abilities.Add( gcnew AbilityPair( Ability::static_abilities[ skill2_index - 1 ], (int)(signed char)data[ offset + 15 ] ) );
			else
				return nullptr;
		}

		return c;
	}
	return nullptr;
}

SaveData::SaveData( String^ filename )
{
	failed = true;

	try
	{
		array< Byte >^ data = IO::File::ReadAllBytes( filename );

		characters = gcnew array< SaveDataCharacter^ >( 3 );
		last_used = data[ 0x7 ];

		for( unsigned i = 0; i < 3; ++i )
		{
			if( data[ 0x4 + i ] == 0 )
				continue;

			const unsigned char_offset = ExtractUnsigned( data, 0x10 + i * 4 );
			characters[ i ] = gcnew SaveDataCharacter();
			characters[ i ]->name = Text::Encoding::UTF8->GetString( data, char_offset, 32 );

			for( unsigned e = 0; e < 1400; ++e )
			{
				Charm^ charm = ExtractCharm( data, char_offset + 0x62EE + e * 36 );
				if( charm )
					characters[ i ]->charms.Add( charm );
			}
		}
	}
	catch( Exception^ e )
	{
		String^ error = e->ToString();
		return;
	}

	failed = false;
}