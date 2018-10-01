#pragma once
#include "LoadedData.h"
#include "About.h"
#include "Advanced.h"
#include "Find.h"
#include "CharmDatabase.h"
#include "Version.h"
#include "ManageCharms.h"
#include "SkillHelp.h"
#include "NumericUpDownHR.h"
#include "PreviewImage.h"

namespace MHGUASS
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#pragma region Comparison Functions
	int Compare( const int x, const int y )
	{
		return x < y ? -1 : y < x ? 1 : 0;
	}

	int ReverseCompare( const int x, const int y )
	{
		return x < y ? 1 : y < x ? -1 : 0;
	}
#define TryReverseCompare( X, Y ) X != Y ? ReverseCompare( X, Y )

#define BasicCompare( Name, Var ) \
	int CompareSolutionBy##Name( Solution^ s1, Solution^ s2 )\
	{\
		return s1->Var != s2->Var ? ReverseCompare( s1->Var, s2->Var ) : ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );\
	}

	BasicCompare( Defence, defence );
	BasicCompare( MaxDefence, max_defence );
	BasicCompare( Rarity, rarity );

	BasicCompare( FireRes, fire_res );
	BasicCompare( IceRes, ice_res );
	BasicCompare( WaterRes, water_res );
	BasicCompare( ThunderRes, thunder_res );
	BasicCompare( DragonRes, dragon_res );

#undef BasicCompare
	
	int CompareSolutionByDifficulty( Solution^ s1, Solution^ s2 )
	{
		return Compare( s1->difficulty, s2->difficulty );
	}

	int CompareSolutionBySlotsSpare( Solution^ s1, Solution^ s2 )
	{
		return ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );
	}

	int CompareSolutionByFamily( Solution^ s1, Solution^ s2 )
	{
		return TryReverseCompare( s1->family_score[0], s2->family_score[0] ) :
			   TryReverseCompare( s1->family_score[1], s2->family_score[1] ) :
			   TryReverseCompare( s1->family_score[2], s2->family_score[2] ) :
			   ReverseCompare( s1->total_slots_spare, s2->total_slots_spare );
	}

	int CompareSolutionsByExtraSkills( Solution^ s1, Solution^ s2 )
	{
		return TryReverseCompare( s1->extra_skills.Count, s2->extra_skills.Count ) :
			   ReverseCompare( s1->extra_skill_score, s2->extra_skill_score );
	}

	int CompareSkillsByName( Skill^ s1, Skill^ s2 )
	{
		return s1->name->CompareTo( s2->name );
	}
#pragma endregion

	public ref class Form1 : public System::Windows::Forms::Form
	{
		typedef System::Windows::Forms::DialogResult DialogResult_t;
		const static DialogResult_t OK = DialogResult_t::OK;
		int MAX_LIMIT;
		const static int NumSkills = 7;
		const static int MaxSolutions = 100000;
		static Threading::Mutex^ progress_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ results_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ charm_map_mutex = gcnew Threading::Mutex;
		static Threading::Mutex^ worker_mutex = gcnew Threading::Mutex;
		String^ CFG_FILE;
		String^ endl;
		String^ last_result;
		bool lock_skills, sort_off, can_save, last_search_gunner, updating_language, construction_complete, lock_related, search_cancelled, updating_extra_skills;
		LoadedData^ data;
		Query^ query;
		Find^ find_dialog;
		typedef Generic::Dictionary< unsigned, unsigned > IndexMap;
		typedef Generic::Dictionary< String^, List_t< Solution^ >^ > CharmSolutionMap;
		CharmSolutionMap charm_solution_map;
		Generic::Dictionary< String^, Generic::Dictionary< __int64, bool >^ > existing_armor;
		Generic::Dictionary< __int64, bool > existing_no_charm_armor;
		SkillHelp^ help_window;
		PreviewImage^ preview_pane;

		List_t< Solution^ > final_solutions, no_charm_solutions, all_solutions;
		List_t< ComboBox^ > bSkills, gSkills;
		List_t< ComboBox^ > bSkillFilters, gSkillFilters;
		List_t< IndexMap^ > bIndexMaps, gIndexMaps;
		List_t< String^ > languages;
		List_t< Charm^ > charm_box_charms;
		List_t< int > result_offsets;
		List_t< Ability^ > b_last_selected_ability, g_last_selected_ability;
		List_t< Skill^ > solutions_extra_skills;
		int language, adv_x, adv_y;
#pragma region Members
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuFile;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuExit;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuHelp;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAbout;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuLanguage;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::RadioButton^  rdoFemale;
	private: System::Windows::Forms::RadioButton^  rdoMale;
	private: System::Windows::Forms::Button^  btnAdvancedSearch;
	private: System::Windows::Forms::TabControl^  tabHunterType;
	private: System::Windows::Forms::TabPage^  tabBlademaster;
	private: System::Windows::Forms::TabPage^  tabGunner;
	private: System::Windows::Forms::GroupBox^  grpGSkillFilters;
	private: System::Windows::Forms::GroupBox^  grpGSkills;
	private: MHGUASS::NumericUpDownHR^  nudHR;
	private: MHGUASS::NumericUpDownVE^  nudElder;
	private: System::Windows::Forms::NumericUpDown^  nudWeaponSlots;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  lblHR;
	private: System::Windows::Forms::Label^  lblElder;
	private: System::Windows::Forms::Label^  lblSlots;
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::GroupBox^  grpBSkills;
	private: System::Windows::Forms::Button^  btnSearch;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::RichTextBox^  txtSolutions;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  grpResults;
	private: System::Windows::Forms::GroupBox^  grpBSkillFilters;
	private: System::Windows::Forms::GroupBox^  grpSortFilter;
	private: System::Windows::Forms::ComboBox^  cmbSort;
	private: System::Windows::Forms::ContextMenuStrip^  cmsSolutions;
	private: System::Windows::Forms::ComboBox^  cmbCharms;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuOptions;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowBadSkills;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowArena;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowEventArmor;
	private: System::Windows::Forms::Button^  btnCharms;
	private: System::Windows::Forms::GroupBox^  grpCharms;
	private: System::Windows::Forms::ComboBox^  cmbCharmSelect;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuCheckForUpdates;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuClearSettings;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ContextMenuStrip^  cmsCharms;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuLoadData;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSaveData;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSortSkillsAlphabetically;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuPrintDecoNames;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuMaxResults;
	private: System::Windows::Forms::ToolStripTextBox^  mnuNumResults;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowLowerTierArmor;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSkillHelp;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowJapaneseOnlyDLC;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuSpendSpareSlots;
	private: System::Windows::Forms::ComboBox^  cmbFilterByExtraSkill;
	private: System::Windows::Forms::ContextMenuStrip^  cmsSkills;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuClearSkill;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuShowRequiredSkillPoints;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuDonate;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuAllowGunnerHelms;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuPrintMaterials;
	private: System::Windows::Forms::ToolStripMenuItem^  mnuZoom;




#pragma endregion

		List_t< BackgroundWorker^ >  workers;
		List_t< Object^ > worker_data;
		unsigned finished_workers, total_progress, worker_start_index, num_updates;

		void ClearFilters()
		{
			for each( ComboBox^ box in bSkillFilters )
				box->Items->Clear();
			for each( ComboBox^ box in gSkillFilters )
				box->Items->Clear();
		}
	
		void AddFilters()
		{
			for each( SkillTag^ tag in SkillTag::tags )
			{
				if( !tag->disable_b )
				{
					for each( ComboBox^ box in bSkillFilters )
						box->Items->Add( tag->name );
				}
				if( !tag->disable_g )
				{
					for each( ComboBox^ box in gSkillFilters )
						box->Items->Add( tag->name );
				}
			}
		}

		void InitFilters()
		{
			ClearFilters();
			AddFilters();

			for each( ComboBox^ box in gSkillFilters )
				box->SelectedIndex = 0;
			for each( ComboBox^ box in bSkillFilters )
				box->SelectedIndex = 0;
		}

		void ResetSkill( ComboBox^ box, IndexMap^ map, Skill^ skill )
		{
			if( skill == nullptr ) return;
			IndexMap::Enumerator iter = map->GetEnumerator();
			while( iter.MoveNext() )
			{
				if( Skill::static_skills[ iter.Current.Value ] == skill )
				{
					box->SelectedIndex = iter.Current.Key;
					return;
				}
			}
		}

		void InitSkills2( ComboBox^ box, IndexMap^ map, const int filter, List_t< Ability^ >^ disallowed )
		{
			map->Clear();
			box->SelectedIndex = -1;
			box->Items->Clear();
			if( filter < 0 || !StringTable::text )
				return;

			box->Items->Add( StaticString( NoneBrackets ) );
			List_t< Skill^ >^ the_list = mnuSortSkillsAlphabetically->Checked ? %Skill::ordered_skills : %Skill::static_skills;
			for each( Skill^ skill in the_list )
			{
				if( skill->points_required <= 0 && !skill->is_taunt || 
					Utility::Contains( disallowed, skill->ability ) )
					continue;

				if( filter == 0 || filter == 1 && skill->ability->tags.Count == 0 || 
					filter == 2 && skill->ability->related ||
					!!Utility::Contains( %skill->ability->tags, SkillTag::tags[ filter ] ) )
				{
					map[ box->Items->Count ] = skill->static_index;
					if( mnuShowRequiredSkillPoints->Checked )
					{
						String^ num = ( skill->points_required < 0 ? L"" : L"+" ) + Convert::ToString( skill->points_required );
						box->Items->Add( FormatString2( SkillPointsRequiredFormat, skill->name, num ) );
					}
					else	
						box->Items->Add( skill->name );
				}
			}
		}

		void InitSkills( ComboBox^ box, IndexMap^ map, const int filter, List_t< Ability^ >^ disallowed, const bool blade )
		{
			unsigned index = filter;
			if( blade )
			{
				for( int i = 0; i <= filter; ++i )
					index += SkillTag::tags[i]->disable_b;
			}
			else
			{
				for( int i = 0; i <= filter; ++i )
					index += SkillTag::tags[i]->disable_g;
			}
			InitSkills2( box, map, index, disallowed );
		}

		void InitSkills()
		{
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				InitSkills( gSkills[ i ], gIndexMaps[ i ], gSkillFilters[ i ]->SelectedIndex, gcnew List_t< Ability^ >(), false );
				InitSkills( bSkills[ i ], bIndexMaps[ i ], bSkillFilters[ i ]->SelectedIndex, gcnew List_t< Ability^ >(), true );
			}
		}

		ref class MyComboBox : public ComboBox
		{
			bool draw_focus;
		public:
			MyComboBox() : draw_focus( false ) {}

			void SetOwnerDraw()
			{
				DrawMode = Windows::Forms::DrawMode::OwnerDrawFixed;
				try
				{
					VisualStyles::VisualStyleRenderer renderer( "COMBOBOX", 1, (int)VisualStyles::ComboBoxState::Normal );
				}
				catch( Exception^ e )
				{
					Assert( false, e->Message );
					return; //failed to create style renderer. don't set user paint
				}

				SetStyle( ControlStyles::UserPaint | ControlStyles::DoubleBuffer, true );
			}

			virtual void OnLostFocus( System::EventArgs^ e ) override
			{
				draw_focus = false;
				Refresh();
			}

			virtual void OnGotFocus( System::EventArgs^ e ) override
			{
				draw_focus = true;
				Refresh();
			}

			virtual void OnDrawItem( DrawItemEventArgs^ e ) override
			{
				e->DrawBackground();
				e->DrawFocusRectangle();

				if( e->Index < 0 )
					return;

				String^ text = Items[ e->Index ]->ToString();
				Skill^ skill = Skill::FindSkill( text );

				TextRenderer::DrawText( e->Graphics, text, e->Font, e->Bounds, ( skill && skill->impossible ) ? Color::Red : Color::Black, TextFormatFlags::Left );
			}

			virtual void OnDropDown( System::EventArgs^ e ) override
			{
				draw_focus = false;
				Invalidate();
			}

			virtual void OnDropDownClosed( System::EventArgs^ e ) override
			{
				if( SelectedIndex == 0 && DrawMode == Windows::Forms::DrawMode::OwnerDrawFixed )
				{
					SuspendDrawing( this );
					SelectedIndex = -1;
					ResumeDrawing( this );
				}
				draw_focus = false;
			}

			virtual void OnPaint( PaintEventArgs^ e ) override
			{
				ButtonRenderer::DrawButton( e->Graphics, Rectangle( -1, -1, Size.Width + 2, Size.Height + 2 ), DroppedDown ? VisualStyles::PushButtonState::Pressed : VisualStyles::PushButtonState::Normal );
				
				//See https://www.gittprogram.com/question/479791_comboboxrenderer-does-not-look-like-windows-7-combobox.html
				VisualStyles::VisualStyleRenderer renderer( "COMBOBOX", 1, (int)(DroppedDown ? VisualStyles::ComboBoxState::Pressed : VisualStyles::ComboBoxState::Normal) );
				renderer.DrawBackground( e->Graphics, Rectangle( Size.Width - 20, 0, 20, Size.Height ), Rectangle( Size.Width - 15, 5, 10, Size.Height - 10 ) );

				if( Focused && !DroppedDown && draw_focus )
					ControlPaint::DrawFocusRectangle( e->Graphics, Rectangle( 3, 3, Width - 23, Height - 6 ) );

				if( SelectedIndex < 0 )
					return;

				Skill^ skill = Skill::FindSkill( Text );

				TextRenderer::DrawText( e->Graphics, Text, Font, Point( 2, 4 ), ( skill && skill->impossible ) ? Color::Red : Color::Black );
			}
		};

		ComboBox^ GetNewComboBox( const unsigned width, const unsigned i, const bool is_filter )
		{
			MyComboBox^ box = gcnew MyComboBox;
			box->Location = System::Drawing::Point( 6, 19 + i * 27 );
			box->Size = System::Drawing::Size( width, box->Size.Height );
			box->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			box->AutoCompleteSource = AutoCompleteSource::ListItems;
			box->AutoCompleteMode = AutoCompleteMode::Suggest;

			if( is_filter )
			{
				box->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cmbSkillFilter_SelectedIndexChanged);
			}
			else
			{
				box->SelectionChangeCommitted += gcnew System::EventHandler(this, &Form1::cmbSkill_SelectedIndexChanged);
				box->ContextMenuStrip = cmsSkills;
				box->SetOwnerDraw();
			}
			return box;
		}

		void ClearSkillClicked( Object^ sender, EventArgs^ e )
		{
			ComboBox^ box = safe_cast< ComboBox^ >( cmsSkills->SourceControl );
			if( box )
				box->SelectedIndex = -1;
		}

		void InitializeComboBoxes()
		{
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				gSkillFilters.Add( GetNewComboBox( 134, i, true ) );
				bSkillFilters.Add( GetNewComboBox( 134, i, true ) );
				grpGSkillFilters->Controls->Add( gSkillFilters[ i ] );
				grpBSkillFilters->Controls->Add( bSkillFilters[ i ] );

				gSkills.Add( GetNewComboBox( 171, i, false ) );
				bSkills.Add( GetNewComboBox( 171, i, false ) );
				grpGSkills->Controls->Add( gSkills[ i ] );
				grpBSkills->Controls->Add( bSkills[ i ] );

				gIndexMaps.Add( gcnew IndexMap );
				bIndexMaps.Add( gcnew IndexMap );

				b_last_selected_ability.Add( nullptr );
				g_last_selected_ability.Add( nullptr );
			}
			charm_solution_map.Clear();
			cmbSort->SelectedIndex = 0;
			cmbCharms->SelectedIndex = 1;
			cmbCharmSelect->SelectedIndex = 2;
		}

		void AddZoomOption( const int zoom_level )
		{
			ToolStripMenuItem^ item = gcnew ToolStripMenuItem( zoom_level + L"%" );
			item->Tag = zoom_level;
			item->Checked = zoom_level == PreviewImage::zoom;
			item->Name = L"mnuZoom" + zoom_level;
			item->Size = System::Drawing::Size(152, 22);
			item->Click += gcnew EventHandler( this, &Form1::ZoomLevelChanged );

			mnuZoom->DropDownItems->Add( item );
		}

	public:

		Form1(void) : CFG_FILE( L"settings.cfg" ), endl( L"\r\n" ), construction_complete( false ), MAX_LIMIT( 1000 )
		{
			DoubleBuffered = true;
			language = -1;
			sort_off = false;
			updating_language = false;
			can_save = false;
			last_search_gunner = false;
			lock_related = false;
			updating_extra_skills = false;
			InitializeComponent();
			InitializeComboBoxes();
			this->Text += GAMES;
			mnuNumResults->KeyPress += gcnew KeyPressEventHandler( this, &Form1::MaxResultsTextBoxKeyPress );
			mnuNumResults->TextChanged += gcnew EventHandler( this, &Form1::MaxResultsTextChanged );
			cmsSolutions->Closing += gcnew ToolStripDropDownClosingEventHandler( this, &Form1::contextMenuStrip1_Closing );
			cmbFilterByExtraSkill->SelectedIndex = 0;
			mnuClearSkill->Click += gcnew EventHandler( this, &Form1::ClearSkillClicked );

			AddZoomOption( 50 );
			AddZoomOption( 75 );
			AddZoomOption( 100 );
			AddZoomOption( 150 );
			AddZoomOption( 200 );
			
			can_save = true;
			adv_x = 1031;
			adv_y = 587;

			preview_pane = nullptr;

			data = gcnew LoadedData();
			data->LoadDataFiles();
			CharmDatabase::GenerateCharmTable();
			LoadLanguages();

			InitFilters();
			InitSkills();
		
			lock_skills = false;
			btnCancel->Enabled = false;

			LoadConfig( true );

			if( !CharmDatabase::CreateNewCustom() )
			{
				//try settings language first
				if( !CharmDatabase::LoadCustom() )
				{
					int lang = language;
					bool failed = true;

					for each( Object^ o in mnuLanguage->DropDownItems )
					{
						LanguageSelect_Click( o, nullptr );
						if( CharmDatabase::LoadCustom() )
						{
							failed = false;
							break;
						}
					}

					LanguageSelect_Click( mnuLanguage->DropDownItems[ lang ], nullptr );

					if( failed )
						MessageBox::Show( L"Failed to load charms file!" );
				}
			}

			LoadConfig( false );

			UpdateImpossibleSkills();

			Text += " v" +  STRINGIZE( VERSION_NO );

			construction_complete = true;
		}

		void LoadLanguages()
		{
			array< String^ >^ langs = IO::Directory::GetDirectories( L"Data/Languages" );
			for each( String^% lang in langs )
			{
				ToolStripMenuItem^ item = gcnew ToolStripMenuItem( lang->Substring( lang->LastIndexOf( L'\\' ) + 1 ) );
				item->Click += gcnew EventHandler( this, &Form1::LanguageSelect_Click );
				mnuLanguage->DropDownItems->Add( item );
			}
		}

		void AddSolution( String^ line, const unsigned version )
		{
			if( all_solutions.Count >= MaxSolutions )
				return;

			List_t< String^ > split;
			Utility::SplitString( %split, line, L' ' );
			Solution^ solution = gcnew Solution;
			for( unsigned i = 0; i < 5; ++i )
			{
				const int index = Convert::ToInt32( split[ i ] );
				if( index < 0 )
					solution->armors.Add( nullptr );
				else solution->armors.Add( Armor::static_armors[ i ][ index ] );
			}
			unsigned num_decorations = 0;
			const unsigned num_torso_decorations = Convert::ToUInt32( split[ 9 ] );
			const unsigned num_charm_decorations = Convert::ToUInt32( split[ 10 ] );
			const unsigned num_non_torso_decorations = Convert::ToUInt32( split[ 11 ] );
			num_decorations = num_torso_decorations + num_non_torso_decorations + num_charm_decorations;
			{
				unsigned si = 0;
				for( unsigned i = 0; i < num_torso_decorations; ++i, ++si )
					solution->body_decorations.Add( Decoration::static_decorations[ Convert::ToUInt32( split[ 12 + si ] ) ] );
				for( unsigned i = 0; i < num_charm_decorations; ++i, ++si )
					solution->charm_decorations.Add( Decoration::static_decorations[ Convert::ToUInt32( split[ 12 + si ] ) ] );
				for( unsigned i = 0; i < num_non_torso_decorations; ++i, ++si )
					solution->non_body_decorations.Add( Decoration::static_decorations[ Convert::ToUInt32( split[ 12 + si ] ) ] );
			}
			solution->decorations.AddRange( %solution->body_decorations );
			solution->decorations.AddRange( %solution->non_body_decorations );

			const unsigned decorations_end = 12 + num_decorations;
			
			const unsigned num_skills = Convert::ToUInt32( split[ decorations_end ] );
			for( unsigned i = 0; i < num_skills; ++i )
			{
				const int index = Convert::ToUInt32( split[ 1 + decorations_end + i ] );
				solution->extra_skills.Add( Skill::static_skills[ index ] );
			}
			unsigned num_pot_skills = Convert::ToUInt32( split[ 1 + decorations_end + num_skills ] );
			for( unsigned i = 0; i < num_pot_skills; ++i )
			{
				const int index = Convert::ToUInt32( split[ 2 + decorations_end + i + num_skills ] );
				solution->potential_extra_skills.Add( Skill::static_skills[ index ] );
			}
			
			const unsigned charm_start = 2 + decorations_end + num_skills + num_pot_skills;
			if( charm_start < unsigned( split.Count ) )
			{
				solution->charm = gcnew Charm;
				solution->charm->num_slots = Convert::ToUInt32( split[ charm_start ] );
				const unsigned num_abilities = Convert::ToUInt32( split[ charm_start + 1 ] );
				for( unsigned i = 0; i < num_abilities; ++i )
				{
					Ability^ ability = Ability::static_abilities[ Convert::ToUInt32( split[ charm_start + 3 + i * 2 ] ) ];
					const int amount = Convert::ToInt32( split[ charm_start + 2 + i * 2 ] );
					solution->charm->abilities.Add( gcnew AbilityPair( ability, amount ) );
				}
			}

			solution->slots_spare = gcnew array< unsigned >( 4 );
			solution->total_slots_spare = Convert::ToUInt32( split[ 5 ] );
			
			for( unsigned i = 1; i <= 3; ++i )
				solution->slots_spare[ i ] = Convert::ToUInt32( split[ 5 + i ] );

			solution->CalculateData( int( nudHR->Value ), int( nudElder->Value ) );

			if( solution->charm )
				AddSolution( solution->charm->GetName(), solution );
			else
			{
				no_charm_solutions.Add( solution );
				all_solutions.Add( solution );
			}
		}

		void LoadDefaultLanguage()
		{
			language = -1;
			String^ native_lang = System::Globalization::CultureInfo::InstalledUICulture->Parent->NativeName->ToLower();
			for each( ToolStripItem^ item in mnuLanguage->DropDownItems )
			{
				if( item->ToString()->ToLower()->IndexOf( native_lang ) >= 0 )
				{
					LanguageSelect_Click( item, nullptr );
					break;
				}
			}
			if( language == -1 )
			{
				for each( ToolStripItem^ item in mnuLanguage->DropDownItems )
				{
					if( item->ToString()->ToLower()->IndexOf( L"English" ) >= 0 )
					{
						LanguageSelect_Click( item, nullptr );
						break;
					}
				}
			}
			if( language == -1 )
				LanguageSelect_Click( mnuLanguage->DropDownItems[ 0 ], nullptr );
		}

		void LoadConfig( String^ file, const bool lang )
		{
			can_save = false;
			if( IO::File::Exists( file ) )
			{
				IO::StreamReader fin( file );
				String^ version_string = fin.ReadLine();
				const int version = Convert::ToInt32( version_string );
				
				if( version < 1 || version > 1 )
				{
					fin.Close();
					if( mnuLanguage->HasDropDownItems )
						LoadDefaultLanguage();
					can_save = true;
					return;
				}
				else
				{
					String^ lang_name = nullptr;
					lang_name = fin.ReadLine();
					int temp = -1;
					for( int i = 0; i < mnuLanguage->DropDownItems->Count; ++i )
					{
						if( mnuLanguage->DropDownItems[ i ]->ToString() == lang_name )
						{
							temp = i;
							break;
						}
					}

					if( lang )
					{
						language = -1;
						if( temp < 0 )
							LoadDefaultLanguage();
						else LanguageSelect_Click( mnuLanguage->DropDownItems[ temp ], nullptr );
						return;
					}
					
					MAX_LIMIT = Convert::ToInt32( fin.ReadLine() );
					mnuNumResults->Text = L"" + MAX_LIMIT;

					last_search_gunner = Convert::ToBoolean( fin.ReadLine() );
					const int hunter_type = Convert::ToInt32( fin.ReadLine() );
					rdoMale->Checked = Convert::ToBoolean( fin.ReadLine() );
					rdoFemale->Checked = !rdoMale->Checked;
					mnuAllowBadSkills->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowArena->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowEventArmor->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowJapaneseOnlyDLC->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuAllowLowerTierArmor->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuSpendSpareSlots->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuPrintDecoNames->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuPrintMaterials->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuSortSkillsAlphabetically->Checked = Convert::ToBoolean( fin.ReadLine() );
					mnuShowRequiredSkillPoints->Checked = Convert::ToBoolean( fin.ReadLine() );
					
					const int zoom = Convert::ToInt32( fin.ReadLine() );
					for each( ToolStripMenuItem^ item in mnuZoom->DropDownItems )
					{
						if( zoom == (Int32)item->Tag )
						{
							ZoomLevelChanged( item, nullptr );
							break;
						}
					}

					cmbSort->SelectedIndex = Convert::ToInt32( fin.ReadLine() );
					cmbCharmSelect->SelectedIndex = Convert::ToInt32( fin.ReadLine() );

					nudHR->Value = Convert::ToInt32( fin.ReadLine() );
					nudElder->Value = Convert::ToInt32( fin.ReadLine() );
					nudWeaponSlots->Value = Convert::ToInt32( fin.ReadLine() );

					adv_x = Convert::ToInt32( fin.ReadLine() );
					adv_y = Convert::ToInt32( fin.ReadLine() );

					tabHunterType->SuspendLayout();
					const unsigned num_saved = NumSkills;
					for( unsigned i = 0; i < num_saved; ++i )
					{
						tabHunterType->SelectedIndex = 0;
						
						const int bfilter_index = Convert::ToInt32( fin.ReadLine() );
						if( bSkillFilters[ i ]->SelectedIndex == bfilter_index )
							bSkillFilters[ i ]->SelectedIndex = -1;
						bSkillFilters[ i ]->SelectedIndex = bfilter_index; //force selected index to change

						if( bSkillFilters[ i ]->SelectedIndex == 2 ) //related
						{
							bSkillFilters[ i ]->SelectedIndex = 0;
							bSkills[ i ]->SelectedIndex = SearchIndexMap( bIndexMaps[ i ], Convert::ToInt32( fin.ReadLine() ) );
							bSkillFilters[ i ]->SelectedIndex = 2;
						}
						else bSkills[ i ]->SelectedIndex = SearchIndexMap( bIndexMaps[ i ], Convert::ToInt32( fin.ReadLine() ) );
						
						tabHunterType->SelectedIndex = 1;
						
						const int gfilter_index = Convert::ToInt32( fin.ReadLine() );
						if( gSkillFilters[ i ]->SelectedIndex == gfilter_index )
							gSkillFilters[ i ]->SelectedIndex = -1;
						gSkillFilters[ i ]->SelectedIndex = gfilter_index; //force selected index to change

						if( gSkillFilters[ i ]->SelectedIndex == 2 ) //related
						{
							gSkillFilters[ i ]->SelectedIndex = 0;
							gSkills[ i ]->SelectedIndex = SearchIndexMap( gIndexMaps[ i ], Convert::ToInt32( fin.ReadLine() ) );
							gSkillFilters[ i ]->SelectedIndex = 2;
						}
						gSkills[ i ]->SelectedIndex = SearchIndexMap( gIndexMaps[ i ], Convert::ToInt32( fin.ReadLine() ) );
					}
					tabHunterType->SelectedIndex = hunter_type;
					tabHunterType->ResumeLayout();

					const unsigned num_searched_skills = Convert::ToUInt32( fin.ReadLine() );
					List_t< Skill^ > searched_skills;
					for( unsigned i = 0; i < num_searched_skills; ++i )
					{
						searched_skills.Add( Skill::static_skills[ Convert::ToUInt32( fin.ReadLine() ) ] );
					}

					FormulateQuery( false, last_search_gunner, %searched_skills );

					charm_solution_map.Clear();
					all_solutions.Clear();
					no_charm_solutions.Clear();
					existing_armor.Clear();
					existing_no_charm_armor.Clear();
					while( !fin.EndOfStream )
						AddSolution( fin.ReadLine(), version );
					last_result = nullptr;
					fin.Close();

					UpdateCharmComboBox( 1 );
					UpdateExtraSkillCombo( false );
				}	
			}
			else if( mnuLanguage->HasDropDownItems )
			{
				LoadDefaultLanguage();
			}
			can_save = true;
			SaveConfig();
		}

		void LoadConfig( const bool lang )
		{
			LoadConfig( CFG_FILE, lang );
		}

		void SaveConfig( String^ file )
		{
			if( !can_save )
				return;

			IO::StreamWriter fout( file );
			fout.WriteLine( L"1" ); //version
			fout.WriteLine( mnuLanguage->DropDownItems[ language ]->ToString() );
			fout.WriteLine( MAX_LIMIT );
			fout.WriteLine( last_search_gunner );
			fout.WriteLine( tabHunterType->SelectedIndex );
			fout.WriteLine( rdoMale->Checked );
			fout.WriteLine( mnuAllowBadSkills->Checked );
			fout.WriteLine( mnuAllowArena->Checked );
			fout.WriteLine( mnuAllowEventArmor->Checked );
			fout.WriteLine( mnuAllowJapaneseOnlyDLC->Checked );
			fout.WriteLine( mnuAllowLowerTierArmor->Checked );
			fout.WriteLine( mnuSpendSpareSlots->Checked );
			fout.WriteLine( mnuPrintDecoNames->Checked );
			fout.WriteLine( mnuPrintMaterials->Checked );
			fout.WriteLine( mnuSortSkillsAlphabetically->Checked );
			fout.WriteLine( mnuShowRequiredSkillPoints->Checked );
			fout.WriteLine( PreviewImage::zoom );
			fout.WriteLine( cmbSort->SelectedIndex );
			fout.WriteLine( cmbCharmSelect->SelectedIndex );
			fout.WriteLine( nudHR->Value );
			fout.WriteLine( nudElder->Value );
			fout.WriteLine( nudWeaponSlots->Value );
			fout.WriteLine( adv_x );
			fout.WriteLine( adv_y );
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				fout.WriteLine( bSkillFilters[ i ]->SelectedIndex );
				if( bSkills[ i ]->SelectedIndex <= 0 )
					fout.WriteLine( -1 );
				else
				{
					Map_t< unsigned, unsigned >^ imap = bIndexMaps[ i ];
					fout.WriteLine( imap[ bSkills[ i ]->SelectedIndex ] );
				}

				fout.WriteLine( gSkillFilters[ i ]->SelectedIndex );
				if( gSkills[ i ]->SelectedIndex <= 0 )
					fout.WriteLine( -1 );
				else
				{
					Map_t< unsigned, unsigned >^ imap = gIndexMaps[ i ];
					fout.WriteLine( imap[ gSkills[ i ]->SelectedIndex ] );
				}
			}

			const unsigned num_searched_skills = query ? query->skills.Count : 0;
			fout.WriteLine( num_searched_skills );
			for( unsigned i = 0; i < num_searched_skills; ++i )
				fout.WriteLine( query->skills[ i ]->static_index );
			
			for each( Solution^ solution in all_solutions )
			{
				for( unsigned i = 0; i < 5; ++i )
				{
					Armor^ a = solution->armors[ i ];
					if( a )
						fout.Write( Convert::ToString( a->index ) );
					else
						fout.Write( L"-1" );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->total_slots_spare ) );
				fout.Write( L" " );
				for( unsigned i = 1; i <= 3; ++i )
				{
					fout.Write( Convert::ToString( solution->slots_spare[ i ] ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->body_decorations.Count ) );
				fout.Write( L" " );
				fout.Write( Convert::ToString( solution->charm_decorations.Count ) );
				fout.Write( L" " );
				fout.Write( Convert::ToString( solution->non_body_decorations.Count ) );
				fout.Write( L" " );
				for each( Decoration^ decoration in solution->body_decorations )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Decoration::static_decorations, decoration ) ) );
					fout.Write( L" " );
				}
				for each( Decoration^ decoration in solution->charm_decorations )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Decoration::static_decorations, decoration ) ) );
					fout.Write( L" " );
				}
				for each( Decoration^ decoration in solution->non_body_decorations )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Decoration::static_decorations, decoration ) ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->extra_skills.Count ) );
				fout.Write( L" " );
				for each( Skill^ skill in solution->extra_skills )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Skill::static_skills, skill ) ) );
					fout.Write( L" " );
				}
				fout.Write( Convert::ToString( solution->potential_extra_skills.Count ) );
				fout.Write( L" " );
				for each( Skill^ skill in solution->potential_extra_skills )
				{
					fout.Write( Convert::ToString( Utility::GetIndexOf( %Skill::static_skills, skill ) ) );
					fout.Write( L" " );
				}
				if( solution->charm )
				{
					fout.Write( Convert::ToString( solution->charm->num_slots ) );
					fout.Write( L" " );
					fout.Write( Convert::ToString( solution->charm->abilities.Count ) );
					fout.Write( L" " );
					for each( AbilityPair^ ap in solution->charm->abilities )
					{
						fout.Write( Convert::ToString( ap->amount ) );
						fout.Write( L" " );
						fout.Write( Convert::ToString( Utility::GetIndexOf( %Ability::static_abilities, ap->ability ) ) );
						fout.Write( L" " );
					}
				}

				fout.WriteLine();
			}
			fout.Close();
		}

		void SaveConfig()
		{
			SaveConfig( CFG_FILE );
		}

		int SearchIndexMap( IndexMap^ imap, int skill_index )
		{
			for each( Generic::KeyValuePair< unsigned, unsigned > kvp in imap )
			{
				if( kvp.Value == skill_index )
					return kvp.Key;
			}
			return -1;
		}

	protected:

		~Form1()
		{
			SaveConfig();
		}

	private: System::ComponentModel::IContainer^  components;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = ( gcnew System::ComponentModel::Container() );
			System::ComponentModel::ComponentResourceManager^  resources = ( gcnew System::ComponentModel::ComponentResourceManager( Form1::typeid ) );
			this->groupBox1 = ( gcnew System::Windows::Forms::GroupBox() );
			this->nudWeaponSlots = ( gcnew System::Windows::Forms::NumericUpDown() );
			this->lblElder = ( gcnew System::Windows::Forms::Label() );
			this->lblSlots = ( gcnew System::Windows::Forms::Label() );
			this->nudHR = ( gcnew MHGUASS::NumericUpDownHR() );
			this->nudElder = ( gcnew MHGUASS::NumericUpDownVE() );
			this->lblHR = ( gcnew System::Windows::Forms::Label() );
			this->grpBSkills = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnSearch = ( gcnew System::Windows::Forms::Button() );
			this->progressBar1 = ( gcnew System::Windows::Forms::ProgressBar() );
			this->txtSolutions = ( gcnew System::Windows::Forms::RichTextBox() );
			this->cmsSolutions = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->groupBox4 = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnAdvancedSearch = ( gcnew System::Windows::Forms::Button() );
			this->btnCancel = ( gcnew System::Windows::Forms::Button() );
			this->grpResults = ( gcnew System::Windows::Forms::GroupBox() );
			this->btnCharms = ( gcnew System::Windows::Forms::Button() );
			this->grpBSkillFilters = ( gcnew System::Windows::Forms::GroupBox() );
			this->menuStrip1 = ( gcnew System::Windows::Forms::MenuStrip() );
			this->mnuFile = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuLoadData = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSaveData = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuExit = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuOptions = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuClearSettings = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->toolStripSeparator1 = ( gcnew System::Windows::Forms::ToolStripSeparator() );
			this->mnuAllowBadSkills = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowArena = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowEventArmor = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowJapaneseOnlyDLC = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowLowerTierArmor = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAllowGunnerHelms = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->toolStripSeparator2 = ( gcnew System::Windows::Forms::ToolStripSeparator() );
			this->mnuMaxResults = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuNumResults = ( gcnew System::Windows::Forms::ToolStripTextBox() );
			this->mnuZoom = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuPrintDecoNames = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuPrintMaterials = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSortSkillsAlphabetically = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuShowRequiredSkillPoints = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSpendSpareSlots = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuLanguage = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuHelp = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuSkillHelp = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuDonate = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuCheckForUpdates = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->mnuAbout = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->groupBox6 = ( gcnew System::Windows::Forms::GroupBox() );
			this->rdoFemale = ( gcnew System::Windows::Forms::RadioButton() );
			this->rdoMale = ( gcnew System::Windows::Forms::RadioButton() );
			this->tabHunterType = ( gcnew System::Windows::Forms::TabControl() );
			this->tabBlademaster = ( gcnew System::Windows::Forms::TabPage() );
			this->tabGunner = ( gcnew System::Windows::Forms::TabPage() );
			this->grpGSkillFilters = ( gcnew System::Windows::Forms::GroupBox() );
			this->grpGSkills = ( gcnew System::Windows::Forms::GroupBox() );
			this->grpSortFilter = ( gcnew System::Windows::Forms::GroupBox() );
			this->cmbFilterByExtraSkill = ( gcnew System::Windows::Forms::ComboBox() );
			this->cmbCharms = ( gcnew System::Windows::Forms::ComboBox() );
			this->cmbSort = ( gcnew System::Windows::Forms::ComboBox() );
			this->grpCharms = ( gcnew System::Windows::Forms::GroupBox() );
			this->cmbCharmSelect = ( gcnew System::Windows::Forms::ComboBox() );
			this->cmsCharms = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->cmsSkills = ( gcnew System::Windows::Forms::ContextMenuStrip( this->components ) );
			this->mnuClearSkill = ( gcnew System::Windows::Forms::ToolStripMenuItem() );
			this->groupBox1->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudHR ) )->BeginInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudElder ) )->BeginInit();
			this->groupBox4->SuspendLayout();
			this->grpResults->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->tabHunterType->SuspendLayout();
			this->tabBlademaster->SuspendLayout();
			this->tabGunner->SuspendLayout();
			this->grpSortFilter->SuspendLayout();
			this->grpCharms->SuspendLayout();
			this->cmsSkills->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add( this->nudWeaponSlots );
			this->groupBox1->Controls->Add( this->lblElder );
			this->groupBox1->Controls->Add( this->lblSlots );
			this->groupBox1->Controls->Add( this->nudHR );
			this->groupBox1->Controls->Add( this->nudElder );
			this->groupBox1->Controls->Add( this->lblHR );
			this->groupBox1->Location = System::Drawing::Point( 12, 27 );
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size( 154, 111 );
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			// 
			// nudWeaponSlots
			// 
			this->nudWeaponSlots->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudWeaponSlots->BackColor = System::Drawing::SystemColors::Control;
			this->nudWeaponSlots->Location = System::Drawing::Point( 109, 80 );
			this->nudWeaponSlots->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 3, 0, 0, 0 } );
			this->nudWeaponSlots->Name = L"nudWeaponSlots";
			this->nudWeaponSlots->Size = System::Drawing::Size( 35, 20 );
			this->nudWeaponSlots->TabIndex = 5;
			this->nudWeaponSlots->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// lblElder
			// 
			this->lblElder->AutoSize = true;
			this->lblElder->Location = System::Drawing::Point( 7, 52 );
			this->lblElder->Name = L"lblElder";
			this->lblElder->Size = System::Drawing::Size( 74, 13 );
			this->lblElder->TabIndex = 2;
			this->lblElder->Text = L"Village Quests";
			// 
			// lblSlots
			// 
			this->lblSlots->AutoSize = true;
			this->lblSlots->Location = System::Drawing::Point( 7, 82 );
			this->lblSlots->Name = L"lblSlots";
			this->lblSlots->Size = System::Drawing::Size( 97, 13 );
			this->lblSlots->TabIndex = 4;
			this->lblSlots->Text = L"Max Weapon Slots";
			// 
			// nudHR
			// 
			this->nudHR->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudHR->BackColor = System::Drawing::SystemColors::Control;
			this->nudHR->Location = System::Drawing::Point( 100, 20 );
			this->nudHR->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 13, 0, 0, 0 } );
			this->nudHR->Minimum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 1, 0, 0, 0 } );
			this->nudHR->Name = L"nudHR";
			this->nudHR->Size = System::Drawing::Size( 44, 20 );
			this->nudHR->TabIndex = 1;
			this->nudHR->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->nudHR->Value = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 13, 0, 0, 0 } );
			this->nudHR->ValueChanged += gcnew System::EventHandler( this, &Form1::nudHR_ValueChanged );
			// 
			// nudElder
			// 
			this->nudElder->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right ) );
			this->nudElder->BackColor = System::Drawing::SystemColors::Control;
			this->nudElder->Location = System::Drawing::Point( 100, 50 );
			this->nudElder->Maximum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 10, 0, 0, 0 } );
			this->nudElder->Minimum = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 1, 0, 0, 0 } );
			this->nudElder->Name = L"nudElder";
			this->nudElder->Size = System::Drawing::Size( 44, 20 );
			this->nudElder->TabIndex = 3;
			this->nudElder->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->nudElder->Value = System::Decimal( gcnew cli::array< System::Int32 >( 4 ) { 10, 0, 0, 0 } );
			this->nudElder->ValueChanged += gcnew System::EventHandler( this, &Form1::nudElder_ValueChanged );
			// 
			// lblHR
			// 
			this->lblHR->AutoSize = true;
			this->lblHR->Location = System::Drawing::Point( 7, 22 );
			this->lblHR->Name = L"lblHR";
			this->lblHR->Size = System::Drawing::Size( 67, 13 );
			this->lblHR->TabIndex = 0;
			this->lblHR->Text = L"Guild Quests";
			// 
			// grpBSkills
			// 
			this->grpBSkills->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->grpBSkills->Location = System::Drawing::Point( 6, 8 );
			this->grpBSkills->Name = L"grpBSkills";
			this->grpBSkills->Size = System::Drawing::Size( 183, 209 );
			this->grpBSkills->TabIndex = 0;
			this->grpBSkills->TabStop = false;
			this->grpBSkills->Text = L"Skills";
			// 
			// btnSearch
			// 
			this->btnSearch->Location = System::Drawing::Point( 6, 12 );
			this->btnSearch->Name = L"btnSearch";
			this->btnSearch->Size = System::Drawing::Size( 107, 27 );
			this->btnSearch->TabIndex = 0;
			this->btnSearch->Text = L"&Quick Search";
			this->btnSearch->UseVisualStyleBackColor = true;
			this->btnSearch->Click += gcnew System::EventHandler( this, &Form1::btnSearch_Click );
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point( 12, 495 );
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size( 353, 10 );
			this->progressBar1->Step = 1;
			this->progressBar1->TabIndex = 6;
			// 
			// txtSolutions
			// 
			this->txtSolutions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->txtSolutions->ContextMenuStrip = this->cmsSolutions;
			this->txtSolutions->Font = ( gcnew System::Drawing::Font( L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->txtSolutions->Location = System::Drawing::Point( 6, 16 );
			this->txtSolutions->Name = L"txtSolutions";
			this->txtSolutions->ReadOnly = true;
			this->txtSolutions->Size = System::Drawing::Size( 332, 454 );
			this->txtSolutions->TabIndex = 0;
			this->txtSolutions->Text = L"";
			this->txtSolutions->WordWrap = false;
			this->txtSolutions->KeyDown += gcnew System::Windows::Forms::KeyEventHandler( this, &Form1::KeyDown );
			// 
			// cmsSolutions
			// 
			this->cmsSolutions->Name = L"contextMenuStrip1";
			this->cmsSolutions->Size = System::Drawing::Size( 61, 4 );
			this->cmsSolutions->Opening += gcnew System::ComponentModel::CancelEventHandler( this, &Form1::contextMenuStrip1_Opening );
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add( this->btnAdvancedSearch );
			this->groupBox4->Controls->Add( this->btnCancel );
			this->groupBox4->Controls->Add( this->btnSearch );
			this->groupBox4->Location = System::Drawing::Point( 12, 443 );
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size( 353, 46 );
			this->groupBox4->TabIndex = 5;
			this->groupBox4->TabStop = false;
			// 
			// btnAdvancedSearch
			// 
			this->btnAdvancedSearch->Location = System::Drawing::Point( 119, 12 );
			this->btnAdvancedSearch->Name = L"btnAdvancedSearch";
			this->btnAdvancedSearch->Size = System::Drawing::Size( 137, 27 );
			this->btnAdvancedSearch->TabIndex = 1;
			this->btnAdvancedSearch->Text = L"&Advanced Search";
			this->btnAdvancedSearch->UseVisualStyleBackColor = true;
			this->btnAdvancedSearch->Click += gcnew System::EventHandler( this, &Form1::btnAdvancedSearch_Click );
			// 
			// btnCancel
			// 
			this->btnCancel->Location = System::Drawing::Point( 262, 12 );
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size( 85, 27 );
			this->btnCancel->TabIndex = 2;
			this->btnCancel->Text = L"Ca&ncel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler( this, &Form1::btnCancel_Click );
			// 
			// grpResults
			// 
			this->grpResults->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->grpResults->Controls->Add( this->txtSolutions );
			this->grpResults->Location = System::Drawing::Point( 373, 27 );
			this->grpResults->Name = L"grpResults";
			this->grpResults->Size = System::Drawing::Size( 344, 480 );
			this->grpResults->TabIndex = 7;
			this->grpResults->TabStop = false;
			this->grpResults->Text = L"Results";
			// 
			// btnCharms
			// 
			this->btnCharms->Location = System::Drawing::Point( 5, 46 );
			this->btnCharms->Name = L"btnCharms";
			this->btnCharms->Size = System::Drawing::Size( 89, 23 );
			this->btnCharms->TabIndex = 1;
			this->btnCharms->Text = L"My &Charms";
			this->btnCharms->UseVisualStyleBackColor = true;
			this->btnCharms->Click += gcnew System::EventHandler( this, &Form1::btnCharms_Click );
			// 
			// grpBSkillFilters
			// 
			this->grpBSkillFilters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->grpBSkillFilters->Location = System::Drawing::Point( 195, 8 );
			this->grpBSkillFilters->Name = L"grpBSkillFilters";
			this->grpBSkillFilters->Size = System::Drawing::Size( 146, 209 );
			this->grpBSkillFilters->TabIndex = 1;
			this->grpBSkillFilters->TabStop = false;
			this->grpBSkillFilters->Text = L"Skill Filters";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 4 )
			{
				this->mnuFile, this->mnuOptions,
					this->mnuLanguage, this->mnuHelp
			} );
			this->menuStrip1->Location = System::Drawing::Point( 0, 0 );
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size( 729, 24 );
			this->menuStrip1->TabIndex = 16;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// mnuFile
			// 
			this->mnuFile->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 3 )
			{
				this->mnuLoadData,
					this->mnuSaveData, this->mnuExit
			} );
			this->mnuFile->Name = L"mnuFile";
			this->mnuFile->Size = System::Drawing::Size( 37, 20 );
			this->mnuFile->Text = L"&File";
			// 
			// mnuLoadData
			// 
			this->mnuLoadData->Name = L"mnuLoadData";
			this->mnuLoadData->Size = System::Drawing::Size( 100, 22 );
			this->mnuLoadData->Text = L"&Load";
			this->mnuLoadData->Click += gcnew System::EventHandler( this, &Form1::mnuLoad_Click );
			// 
			// mnuSaveData
			// 
			this->mnuSaveData->Name = L"mnuSaveData";
			this->mnuSaveData->Size = System::Drawing::Size( 100, 22 );
			this->mnuSaveData->Text = L"&Save";
			this->mnuSaveData->Click += gcnew System::EventHandler( this, &Form1::mnuSave_Click );
			// 
			// mnuExit
			// 
			this->mnuExit->Name = L"mnuExit";
			this->mnuExit->Size = System::Drawing::Size( 100, 22 );
			this->mnuExit->Text = L"E&xit";
			this->mnuExit->Click += gcnew System::EventHandler( this, &Form1::exitToolStripMenuItem_Click );
			// 
			// mnuOptions
			// 
			this->mnuOptions->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 16 )
			{
				this->mnuClearSettings,
					this->toolStripSeparator1, this->mnuAllowBadSkills, this->mnuAllowArena, this->mnuAllowEventArmor, this->mnuAllowJapaneseOnlyDLC,
					this->mnuAllowLowerTierArmor, this->mnuAllowGunnerHelms, this->toolStripSeparator2, this->mnuMaxResults, this->mnuZoom, this->mnuPrintDecoNames,
					this->mnuPrintMaterials, this->mnuSortSkillsAlphabetically, this->mnuShowRequiredSkillPoints, this->mnuSpendSpareSlots
			} );
			this->mnuOptions->Name = L"mnuOptions";
			this->mnuOptions->Size = System::Drawing::Size( 61, 20 );
			this->mnuOptions->Text = L"&Options";
			// 
			// mnuClearSettings
			// 
			this->mnuClearSettings->Name = L"mnuClearSettings";
			this->mnuClearSettings->Size = System::Drawing::Size( 238, 22 );
			this->mnuClearSettings->Text = L"&Clear Settings";
			this->mnuClearSettings->Click += gcnew System::EventHandler( this, &Form1::mnuClearSettings_Click );
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size( 235, 6 );
			// 
			// mnuAllowBadSkills
			// 
			this->mnuAllowBadSkills->CheckOnClick = true;
			this->mnuAllowBadSkills->Name = L"mnuAllowBadSkills";
			this->mnuAllowBadSkills->Size = System::Drawing::Size( 238, 22 );
			this->mnuAllowBadSkills->Text = L"Allow &Bad Skills";
			this->mnuAllowBadSkills->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowArena
			// 
			this->mnuAllowArena->Checked = true;
			this->mnuAllowArena->CheckOnClick = true;
			this->mnuAllowArena->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowArena->Name = L"mnuAllowArena";
			this->mnuAllowArena->Size = System::Drawing::Size( 238, 22 );
			this->mnuAllowArena->Text = L"Allow &Arena Armor";
			this->mnuAllowArena->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowEventArmor
			// 
			this->mnuAllowEventArmor->Checked = true;
			this->mnuAllowEventArmor->CheckOnClick = true;
			this->mnuAllowEventArmor->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowEventArmor->Name = L"mnuAllowEventArmor";
			this->mnuAllowEventArmor->Size = System::Drawing::Size( 238, 22 );
			this->mnuAllowEventArmor->Text = L"Allow &Event Armor";
			this->mnuAllowEventArmor->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowJapaneseOnlyDLC
			// 
			this->mnuAllowJapaneseOnlyDLC->CheckOnClick = true;
			this->mnuAllowJapaneseOnlyDLC->Name = L"mnuAllowJapaneseOnlyDLC";
			this->mnuAllowJapaneseOnlyDLC->Size = System::Drawing::Size( 238, 22 );
			this->mnuAllowJapaneseOnlyDLC->Text = L"Allow &Japanese-Only DLC";
			this->mnuAllowJapaneseOnlyDLC->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowLowerTierArmor
			// 
			this->mnuAllowLowerTierArmor->Checked = true;
			this->mnuAllowLowerTierArmor->CheckOnClick = true;
			this->mnuAllowLowerTierArmor->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowLowerTierArmor->Name = L"mnuAllowLowerTierArmor";
			this->mnuAllowLowerTierArmor->Size = System::Drawing::Size( 238, 22 );
			this->mnuAllowLowerTierArmor->Text = L"Allow &Lower Tier Armor";
			this->mnuAllowLowerTierArmor->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuAllowGunnerHelms
			// 
			this->mnuAllowGunnerHelms->Checked = true;
			this->mnuAllowGunnerHelms->CheckOnClick = true;
			this->mnuAllowGunnerHelms->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuAllowGunnerHelms->Name = L"mnuAllowGunnerHelms";
			this->mnuAllowGunnerHelms->Size = System::Drawing::Size( 238, 22 );
			this->mnuAllowGunnerHelms->Text = L"Allow &Gunner Helms";
			this->mnuAllowGunnerHelms->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size( 235, 6 );
			// 
			// mnuMaxResults
			// 
			this->mnuMaxResults->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 1 ) { this->mnuNumResults } );
			this->mnuMaxResults->Name = L"mnuMaxResults";
			this->mnuMaxResults->Size = System::Drawing::Size( 238, 22 );
			this->mnuMaxResults->Text = L"&Max Results";
			// 
			// mnuNumResults
			// 
			this->mnuNumResults->Name = L"mnuNumResults";
			this->mnuNumResults->Size = System::Drawing::Size( 100, 23 );
			this->mnuNumResults->Text = L"1000";
			// 
			// mnuZoom
			// 
			this->mnuZoom->Enabled = false;
			this->mnuZoom->Name = L"mnuZoom";
			this->mnuZoom->Size = System::Drawing::Size( 238, 22 );
			this->mnuZoom->Text = L"Preview Image &Zoom";
			// 
			// mnuPrintDecoNames
			// 
			this->mnuPrintDecoNames->Checked = true;
			this->mnuPrintDecoNames->CheckOnClick = true;
			this->mnuPrintDecoNames->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuPrintDecoNames->Name = L"mnuPrintDecoNames";
			this->mnuPrintDecoNames->Size = System::Drawing::Size( 238, 22 );
			this->mnuPrintDecoNames->Text = L"Print &Decoration Names";
			this->mnuPrintDecoNames->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuPrintMaterials
			// 
			this->mnuPrintMaterials->CheckOnClick = true;
			this->mnuPrintMaterials->Name = L"mnuPrintMaterials";
			this->mnuPrintMaterials->Size = System::Drawing::Size( 238, 22 );
			this->mnuPrintMaterials->Text = L"Print &Materials";
			this->mnuPrintMaterials->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuSortSkillsAlphabetically
			// 
			this->mnuSortSkillsAlphabetically->Checked = true;
			this->mnuSortSkillsAlphabetically->CheckOnClick = true;
			this->mnuSortSkillsAlphabetically->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuSortSkillsAlphabetically->Name = L"mnuSortSkillsAlphabetically";
			this->mnuSortSkillsAlphabetically->Size = System::Drawing::Size( 238, 22 );
			this->mnuSortSkillsAlphabetically->Text = L"Sort Skills &Alphabetically";
			this->mnuSortSkillsAlphabetically->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuShowRequiredSkillPoints
			// 
			this->mnuShowRequiredSkillPoints->CheckOnClick = true;
			this->mnuShowRequiredSkillPoints->Name = L"mnuShowRequiredSkillPoints";
			this->mnuShowRequiredSkillPoints->Size = System::Drawing::Size( 238, 22 );
			this->mnuShowRequiredSkillPoints->Text = L"Show Points &Required for Skills";
			this->mnuShowRequiredSkillPoints->Click += gcnew System::EventHandler( this, &Form1::mnuShowRequiredSkillPoints_Click );
			// 
			// mnuSpendSpareSlots
			// 
			this->mnuSpendSpareSlots->Checked = true;
			this->mnuSpendSpareSlots->CheckOnClick = true;
			this->mnuSpendSpareSlots->CheckState = System::Windows::Forms::CheckState::Checked;
			this->mnuSpendSpareSlots->Name = L"mnuSpendSpareSlots";
			this->mnuSpendSpareSlots->Size = System::Drawing::Size( 238, 22 );
			this->mnuSpendSpareSlots->Text = L"&Spend spare slots on extra skills";
			this->mnuSpendSpareSlots->Click += gcnew System::EventHandler( this, &Form1::OptionsChanged );
			// 
			// mnuLanguage
			// 
			this->mnuLanguage->Name = L"mnuLanguage";
			this->mnuLanguage->Size = System::Drawing::Size( 71, 20 );
			this->mnuLanguage->Text = L"&Language";
			// 
			// mnuHelp
			// 
			this->mnuHelp->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 4 )
			{
				this->mnuSkillHelp,
					this->mnuDonate, this->mnuCheckForUpdates, this->mnuAbout
			} );
			this->mnuHelp->Name = L"mnuHelp";
			this->mnuHelp->Size = System::Drawing::Size( 44, 20 );
			this->mnuHelp->Text = L"&Help";
			// 
			// mnuSkillHelp
			// 
			this->mnuSkillHelp->Name = L"mnuSkillHelp";
			this->mnuSkillHelp->Size = System::Drawing::Size( 189, 22 );
			this->mnuSkillHelp->Text = L"&What do the skills do\?";
			this->mnuSkillHelp->Click += gcnew System::EventHandler( this, &Form1::mnuSkillHelp_Click );
			// 
			// mnuDonate
			// 
			this->mnuDonate->Font = ( gcnew System::Drawing::Font( L"Segoe UI", 9, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->mnuDonate->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->mnuDonate->Name = L"mnuDonate";
			this->mnuDonate->Size = System::Drawing::Size( 189, 22 );
			this->mnuDonate->Text = L"&Donate";
			this->mnuDonate->Click += gcnew System::EventHandler( this, &Form1::mnuDonate_Click );
			// 
			// mnuCheckForUpdates
			// 
			this->mnuCheckForUpdates->Font = ( gcnew System::Drawing::Font( L"Segoe UI", 9, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>( 0 ) ) );
			this->mnuCheckForUpdates->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->mnuCheckForUpdates->Name = L"mnuCheckForUpdates";
			this->mnuCheckForUpdates->Size = System::Drawing::Size( 189, 22 );
			this->mnuCheckForUpdates->Text = L"Check for &Updates";
			this->mnuCheckForUpdates->Click += gcnew System::EventHandler( this, &Form1::UpdateMenuItem_Click );
			// 
			// mnuAbout
			// 
			this->mnuAbout->Name = L"mnuAbout";
			this->mnuAbout->Size = System::Drawing::Size( 189, 22 );
			this->mnuAbout->Text = L"&About";
			this->mnuAbout->Click += gcnew System::EventHandler( this, &Form1::aboutToolStripMenuItem_Click );
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add( this->rdoFemale );
			this->groupBox6->Controls->Add( this->rdoMale );
			this->groupBox6->Location = System::Drawing::Point( 12, 144 );
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size( 154, 40 );
			this->groupBox6->TabIndex = 2;
			this->groupBox6->TabStop = false;
			// 
			// rdoFemale
			// 
			this->rdoFemale->AutoSize = true;
			this->rdoFemale->Checked = true;
			this->rdoFemale->Location = System::Drawing::Point( 83, 15 );
			this->rdoFemale->Name = L"rdoFemale";
			this->rdoFemale->Size = System::Drawing::Size( 59, 17 );
			this->rdoFemale->TabIndex = 1;
			this->rdoFemale->TabStop = true;
			this->rdoFemale->Text = L"Female";
			this->rdoFemale->UseVisualStyleBackColor = true;
			// 
			// rdoMale
			// 
			this->rdoMale->AutoSize = true;
			this->rdoMale->Location = System::Drawing::Point( 13, 15 );
			this->rdoMale->Name = L"rdoMale";
			this->rdoMale->Size = System::Drawing::Size( 48, 17 );
			this->rdoMale->TabIndex = 0;
			this->rdoMale->TabStop = true;
			this->rdoMale->Text = L"Male";
			this->rdoMale->UseVisualStyleBackColor = true;
			// 
			// tabHunterType
			// 
			this->tabHunterType->Controls->Add( this->tabBlademaster );
			this->tabHunterType->Controls->Add( this->tabGunner );
			this->tabHunterType->Location = System::Drawing::Point( 12, 190 );
			this->tabHunterType->Name = L"tabHunterType";
			this->tabHunterType->SelectedIndex = 0;
			this->tabHunterType->Size = System::Drawing::Size( 355, 247 );
			this->tabHunterType->TabIndex = 4;
			// 
			// tabBlademaster
			// 
			this->tabBlademaster->BackColor = System::Drawing::SystemColors::Control;
			this->tabBlademaster->Controls->Add( this->grpBSkills );
			this->tabBlademaster->Controls->Add( this->grpBSkillFilters );
			this->tabBlademaster->Location = System::Drawing::Point( 4, 22 );
			this->tabBlademaster->Name = L"tabBlademaster";
			this->tabBlademaster->Padding = System::Windows::Forms::Padding( 3 );
			this->tabBlademaster->Size = System::Drawing::Size( 347, 221 );
			this->tabBlademaster->TabIndex = 0;
			this->tabBlademaster->Text = L"Blademaster";
			// 
			// tabGunner
			// 
			this->tabGunner->BackColor = System::Drawing::SystemColors::Control;
			this->tabGunner->Controls->Add( this->grpGSkillFilters );
			this->tabGunner->Controls->Add( this->grpGSkills );
			this->tabGunner->Location = System::Drawing::Point( 4, 22 );
			this->tabGunner->Name = L"tabGunner";
			this->tabGunner->Padding = System::Windows::Forms::Padding( 3 );
			this->tabGunner->Size = System::Drawing::Size( 347, 221 );
			this->tabGunner->TabIndex = 1;
			this->tabGunner->Text = L"Gunner";
			// 
			// grpGSkillFilters
			// 
			this->grpGSkillFilters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->grpGSkillFilters->Location = System::Drawing::Point( 195, 8 );
			this->grpGSkillFilters->Name = L"grpGSkillFilters";
			this->grpGSkillFilters->Size = System::Drawing::Size( 146, 209 );
			this->grpGSkillFilters->TabIndex = 1;
			this->grpGSkillFilters->TabStop = false;
			this->grpGSkillFilters->Text = L"Skill Filters";
			// 
			// grpGSkills
			// 
			this->grpGSkills->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom )
				| System::Windows::Forms::AnchorStyles::Left ) );
			this->grpGSkills->Location = System::Drawing::Point( 6, 8 );
			this->grpGSkills->Name = L"grpGSkills";
			this->grpGSkills->Size = System::Drawing::Size( 183, 209 );
			this->grpGSkills->TabIndex = 0;
			this->grpGSkills->TabStop = false;
			this->grpGSkills->Text = L"Skills";
			// 
			// grpSortFilter
			// 
			this->grpSortFilter->Controls->Add( this->cmbFilterByExtraSkill );
			this->grpSortFilter->Controls->Add( this->cmbCharms );
			this->grpSortFilter->Controls->Add( this->cmbSort );
			this->grpSortFilter->Location = System::Drawing::Point( 172, 27 );
			this->grpSortFilter->Name = L"grpSortFilter";
			this->grpSortFilter->Size = System::Drawing::Size( 193, 100 );
			this->grpSortFilter->TabIndex = 1;
			this->grpSortFilter->TabStop = false;
			this->grpSortFilter->Text = L"Sort/Filter Results";
			// 
			// cmbFilterByExtraSkill
			// 
			this->cmbFilterByExtraSkill->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->cmbFilterByExtraSkill->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFilterByExtraSkill->FormattingEnabled = true;
			this->cmbFilterByExtraSkill->Items->AddRange( gcnew cli::array< System::Object^  >( 1 ) { L"No extra skill filtering" } );
			this->cmbFilterByExtraSkill->Location = System::Drawing::Point( 6, 70 );
			this->cmbFilterByExtraSkill->Name = L"cmbFilterByExtraSkill";
			this->cmbFilterByExtraSkill->Size = System::Drawing::Size( 181, 21 );
			this->cmbFilterByExtraSkill->TabIndex = 2;
			this->cmbFilterByExtraSkill->SelectedIndexChanged += gcnew System::EventHandler( this, &Form1::cmbFilterByExtraSkill_SelectedIndexChanged );
			// 
			// cmbCharms
			// 
			this->cmbCharms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->cmbCharms->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbCharms->FormattingEnabled = true;
			this->cmbCharms->Items->AddRange( gcnew cli::array< System::Object^  >( 2 ) { L"None", L"All" } );
			this->cmbCharms->Location = System::Drawing::Point( 6, 43 );
			this->cmbCharms->Name = L"cmbCharms";
			this->cmbCharms->Size = System::Drawing::Size( 181, 21 );
			this->cmbCharms->TabIndex = 1;
			this->cmbCharms->SelectedIndexChanged += gcnew System::EventHandler( this, &Form1::cmbCharms_SelectedIndexChanged );
			// 
			// cmbSort
			// 
			this->cmbSort->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->cmbSort->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSort->FormattingEnabled = true;
			this->cmbSort->Items->AddRange( gcnew cli::array< System::Object^  >( 13 )
			{
				L"None", L"Dragon res", L"Fire res", L"Ice res",
					L"Thunder res", L"Water res", L"Base defence", L"Max defence", L"Difficulty", L"Rarity", L"Slots spare", L"Family", L"Extra Skills"
			} );
			this->cmbSort->Location = System::Drawing::Point( 6, 16 );
			this->cmbSort->Name = L"cmbSort";
			this->cmbSort->Size = System::Drawing::Size( 181, 21 );
			this->cmbSort->TabIndex = 0;
			this->cmbSort->SelectedIndexChanged += gcnew System::EventHandler( this, &Form1::cmbSort_SelectedIndexChanged );
			// 
			// grpCharms
			// 
			this->grpCharms->Controls->Add( this->cmbCharmSelect );
			this->grpCharms->Controls->Add( this->btnCharms );
			this->grpCharms->Location = System::Drawing::Point( 172, 129 );
			this->grpCharms->Name = L"grpCharms";
			this->grpCharms->Size = System::Drawing::Size( 193, 77 );
			this->grpCharms->TabIndex = 3;
			this->grpCharms->TabStop = false;
			this->grpCharms->Text = L"Charms";
			// 
			// cmbCharmSelect
			// 
			this->cmbCharmSelect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>( ( ( System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left )
				| System::Windows::Forms::AnchorStyles::Right ) );
			this->cmbCharmSelect->ContextMenuStrip = this->cmsCharms;
			this->cmbCharmSelect->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbCharmSelect->FormattingEnabled = true;
			this->cmbCharmSelect->Items->AddRange( gcnew cli::array< System::Object^  >( 5 )
			{
				L"Use no charms", L"Use my charms", L"Use only slotted charms",
					L"Use up to one skill charms", L"Use up to two skill charms"
			} );
			this->cmbCharmSelect->Location = System::Drawing::Point( 6, 19 );
			this->cmbCharmSelect->Name = L"cmbCharmSelect";
			this->cmbCharmSelect->Size = System::Drawing::Size( 181, 21 );
			this->cmbCharmSelect->TabIndex = 0;
			// 
			// cmsCharms
			// 
			this->cmsCharms->Name = L"cmsCharms";
			this->cmsCharms->Size = System::Drawing::Size( 61, 4 );
			// 
			// cmsSkills
			// 
			this->cmsSkills->Items->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 1 ) { this->mnuClearSkill } );
			this->cmsSkills->Name = L"cmsSkills";
			this->cmsSkills->Size = System::Drawing::Size( 102, 26 );
			// 
			// mnuClearSkill
			// 
			this->mnuClearSkill->Name = L"mnuClearSkill";
			this->mnuClearSkill->Size = System::Drawing::Size( 101, 22 );
			this->mnuClearSkill->Text = L"&Clear";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF( 6, 13 );
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size( 729, 512 );
			this->Controls->Add( this->grpSortFilter );
			this->Controls->Add( this->grpResults );
			this->Controls->Add( this->groupBox1 );
			this->Controls->Add( this->menuStrip1 );
			this->Controls->Add( this->grpCharms );
			this->Controls->Add( this->groupBox6 );
			this->Controls->Add( this->tabHunterType );
			this->Controls->Add( this->groupBox4 );
			this->Controls->Add( this->progressBar1 );
			this->Icon = ( cli::safe_cast<System::Drawing::Icon^>( resources->GetObject( L"$this.Icon" ) ) );
			this->Name = L"Form1";
			this->Text = L"Athena\'s ASS for ";
			this->groupBox1->ResumeLayout( false );
			this->groupBox1->PerformLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudWeaponSlots ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudHR ) )->EndInit();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->nudElder ) )->EndInit();
			this->groupBox4->ResumeLayout( false );
			this->grpResults->ResumeLayout( false );
			this->menuStrip1->ResumeLayout( false );
			this->menuStrip1->PerformLayout();
			this->groupBox6->ResumeLayout( false );
			this->groupBox6->PerformLayout();
			this->tabHunterType->ResumeLayout( false );
			this->tabBlademaster->ResumeLayout( false );
			this->tabGunner->ResumeLayout( false );
			this->grpSortFilter->ResumeLayout( false );
			this->grpCharms->ResumeLayout( false );
			this->cmsSkills->ResumeLayout( false );
			this->ResumeLayout( false );
			this->PerformLayout();

		}
#pragma endregion

private: 
	System::Void FormulateQuery( const bool danger, const bool use_gunner_skills, List_t< Skill^ >^ skills )
	{
		query = gcnew Query;
		for( int i = 0; i < int( Armor::ArmorType::NumArmorTypes ); ++i )
		{
			query->rel_armor.Add( gcnew List_t< Armor^ >() );
			query->inf_armor.Add( gcnew List_t< Armor^ >() );
		}
		query->weapon_slots_allowed = int( nudWeaponSlots->Value );
		query->elder_star = int( nudElder->Value );
		query->hr = int( nudHR->Value );

		query->gender = rdoMale->Checked ? Gender::MALE : Gender::FEMALE;
		query->hunter_type = !use_gunner_skills ? HunterType::BLADEMASTER : HunterType::GUNNER;
		query->include_arena = mnuAllowArena->Checked;
		query->allow_bad = mnuAllowBadSkills->Checked;
		query->allow_event = mnuAllowEventArmor->Checked;
		query->allow_japs = mnuAllowJapaneseOnlyDLC->Checked;
		query->allow_lower_tier = mnuAllowLowerTierArmor->Checked;
		query->add_extra = mnuSpendSpareSlots->Checked;
		query->allow_gunner_helms = mnuAllowGunnerHelms->Checked;
		query->neset_disabled = false;
		
		query->want_taunt = false;
		for each( Skill^ s in skills )
		{
			if( s->is_taunt )
				query->want_taunt = true;
		}

		query->skills.AddRange( skills );

		data->GetRelevantData( query );
	}

	System::Void FormulateQuery( const bool danger, const bool use_gunner_skills )
	{
		List_t< Skill^ > skills;

		if( !use_gunner_skills )
		{
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				if( bSkills[ i ]->SelectedIndex >= 0 )
				{
					Map_t< unsigned, unsigned >^ imap = bIndexMaps[ i ];
					skills.Add( data->FindSkill( imap[ bSkills[ i ]->SelectedIndex ] ) );
				}
			}
		}
		else
		{
			for( unsigned i = 0; i < NumSkills; ++i )
			{
				if( gSkills[ i ]->SelectedIndex >= 0 )
				{
					Map_t< unsigned, unsigned >^ imap = gIndexMaps[ i ];
					skills.Add( data->FindSkill( imap[ gSkills[ i ]->SelectedIndex ] ) );
				}
			}
		}

		FormulateQuery( danger, use_gunner_skills, %skills );
	}

	System::Void UpdateImpossibleSkills()
	{
		if( Skill::UpdateImpossibleSkills( (int)nudHR->Value, (int)nudElder->Value ) )
		{
			for each( ComboBox^ cb in bSkillFilters )
				cmbSkillFilter_SelectedIndexChanged( cb, nullptr );
			for each( ComboBox^ cb in gSkillFilters )
				cmbSkillFilter_SelectedIndexChanged( cb, nullptr );
		}
	}

	System::Void MaxResultsTextBoxKeyPress( System::Object^ sender, KeyPressEventArgs^ e )
	{
		//ignore all keypresses that aren't numeric digits or control chars
		if( !Char::IsControl( e->KeyChar ) &&
			!Char::IsDigit( e->KeyChar ) )
			e->Handled = true;
	}

	System::Void MaxResultsTextChanged( System::Object^ sender, EventArgs^ e )
	{
		try
		{
			MAX_LIMIT = Convert::ToUInt32( mnuNumResults->Text );
		}
		catch( Exception^ )
		{
			return;
		}
		SaveConfig();
	}

	System::Void OptionsChanged( System::Object^ sender, System::EventArgs^ e )
	{
		SaveConfig();
		
		if( sender == mnuPrintDecoNames ||
			sender == mnuPrintMaterials )
			cmbCharms_SelectedIndexChanged( nullptr, nullptr );

		else if( sender == mnuSortSkillsAlphabetically )
		{
			for each( ComboBox^ cb in bSkillFilters )
				cmbSkillFilter_SelectedIndexChanged( cb, nullptr );
			for each( ComboBox^ cb in gSkillFilters )
				cmbSkillFilter_SelectedIndexChanged( cb, nullptr );
		}
	}

	System::Void QueueTask( Query^ query, Charm^ ct )
	{
		BackgroundWorker^ new_thread = gcnew BackgroundWorker;
		new_thread->WorkerSupportsCancellation = true;
		new_thread->WorkerReportsProgress = true;
		new_thread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::backgroundWorker1_DoWork);
		new_thread->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorker1_RunWorkerCompleted);
		new_thread->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::backgroundWorker1_ProgressChanged);

		ThreadSearchData^ tsd = gcnew ThreadSearchData;
		tsd->charm_template = ct;
		tsd->query = query;
		worker_data.Add( tsd );

		workers.Add( new_thread );
	}

	System::Void QueueTask2( Query^ query, List_t< Charm^ >^ cts )
	{
		if( query->neset_disabled )
			return;

		BackgroundWorker^ new_thread = gcnew BackgroundWorker;
		new_thread->WorkerSupportsCancellation = true;
		new_thread->WorkerReportsProgress = true;
		new_thread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::backgroundWorker1_DoWork2);
		new_thread->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorker1_RunWorkerCompleted);
		new_thread->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::backgroundWorker1_ProgressChanged);

		ThreadSearchData2^ tsd = gcnew ThreadSearchData2;
		tsd->charm_templates = cts;
		tsd->query = query;
		worker_data.Add( tsd );

		workers.Add( new_thread );
	}

	void EnableSearchControls( const bool enabled )
	{
		btnSearch->Enabled = enabled;
		btnAdvancedSearch->Enabled = enabled;
		btnCancel->Enabled = !enabled;
		btnCharms->Enabled = enabled;
		mnuMaxResults->Enabled = enabled;
		mnuNumResults->Enabled = enabled;
		cmbSort->Enabled = enabled;
		cmbCharms->Enabled = enabled;
		cmbFilterByExtraSkill->Enabled = enabled;
	}

	void StartTasks()
	{
		worker_mutex->WaitOne();

		worker_start_index = Math::Max( 1, Math::Min( Environment::ProcessorCount, workers.Count ) );
		//worker_start_index = workers.Count;

		if( workers.Count > 0 )
			for( unsigned i = 0; i < worker_start_index; ++i )
				workers[ i ]->RunWorkerAsync( worker_data[ i ] );

		worker_mutex->ReleaseMutex();
	}

	System::Void StartSearch()
	{
		progressBar1->Value = 0;
		total_progress = 0;
		num_updates = 0;
		search_cancelled = false;
		if( query->skills.Count > 0 )
		{
			existing_armor.Clear();
			existing_no_charm_armor.Clear();
			EnableSearchControls( false );
			charm_solution_map.Clear();
			cmbCharms->SelectedIndex = 1;
			txtSolutions->Text = StartString( SolutionsFound ) + L"0";
			final_solutions.Clear();
			all_solutions.Clear();
			no_charm_solutions.Clear();
			workers.Clear();
			worker_data.Clear();
			last_result = nullptr;
			last_search_gunner = tabHunterType->SelectedIndex == 1;
			finished_workers = 0;

			List_t< Charm^ >^ to_search;

			switch( cmbCharmSelect->SelectedIndex )
			{
			case 0: // no charms
				{
					//do nothing
				}
				break;
			case 1: // my charms only
				{
					to_search = gcnew List_t< Charm^ >();
					
					for each( Charm^ ct in CharmDatabase::mycharms )
					{
						Charm::AddToOptimalList( *to_search, ct );
					}
				}
				break;
			case 2: // slotted charms only
				{
					to_search = gcnew List_t< Charm^ >();

					//if( CharmDatabase::have_slots[ cmbCharmTable->SelectedIndex, 1 ] & 0x2 )
					if( query->elder_star > 2 )
						to_search->Add( gcnew Charm( 1 ) );
					//if( ( query->elder_star > 6 || query->hr >= 3 ) && CharmDatabase::have_slots[ cmbCharmTable->SelectedIndex, 2 ] & 0x4 )
					if( query->hr > 3 || query->elder_star > 6 )
						to_search->Add( gcnew Charm( 2 ) );
					//if( ( query->elder_star > 8 || query->hr >= 4 ) && CharmDatabase::have_slots[ cmbCharmTable->SelectedIndex, 3 ] & 0x8 )
					if( query->hr > 4 || query->elder_star > 7 )
						to_search->Add( gcnew Charm( 3 ) );

					if( to_search->Count > 0 )
						QueueTask( query, nullptr );
				}
				break;
			case 3: // one skill charms
				{
					to_search = CharmDatabase::GetCharms( query, false );
					Assert( to_search, L"No one-skill charms found" );
				}
				break;
			case 4: // two skill charms
				{
					to_search = CharmDatabase::GetCharms( query, true );
					Assert( to_search, L"No two-skill charms found" );
				}
				break;
			}

			if( !to_search || to_search->Count == 0 )
			{
				if( query->AllowNesetArmor() )
					QueueTask2( query, nullptr );

				QueueTask( query, nullptr );
			}
			else
			{
				if( query->AllowNesetArmor() )
					QueueTask2( query, to_search );

				for each( Charm^ ct in to_search )
				{
					QueueTask( query, ct );
				}
			}

			StartTasks();
		}
	}

	System::Void btnAdvancedSearch_Click( System::Object^ sender, System::EventArgs^ e )
	{
		FormulateQuery( true, tabHunterType->SelectedIndex == 1 );

		array< Armor^ >^ neset_armors = query->hunter_type == HunterType::GUNNER ? Armor::charm_up_armors_g : Armor::charm_up_armors_b;
		Assert( neset_armors->Length == int( Armor::ArmorType::NumArmorTypes ), L"Wrong number of Neset armors" );

		if( query->AllowNesetArmor() )
		{
			for( int i = 0; i < neset_armors->Length; ++i )
			{
				query->rel_armor[ i ]->Add( neset_armors[ i ] );
				query->inf_armor[ i ]->Add( neset_armors[ i ] );
			}
		}

		Advanced advanced_search( query );
		advanced_search.Width = adv_x;
		advanced_search.Height = adv_y;
		advanced_search.ShowDialog( this );

		adv_x = advanced_search.Width;
		adv_y = advanced_search.Height;
		SaveConfig();

		if( advanced_search.DialogResult != System::Windows::Forms::DialogResult::OK )
			return;

		if( query->AllowNesetArmor() )
		{
			query->neset_disabled = false;
			for( int i = 0; i < neset_armors->Length; ++i )
			{
				if( neset_armors[ i ]->force_disable )
				{
					query->neset_disabled = true;
					break;
				}
			}
		}

		for( int p = 0; p < int( Armor::ArmorType::NumArmorTypes ); ++p )
		{
			query->rel_armor[ p ]->Clear();
			List_t< Armor^ >^ inf_armor = query->inf_armor[ p ];
			inf_armor->Remove( neset_armors[ p ] );
			for( int i = 0; i < inf_armor->Count; ++i )
			{
				if( advanced_search.boxes[ p ]->Items[ i ]->Checked )
					query->rel_armor[ p ]->Add( inf_armor[ i ] );
			}
		}
		query->rel_decorations.Clear();
		for( int i = 0; i < query->inf_decorations.Count; ++i )
		{
			if( advanced_search.boxes[ int( Armor::ArmorType::NumArmorTypes ) ]->Items[ i ]->Checked )
				query->rel_decorations.Add( query->inf_decorations[ i ] );
		}

		StartSearch();
	}

	System::Void btnSearch_Click( System::Object^ sender, System::EventArgs^ e )
	{		
		FormulateQuery( false, tabHunterType->SelectedIndex == 1 );
		
		StartSearch();
	}

	System::Void btnCancel_Click( System::Object^ sender, System::EventArgs^ e )
	{
		search_cancelled = true;
		for each( BackgroundWorker^ worker in workers )
			worker->CancelAsync();
		EnableSearchControls( true );
		progressBar1->Value = 0;
	}

	System::Void cmbSkillFilter_SelectedIndexChanged( System::Object^ sender, List_t< ComboBox^ >% skills, List_t< ComboBox^ >% skill_filters, List_t< IndexMap^ >% index_maps )
	{
		List_t< Ability^ > old_skills;
		int index = -1;
		Skill^ selected_skill = nullptr;
		for( unsigned i = 0; i < NumSkills; ++i )
		{
			if( sender == skill_filters[ i ] )
				index = i;
			//Assert( skills[ i ]->SelectedIndex != 0, L"What happen" );
			if( skills[ i ]->SelectedIndex < 1 )
				continue;

			Map_t< unsigned, unsigned >^ imap = index_maps[ i ];
			Skill^ skill = Skill::static_skills[ imap[ skills[ i ]->SelectedIndex ] ];
			if( sender == skill_filters[ i ] )
				selected_skill = skill;
			else old_skills.Add( skill->ability );
		}
		if( index == -1 )
			return;

		if( skill_filters[ index ]->SelectedIndex == 2 )
			FindRelatedSkills( skills, skill_filters, index_maps );

		skills[ index ]->BeginUpdate();
		lock_related = true;
		InitSkills( skills[ index ], index_maps[ index ], skill_filters[ index ]->SelectedIndex, %old_skills, %skill_filters == %bSkillFilters );
		ResetSkill( skills[ index ], index_maps[ index ], selected_skill );
		lock_related = false;
		skills[ index ]->EndUpdate();
	}

	System::Void cmbSkillFilter_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ )
	{
		if( tabHunterType->SelectedIndex == 0 )
		{
			cmbSkillFilter_SelectedIndexChanged( sender, bSkills, bSkillFilters, bIndexMaps );
		}
		else if( tabHunterType->SelectedIndex == 1 )
		{
			cmbSkillFilter_SelectedIndexChanged( sender, gSkills, gSkillFilters, gIndexMaps );
		}
	}

	void cmbSkill_SelectedIndexChanged( System::Object^ sender, List_t< ComboBox^ >% skills, List_t< ComboBox^ >% skill_filters, List_t< IndexMap^ >% index_maps, List_t< Ability^ >% last_abilities )
	{
		int index = -1;
		for( int i = 0; i < NumSkills; ++i )
		{
			if( sender == skills[ i ] )
			{
				index = i;
				break;
			}
		}
		if( index == -1 )
			return;

		Skill^ selected_skill = nullptr;
		if( skills[ index ]->SelectedIndex < 1 )
		{
			last_abilities[ index ] = nullptr;
		}
		else
		{
			Map_t< unsigned, unsigned >^ imap = index_maps[ index ];
			selected_skill = Skill::static_skills[ imap[ skills[ index ]->SelectedIndex ] ];
			
			//if( last_abilities[ index ] == selected_skill->ability )
			//	return;

			last_abilities[ index ] = selected_skill->ability;
		}

		lock_skills = true;

		if( skills[ index ]->SelectedIndex == 0 && !skills[ index ]->DroppedDown )
		{
			SuspendDrawing( skills[ index ] );
			skills[ index ]->SelectedIndex = -1;
			ResumeDrawing( skills[ index ] );
		}

		if( !lock_related )
			FindRelatedSkills( skills, skill_filters, index_maps );
		
		for( int i = 0; i < NumSkills; ++i )
		{
			if( i == index && skill_filters[ i ]->SelectedIndex != 2 )
				continue;

			Map_t< unsigned, unsigned >^ imap = index_maps[ i ];
			Skill^ skill = skills[ i ]->SelectedIndex <= 0 ? nullptr : Skill::static_skills[ imap[ skills[ i ]->SelectedIndex ] ];
			List_t< Ability^ > old_skills;
			for( int j = 0; j < NumSkills; ++j )
			{
				if( skills[ j ]->SelectedIndex <= 0 )
					continue;

				Map_t< unsigned, unsigned >^ jmap = index_maps[ j ];
				Skill^ os = Skill::static_skills[ jmap[ skills[ j ]->SelectedIndex ] ];
				if( j != i )
					old_skills.Add( os->ability );


				if( i == index && i == j )
					continue;
			}

			skills[ i ]->BeginUpdate();
			InitSkills( skills[ i ], index_maps[ i ], skill_filters[ i ]->SelectedIndex, %old_skills, %skill_filters == %bSkillFilters );
			ResetSkill( skills[ i ], index_maps[ i ], skill );
			skills[ i ]->EndUpdate();
		}
		lock_skills = false;

		//check compound skills aren't overlapping their component skills
		for( int i = 0; i < NumSkills; ++i )
		{
			Map_t< unsigned, unsigned >^ imap = index_maps[ i ];
			Skill^ skill = skills[ i ]->SelectedIndex <= 0 ? nullptr : Skill::static_skills[ imap[ skills[ i ]->SelectedIndex ] ];
			if( i != index && skill && selected_skill )
			{
				if( Skill::CompoundSkillOverrides( skill, selected_skill ) ||
					Skill::CompoundSkillOverrides( selected_skill, skill ) )
					skills[ i ]->SelectedIndex = -1;
			}
		}
		return;
	}

	System::Void cmbSkill_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e ) 
	{
		if( lock_skills )
			return;

		if( tabHunterType->SelectedIndex == 0 )
		{
			cmbSkill_SelectedIndexChanged( sender, bSkills, bSkillFilters, bIndexMaps, b_last_selected_ability );
		}
		else if( tabHunterType->SelectedIndex == 1 )
		{
			cmbSkill_SelectedIndexChanged( sender, gSkills, gSkillFilters, gIndexMaps, g_last_selected_ability );
		}
	}

	unsigned UpdateResultDecorations( System::Text::StringBuilder% sb, List_t< Decoration^ >% decorations, String^ suffix )
	{
		Generic::Dictionary< Decoration^, unsigned > deco_dict;
		for each( Decoration^ decoration in decorations )
		{
			if( !deco_dict.ContainsKey( decoration ) )
				deco_dict.Add( decoration, 1 );
			else deco_dict[ decoration ]++;
		}
		
		Generic::Dictionary< Decoration^, unsigned >::Enumerator iter = deco_dict.GetEnumerator();
		while( iter.MoveNext() )
		{
			sb.Append( Convert::ToString( iter.Current.Value ) )->Append( L"x " );
			if( mnuPrintDecoNames->Checked )
				sb.AppendLine( iter.Current.Key->name + suffix );
			else
			{
				sb.AppendLine( FormatString2( JewelFormat, L"+" + Convert::ToString( iter.Current.Key->abilities[ 0 ]->amount ), iter.Current.Key->abilities[ 0 ]->ability->name ) + suffix );
			}
		}
		return deco_dict.Count;
	}

	System::Void UpdateResultString( List_t< Solution^ >^ solutions )
	{
		if( !StringTable::text )
			return;

		result_offsets.Clear();
		unsigned offset = 1;
		if( solutions->Count > MAX_LIMIT )
			offset++;

		System::Text::StringBuilder sb( solutions->Count * 1024 );

		if( last_result ) sb.Append( last_result );
		
		System::String^ dash = L"-----------------";
		for each( Solution^ solution in solutions )
		{
			sb.Append( endl );
			result_offsets.Add( ++offset );
		
			for each( Armor^ armor in solution->armors )
			{
				if( armor )
				{
					sb.Append( armor->name );
					if( armor->no_skills )
					{
						if( armor->num_slots == 1 )
							sb.Append( StaticString( OrAnythingWithSingular ) );
						else
							sb.Append( FormatStringN( OrAnythingWithPlural, armor->num_slots ) );
					}
					else if( armor->torso_inc )
						sb.Append( StaticString( OrAnythingWithTorsoInc ) );
					sb.Append( endl );
					offset++;
				}
			}
			if( solution->charm )
			{
				sb.AppendLine( dash );
				sb.AppendLine( solution->charm->GetName() );
				offset += 2;
			}
			if( solution->decorations.Count > 0 )
			{
				sb.AppendLine( dash );
				offset++;

				if( solution->torso_multiplier > 1 )
					offset += UpdateResultDecorations( sb, solution->body_decorations, StaticString( InTorso ) );

				if( solution->charm_up )
					offset += UpdateResultDecorations( sb, solution->charm_decorations, StaticString( InCharm ) );

				if( solution->torso_multiplier > 1 || solution->charm_up )
					offset += UpdateResultDecorations( sb, solution->non_body_decorations, "" );
				else
					offset += UpdateResultDecorations( sb, solution->decorations, "" );
			}
			if( solution->total_slots_spare > 0 || cmbSort->SelectedIndex == 10 )
			{
				if( solution->total_slots_spare == 1 )
				{
					sb.AppendLine( StaticString( SlotSpare ) );
				}
				else if( solution->total_slots_spare == 0 )
				{
					sb.AppendLine( FormatString1( SlotsSpare, L"0" ) );
				}
				else if( !solution->slots_spare || ( solution->total_slots_spare <= 3 && solution->slots_spare[ solution->total_slots_spare ] == 1 ) )
				{
					sb.AppendLine( FormatStringN( SlotsSpare, solution->total_slots_spare ) );
				}
				else
				{
					sb.Append( FormatStringN( SlotsSpare, solution->total_slots_spare ) );
					sb.Append( L" (" );
					bool first = true;
					for( unsigned i = 1; i <= 3; ++i )
						for( unsigned j = 0; j < solution->slots_spare[ i ]; ++j )
						{
							sb.Append( ( first ? L"" : L"+" ) + Convert::ToString( i ) );
							first = false;
						}
					sb.AppendLine( L")" );
				}
				offset++;
			}
			if( cmbSort->SelectedIndex > 0 && cmbSort->SelectedIndex < 10 )
			{
				if( cmbSort->SelectedIndex < 8 )
				{
					sb.AppendLine( dash );
					sb.AppendLine( FormatString7( DefEleAbbrev, solution->defence, solution->max_defence, solution->fire_res, solution->water_res, solution->ice_res, solution->thunder_res, solution->dragon_res ) );
					offset += 2;
				}
				else
				{
					if( cmbSort->SelectedIndex == 8 ) sb.Append( solution->difficulty );
					else if( cmbSort->SelectedIndex == 9 ) sb.Append( solution->rarity );
					sb.Append( L" " )->AppendLine( (String^)cmbSort->SelectedItem );
					offset++;
				}
			}
			if( solution->extra_skills.Count > 0 || solution->potential_extra_skills.Count > 0 )
			{
				sb.AppendLine( dash );
				offset++;
				for each( Skill^ skill in solution->extra_skills )
				{
					if( !skill->autodisabled && !Utility::Contains( %query->skills, skill ) )
					{
						sb.AppendLine( skill->name );
						offset++;
					}
				}
				for each( Skill^ skill in solution->potential_extra_skills )
				{
					if( !skill->autodisabled && !Utility::Contains( %query->skills, skill ) )
					{
						sb.AppendLine( FormatString1( SkillPossible, skill->name ) );
						offset++;
					}
				}
			}
			if( cmbSort->SelectedIndex == 12 ) //extra skills
			{
				bool first = true;
				AbilityMap::Enumerator e = solution->abilities.GetEnumerator();
				while( e.MoveNext() )
				{
					Skill^ sk = e.Current.Key->GetWorstGoodSkill();
					if( sk && sk->autodisabled  )
						continue;

					if( e.Current.Value < 10 && e.Current.Value > 3 ||
						Utility::Contains( %solution->potential_extra_skills, e.Current.Key ) )
					{
						if( first )
						{
							first = false;
							sb.AppendLine( dash );
							offset++;
						}
						sb.Append( L"+" )->Append( e.Current.Value )->Append( L" " )->AppendLine( e.Current.Key->name );
						offset++;
					}
				}
			}

			if( mnuPrintMaterials->Checked )
			{
				sb.AppendLine( dash );
				offset++;
				Generic::SortedDictionary< String^, int > materials;
				for each( Armor^ armor in solution->armors )
					for each( MaterialComponent^ mat in armor->components )
					{
						if( materials.ContainsKey( mat->material->name ) )
							materials[ mat->material->name ] += mat->amount;
						else
							materials.Add( mat->material->name, mat->amount );
					}
				for each( Decoration^ deco in solution->decorations )
					for each( MaterialComponent^ mat in deco->components )
					{
						if( materials.ContainsKey( mat->material->name ) )
							materials[ mat->material->name ] += mat->amount;
						else
							materials.Add( mat->material->name, mat->amount );
					}

				Generic::SortedDictionary< String^, int >::Enumerator e = materials.GetEnumerator();
				while( e.MoveNext() )
				{
					sb.Append( e.Current.Value )->Append( L"x " )->AppendLine( e.Current.Key );
					offset++;
				}
			}
		}

		if( %final_solutions != solutions )
			final_solutions.AddRange( solutions );

		System::Text::StringBuilder sb2;
		sb2.Append( StartString( SolutionsFound ) )->AppendLine( Convert::ToString( final_solutions.Count ) );

		if( solutions->Count > MAX_LIMIT )
		{
			sb2.AppendLine( FormatStringN( ShowingFirstSolutionsOnly, MAX_LIMIT ) );
		}

		sb2.Append( last_result = sb.ToString() );
		
		results_mutex->WaitOne();
		//if( updating_language || language >= 0 && mnuLanguage->DropDownItems[ language ]->ToString() == L"Japanese" )
		{
			//noobery to ensure entire textbox is correct font
			SuspendDrawing( txtSolutions );
			txtSolutions->Text = sb2.ToString();
			txtSolutions->SelectionStart = 0;
			txtSolutions->SelectionLength = txtSolutions->Text->Length;
			txtSolutions->SelectionFont = gcnew Drawing::Font( L"Microsoft Sans Serif", txtSolutions->Font->Size );
			txtSolutions->SelectionLength = 0;
			ResumeDrawing( txtSolutions );
		}
		//else txtSolutions->Text = sb2.ToString();
		
		results_mutex->ReleaseMutex();
	}

#pragma region Worker Thread Stuff

	__int64 HashArmor( List_t< Armor^ >% armors )
	{
		__int64 res = 0;
		for( int i = 0; i < armors.Count; ++i )
		{
			res += armors[ i ] ? ( armors[ i ]->index << ( i * 3 ) ) : 0;
		}
		return res;
	}

	bool SolutionExists( String^ charm, Solution^ sol )
	{
		if( !existing_armor.ContainsKey( charm ) )
		{
			existing_armor.Add( charm, gcnew Generic::Dictionary< __int64, bool > );
			existing_armor[ charm ]->Add( HashArmor( sol->armors ), true );
			return false;
		}
		else
		{
			Generic::Dictionary< __int64, bool >^ set = existing_armor[ charm ];
			const __int64 armor_hash = HashArmor( sol->armors );
			if( set->ContainsKey( armor_hash ) )
				return true;
			else
			{
				set->Add( armor_hash, true );
				return false;
			}
		}
	}

	System::Void AddSolution( String^ charm, Solution^ sol )
	{
		if( SolutionExists( charm, sol ) )
			return;

		if( !charm_solution_map.ContainsKey( charm ) )
			charm_solution_map.Add( charm, gcnew List_t< Solution^ > );

		charm_solution_map[ charm ]->Add( sol );
		all_solutions.Add( sol );
	}

	System::Void AddSolutions( List_t< Solution^ >^ solutions )
	{
		charm_map_mutex->WaitOne();
		for each( Solution^ sol in solutions )
		{
			if( all_solutions.Count >= MaxSolutions )
			{
				charm_map_mutex->ReleaseMutex();
				search_cancelled = true;
				for each( BackgroundWorker^ worker in workers )
					worker->CancelAsync();
				return;
			}

			if( sol->charm )
				AddSolution( sol->charm->GetName(), sol );
			else
			{
				const __int64 armor_hash = HashArmor( sol->armors );
				if( !existing_no_charm_armor.ContainsKey( armor_hash ) )
				{
					existing_no_charm_armor.Add( armor_hash, true );
					all_solutions.Add( sol );
					no_charm_solutions.Add( sol );
				}
			}
		}
		charm_map_mutex->ReleaseMutex();
	}

	System::Void cmbFilterByExtraSkill_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if( solutions_extra_skills.Count == 0 || updating_extra_skills || updating_language )
			return;

		cmbCharms_SelectedIndexChanged( nullptr, nullptr );
	}

	System::Void UpdateExtraSkillCombo( const bool preserve_selected_index )
	{
		updating_extra_skills = true;

		Skill^ prev_selected = nullptr;
		if( preserve_selected_index && cmbFilterByExtraSkill->SelectedIndex > 0 && cmbFilterByExtraSkill->SelectedIndex - 1 < solutions_extra_skills.Count )
		{
			prev_selected = solutions_extra_skills[ cmbFilterByExtraSkill->SelectedIndex - 1 ];
		}

		cmbFilterByExtraSkill->BeginUpdate();
		
		solutions_extra_skills.Clear();
		cmbFilterByExtraSkill->Items->Clear();
		cmbFilterByExtraSkill->Items->Add( StaticString( NoExtraSkillFiltering ) );

		for each( Solution^ sol in all_solutions )
		{
			for each( Skill^ sk in sol->extra_skills )
			{
				if( !sk->autodisabled &&
					( sk->is_taunt || sk->points_required > 0 ) &&
					!Utility::Contains( %solutions_extra_skills, sk ) )
					solutions_extra_skills.Add( sk );
			}

			for each( Skill^ sk in sol->potential_extra_skills )
			{
				if( !sk->autodisabled &&
					!Utility::Contains( %solutions_extra_skills, sk ) )
					solutions_extra_skills.Add( sk );
			}
		}

		solutions_extra_skills.Sort( gcnew Comparison< Skill^ >( CompareSkillsByName ) );

		for each( Skill^ sk in solutions_extra_skills )
		{
			cmbFilterByExtraSkill->Items->Add( sk->name );
		}

		if( prev_selected )
		{
			for( int i = 0; i < solutions_extra_skills.Count; ++i )
			{
				if( solutions_extra_skills[ i ] == prev_selected )
				{
					cmbFilterByExtraSkill->SelectedIndex = i + 1;
					break;
				}
			}
		}
		else cmbFilterByExtraSkill->SelectedIndex = 0;
		cmbFilterByExtraSkill->EndUpdate();

		updating_extra_skills = false;
	}

	System::Void UpdateCharmComboBox()
	{
		UpdateCharmComboBox( -1 );
	}

	System::Void UpdateCharmComboBox( const int new_index )
	{
		Charm^ selected_charm = cmbCharms->SelectedIndex > 1 ? charm_box_charms[ cmbCharms->SelectedIndex - 2 ] : nullptr;
		const int old_index = cmbCharms->SelectedIndex;
		charm_box_charms.Clear();
		List_t< String^ > to_order;
		Map_t< String^, Charm^ > charm_map;
		for each( Solution^ s in all_solutions )
		{
			if( s->charm )
			{
				String^ name = s->charm->GetName();
				if( !charm_map.ContainsKey( name ) )
				{
					to_order.Add( name );
					charm_map.Add( name, s->charm );
				}
			}
		}
		to_order.Sort();
		cmbCharms->BeginUpdate();
		cmbCharms->Items->Clear();
		cmbCharms->Items->Add( BasicString( FilterByNone ) );
		cmbCharms->Items->Add( StaticString( All ) );
		for each( String^% str in to_order )
		{
			charm_box_charms.Add( charm_map[ str ] );
			cmbCharms->Items->Add( str );
		}
		if( new_index == -1 )
		{
			if( old_index == -1 )
				cmbCharms->SelectedIndex = 1;
			else if( old_index < 2 )
				cmbCharms->SelectedIndex = old_index;
			else
			{
				for( int i = 2; i < cmbCharms->Items->Count; ++i )
				{
					if( cmbCharms->Items[ i ]->ToString() == selected_charm->GetName() )
					{
						cmbCharms->SelectedIndex = i;
						break;
					}
				}
			}
		}
		else cmbCharms->SelectedIndex = new_index;
		cmbCharms->EndUpdate();
	}

	System::Void backgroundWorker1_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
	{
		if( e->Error != nullptr )
		{
			MessageBox::Show( e->Error->Message );
			progressBar1->Value = 0;
			return;
		}
		else if( e->Cancelled )
		{
			progressBar1->Value = 0;
			return;
		}
		else if( e->Result )
		{
			if( !search_cancelled )
			{
				worker_mutex->WaitOne();
				if( worker_start_index < unsigned( workers.Count ) )
				{
					workers[ worker_start_index ]->RunWorkerAsync( worker_data[ worker_start_index ] );
					worker_start_index++;
				}
				worker_mutex->ReleaseMutex();
			}

			AddSolutions( static_cast< List_t< Solution^ >^ >( e->Result ) );
			progress_mutex->WaitOne();
			if( ++finished_workers >= worker_start_index )
			{
				EnableSearchControls( true );
				progressBar1->Value = 100;
				UpdateExtraSkillCombo( false );
				SaveConfig();
				UpdateCharmComboBox( 1 );
			}
			else txtSolutions->Text = StartString( SolutionsFound ) + Convert::ToString( all_solutions.Count );
			progress_mutex->ReleaseMutex();
		}
	}		
	

	System::Void backgroundWorker1_ProgressChanged( Object^ /*sender*/, ProgressChangedEventArgs^ e )
	{
		progress_mutex->WaitOne();
		num_updates++;
		total_progress += e->ProgressPercentage;
		progressBar1->Value = total_progress / workers.Count;
		progress_mutex->ReleaseMutex();
		if( e->UserState )
		{
			AddSolutions( static_cast< List_t< Solution^ >^ >( e->UserState ) );
			txtSolutions->Text = StartString( SolutionsFound ) + Convert::ToString( all_solutions.Count );
		}
	}

	System::Void backgroundWorker1_DoWork2( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e )
	{
		try
		{
			BackgroundWorker^ worker = static_cast< BackgroundWorker^ >( sender );
			ThreadSearchData2^ qc2 = static_cast< ThreadSearchData2^ >( e->Argument );
			List_t< Solution ^ >^ solutions = gcnew List_t< Solution^ >;
			array< Armor^ >^ armors = ( query->hunter_type == HunterType::GUNNER ) ? Armor::charm_up_armors_g : Armor::charm_up_armors_b;
			if( qc2->charm_templates )
			{
				for each( Charm^ charm_template in qc2->charm_templates )
				{
					Solution^ good = backgroundWorker1_Search( worker, e, qc2->query, charm_template, armors );
					if( good )
						solutions->Add( good );
				}
			}
			else
			{
				Solution^ good = backgroundWorker1_Search( worker, e, qc2->query, nullptr, armors );
				if( good )
					solutions->Add( good );
			}

			worker->ReportProgress( 100 );
			e->Result = solutions;
		}
		catch( Exception^ e )
		{
			Assert( false, L"Worker thread exception2: " + e->ToString() );
		}
	}

	Solution^ backgroundWorker1_Search( BackgroundWorker^ worker, System::ComponentModel::DoWorkEventArgs^ e, Query^ query, Charm^ charm_template, array< Armor^ >^ armors )
	{
		Solution^ job = gcnew Solution();
		job->armors.AddRange( armors );
		job->charm = charm_template ? gcnew Charm( charm_template ) : nullptr;

		try
		{
			if( job->MatchesQuery( query ) )
				return job;
		}
		catch( Exception^ e )
		{
			MessageBox::Show( e->ToString() );
		}
		return nullptr;
	}

	System::Void backgroundWorker1_DoWork( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e )
	{
		try
		{
			BackgroundWorker^ worker = static_cast< BackgroundWorker^ >( sender );
			ThreadSearchData^ qc = static_cast< ThreadSearchData^ >( e->Argument );
			List_t< Solution ^ >^ solutions = gcnew List_t< Solution^ >;
			List_t< Armor^ >^ head  = query->rel_armor[ int( Armor::ArmorType::HEAD ) ];
			List_t< Armor^ >^ body  = query->rel_armor[ int( Armor::ArmorType::BODY ) ];
			List_t< Armor^ >^ arms  = query->rel_armor[ int( Armor::ArmorType::ARMS ) ];
			List_t< Armor^ >^ waist = query->rel_armor[ int( Armor::ArmorType::WAIST ) ];
			List_t< Armor^ >^ legs  = query->rel_armor[ int( Armor::ArmorType::LEGS ) ];
	 
			if( head->Count  == 0 ) head->Add( nullptr );
			if( body->Count  == 0 ) body->Add( nullptr );
			if( arms->Count  == 0 ) arms->Add( nullptr );
			if( waist->Count == 0 ) waist->Add( nullptr );
			if( legs->Count  == 0 ) legs->Add( nullptr );
			
			//int solutions_found = 0;
			unsigned last_percent = 0;
			bool new_stuff = false;
			for( int i = 0; i < head->Count; ++i )
			{
				for( int j = 0; j < body->Count; ++j )
				{
					const int progress = ( 100 * ( i * body->Count + j ) ) / ( head->Count * body->Count );
				
					if( !new_stuff )
					{
						worker->ReportProgress( progress - last_percent );
						last_percent = progress;
					}
					else
					{
						List_t< Solution ^ >^ temp = solutions; //handle race-condition: shouldn't modify solution list while iterating through it
						solutions = gcnew List_t< Solution^ >;
						worker->ReportProgress( progress - last_percent, temp );
						last_percent = progress;
						new_stuff = false;
					}
					for( int k = 0; k < arms->Count; ++k )
					{
						for( int l = 0; l < waist->Count; ++l )
						{
							for( int m = 0; m < legs->Count; ++m )
							{
								if( worker->CancellationPending )
								{
									e->Result = solutions;
									return;
								}
								array< Armor^ >^ search_armors = { head[i], body[j], arms[k], waist[l], legs[m] };
								Solution^ good = backgroundWorker1_Search( worker, e, query, qc->charm_template, search_armors );
								if( good )
								{
									solutions->Add( good );
									new_stuff = true;
									/*if( ++solutions_found >= MAX_LIMIT )
									{
										e->Result = solutions;
										worker->ReportProgress( 100 - last_percent );
										return;
									}*/
								}
							}
						}
					}
				}
			}
			worker->ReportProgress( 100 - last_percent );
			e->Result = solutions;
		}
		catch( Exception^ e )
		{
			Assert( false, L"Worker thread exception: " + e->ToString() );
		}
	}
#pragma endregion

	System::Void FindDialogClosed( System::Object^ sender, System::EventArgs^ e )
	{
		find_dialog = nullptr;
	}

	System::Void FindDialogFoundText( System::Object^ sender, System::EventArgs^ e )
	{
		Find^ find = (Find^)sender;
		if( find->index == -1 )
		{
			txtSolutions->SelectionStart = txtSolutions->Text->Length;
			txtSolutions->SelectionLength = 0;
		}
		else
		{
			txtSolutions->SelectionStart = find->index;
			txtSolutions->SelectionLength = find->length;
		}
		txtSolutions->ScrollToCaret();
		txtSolutions->Focus();
	}

	System::Void KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if( sender == txtSolutions && e->Control )
		{
			if( e->KeyValue == L'A' )
			{
				txtSolutions->SelectAll();
				e->Handled = true;
				e->SuppressKeyPress = true;
			}
			else if( e->KeyValue == L'F' && !find_dialog )
			{
				find_dialog = gcnew Find( txtSolutions );
				find_dialog->DialogClosing += gcnew EventHandler( this, &Form1::FindDialogClosed );
				find_dialog->TextFound += gcnew EventHandler( this, &Form1::FindDialogFoundText );
				find_dialog->Show( this );
				e->Handled = true;
				e->SuppressKeyPress = true;
			}
			else if( e->KeyValue == L'X' )
			{
				
			}
		}
	}

	System::Void cmbSort_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		static int last_index = -1;
		if( cmbSort->SelectedIndex == last_index ) return;
		last_index = cmbSort->SelectedIndex;
		if( data )
		{
			SaveConfig();
			if( final_solutions.Count > 0 )
			{
				SortResults();
				last_result = nullptr;
				UpdateResultString( %final_solutions );
			}
		}
	}

	System::Void UpdateComboBoxLanguage( ComboBox^ cb )
	{
		cb->BeginUpdate();
		for( int i = 1; i < cb->Items->Count; ++i )
		{
			String^ name = (String^)cb->Items[ i ];
			Skill^ skill = Skill::FindSkill( name );
			Assert( skill, L"No such skill!" );
			cb->Items[ i ] = skill->name;
		}
		cb->EndUpdate();
	}

	System::Void LanguageSelect_Click( System::Object^ sender, System::EventArgs^ e )
	{
		const int old_language = language;
		for( int index = 0; index < mnuLanguage->DropDownItems->Count; ++index )
		{
			ToolStripMenuItem^ item = static_cast< ToolStripMenuItem^ >( mnuLanguage->DropDownItems[ index ] );
			if( mnuLanguage->DropDownItems[ index ] == sender )
			{
				if( index == language )
					return;
				language = index;
				item->Checked = true;
			}
			else item->Checked = false;
		}
		if( old_language == language )
			return;

		updating_language = true;

		StringTable::LoadLanguage( static_cast< ToolStripMenuItem^ >( sender )->ToString() );

		for each( ComboBox^ box in bSkillFilters )
		{
			box->BeginUpdate();
			for( int i = 0, j = 0; i < SkillTag::tags.Count; ++i )
				if( !SkillTag::tags[ i ]->disable_b )
					box->Items[ j++ ] = SkillTag::tags[ i ]->name;
			box->EndUpdate();
		}
		for each( ComboBox^ box in gSkillFilters )
		{
			box->BeginUpdate();
			for( int i = 0, j = 0; i < SkillTag::tags.Count; ++i )
				if( !SkillTag::tags[ i ]->disable_g )
					box->Items[ j++ ] = SkillTag::tags[ i ]->name;
			box->EndUpdate();
		}

#define UpdateMenu( X ) mnu##X->Text = StaticString( X )
		UpdateMenu( File );
		UpdateMenu( Options );
		UpdateMenu( Language );
		UpdateMenu( Help );
		UpdateMenu( LoadData );
		UpdateMenu( SaveData );
		UpdateMenu( Exit );
		UpdateMenu( ClearSettings );
		UpdateMenu( AllowBadSkills );
		UpdateMenu( AllowEventArmor );
		UpdateMenu( AllowLowerTierArmor );
		UpdateMenu( ShowRequiredSkillPoints );
		UpdateMenu( AllowJapaneseOnlyDLC );
		UpdateMenu( Donate );
		UpdateMenu( AllowGunnerHelms );
		UpdateMenu( AllowArena );
		UpdateMenu( CheckForUpdates );
		UpdateMenu( SpendSpareSlots );
		UpdateMenu( About );
		UpdateMenu( MaxResults );
		UpdateMenu( PrintDecoNames );
		UpdateMenu( SortSkillsAlphabetically );
		UpdateMenu( SkillHelp );
		UpdateMenu( Zoom );
		UpdateMenu( PrintMaterials );
#undef UpdateMenu
		nudHR->UpdateEditText();
		nudElder->UpdateEditText();
		lblHR->Text = StaticString( HR );
		lblElder->Text = StaticString( VillageQuests );
		lblSlots->Text = StaticString( MaxWeaponSlots );
		grpSortFilter->Text = StaticString( SortFilterResults );
		grpCharms->Text = StaticString( Charms );
		grpResults->Text = StaticString( Results );
		rdoMale->Text = StaticString( Male );
		rdoFemale->Text = StaticString( Female );
		tabBlademaster->Text = StaticString( Blademaster );
		tabGunner->Text = StaticString( Gunner );
		grpBSkills->Text = StaticString( Skills );
		grpGSkills->Text = StaticString( Skills );
		grpBSkillFilters->Text = StaticString( SkillFilters );
		grpGSkillFilters->Text = StaticString( SkillFilters );
		btnCharms->Text = StaticString( MyCharms );
		btnSearch->Text = StaticString( QuickSearch );
		btnAdvancedSearch->Text = StaticString( AdvancedSearch );
		btnCancel->Text = StaticString( Cancel );
		cmbCharmSelect->Items[ 0 ] = StaticString( UseNoCharms );
		cmbCharmSelect->Items[ 1 ] = StaticString( UseOnlyMyCharms );
		cmbCharmSelect->Items[ 2 ] = StaticString( UseOnlySlottedCharms );
		cmbCharmSelect->Items[ 3 ] = StaticString( UseUpToOneSkillCharms );
		cmbCharmSelect->Items[ 4 ] = StaticString( UseOnlyTwoSkillCharms );
		cmbSort->Items[ 0 ] = BasicString( SortByNone );
		cmbSort->Items[ 1 ] = StaticString( DragonRes );
		cmbSort->Items[ 2 ] = StaticString( FireRes );
		cmbSort->Items[ 3 ] = StaticString( IceRes );
		cmbSort->Items[ 4 ] = StaticString( ThunderRes );
		cmbSort->Items[ 5 ] = StaticString( WaterRes );
		cmbSort->Items[ 6 ] = StaticString( BaseDefence );
		cmbSort->Items[ 7 ] = StaticString( MaxDefence );
		cmbSort->Items[ 8 ] = StaticString( Difficulty );
		cmbSort->Items[ 9 ] = StaticString( Rarity );
		cmbSort->Items[ 10 ] = StaticString( SortSlotsSpare );
		cmbSort->Items[ 11 ] = StaticString( SortFamily );
		cmbSort->Items[ 12 ] = StaticString( SortExtraSkills );

		charm_solution_map.Clear();
		for each( Solution^ s in all_solutions )
		{
			if( s->charm )
			{
				String^ str = s->charm->GetName();
				if( !charm_solution_map.ContainsKey( str ) )
					charm_solution_map.Add( str, gcnew List_t< Solution^ > );
				charm_solution_map[ str ]->Add( s );
			}
		}
		
		for( int i = 0; i < NumSkills; ++i )
		{
			bSkills[ i ]->BeginUpdate();
			cmbSkillFilter_SelectedIndexChanged( bSkillFilters[ i ], bSkills, bSkillFilters, bIndexMaps );	
			bSkills[ i ]->EndUpdate();
			gSkills[ i ]->BeginUpdate();
			cmbSkillFilter_SelectedIndexChanged( gSkillFilters[ i ], gSkills, gSkillFilters, gIndexMaps );
			gSkills[ i ]->EndUpdate();
		}
		
		UpdateCharmComboBox();
		UpdateExtraSkillCombo( true );

		if( construction_complete )
		{
			CharmDatabase::SaveCustom();
		}
		
		updating_language = false;
		cmbCharms_SelectedIndexChanged( nullptr, nullptr );
	}

	System::Void aboutToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		About about_form;
		about_form.ShowDialog( this );
	}

	System::Void SortResults()
	{
		if( cmbSort->SelectedIndex < 1 || sort_off ) return;
		else if( cmbSort->SelectedIndex == 1 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDragonRes ) );
		else if( cmbSort->SelectedIndex == 2 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByFireRes ) );
		else if( cmbSort->SelectedIndex == 3 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByIceRes ) );
		else if( cmbSort->SelectedIndex == 4 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByThunderRes ) );
		else if( cmbSort->SelectedIndex == 5 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByWaterRes ) );
		else if( cmbSort->SelectedIndex == 6 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDefence ) );
		else if( cmbSort->SelectedIndex == 7 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByMaxDefence ) );
		else if( cmbSort->SelectedIndex == 8 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByDifficulty ) );
		else if( cmbSort->SelectedIndex == 9 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByRarity ) );
		else if( cmbSort->SelectedIndex == 10 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionBySlotsSpare ) );
		else if( cmbSort->SelectedIndex == 11 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionByFamily ) );
		else if( cmbSort->SelectedIndex == 12 )
			final_solutions.Sort( gcnew Comparison< Solution^ >( CompareSolutionsByExtraSkills ) );
	}

	bool EndsWithSlots( String^% s )
	{
		return s->EndsWith( L" ---" ) ||
			   s->EndsWith( L"O--" ) ||
			   s->EndsWith( L"OO-" ) ||
			   s->EndsWith( L"OOO" );
	}

	void contextMenuStrip1_Closing( System::Object^ sender, ToolStripDropDownClosingEventArgs^ e )
	{
		if( preview_pane )
		{
			preview_pane->Close();
			preview_pane = nullptr;
		}
	}

	System::Void contextMenuStrip1_Opening( System::Object^ sender, CancelEventArgs^ e )
	{
		if( txtSolutions->Text == L"" )
			return;

		cmsSolutions->Items->Clear();
		e->Cancel = true;

		Point pos = txtSolutions->PointToClient( txtSolutions->MousePosition );
		int chr = txtSolutions->GetCharIndexFromPosition( pos );
		int line = txtSolutions->GetLineFromCharIndex( chr );
		String^ str = txtSolutions->Lines[ line ];
		if( str == L"" )
			return;
		
		//check for decorations
		if( str->Length >= 4 )
		{
			if( str->Substring( 1, 2 ) == L"x " || str->Substring( 2, 2 ) == L"x " )
			{
				String^ deco_name = str->Substring( str->IndexOf( L' ' ) + 1 );
				if( deco_name->EndsWith( StaticString( InTorso ) ) )
					deco_name = deco_name->Substring( 0, deco_name->Length - StaticString( InTorso )->Length );
				else if( deco_name->EndsWith( StaticString( InCharm ) ) )
					deco_name = deco_name->Substring( 0, deco_name->Length - StaticString( InCharm )->Length );
				Decoration^ decoration = Decoration::FindDecoration( deco_name );
				if( decoration )
				{
					Utility::UpdateContextMenu( cmsSolutions, decoration );
					e->Cancel = false;
				}
				else 
				{
					Decoration^ decoration = Decoration::FindDecorationFromString( deco_name );
					if( decoration )
					{
						Utility::UpdateContextMenu( cmsSolutions, decoration );
						e->Cancel = false;
					}
				}
				return;
			}
		}
		//check for armors
		for( int i = 0; i < result_offsets.Count; ++i )
		{
			if( result_offsets[ i ] > line )
				break;

			int offset = result_offsets[ i ];
			Solution^ solution = final_solutions[ i ];

			int num_armors = 0;
			for each( Armor^ a in solution->armors )
			{
				if( a )
					num_armors++;
			}
			
			if( line >= offset &&
				line < offset + num_armors )
			{
				for( int a = 0, c = 0; a < solution->armors.Count; ++a )
				{
					if( solution->armors[ a ] && c++ == line - offset )
					{
						Utility::UpdateContextMenu( cmsSolutions, solution->armors[ a ] );
						break;
					}
				}

				if( preview_pane )
					preview_pane->Close();

				preview_pane = gcnew PreviewImage( rdoFemale->Checked );
				
				solution->preview_image = preview_pane->SetData( solution->armors, solution->preview_image );
				if( solution->preview_image )
				{
					preview_pane->Show( this );
					preview_pane->SetLocation( cmsSolutions );
				}
				else preview_pane = nullptr;

				e->Cancel = false;
				return;
			}
			else if( solution->charm && line == offset + num_armors + 1 )
			{
				Assert( str == solution->charm->GetName(), L"Unexpected charm string" );
				Utility::UpdateContextMenu( cmsSolutions, solution->charm, 0 );
				e->Cancel = false;
				return;
			}
		}

		//check for extra skills
		{
			//full skills
			Skill^ extra = Skill::FindSkill( str );
			if( extra )
			{
				Utility::UpdateContextMenu( cmsSolutions, extra->ability );
				e->Cancel = false;
				return;
			}
			//partial abilities
			else if( str->StartsWith( L"+" ) )
			{
				const int space = str->IndexOf( L" " );
				if( space >= 0 && space + 1 < str->Length )
				{
					Ability^ a = Ability::FindAbility( str->Substring( space + 1 ) );
					if( a )
					{
						Utility::UpdateContextMenu( cmsSolutions, a );
						e->Cancel = false;
						return;
					}
				}
			}
			
			//check potential extra skills
			{
				String^ pots =  StaticString( SkillPossible );
				const int i = pots->IndexOf( "%1" );
				if( i >= 0 )
				{
					const unsigned prefix = pots->Substring( 0, i )->Length;
					const unsigned suffix = pots->Substring( i + 2 )->Length;
					const int len = str->Length - prefix - suffix;
					if( len > 0 )
					{
						String^ skillname = str->Substring( prefix, len );
						extra = Skill::FindSkill( skillname );
						if( extra )
						{
							Utility::UpdateContextMenu( cmsSolutions, extra->ability );
							e->Cancel = false;
						}
					}
				}
			}
		}
	}

	System::Void cmbCharms_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if( updating_language || cmbCharms->SelectedIndex < 0 )
			return;
		last_result = L"";
		final_solutions.Clear();
		String^ charm = (String^)cmbCharms->SelectedItem;
		List_t< Solution^ >^ to_add = nullptr;
		if( cmbCharms->SelectedIndex == 0 ) // None
		{
			to_add = %no_charm_solutions;
		}
		else if( cmbCharms->SelectedIndex == 1 ) // All
		{
			to_add = %all_solutions;
		}
		else if( charm_solution_map.ContainsKey( charm ) )
		{
			to_add = charm_solution_map[ charm ];
		}
		else
			UpdateResultString( gcnew List_t< Solution^ > );

		if( to_add )
		{
			if( cmbFilterByExtraSkill->SelectedIndex < 1 || cmbFilterByExtraSkill->SelectedIndex - 1 >= solutions_extra_skills.Count )
			{
				final_solutions.AddRange( to_add );
				
			}
			else
			{
				Skill^ selected_extra_skill = solutions_extra_skills[ cmbFilterByExtraSkill->SelectedIndex - 1 ];
				for each( Solution^ s in to_add )
				{
					if( Utility::Contains( %s->extra_skills, selected_extra_skill ) ||
						Utility::Contains( %s->potential_extra_skills, selected_extra_skill ) )
						final_solutions.Add( s );
				}
			}
			SortResults();
			UpdateResultString( %final_solutions );
		}
	}

	System::Void btnCharms_Click( System::Object^ sender, System::EventArgs^ e )
	{
		ManageCharms mycharms( language, data, mnuSortSkillsAlphabetically->Checked );
		mycharms.ShowDialog( this );
		UpdateImpossibleSkills();
	}
	
	System::Void UpdateMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		System::Diagnostics::Process::Start( "https://www.facebook.com/AthenasArmorSetSearch/" );
	}

	System::Void mnuDonate_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::Diagnostics::Process::Start( "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=8A7BDLKN7ZK6W&lc=NZ&item_name=Athena%27s%20Armor%20Set%20Search&currency_code=NZD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted" );
	}

	System::Void mnuClearSettings_Click( System::Object^ sender, System::EventArgs^ e )
	{
		cmbSort->SelectedIndex = 0;

		SuspendUpdate::Suspend( tabHunterType );

		lock_skills = true;

		for each( ComboBox^ cb in bSkills )
		{
			cb->SelectedIndex = -1;
		}
		for each( ComboBox^ cb in gSkills )
		{
			cb->SelectedIndex = -1;
		}

		lock_skills = false;

		for each( ComboBox^ cb in bSkillFilters )
		{
			if( cb->SelectedIndex == 0 )
				cmbSkillFilter_SelectedIndexChanged( cb, bSkills, bSkillFilters, bIndexMaps );
			else
				cb->SelectedIndex = 0;
		}
		for each( ComboBox^ cb in gSkillFilters )
		{
			if( cb->SelectedIndex == 0 )
				cmbSkillFilter_SelectedIndexChanged( cb, gSkills, gSkillFilters, gIndexMaps );
			else
			cb->SelectedIndex = 0;
		}

		SuspendUpdate::Resume( tabHunterType );

		OptionsChanged( sender, e );
	}

	System::Void nudHR_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		UpdateImpossibleSkills();
	}

	System::Void nudElder_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		UpdateImpossibleSkills();
	}
	
	System::Void mnuLoad_Click( System::Object^ sender, System::EventArgs^ e )
	{
		OpenFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".ass";
		dlg.AddExtension = true;
		dlg.Filter = StartString( ASSSettings ) + L"(*.ass)|*.ass";
		dlg.FilterIndex = 0;
		Windows::Forms::DialogResult res = dlg.ShowDialog();
		if( res == Windows::Forms::DialogResult::OK )
		{
			LoadConfig( dlg.FileName, true );
			LoadConfig( dlg.FileName, false );
		}
	}
	
	System::Void mnuSave_Click( System::Object^ sender, System::EventArgs^ e )
	{
		SaveFileDialog dlg;
		dlg.InitialDirectory = System::Environment::CurrentDirectory;
		dlg.DefaultExt = L".ass";
		dlg.AddExtension = true;
		dlg.Filter = StartString( ASSSettings ) + L"(*.ass)|*.ass";
		dlg.FilterIndex = 0;
		dlg.FileName = L"results.ass";
		Windows::Forms::DialogResult res = dlg.ShowDialog();
		if( res == Windows::Forms::DialogResult::OK )
			SaveConfig( dlg.FileName );
	}

	void SkillHelp_FormClosed( System::Object^ sender, FormClosedEventArgs^ e )
	{
		help_window = nullptr;
	}

	System::Void mnuSkillHelp_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if( help_window != nullptr )
		{
			help_window->SwitchModes( sender == mnuSkillHelp );
			help_window->Focus();
			return;
		}

		help_window = gcnew SkillHelp( -1, sender == mnuSkillHelp, mnuSortSkillsAlphabetically->Checked, rdoFemale->Checked );
		help_window->FormClosed += gcnew FormClosedEventHandler( this, &Form1::SkillHelp_FormClosed );
		help_window->Show( this );
	}

	System::Void mnuShowRequiredSkillPoints_Click(System::Object^  sender, System::EventArgs^  e)
	{
		SuspendUpdate::Suspend( tabHunterType );

		for( int i = 0; i < NumSkills; ++i )
		{
			cmbSkillFilter_SelectedIndexChanged( bSkillFilters[ i ], bSkills, bSkillFilters, bIndexMaps );
			cmbSkillFilter_SelectedIndexChanged( gSkillFilters[ i ], gSkills, gSkillFilters, gIndexMaps );
		}

		SuspendUpdate::Resume( tabHunterType );

		OptionsChanged( sender, e );
	}

	System::Void ZoomLevelChanged( System::Object^ sender, System::EventArgs^ e )
	{
		ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( sender );
		if( !item )
			return;

		for each( ToolStripMenuItem^ m in mnuZoom->DropDownItems )
			if( m != sender )
				m->Checked = false;
		item->Checked = true;
	
		PreviewImage::zoom = (int)(Int32)item->Tag;
		SaveConfig();
	}

	System::Void exitToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->Close();
	}
};
}
