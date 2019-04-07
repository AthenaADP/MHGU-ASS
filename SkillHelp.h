#pragma once
#include "Skill.h"
#include "Armor.h"
#include "PreviewImage.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MHGUASS
{
	ref struct SubSkill
	{
		Skill^ skill;
		int char_start, char_end;
	};


	template< class T >
	int SortByOrder( T^ a, T^ b )
	{
		return a->order - b->order;
	}

	public ref class SkillHelp : public System::Windows::Forms::Form
	{
		bool skill_mode, alphabetic, default_female;
		int armor_list_begin;
		Generic::List< Armor^ > armors;
		Generic::List< unsigned > armor_types;
		Generic::List< Skill^ > skills;
		Generic::List< Ability^ > abilities;
		Ability^ last_selected_ability;
		PreviewImage^ preview_pane;

		Generic::List< SubSkill^ > subskills;

	public:
		SkillHelp( const int static_index, const bool skill_based, const bool alphabetic, const bool default_female )
		{
			InitializeComponent();

			txtDescription->Text = StaticString( Description );
			
			rdoSkills->Text = StaticString( SkillsHelp );
			
			rdoAbilities->Location = Point( 19 + rdoSkills->Location.X + rdoSkills->Width, rdoAbilities->Location.Y );
			rdoAbilities->Text = StaticString( SkillTrees );

			grpShow->Size = Drawing::Size( 22 + rdoAbilities->Location.X + rdoAbilities->Width, grpShow->Size.Height );
			grpShow->Text = StaticString( Show );

			grpSkills->Text = StaticString( Skills );
			grpSkills->Size = Drawing::Size( grpShow->Size.Width, grpSkills->Size.Height );

			grpDescription->Text = StaticString( Description );
			grpDescription->Location = Point( 6 + grpShow->Location.X + grpShow->Size.Width, grpDescription->Location.Y );
			grpDescription->Size = Drawing::Size( this->Width - grpDescription->Location.X - 28, grpDescription->Size.Height );

			this->Text = BasicString( SkillHelp );

			this->alphabetic = alphabetic;
			this->default_female = default_female;

			//Setup skills
			for each( Skill^ skill in Skill::static_skills )
			{
				if( skill->points_required > 0 )
					skills.Add( skill );
			}
			if( alphabetic )
				skills.Sort( gcnew Comparison< Skill^ >( SortByOrder< Skill > ) );
			
			//Setup abilities
			last_selected_ability = nullptr;
			abilities.AddRange( %Ability::static_abilities );
			if( alphabetic )
				abilities.Sort( gcnew Comparison< Ability^ >( SortByOrder< Ability > ) );

			SwitchModes( skill_based );

			if( static_index >= 0 && static_index < listBox1->Items->Count )
				SelectInfo( static_index, skill_mode );
		}

		System::Void SelectInfo( const int static_index, const bool new_mode )
		{
			if( new_mode != skill_mode )
				SwitchModes( new_mode );

			if( skill_mode )
				listBox1->SelectedIndex = skills.IndexOf( Skill::static_skills[ static_index ] );
			else
				listBox1->SelectedIndex = abilities.IndexOf( Ability::static_abilities[ static_index ] );
		}

		System::Void SwitchModes( const bool use_skills )
		{
			if( use_skills )
				rdoSkills->Checked = true;
			else
				rdoAbilities->Checked = true;
		}

	protected:

		~SkillHelp()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::GroupBox^  grpSkills;
	private: System::Windows::Forms::GroupBox^  grpDescription;
	private: System::Windows::Forms::GroupBox^  grpShow;
	private: System::Windows::Forms::RichTextBox^  txtDescription;
	private: System::Windows::Forms::RadioButton^  rdoSkills;
	private: System::Windows::Forms::RadioButton^  rdoAbilities;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::ContextMenuStrip^  cmsDescription;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->grpSkills = (gcnew System::Windows::Forms::GroupBox());
			this->grpDescription = (gcnew System::Windows::Forms::GroupBox());
			this->txtDescription = (gcnew System::Windows::Forms::RichTextBox());
			this->cmsDescription = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->grpShow = (gcnew System::Windows::Forms::GroupBox());
			this->rdoSkills = (gcnew System::Windows::Forms::RadioButton());
			this->rdoAbilities = (gcnew System::Windows::Forms::RadioButton());
			this->grpSkills->SuspendLayout();
			this->grpDescription->SuspendLayout();
			this->grpShow->SuspendLayout();
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(6, 19);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(158, 342);
			this->listBox1->TabIndex = 0;
			this->listBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &SkillHelp::listBox1_SelectedIndexChanged);
			// 
			// grpSkills
			// 
			this->grpSkills->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->grpSkills->Controls->Add(this->listBox1);
			this->grpSkills->Location = System::Drawing::Point(12, 65);
			this->grpSkills->Name = L"grpSkills";
			this->grpSkills->Size = System::Drawing::Size(170, 380);
			this->grpSkills->TabIndex = 1;
			this->grpSkills->TabStop = false;
			this->grpSkills->Text = L"Skills";
			// 
			// grpDescription
			// 
			this->grpDescription->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpDescription->Controls->Add(this->txtDescription);
			this->grpDescription->Location = System::Drawing::Point(188, 12);
			this->grpDescription->Name = L"grpDescription";
			this->grpDescription->Size = System::Drawing::Size(720, 433);
			this->grpDescription->TabIndex = 2;
			this->grpDescription->TabStop = false;
			this->grpDescription->Text = L"Description";
			// 
			// txtDescription
			// 
			this->txtDescription->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->txtDescription->ContextMenuStrip = this->cmsDescription;
			this->txtDescription->Location = System::Drawing::Point(6, 19);
			this->txtDescription->Name = L"txtDescription";
			this->txtDescription->ReadOnly = true;
			this->txtDescription->Size = System::Drawing::Size(708, 408);
			this->txtDescription->TabIndex = 0;
			this->txtDescription->Text = L"";
			this->txtDescription->WordWrap = false;
			// 
			// cmsDescription
			// 
			this->cmsDescription->Name = L"cmsDescription";
			this->cmsDescription->Size = System::Drawing::Size(61, 4);
			this->cmsDescription->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &SkillHelp::cmsDescription_Opening);
			this->cmsDescription->Closing += gcnew ToolStripDropDownClosingEventHandler( this, &SkillHelp::cmsDescription_Closing );
			// 
			// grpShow
			// 
			this->grpShow->Controls->Add(this->rdoSkills);
			this->grpShow->Controls->Add(this->rdoAbilities);
			this->grpShow->Location = System::Drawing::Point(12, 12);
			this->grpShow->Name = L"grpShow";
			this->grpShow->Size = System::Drawing::Size(170, 47);
			this->grpShow->TabIndex = 3;
			this->grpShow->TabStop = false;
			this->grpShow->Text = L"Show";
			// 
			// rdoSkills
			// 
			this->rdoSkills->AutoSize = true;
			this->rdoSkills->Location = System::Drawing::Point(6, 19);
			this->rdoSkills->Name = L"rdoSkills";
			this->rdoSkills->Size = System::Drawing::Size(49, 17);
			this->rdoSkills->TabIndex = 0;
			this->rdoSkills->TabStop = true;
			this->rdoSkills->Text = L"Skills";
			this->rdoSkills->UseVisualStyleBackColor = true;
			this->rdoSkills->CheckedChanged += gcnew System::EventHandler(this, &SkillHelp::RadioButtonChanged);
			// 
			// rdoAbilities
			// 
			this->rdoAbilities->AutoSize = true;
			this->rdoAbilities->Location = System::Drawing::Point(74, 19);
			this->rdoAbilities->Name = L"rdoAbilities";
			this->rdoAbilities->Size = System::Drawing::Size(74, 17);
			this->rdoAbilities->TabIndex = 1;
			this->rdoAbilities->TabStop = true;
			this->rdoAbilities->Text = L"Skill Trees";
			this->rdoAbilities->UseVisualStyleBackColor = true;
			this->rdoAbilities->CheckedChanged += gcnew System::EventHandler(this, &SkillHelp::RadioButtonChanged);
			// 
			// SkillHelp
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(920, 453);
			this->Controls->Add(this->grpShow);
			this->Controls->Add(this->grpDescription);
			this->Controls->Add(this->grpSkills);
			this->Name = L"SkillHelp";
			this->Text = L"Skill Help";
			this->grpSkills->ResumeLayout(false);
			this->grpDescription->ResumeLayout(false);
			this->grpShow->ResumeLayout(false);
			this->grpShow->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

		System::Void SelectInfoInternal( String^ name )
		{
			if( skill_mode )
			{
				Skill^ skill = Skill::FindSkill( name );
				if( skill == nullptr )
					return;
				last_selected_ability = skill->ability;
			}
			else
			{
				Ability^ ability = Ability::FindAbility( name );
				if( ability == nullptr )
				{
					if( name == SpecificAbility::torso_inc->name )
						ability = SpecificAbility::torso_inc;
					else return;
				}
				last_selected_ability = ability;
			}

			armors.Clear();
			armor_types.Clear();
			armor_list_begin = -1;
			subskills.Clear();

			SuspendDrawing( txtDescription );

			txtDescription->Text = last_selected_ability->name;

			txtDescription->AppendText( Environment::NewLine );
			txtDescription->AppendText( Environment::NewLine );

			txtDescription->SelectionStart = 0;
			txtDescription->SelectionLength = last_selected_ability->name->Length;
			txtDescription->SelectionColor = Drawing::Color::Blue;

			if( last_selected_ability == SpecificAbility::torso_inc )
			{
				txtDescription->Text = SpecificAbility::torso_inc_desc;
				txtDescription->AppendText( Environment::NewLine );
			}
			else
			{
				Ability::SkillMap_t::Enumerator e = last_selected_ability->skills.GetEnumerator();
				while( e.MoveNext() )
				{
					const int sel_start = txtDescription->TextLength;
					if( e.Current.Key > 0 )
						txtDescription->AppendText( L"+" );

					txtDescription->AppendText( e.Current.Key + L": " );
					const int name_start = txtDescription->TextLength;
					txtDescription->AppendText( e.Current.Value->name );
					const int name_end = txtDescription->TextLength;

					String^ desc = e.Current.Value->description;

					int subskill_start = desc->IndexOf( L"<<" );
					while( subskill_start != -1 )
					{
						int subskill_end = desc->IndexOf( L">>", subskill_start + 2 );
						if( subskill_end == -1 )
							break;

						SubSkill^ ss = gcnew SubSkill();

						String^ skill_name = desc->Substring( subskill_start + 2, subskill_end - subskill_start - 2 );
						ss->skill = Skill::FindSkill( skill_name );
						Assert( ss->skill, "Subskill not found: " + skill_name );
						if( ss->skill == nullptr )
							break;

						ss->char_start = subskill_start;
						ss->char_end = subskill_end - 2;
						
						subskills.Add( ss );

						desc = desc->Substring( 0, subskill_start ) + skill_name + desc->Substring( subskill_end + 2 );
						subskill_start = desc->IndexOf( L"<<" );
					}

					txtDescription->AppendText( L".  " );

					const int desc_start = txtDescription->TextLength;
					
					txtDescription->AppendText( desc );
					txtDescription->AppendText( Environment::NewLine );

					txtDescription->SelectionStart = sel_start;
					txtDescription->SelectionLength = txtDescription->TextLength - sel_start;
					txtDescription->SelectionColor = ( e.Current.Key >= 0 ) ? Drawing::Color::Black : Drawing::Color::Red;

					txtDescription->SelectionStart = name_start;
					txtDescription->SelectionLength = name_end - name_start;
					txtDescription->SelectionColor = e.Current.Value->points_required < 0 ? Drawing::Color::DarkBlue : Drawing::Color::Blue;

					for each( SubSkill^ ss in subskills )
					{
						ss->char_end += desc_start;
						ss->char_start += desc_start;

						txtDescription->SelectionStart = ss->char_start;
						txtDescription->SelectionLength = ss->char_end - ss->char_start;
						txtDescription->SelectionColor = Drawing::Color::Blue;
					}
				}
			}

			const int armor_start = txtDescription->Text->Length;

			txtDescription->AppendText( Environment::NewLine );
			txtDescription->AppendText( StaticString( ArmorsWithSkill ) );
			txtDescription->AppendText( Environment::NewLine );

			armor_list_begin = txtDescription->Lines->Length - 1;

			Generic::List< Ability^ > lollist;
			lollist.Add( last_selected_ability );

			for( int i = 0; i < 5; ++i )
			{
				for( int j = 0; j < Armor::static_armors[ i ]->Count; ++j )
				{
					Armor^ armor = Armor::static_armors[ i ][ j ];
					if( armor->ContainsAnyAbility( lollist ) )
					{
						txtDescription->AppendText( armor->name );
						txtDescription->AppendText( Environment::NewLine );
						armors.Add( armor );
						armor_types.Add( i );
					}
				}
			}

			txtDescription->Select( armor_start, txtDescription->Text->Length - armor_start );
			txtDescription->SelectionColor = Drawing::Color::Black;

			txtDescription->Select( 0, 0 );
			
			//noobery to ensure correct font when showing Japanese text
			txtDescription->SelectionStart = 0;
			txtDescription->SelectionLength = txtDescription->Text->Length;
			txtDescription->SelectionFont = gcnew Drawing::Font( L"Microsoft Sans Serif", txtDescription->Font->Size );
			txtDescription->SelectionLength = 0;

			ResumeDrawing( txtDescription );
		}

		System::Void listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( listBox1->SelectedIndex < 0 )
			{
				txtDescription->Clear();
				return;
			}

			SelectInfoInternal( listBox1->Items[ listBox1->SelectedIndex ]->ToString() );
		}

		System::Void RadioButtonChanged(System::Object^  sender, System::EventArgs^  e)
		{
			skill_mode = sender == rdoSkills;

			listBox1->BeginUpdate();
			listBox1->Items->Clear();
			if( skill_mode )
			{
				for( int i = 0; i < skills.Count; ++i )
					listBox1->Items->Add( skills[ i ]->name );

				if( last_selected_ability != nullptr )
				{
					Skill^ skill = last_selected_ability->GetSkill( 100 );
					if( skill != nullptr )
						listBox1->SelectedIndex = skills.IndexOf( skill );
					else
						listBox1->SelectedIndex = -1;
				}
			}
			else
			{
				for( int i = 0; i < abilities.Count; ++i )
					listBox1->Items->Add( abilities[ i ]->name );

				if( last_selected_ability != nullptr )
					listBox1->SelectedIndex = abilities.IndexOf( last_selected_ability );
			}

			listBox1->EndUpdate();
		}

		System::Void cmsDescription_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
		{
			cmsDescription->Items->Clear();
			e->Cancel = true;

			Point pos = txtDescription->PointToClient( txtDescription->MousePosition );
			int chr = txtDescription->GetCharIndexFromPosition( pos );
			int line = txtDescription->GetLineFromCharIndex( chr );
			if( line < 0 || line >= txtDescription->Lines->Length )
				return;
			String^ str = txtDescription->Lines[ line ];
			if( str == L"" )
				return;

			if( line < armor_list_begin )
			{
				for each( SubSkill^ ss in subskills )
				{
					if( chr >= ss->char_start && chr < ss->char_end )
					{
						listBox1->SelectedIndex = listBox1->FindString( skill_mode ? ss->skill->name : ss->skill->ability->name );
						break;
					}
				}
			}
			else if( line - armor_list_begin < armors.Count )
			{
				Armor^ armor = armors[ line - armor_list_begin ];
				Utility::UpdateContextMenu( cmsDescription, armor );

				if( preview_pane )
					preview_pane->Close();

				if( armor->gender == Gender::MALE )
					preview_pane = gcnew PreviewImage( false );
				else if( armor->gender == Gender::FEMALE )
					preview_pane = gcnew PreviewImage( true );
				else
					preview_pane = gcnew PreviewImage( default_female );

				Generic::List< Armor^ > fake_list;
				for( unsigned i = 0; i < (unsigned)Armor::ArmorType::NumArmorTypes; ++i )
					fake_list.Add( nullptr );
				fake_list[ armor_types[ line - armor_list_begin ] ] = armor;
			
				if( preview_pane->SetData( fake_list ) )
				{
					preview_pane->Show( this );

					preview_pane->SetLocation( cmsDescription );
				}
				else preview_pane = nullptr;

				e->Cancel = false;
			}
		}

		void cmsDescription_Closing( System::Object^ sender, ToolStripDropDownClosingEventArgs^ e )
		{
			if( preview_pane )
			{
				preview_pane->Close();
				preview_pane = nullptr;
			}
		}
};
}
