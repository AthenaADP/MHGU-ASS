#pragma once
#include "Skill.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace MHXXASS
{
	public ref class SelectJapaneseSkill : public System::Windows::Forms::Form
	{
		int selected_length;
		String^ selected_text;
		array< String^ >^ possible_text;
		array< List_t< Ability^ >^ >^ abilities_by_length;
		array< Button^ >^ buttons;
		array< RadioButton^ >^ radio_buttons;
	
	public:
		int selected_ability_index;

		SelectJapaneseSkill(void)
		{
			InitializeComponent();

			this->Text = StaticString( SelectJapaneseSkill );
			rdoLength1->Text = FormatString1( LengthN, L"1" );
			rdoLength2->Text = FormatString1( LengthN, L"2" );
			rdoLength3->Text = FormatString1( LengthN, L"3" );
			rdoLength4->Text = FormatString1( LengthN, L"4" );
			rdoLength5->Text = FormatString1( LengthN, L"5" );
			rdoLength6->Text = FormatString1( LengthN, L"6" );
			rdoLength7->Text = FormatString1( LengthN, L"7" );

			selected_ability_index = -1;
			selected_length = 0;
			selected_text = L"";

			possible_text = gcnew array< String^ >( 7 );
			abilities_by_length = gcnew array< List_t< Ability^ >^ >( 8 );
			for( int i = 0; i < abilities_by_length->Length; ++i )
				abilities_by_length[ i ] = gcnew List_t< Ability^ >();

			abilities_by_length[ 0 ]->AddRange( %Ability::static_abilities );

			for each( Ability^ a in Ability::static_abilities )
			{
				abilities_by_length[ a->jap_name->Length ]->Add( a );
			}

			for( int i = 0; i < abilities_by_length->Length; ++i )
				abilities_by_length[ i ]->Sort( gcnew Comparison< Ability^ >( CompareAbilitiesByName ) );

			array< Button^ >^ _buttons = { btnSymbol1, btnSymbol2, btnSymbol3, btnSymbol4, btnSymbol5, btnSymbol6, btnSymbol7 };
			buttons = _buttons;

			array< RadioButton^ >^ _radio_buttons = { rdoLength1, rdoLength2, rdoLength3, rdoLength4, rdoLength5, rdoLength6, rdoLength7 };
			radio_buttons = _radio_buttons;

			CalculateOptionsFromLength( 0 );

			for( int i = 0; i < buttons->Length; ++i )
				buttons[ i ]->Tag = i;

			for( int i = 0; i < radio_buttons->Length; ++i )
				radio_buttons[ i ]->Tag = i + 1;

			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
		}

	protected:
		~SelectJapaneseSkill()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::ListBox^  lstAbilities;
	private: System::Windows::Forms::Button^  btnSymbol1;
	private: System::Windows::Forms::Button^  btnSymbol2;
	private: System::Windows::Forms::Button^  btnSymbol3;
	private: System::Windows::Forms::Button^  btnSymbol4;
	private: System::Windows::Forms::Button^  btnSymbol5;
	private: System::Windows::Forms::Button^  btnSymbol6;
	private: System::Windows::Forms::Button^  btnSymbol7;
	private: System::Windows::Forms::RadioButton^  rdoLength1;
	private: System::Windows::Forms::RadioButton^  rdoLength2;
	private: System::Windows::Forms::RadioButton^  rdoLength3;
	private: System::Windows::Forms::RadioButton^  rdoLength4;
	private: System::Windows::Forms::RadioButton^  rdoLength5;
	private: System::Windows::Forms::RadioButton^  rdoLength6;
	private: System::Windows::Forms::RadioButton^  rdoLength7;
	private: System::Windows::Forms::Button^  btnOK;
	private: System::Windows::Forms::GroupBox^  grpButtons;
	private: System::Windows::Forms::GroupBox^  grpRadio;
	private: System::Windows::Forms::GroupBox^  grpAbilities;
	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
	private: System::ComponentModel::IContainer^  components;
	private:


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->lstAbilities = (gcnew System::Windows::Forms::ListBox());
			this->btnSymbol1 = (gcnew System::Windows::Forms::Button());
			this->btnSymbol2 = (gcnew System::Windows::Forms::Button());
			this->btnSymbol3 = (gcnew System::Windows::Forms::Button());
			this->btnSymbol4 = (gcnew System::Windows::Forms::Button());
			this->btnSymbol5 = (gcnew System::Windows::Forms::Button());
			this->btnSymbol6 = (gcnew System::Windows::Forms::Button());
			this->btnSymbol7 = (gcnew System::Windows::Forms::Button());
			this->rdoLength1 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLength2 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLength3 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLength4 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLength5 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLength6 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLength7 = (gcnew System::Windows::Forms::RadioButton());
			this->btnOK = (gcnew System::Windows::Forms::Button());
			this->grpButtons = (gcnew System::Windows::Forms::GroupBox());
			this->grpRadio = (gcnew System::Windows::Forms::GroupBox());
			this->grpAbilities = (gcnew System::Windows::Forms::GroupBox());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->grpButtons->SuspendLayout();
			this->grpRadio->SuspendLayout();
			this->grpAbilities->SuspendLayout();
			this->SuspendLayout();
			// 
			// lstAbilities
			// 
			this->lstAbilities->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->lstAbilities->FormattingEnabled = true;
			this->lstAbilities->Location = System::Drawing::Point(6, 14);
			this->lstAbilities->Name = L"lstAbilities";
			this->lstAbilities->Size = System::Drawing::Size(120, 160);
			this->lstAbilities->TabIndex = 1;
			this->lstAbilities->SelectedIndexChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::lstAbilities_SelectedIndexChanged);
			// 
			// btnSymbol1
			// 
			this->btnSymbol1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol1->Location = System::Drawing::Point(11, 19);
			this->btnSymbol1->Name = L"btnSymbol1";
			this->btnSymbol1->Size = System::Drawing::Size(61, 45);
			this->btnSymbol1->TabIndex = 2;
			this->btnSymbol1->UseVisualStyleBackColor = true;
			this->btnSymbol1->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// btnSymbol2
			// 
			this->btnSymbol2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol2->Location = System::Drawing::Point(78, 19);
			this->btnSymbol2->Name = L"btnSymbol2";
			this->btnSymbol2->Size = System::Drawing::Size(61, 45);
			this->btnSymbol2->TabIndex = 3;
			this->btnSymbol2->UseVisualStyleBackColor = true;
			this->btnSymbol2->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// btnSymbol3
			// 
			this->btnSymbol3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol3->Location = System::Drawing::Point(145, 19);
			this->btnSymbol3->Name = L"btnSymbol3";
			this->btnSymbol3->Size = System::Drawing::Size(61, 45);
			this->btnSymbol3->TabIndex = 4;
			this->btnSymbol3->UseVisualStyleBackColor = true;
			this->btnSymbol3->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// btnSymbol4
			// 
			this->btnSymbol4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol4->Location = System::Drawing::Point(210, 19);
			this->btnSymbol4->Name = L"btnSymbol4";
			this->btnSymbol4->Size = System::Drawing::Size(61, 45);
			this->btnSymbol4->TabIndex = 5;
			this->btnSymbol4->UseVisualStyleBackColor = true;
			this->btnSymbol4->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// btnSymbol5
			// 
			this->btnSymbol5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol5->Location = System::Drawing::Point(277, 19);
			this->btnSymbol5->Name = L"btnSymbol5";
			this->btnSymbol5->Size = System::Drawing::Size(61, 45);
			this->btnSymbol5->TabIndex = 6;
			this->btnSymbol5->UseVisualStyleBackColor = true;
			this->btnSymbol5->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// btnSymbol6
			// 
			this->btnSymbol6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol6->Location = System::Drawing::Point(344, 19);
			this->btnSymbol6->Name = L"btnSymbol6";
			this->btnSymbol6->Size = System::Drawing::Size(61, 45);
			this->btnSymbol6->TabIndex = 7;
			this->btnSymbol6->UseVisualStyleBackColor = true;
			this->btnSymbol6->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// btnSymbol7
			// 
			this->btnSymbol7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSymbol7->Location = System::Drawing::Point(411, 19);
			this->btnSymbol7->Name = L"btnSymbol7";
			this->btnSymbol7->Size = System::Drawing::Size(61, 45);
			this->btnSymbol7->TabIndex = 8;
			this->btnSymbol7->UseVisualStyleBackColor = true;
			this->btnSymbol7->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnSymbol_Click);
			// 
			// rdoLength1
			// 
			this->rdoLength1->AutoSize = true;
			this->rdoLength1->Location = System::Drawing::Point(11, 14);
			this->rdoLength1->Name = L"rdoLength1";
			this->rdoLength1->Size = System::Drawing::Size(67, 17);
			this->rdoLength1->TabIndex = 9;
			this->rdoLength1->TabStop = true;
			this->rdoLength1->Text = L"Length 1";
			this->rdoLength1->UseVisualStyleBackColor = true;
			this->rdoLength1->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// rdoLength2
			// 
			this->rdoLength2->AutoSize = true;
			this->rdoLength2->Location = System::Drawing::Point(11, 37);
			this->rdoLength2->Name = L"rdoLength2";
			this->rdoLength2->Size = System::Drawing::Size(67, 17);
			this->rdoLength2->TabIndex = 10;
			this->rdoLength2->TabStop = true;
			this->rdoLength2->Text = L"Length 2";
			this->rdoLength2->UseVisualStyleBackColor = true;
			this->rdoLength2->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// rdoLength3
			// 
			this->rdoLength3->AutoSize = true;
			this->rdoLength3->Location = System::Drawing::Point(11, 60);
			this->rdoLength3->Name = L"rdoLength3";
			this->rdoLength3->Size = System::Drawing::Size(67, 17);
			this->rdoLength3->TabIndex = 11;
			this->rdoLength3->TabStop = true;
			this->rdoLength3->Text = L"Length 3";
			this->rdoLength3->UseVisualStyleBackColor = true;
			this->rdoLength3->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// rdoLength4
			// 
			this->rdoLength4->AutoSize = true;
			this->rdoLength4->Location = System::Drawing::Point(11, 83);
			this->rdoLength4->Name = L"rdoLength4";
			this->rdoLength4->Size = System::Drawing::Size(67, 17);
			this->rdoLength4->TabIndex = 12;
			this->rdoLength4->TabStop = true;
			this->rdoLength4->Text = L"Length 4";
			this->rdoLength4->UseVisualStyleBackColor = true;
			this->rdoLength4->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// rdoLength5
			// 
			this->rdoLength5->AutoSize = true;
			this->rdoLength5->Location = System::Drawing::Point(11, 106);
			this->rdoLength5->Name = L"rdoLength5";
			this->rdoLength5->Size = System::Drawing::Size(67, 17);
			this->rdoLength5->TabIndex = 13;
			this->rdoLength5->TabStop = true;
			this->rdoLength5->Text = L"Length 5";
			this->rdoLength5->UseVisualStyleBackColor = true;
			this->rdoLength5->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// rdoLength6
			// 
			this->rdoLength6->AutoSize = true;
			this->rdoLength6->Location = System::Drawing::Point(11, 131);
			this->rdoLength6->Name = L"rdoLength6";
			this->rdoLength6->Size = System::Drawing::Size(67, 17);
			this->rdoLength6->TabIndex = 14;
			this->rdoLength6->TabStop = true;
			this->rdoLength6->Text = L"Length 6";
			this->rdoLength6->UseVisualStyleBackColor = true;
			this->rdoLength6->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// rdoLength7
			// 
			this->rdoLength7->AutoSize = true;
			this->rdoLength7->Location = System::Drawing::Point(11, 154);
			this->rdoLength7->Name = L"rdoLength7";
			this->rdoLength7->Size = System::Drawing::Size(67, 17);
			this->rdoLength7->TabIndex = 15;
			this->rdoLength7->TabStop = true;
			this->rdoLength7->Text = L"Length 7";
			this->rdoLength7->UseVisualStyleBackColor = true;
			this->rdoLength7->CheckedChanged += gcnew System::EventHandler(this, &SelectJapaneseSkill::rdoLength_CheckedChanged);
			// 
			// btnOK
			// 
			this->btnOK->Location = System::Drawing::Point(132, 152);
			this->btnOK->Name = L"btnOK";
			this->btnOK->Size = System::Drawing::Size(75, 23);
			this->btnOK->TabIndex = 16;
			this->btnOK->Text = L"&OK";
			this->btnOK->UseVisualStyleBackColor = true;
			this->btnOK->Click += gcnew System::EventHandler(this, &SelectJapaneseSkill::btnOK_Click);
			// 
			// grpButtons
			// 
			this->grpButtons->Controls->Add(this->btnSymbol2);
			this->grpButtons->Controls->Add(this->btnSymbol1);
			this->grpButtons->Controls->Add(this->btnSymbol3);
			this->grpButtons->Controls->Add(this->btnSymbol4);
			this->grpButtons->Controls->Add(this->btnSymbol5);
			this->grpButtons->Controls->Add(this->btnSymbol6);
			this->grpButtons->Controls->Add(this->btnSymbol7);
			this->grpButtons->Location = System::Drawing::Point(12, 12);
			this->grpButtons->Name = L"grpButtons";
			this->grpButtons->Size = System::Drawing::Size(482, 79);
			this->grpButtons->TabIndex = 17;
			this->grpButtons->TabStop = false;
			// 
			// grpRadio
			// 
			this->grpRadio->Controls->Add(this->rdoLength7);
			this->grpRadio->Controls->Add(this->rdoLength1);
			this->grpRadio->Controls->Add(this->rdoLength2);
			this->grpRadio->Controls->Add(this->rdoLength3);
			this->grpRadio->Controls->Add(this->rdoLength6);
			this->grpRadio->Controls->Add(this->rdoLength4);
			this->grpRadio->Controls->Add(this->rdoLength5);
			this->grpRadio->Location = System::Drawing::Point(12, 97);
			this->grpRadio->Name = L"grpRadio";
			this->grpRadio->Size = System::Drawing::Size(139, 188);
			this->grpRadio->TabIndex = 18;
			this->grpRadio->TabStop = false;
			// 
			// grpAbilities
			// 
			this->grpAbilities->Controls->Add(this->lstAbilities);
			this->grpAbilities->Controls->Add(this->btnOK);
			this->grpAbilities->Location = System::Drawing::Point(157, 97);
			this->grpAbilities->Name = L"grpAbilities";
			this->grpAbilities->Size = System::Drawing::Size(216, 188);
			this->grpAbilities->TabIndex = 19;
			this->grpAbilities->TabStop = false;
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(61, 4);
			// 
			// SelectJapaneseSkill
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(510, 296);
			this->Controls->Add(this->grpAbilities);
			this->Controls->Add(this->grpRadio);
			this->Controls->Add(this->grpButtons);
			this->Name = L"SelectJapaneseSkill";
			this->Text = L"Select Japanese Skill";
			this->grpButtons->ResumeLayout(false);
			this->grpRadio->ResumeLayout(false);
			this->grpRadio->PerformLayout();
			this->grpAbilities->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	
	void CalculatePossibleText()
	{
		for( int i = 0; i < possible_text->Length; ++i )
			possible_text[ i ] = L"";

		for each( Ability^ ability in abilities_by_length[ selected_length ] )
		{
			for( int i = 0; i < ability->jap_name->Length; ++i )
			{
				if( possible_text[ i ]->IndexOf( L"" + ability->jap_name[ i ] ) == -1 )
				{
					const int limit = ( selected_length == 0 ) ? buttons->Length : ability->jap_name->Length;
					bool fail = false;
					for( int j = 0; j < limit; ++j )
					{
						if( j == i )
							continue;

						if( buttons[ j ]->Text != L"" && ( j >= ability->jap_name->Length || buttons[ j ]->Text[ 0 ] != ability->jap_name[ j ] ) )
						{
							fail = true;
							break;
						}
					}
					if( !fail)
						possible_text[ i ] += ability->jap_name[ i ];
				}
			}
		}
	}

	void CalculateOptionsFromLength( const int len )
	{
		selected_length = len;

		for( int i = 0; i < buttons->Length; ++i )
			buttons[ i ]->Enabled = ( len == 0 || i < len );

		CalculatePossibleText();

		for( int i = 7; i --> 0; )
		{
			UpdateAbilityList();
			if( lstAbilities->Items->Count > 0 )
				break;

			while( i >= 0 && buttons[ i ]->Text == L"" )
				--i;

			if( i >= 0 )
			{
				buttons[ i ]->Text = L"";
				CalculatePossibleText();
			}
		}
	}

	void UpdateAbilityList()
	{
		lstAbilities->SuspendLayout();
		lstAbilities->Items->Clear();
		for each( Ability^ ability in abilities_by_length[ selected_length ] )
		{
			bool fail = false;
			for( int i = 0; i < ability->jap_name->Length; ++i )
			{
				if( buttons[ i ]->Text == L"" )
					continue;

				if( ability->jap_name[ i ] != buttons[ i ]->Text[ 0 ] )
				{
					fail = true;
					break;
				}
			}
			if( selected_length == 0 )
			{
				for( int i = ability->jap_name->Length; i < buttons->Length; ++i )
				{
					if( buttons[ i ]->Text != L"" )
					{
						fail = true;
						break;
					}
				}
			}
			if( !fail )
				lstAbilities->Items->Add( ability->jap_name );
		}
		lstAbilities->ResumeLayout();

		btnOK->Enabled = lstAbilities->Items->Count == 1;
	}

	System::Void btnOK_Click( System::Object^, System::EventArgs^ )
	{
		if( lstAbilities->Items->Count == 1 )
		{
			if( Ability^ a = Ability::FindCharmAbility( lstAbilities->Items[ 0 ]->ToString() ) )
			{
				selected_ability_index = a->static_index;
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
			}
		}
		else if( lstAbilities->SelectedItems->Count == 1 )
		{
			if( Ability^ a = Ability::FindCharmAbility( lstAbilities->SelectedItem->ToString() ) )
			{
				selected_ability_index = a->static_index;
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
			}
		}

		Close();
	}

	System::Void rdoLength_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if( RadioButton^ rb = safe_cast< RadioButton^ >( sender ) )
			CalculateOptionsFromLength( (int)rb->Tag );
	}

	void OnContextMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( ToolStripMenuItem^ item = safe_cast< ToolStripMenuItem^ >( sender ) )
		{
			if( Button^ button = safe_cast< Button^ >( contextMenuStrip1->Tag ) )
			{
				if( item->Text->Length > 1 )
					button->Text = L"";
				else
					button->Text = item->Text;

				CalculateOptionsFromLength( selected_length );
			}
		}
	}

	void AddContextMenuStrip( String^ text )
	{
		contextMenuStrip1->Items->Add( gcnew ToolStripMenuItem( text, nullptr, gcnew EventHandler( this, &SelectJapaneseSkill::OnContextMenuItem_Click ) ) );
	}

	System::Void btnSymbol_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if( Button^ button = safe_cast< Button^ >( sender ) )
		{
			if( String^ chars = safe_cast< String^ >( possible_text[ (int)button->Tag ] ) )
			{
				contextMenuStrip1->Tag = button;
				contextMenuStrip1->Items->Clear();
				AddContextMenuStrip( L"Clear" );
				for( int i = 0; i < chars->Length; ++i )
					AddContextMenuStrip( L"" + chars[ i ] );
				contextMenuStrip1->Show( button, button->Width, 0 );
			}
		}
		
	}

	System::Void lstAbilities_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if( lstAbilities->SelectedIndex != -1 )
		{
			btnOK->Enabled = true;
			
			if( Ability^ a = Ability::FindCharmAbility( lstAbilities->SelectedItem->ToString() ) )
			{
				for( int i = 0; i < a->jap_name->Length; ++i )
					buttons[ i ]->Text = L"" + a->jap_name[ i ];
				for( int i = a->jap_name->Length; i < buttons->Length; ++i )
					buttons[ i ]->Text = L"";
			}
		}
		else btnOK->Enabled = false;
	}
};
}
