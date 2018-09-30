#pragma once
#include "CharmDatabase.h"
#include "Armor.h"
#include "LoadedData.h"
#include "Common.h"
#include "ImportCharms.h"
#include "SelectJapaneseSkill.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#pragma warning( disable: 4677 ) //warning for passing my own class into this form

namespace MHGUASS {

	public ref class ManageCharms : public System::Windows::Forms::Form
	{
		const int language;
		bool update_skills, updating_charms, sort_alphabetically;
		LoadedData^ data;
		List_t< Charm^ > best_charms;
	
	public:

		ManageCharms( const int language, LoadedData^ _data, const bool sort_alphabetically )
			: language( language ), data(_data ), update_skills( true ), updating_charms( false ), sort_alphabetically( sort_alphabetically )
		{
			InitializeComponent();
			lstCharms->DrawItem += gcnew DrawItemEventHandler( this, &ManageCharms::lstCharms_DrawItem );
			for each( Ability^ ab in Ability::static_abilities )
				ab->relevant = true;
			
			RefreshList( -1 );
			UpdateBestCharms();

			InitFilter( cmbSkillFilters1 );
			InitFilter( cmbSkillFilters2 );
			
			cmbAmount1->SelectedIndex = 0;
			cmbAmount2->SelectedIndex = 0;
			cmbAmount1->LostFocus += gcnew EventHandler( this, &ManageCharms::cmbAmount_LostFocus );
			cmbAmount2->LostFocus += gcnew EventHandler( this, &ManageCharms::cmbAmount_LostFocus );

			Text = BasicString( MyCharms );
			btnAddNew->Text = StaticString( AddNewCharm );
			btnSave->Text = StaticString( SaveCharms );
			btnDelete->Text = StaticString( DeleteCharm );
			btnDeleteAll->Text = StaticString( DeleteAllCharms );
			btnMoveUp->Text = StaticString( MoveUp );
			btnMoveDown->Text = StaticString( MoveDown );
			btnTrim->Text = StaticString( TrimCharms );
			btnSort1->Text = StaticString( SortBySkill1 );
			btnSort2->Text = StaticString( SortBySkill2 );
			lblSlots->Text = StaticString( Slots );
			btnImport->Text = StaticString( Import );
			btnReset->Text = StaticString( ResetCharm );
			btnJap1->Text = StaticString( Jap );
			btnJap2->Text = StaticString( Jap );

			MatchWidthToText( btnAddNew );
			MatchWidthToText( btnSave );
			MatchWidthToText( btnReset );

			lblSlots->Width = TextRenderer::MeasureText( lblSlots->Text, lblSlots->Font ).Width;

			PlaceToTheRightOf( nudSlots, lblSlots, 6 );
			PlaceToTheRightOf( btnAddNew, nudSlots, 6 );
			PlaceToTheRightOf( btnSave, btnAddNew, 6 );
			PlaceToTheRightOf( btnReset, btnSave, 6 );

			if( lstCharms->SelectedIndex == -1 )
				btnSave->Enabled = false;

			cmbSkills1->DropDown += gcnew System::EventHandler( this, &ManageCharms::SkillCombo_DropDown );
			cmbSkills2->DropDown += gcnew System::EventHandler( this, &ManageCharms::SkillCombo_DropDown );
			cmbSkills1->DropDownClosed += gcnew System::EventHandler( this, &ManageCharms::SkillCombo_DropDownClosed );
			cmbSkills2->DropDownClosed += gcnew System::EventHandler( this, &ManageCharms::SkillCombo_DropDownClosed );
		}

		void MatchWidthToText( Control^ b )
		{
			b->Width = 10 + TextRenderer::MeasureText( b->Text, b->Font ).Width;
		}

		void PlaceToTheRightOf( Control^ to_move, Control^ left, const int extra )
		{
			to_move->Location = Point( left->Location.X + left->Width + extra, to_move->Location.Y );
		}

		void InitFilter( ComboBox^ cb )
		{
			cb->BeginUpdate();
			cb->Items->Clear();
			for each( SkillTag^ tag in SkillTag::tags )
			{
				cb->Items->Add( tag->name );
			}
			cb->SelectedIndex = 0;
			cb->EndUpdate();
		}

	protected:
		~ManageCharms()
		{
			if( components )
			{
				for each( Ability^ ab in Ability::static_abilities )
					ab->relevant = false;

				delete components;
			}
		}

	private: System::Windows::Forms::ListBox^  lstCharms;
	private: System::Windows::Forms::Label^  lblSlots;
	private: System::Windows::Forms::NumericUpDown^  nudSlots;
	private: System::Windows::Forms::Button^  btnAddNew;
	private: System::Windows::Forms::Button^  btnSave;
	private: System::Windows::Forms::ComboBox^  cmbSkills1;
	private: System::Windows::Forms::ComboBox^  cmbSkills2;
	private: System::Windows::Forms::ComboBox^  cmbAmount1;
	private: System::Windows::Forms::ComboBox^  cmbAmount2;
	private: System::Windows::Forms::Button^  btnDelete;
	private: System::Windows::Forms::Button^  btnMoveUp;
	private: System::Windows::Forms::Button^  btnMoveDown;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ComboBox^  cmbSkillFilters2;
	private: System::Windows::Forms::ComboBox^  cmbSkillFilters1;
	private: System::Windows::Forms::Button^  btnDeleteAll;
	private: System::Windows::Forms::Button^  btnTrim;
	private: System::Windows::Forms::Button^  btnSort1;
	private: System::Windows::Forms::Button^  btnSort2;
	private: System::Windows::Forms::Button^  btnJap2;
	private: System::Windows::Forms::Button^  btnJap1;
	private: System::Windows::Forms::Button^  btnImport;
	private: System::Windows::Forms::Button^  btnReset;
private: System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->lstCharms = (gcnew System::Windows::Forms::ListBox());
			this->lblSlots = (gcnew System::Windows::Forms::Label());
			this->nudSlots = (gcnew System::Windows::Forms::NumericUpDown());
			this->btnAddNew = (gcnew System::Windows::Forms::Button());
			this->btnSave = (gcnew System::Windows::Forms::Button());
			this->cmbSkills1 = (gcnew System::Windows::Forms::ComboBox());
			this->cmbSkills2 = (gcnew System::Windows::Forms::ComboBox());
			this->cmbAmount1 = (gcnew System::Windows::Forms::ComboBox());
			this->cmbAmount2 = (gcnew System::Windows::Forms::ComboBox());
			this->btnDelete = (gcnew System::Windows::Forms::Button());
			this->btnMoveUp = (gcnew System::Windows::Forms::Button());
			this->btnMoveDown = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->btnReset = (gcnew System::Windows::Forms::Button());
			this->btnJap2 = (gcnew System::Windows::Forms::Button());
			this->btnJap1 = (gcnew System::Windows::Forms::Button());
			this->cmbSkillFilters2 = (gcnew System::Windows::Forms::ComboBox());
			this->cmbSkillFilters1 = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->btnSort2 = (gcnew System::Windows::Forms::Button());
			this->btnImport = (gcnew System::Windows::Forms::Button());
			this->btnSort1 = (gcnew System::Windows::Forms::Button());
			this->btnTrim = (gcnew System::Windows::Forms::Button());
			this->btnDeleteAll = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudSlots))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// lstCharms
			// 
			this->lstCharms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->lstCharms->DrawMode = System::Windows::Forms::DrawMode::OwnerDrawFixed;
			this->lstCharms->FormattingEnabled = true;
			this->lstCharms->IntegralHeight = false;
			this->lstCharms->Location = System::Drawing::Point(6, 19);
			this->lstCharms->Name = L"lstCharms";
			this->lstCharms->Size = System::Drawing::Size(231, 211);
			this->lstCharms->TabIndex = 12;
			this->lstCharms->SelectedIndexChanged += gcnew System::EventHandler(this, &ManageCharms::lstCharms_SelectedIndexChanged);
			// 
			// lblSlots
			// 
			this->lblSlots->AutoSize = true;
			this->lblSlots->Location = System::Drawing::Point(5, 87);
			this->lblSlots->Name = L"lblSlots";
			this->lblSlots->Size = System::Drawing::Size(30, 13);
			this->lblSlots->TabIndex = 2;
			this->lblSlots->Text = L"Slots";
			// 
			// nudSlots
			// 
			this->nudSlots->Location = System::Drawing::Point(41, 84);
			this->nudSlots->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			this->nudSlots->Name = L"nudSlots";
			this->nudSlots->Size = System::Drawing::Size(31, 20);
			this->nudSlots->TabIndex = 8;
			// 
			// btnAddNew
			// 
			this->btnAddNew->Location = System::Drawing::Point(78, 81);
			this->btnAddNew->Name = L"btnAddNew";
			this->btnAddNew->Size = System::Drawing::Size(66, 25);
			this->btnAddNew->TabIndex = 9;
			this->btnAddNew->Text = L"&Add New";
			this->btnAddNew->UseVisualStyleBackColor = true;
			this->btnAddNew->Click += gcnew System::EventHandler(this, &ManageCharms::btnAddNew_Click);
			// 
			// btnSave
			// 
			this->btnSave->Location = System::Drawing::Point(150, 81);
			this->btnSave->Name = L"btnSave";
			this->btnSave->Size = System::Drawing::Size(66, 25);
			this->btnSave->TabIndex = 10;
			this->btnSave->Text = L"&Save";
			this->btnSave->UseVisualStyleBackColor = true;
			this->btnSave->Click += gcnew System::EventHandler(this, &ManageCharms::btnSave_Click);
			// 
			// cmbSkills1
			// 
			this->cmbSkills1->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->cmbSkills1->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->cmbSkills1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSkills1->FormattingEnabled = true;
			this->cmbSkills1->Location = System::Drawing::Point(52, 19);
			this->cmbSkills1->Name = L"cmbSkills1";
			this->cmbSkills1->Size = System::Drawing::Size(132, 21);
			this->cmbSkills1->TabIndex = 1;
			this->cmbSkills1->SelectedIndexChanged += gcnew System::EventHandler(this, &ManageCharms::cmbSkills1_SelectedIndexChanged);
			// 
			// cmbSkills2
			// 
			this->cmbSkills2->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->cmbSkills2->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->cmbSkills2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSkills2->FormattingEnabled = true;
			this->cmbSkills2->Location = System::Drawing::Point(52, 46);
			this->cmbSkills2->Name = L"cmbSkills2";
			this->cmbSkills2->Size = System::Drawing::Size(132, 21);
			this->cmbSkills2->TabIndex = 5;
			this->cmbSkills2->SelectedIndexChanged += gcnew System::EventHandler(this, &ManageCharms::cmbSkills2_SelectedIndexChanged);
			// 
			// cmbAmount1
			// 
			this->cmbAmount1->FormattingEnabled = true;
			this->cmbAmount1->Items->AddRange(gcnew cli::array< System::Object^  >(15) {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", 
				L"8", L"9", L"10", L"11", L"12", L"13", L"14"});
			this->cmbAmount1->Location = System::Drawing::Point(6, 19);
			this->cmbAmount1->MaxLength = 3;
			this->cmbAmount1->Name = L"cmbAmount1";
			this->cmbAmount1->Size = System::Drawing::Size(40, 21);
			this->cmbAmount1->TabIndex = 0;
			// 
			// cmbAmount2
			// 
			this->cmbAmount2->FormattingEnabled = true;
			this->cmbAmount2->Items->AddRange(gcnew cli::array< System::Object^  >(25) {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", 
				L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"-1", L"-2", L"-3", L"-4", L"-5", L"-6", L"-7", L"-8", L"-9", L"-10"});
			this->cmbAmount2->Location = System::Drawing::Point(6, 46);
			this->cmbAmount2->MaxLength = 3;
			this->cmbAmount2->Name = L"cmbAmount2";
			this->cmbAmount2->Size = System::Drawing::Size(40, 21);
			this->cmbAmount2->TabIndex = 4;
			// 
			// btnDelete
			// 
			this->btnDelete->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnDelete->Location = System::Drawing::Point(243, 19);
			this->btnDelete->Name = L"btnDelete";
			this->btnDelete->Size = System::Drawing::Size(111, 25);
			this->btnDelete->TabIndex = 13;
			this->btnDelete->Text = L"De&lete";
			this->btnDelete->UseVisualStyleBackColor = true;
			this->btnDelete->Click += gcnew System::EventHandler(this, &ManageCharms::btnDelete_Click);
			// 
			// btnMoveUp
			// 
			this->btnMoveUp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnMoveUp->Location = System::Drawing::Point(243, 100);
			this->btnMoveUp->Name = L"btnMoveUp";
			this->btnMoveUp->Size = System::Drawing::Size(111, 25);
			this->btnMoveUp->TabIndex = 16;
			this->btnMoveUp->Text = L"Move &Up";
			this->btnMoveUp->UseVisualStyleBackColor = true;
			this->btnMoveUp->Click += gcnew System::EventHandler(this, &ManageCharms::btnMoveUp_Click);
			// 
			// btnMoveDown
			// 
			this->btnMoveDown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnMoveDown->Location = System::Drawing::Point(243, 125);
			this->btnMoveDown->Name = L"btnMoveDown";
			this->btnMoveDown->Size = System::Drawing::Size(111, 25);
			this->btnMoveDown->TabIndex = 17;
			this->btnMoveDown->Text = L"Move &Down";
			this->btnMoveDown->UseVisualStyleBackColor = true;
			this->btnMoveDown->Click += gcnew System::EventHandler(this, &ManageCharms::btnMoveDown_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->btnReset);
			this->groupBox1->Controls->Add(this->btnJap2);
			this->groupBox1->Controls->Add(this->btnJap1);
			this->groupBox1->Controls->Add(this->cmbSkillFilters2);
			this->groupBox1->Controls->Add(this->cmbSkillFilters1);
			this->groupBox1->Controls->Add(this->cmbAmount2);
			this->groupBox1->Controls->Add(this->lblSlots);
			this->groupBox1->Controls->Add(this->nudSlots);
			this->groupBox1->Controls->Add(this->btnAddNew);
			this->groupBox1->Controls->Add(this->btnSave);
			this->groupBox1->Controls->Add(this->cmbAmount1);
			this->groupBox1->Controls->Add(this->cmbSkills1);
			this->groupBox1->Controls->Add(this->cmbSkills2);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(366, 117);
			this->groupBox1->TabIndex = 14;
			this->groupBox1->TabStop = false;
			// 
			// btnReset
			// 
			this->btnReset->Location = System::Drawing::Point(222, 82);
			this->btnReset->Name = L"btnReset";
			this->btnReset->Size = System::Drawing::Size(75, 23);
			this->btnReset->TabIndex = 11;
			this->btnReset->Text = L"&Reset";
			this->btnReset->UseVisualStyleBackColor = true;
			this->btnReset->Click += gcnew System::EventHandler(this, &ManageCharms::btnReset_Click);
			// 
			// btnJap2
			// 
			this->btnJap2->Location = System::Drawing::Point(306, 45);
			this->btnJap2->Name = L"btnJap2";
			this->btnJap2->Size = System::Drawing::Size(43, 23);
			this->btnJap2->TabIndex = 7;
			this->btnJap2->Text = L"Jap\?";
			this->btnJap2->UseVisualStyleBackColor = true;
			this->btnJap2->Click += gcnew System::EventHandler(this, &ManageCharms::btnJap_Click);
			// 
			// btnJap1
			// 
			this->btnJap1->Location = System::Drawing::Point(306, 18);
			this->btnJap1->Name = L"btnJap1";
			this->btnJap1->Size = System::Drawing::Size(43, 23);
			this->btnJap1->TabIndex = 3;
			this->btnJap1->Text = L"Jap\?";
			this->btnJap1->UseVisualStyleBackColor = true;
			this->btnJap1->Click += gcnew System::EventHandler(this, &ManageCharms::btnJap_Click);
			// 
			// cmbSkillFilters2
			// 
			this->cmbSkillFilters2->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->cmbSkillFilters2->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->cmbSkillFilters2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSkillFilters2->FormattingEnabled = true;
			this->cmbSkillFilters2->Location = System::Drawing::Point(190, 46);
			this->cmbSkillFilters2->Name = L"cmbSkillFilters2";
			this->cmbSkillFilters2->Size = System::Drawing::Size(110, 21);
			this->cmbSkillFilters2->TabIndex = 6;
			this->cmbSkillFilters2->SelectedIndexChanged += gcnew System::EventHandler(this, &ManageCharms::cmbSkillFilters2_SelectedIndexChanged);
			// 
			// cmbSkillFilters1
			// 
			this->cmbSkillFilters1->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->cmbSkillFilters1->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
			this->cmbSkillFilters1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSkillFilters1->FormattingEnabled = true;
			this->cmbSkillFilters1->Location = System::Drawing::Point(190, 19);
			this->cmbSkillFilters1->Name = L"cmbSkillFilters1";
			this->cmbSkillFilters1->Size = System::Drawing::Size(110, 21);
			this->cmbSkillFilters1->TabIndex = 2;
			this->cmbSkillFilters1->SelectedIndexChanged += gcnew System::EventHandler(this, &ManageCharms::cmbSkillFilters1_SelectedIndexChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox2->Controls->Add(this->btnSort2);
			this->groupBox2->Controls->Add(this->btnImport);
			this->groupBox2->Controls->Add(this->btnSort1);
			this->groupBox2->Controls->Add(this->btnTrim);
			this->groupBox2->Controls->Add(this->btnDeleteAll);
			this->groupBox2->Controls->Add(this->lstCharms);
			this->groupBox2->Controls->Add(this->btnDelete);
			this->groupBox2->Controls->Add(this->btnMoveDown);
			this->groupBox2->Controls->Add(this->btnMoveUp);
			this->groupBox2->Location = System::Drawing::Point(12, 135);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(366, 238);
			this->groupBox2->TabIndex = 15;
			this->groupBox2->TabStop = false;
			// 
			// btnSort2
			// 
			this->btnSort2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnSort2->Location = System::Drawing::Point(243, 175);
			this->btnSort2->Name = L"btnSort2";
			this->btnSort2->Size = System::Drawing::Size(111, 25);
			this->btnSort2->TabIndex = 19;
			this->btnSort2->Text = L"S&ort by Skill 2";
			this->btnSort2->UseVisualStyleBackColor = true;
			this->btnSort2->Click += gcnew System::EventHandler(this, &ManageCharms::btnSort_Click);
			// 
			// btnImport
			// 
			this->btnImport->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnImport->Location = System::Drawing::Point(243, 206);
			this->btnImport->Name = L"btnImport";
			this->btnImport->Size = System::Drawing::Size(111, 25);
			this->btnImport->TabIndex = 20;
			this->btnImport->Text = L"&Import Charms";
			this->btnImport->UseVisualStyleBackColor = true;
			this->btnImport->Click += gcnew System::EventHandler(this, &ManageCharms::btnImport_Click);
			// 
			// btnSort1
			// 
			this->btnSort1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnSort1->Location = System::Drawing::Point(243, 150);
			this->btnSort1->Name = L"btnSort1";
			this->btnSort1->Size = System::Drawing::Size(111, 25);
			this->btnSort1->TabIndex = 18;
			this->btnSort1->Text = L"&Sort by Skill 1";
			this->btnSort1->UseVisualStyleBackColor = true;
			this->btnSort1->Click += gcnew System::EventHandler(this, &ManageCharms::btnSort_Click);
			// 
			// btnTrim
			// 
			this->btnTrim->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnTrim->Location = System::Drawing::Point(243, 69);
			this->btnTrim->Name = L"btnTrim";
			this->btnTrim->Size = System::Drawing::Size(111, 25);
			this->btnTrim->TabIndex = 15;
			this->btnTrim->Text = L"&Trim";
			this->btnTrim->UseVisualStyleBackColor = true;
			this->btnTrim->Click += gcnew System::EventHandler(this, &ManageCharms::btnTrim_Click);
			// 
			// btnDeleteAll
			// 
			this->btnDeleteAll->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnDeleteAll->Location = System::Drawing::Point(243, 44);
			this->btnDeleteAll->Name = L"btnDeleteAll";
			this->btnDeleteAll->Size = System::Drawing::Size(111, 25);
			this->btnDeleteAll->TabIndex = 14;
			this->btnDeleteAll->Text = L"Delete &All";
			this->btnDeleteAll->UseVisualStyleBackColor = true;
			this->btnDeleteAll->Click += gcnew System::EventHandler(this, &ManageCharms::btnDeleteAll_Click);
			// 
			// ManageCharms
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(390, 385);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"ManageCharms";
			this->Text = L"My Charms";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudSlots))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 

		void lstCharms_DrawItem( System::Object^ sender, DrawItemEventArgs^ e )
		{
			e->DrawBackground();
			if( ( e->State & DrawItemState::Focus ) == DrawItemState::Focus )
				e->DrawFocusRectangle();
			
			if( e->Index < 0 || e->Index >= lstCharms->Items->Count || !CharmDatabase::mycharms[ e->Index ] )
				return;

			SolidBrush brush( ( cmbSkills1->DroppedDown || cmbSkills2->DroppedDown || !CharmDatabase::mycharms[ e->Index ]->optimal ) ? Color::LightGray : Color::Black );
			e->Graphics->TextRenderingHint = System::Drawing::Text::TextRenderingHint::ClearTypeGridFit;
			e->Graphics->DrawString( lstCharms->Items[ e->Index ]->ToString(), e->Font, %brush, e->Bounds );
		}

		System::Void RefreshList( const int new_index )
		{
			lstCharms->BeginUpdate();
			lstCharms->Items->Clear();
			for each( Charm^ charm in CharmDatabase::mycharms )
			{
				lstCharms->Items->Add( charm->GetName() );
			}
			lstCharms->SelectedIndex = new_index;
			lstCharms->EndUpdate();
		}

		Charm^ CreateCharm()
		{
			const unsigned num_slots = (int)nudSlots->Value;
			int amount1 = 0, amount2 = 0;
			Ability^ ability1 = nullptr, ^ability2 = nullptr;
			if( cmbAmount1->Text != L"0" && cmbSkills1->SelectedIndex != -1 )
			{
				amount1 = Convert::ToInt32( cmbAmount1->Text );
				ability1 = Ability::FindAbility( (String^)cmbSkills1->SelectedItem );
				Assert( ability1 && amount1 >= 0 && amount1 <= 14, L"Create-a-charm skill amount1 is invalid" );
			}
			if( cmbAmount2->Text != L"0" && cmbSkills2->SelectedIndex != -1 )
			{
				amount2 = Convert::ToInt32( cmbAmount2->Text );
				ability2 = Ability::FindAbility( (String^)cmbSkills2->SelectedItem );
				Assert( ability2 && amount2 >= -10 && amount2 <= 14, L"Create-a-charm skill amount2 is invalid" );
			}
			if( num_slots == 0 && !ability1 && !ability2 )
				return nullptr;
		
			Charm^ ch = gcnew Charm( num_slots );
			ch->custom = true;
			if( ability1 )
				ch->abilities.Add( gcnew AbilityPair( ability1, amount1 ) );
			if( ability2 )
				ch->abilities.Add( gcnew AbilityPair( ability2, amount2 ) );				
			return CharmDatabase::CharmIsLegal( ch ) ? ch : nullptr;
		}

		System::Void btnSave_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( lstCharms->SelectedIndex > -1 )
			{
				updating_charms = true;
				try
				{
					Charm^ ch = CreateCharm();
					if( ch )
					{
						CharmDatabase::mycharms[ lstCharms->SelectedIndex ] = ch;
						lstCharms->BeginUpdate();
						lstCharms->Items[ lstCharms->SelectedIndex ] = ch->GetName();
						lstCharms->EndUpdate();
						UpdateBestCharms();
						CharmDatabase::SaveCustom();
					}
				}
				catch( FormatException^ )
				{
				}
				updating_charms = false;
			}
		}

		System::Void btnDelete_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( lstCharms->SelectedIndex > -1 )
			{
				const int old_index = lstCharms->SelectedIndex;
				CharmDatabase::mycharms.RemoveAt( lstCharms->SelectedIndex );
				if( old_index < CharmDatabase::mycharms.Count )
					RefreshList( old_index );
				else
					RefreshList( CharmDatabase::mycharms.Count - 1 );
				UpdateBestCharms();
				CharmDatabase::SaveCustom();
			}
		}

		System::Void btnAddNew_Click(System::Object^  sender, System::EventArgs^  e)
		{
			updating_charms = true;
			try
			{
				if( Charm^ ch = CreateCharm() )
				{
					ch->custom = true;
					CharmDatabase::mycharms.Add( ch );
					lstCharms->BeginUpdate();
					lstCharms->Items->Add( ch->GetName() );
					lstCharms->SelectedIndex = lstCharms->Items->Count - 1;
					lstCharms->EndUpdate();
					btnSave->Enabled = true;
					Charm::AddToOptimalList( best_charms, ch );
					UpdateBestCharmsColours();
					CharmDatabase::SaveCustom();
				}
			}
			catch( FormatException^ )
			{
			}
			updating_charms = false;
		}

		System::Void btnMoveUp_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( lstCharms->SelectedIndex > 0 )
			{
				const int old_index = lstCharms->SelectedIndex;
				Charm^ temp = CharmDatabase::mycharms[ old_index];
				CharmDatabase::mycharms[ old_index ] = CharmDatabase::mycharms[ old_index - 1 ];
				CharmDatabase::mycharms[ old_index - 1 ] = temp;
				RefreshList( old_index - 1 );
				CharmDatabase::SaveCustom();
			}
		}

		System::Void btnMoveDown_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( lstCharms->SelectedIndex > -1 && lstCharms->SelectedIndex < lstCharms->Items->Count - 1 )
			{
				const int old_index = lstCharms->SelectedIndex;
				Charm^ temp = CharmDatabase::mycharms[ old_index ];
				CharmDatabase::mycharms[ old_index ] = CharmDatabase::mycharms[ old_index + 1 ];
				CharmDatabase::mycharms[ old_index + 1 ] = temp;
				RefreshList( old_index + 1 );
				CharmDatabase::SaveCustom();
			}
		}

		System::Void UpdateFilter( ComboBox^ skill, ComboBox^ filter, Ability^ banned )
		{
			if( !update_skills )
				return;
			List_t< Ability^ >^ the_list = sort_alphabetically ? %Ability::ordered_abilities : %Ability::static_abilities;

			update_skills = false;
			String^ old_item = (String^)skill->SelectedItem;
			skill->BeginUpdate();
			skill->Items->Clear();
			skill->Items->Add( StaticString( NoneBrackets ) );
			if( filter->SelectedIndex == 0 )
			{
				for each( Ability^ ab in the_list )
				{
					if( ab != SpecificAbility::torso_inc && ab != banned )
						skill->Items->Add( ab->name );
				}
			}
			else if( filter->SelectedIndex == 1 )
			{
				for each( Ability^ ab in the_list )
				{
					if( ab->tags.Count == 0 && ab != SpecificAbility::torso_inc && ab != banned )
						skill->Items->Add( ab->name );
				}
			}
			else
			{
				String^ tag = (String^)filter->SelectedItem;
				for each( Ability^ ab in the_list )
				{
					if( ab == banned )
						continue;
					for each( SkillTag^ st in ab->tags )
					{
						if( st->name == tag )
						{
							skill->Items->Add( ab->name );
							break;
						}
					}
				}
			}
			//reselect if possible
			for( int i = 0; i < skill->Items->Count; ++i )
			{
				if( (String^)skill->Items[ i ] == old_item )
				{
					skill->SelectedIndex = i;
					break;
				}
			}

			skill->EndUpdate();
			update_skills = true;
		}

		System::Void cmbSkillFilters2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			UpdateFilter( cmbSkills2, cmbSkillFilters2, Ability::FindAbility( (String^)cmbSkills1->SelectedItem ) );
		}
 
		System::Void cmbSkillFilters1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			UpdateFilter( cmbSkills1, cmbSkillFilters1, Ability::FindAbility( (String^)cmbSkills2->SelectedItem ) );
		}

		System::Void cmbSkills1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( cmbSkills1->SelectedIndex == 0 )
				cmbSkills1->SelectedIndex = -1;
			else
				UpdateFilter( cmbSkills2, cmbSkillFilters2, Ability::FindAbility( (String^)cmbSkills1->SelectedItem ) );
		}
		
		System::Void cmbSkills2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( cmbSkills2->SelectedIndex == 0 )
				cmbSkills2->SelectedIndex = -1;
			else
				UpdateFilter( cmbSkills1, cmbSkillFilters1, Ability::FindAbility( (String^)cmbSkills2->SelectedItem ) );
		}

		System::Void SwitchToAbility( ComboBox^ cb, Ability^ ab )
		{
			for( int i = 0; i < cb->Items->Count; ++i )
			{
				if( Ability::FindAbility( (String^)cb->Items[ i ] ) == ab )
				{
					cb->BeginUpdate();
					cb->SelectedIndex = i;
					cb->EndUpdate();
					break;
				}
			}
		}

		System::Void lstCharms_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( updating_charms )
				return;
			else if( lstCharms->SelectedIndex < 0 )
			{
				btnSave->Enabled = false;
				return;
			}
			btnSave->Enabled = true;

			Charm^ ch = CharmDatabase::mycharms[ lstCharms->SelectedIndex ];
			cmbSkills1->BeginUpdate();
			cmbSkills2->BeginUpdate();
			nudSlots->Value = ch->num_slots;
			cmbSkillFilters1->SelectedIndex = 0;
			cmbSkillFilters2->SelectedIndex = 0;
			cmbSkills1->SelectedIndex = 0;
			cmbSkills2->SelectedIndex = 0;
			if( ch->abilities.Count > 0 )
			{
				SwitchToAbility( cmbSkills1, ch->abilities[ 0 ]->ability );
				cmbAmount1->Text = Convert::ToString( ch->abilities[ 0 ]->amount );
				if( ch->abilities.Count > 1 )
				{
					SwitchToAbility( cmbSkills2, ch->abilities[ 1 ]->ability );
					cmbAmount2->Text = Convert::ToString( ch->abilities[ 1 ]->amount );
				}
			}
			cmbSkills1->EndUpdate();
			cmbSkills2->EndUpdate();
		}

		System::Void cmbAmount_LostFocus(System::Object^  sender, System::EventArgs^  e)
		{
			ComboBox^ cb = (ComboBox^)sender;
			try
			{
				int val = Convert::ToInt32( cb->Text );
				if( cb == cmbAmount1 )
				{
					if( val < 0 )
						cb->Text = L"0";
					else if( val > 14 )
						cb->Text = L"14";
				}
				else
				{
					Assert( cb == cmbAmount2, L"Converted amount is invalid" );
					if( val < -10 )
						cb->Text = L"-10";
					else if( val > 14 )
						cb->Text = L"14";
				}
			}
			catch( FormatException^ )
			{
				cb->Text = L"0";
			}
		}

		System::Void btnDeleteAll_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( System::Windows::Forms::DialogResult::OK != MessageBox::Show( StaticString( AreYouSure ), StaticString( DeleteAllCharmsMessage ), MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation, MessageBoxDefaultButton::Button2 ) )
				return;
			CharmDatabase::mycharms.Clear();
			best_charms.Clear();
			RefreshList( -1 );
			CharmDatabase::SaveCustom();
		}

		void UpdateBestCharms()
		{
			best_charms.Clear();
			
			for each( Charm^ charm in CharmDatabase::mycharms )
				Charm::AddToOptimalList( best_charms, charm );

			UpdateBestCharmsColours();
		}

		void UpdateBestCharmsColours()
		{
			lstCharms->BeginUpdate();
			for each( Charm^ charm in CharmDatabase::mycharms )
			{
				charm->optimal = false;
			}
			for each( Charm^ charm in best_charms )
			{
				charm->optimal = true;
			}
			lstCharms->EndUpdate();
		}

		void SkillCombo_DropDown( System::Object^ sender, System::EventArgs^ e )
		{
			lstCharms->ForeColor = Color::LightGray;
		}

		void SkillCombo_DropDownClosed( System::Object^ sender, System::EventArgs^ e )
		{
			lstCharms->ForeColor = Color::Black;
		}

		System::Void btnTrim_Click(System::Object^  sender, System::EventArgs^  e)
		{
			UpdateBestCharms();
			CharmDatabase::mycharms.Clear();
			CharmDatabase::mycharms.AddRange( %best_charms );
			RefreshList( -1 );
			CharmDatabase::SaveCustom();
		}

		System::Void btnSort_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Charm^ selected = nullptr;
			if( lstCharms->SelectedIndex != -1 )
				selected = CharmDatabase::mycharms[ lstCharms->SelectedIndex ];

			if( sender == btnSort1 )
			{
				if( sort_alphabetically  )
					CharmDatabase::mycharms.Sort( gcnew Comparison< Charm^ >( CompareCharms1Alphabetically ) );
				else
					CharmDatabase::mycharms.Sort( gcnew Comparison< Charm^ >( CompareCharms1 ) );
			}
			else if( sender = btnSort2 )
			{
				if( sort_alphabetically  )
					CharmDatabase::mycharms.Sort( gcnew Comparison< Charm^ >( CompareCharms2Alphabetically ) );
				else
					CharmDatabase::mycharms.Sort( gcnew Comparison< Charm^ >( CompareCharms2 ) );
			}

			int new_selected = -1;
			if( selected )
			{
				for( int i = 0; i < CharmDatabase::mycharms.Count; ++i )
				{
					if( CharmDatabase::mycharms[ i ] == selected )
					{
						new_selected = i;
						break;
					}
				}
			}
			RefreshList( new_selected );
			CharmDatabase::SaveCustom();
		}

		System::Void btnImport_Click(System::Object^  sender, System::EventArgs^  e)
		{
			OpenFileDialog dlg;
			dlg.InitialDirectory = System::Environment::CurrentDirectory;
			dlg.Filter = StaticString( SaveDataFile ) + L"|system;system2|" + StaticString( MyCharmList ) + L"|mycharms.txt";
			dlg.FilterIndex = 0;
			System::Windows::Forms::DialogResult res = dlg.ShowDialog();
			
			if( res == System::Windows::Forms::DialogResult::OK )
			{
				ImportCharms ic;
				ic.language = language;
				ic.LoadCharms( dlg.FileName );
				if( ic.failed )
					return;

				res = ic.ShowDialog();
				RefreshList( -1 );
				UpdateBestCharms();
			}
		}

		System::Void btnJap_Click(System::Object^  sender, System::EventArgs^  e)
		{
			SelectJapaneseSkill skill_form;
			if( skill_form.ShowDialog( this ) == System::Windows::Forms::DialogResult::OK &&
				skill_form.selected_ability_index > -1 )
			{
				Ability^ ability = Ability::static_abilities[ skill_form.selected_ability_index ];

				if( sender == btnJap1 )
				{
					if( ability == Ability::FindAbility( (String^)cmbSkills2->SelectedItem ) )
						cmbSkills2->SelectedIndex = -1;

					cmbSkillFilters1->SelectedIndex = 0;
					cmbSkills1->SelectedItem = ability->name;
				}
				else if( sender == btnJap2 )
				{
					if( ability == Ability::FindAbility( (String^)cmbSkills1->SelectedItem ) )
						cmbSkills1->SelectedIndex = -1;

					cmbSkillFilters2->SelectedIndex = 0;
					cmbSkills2->SelectedItem = ability->name;
				}
			}
		}

		System::Void btnReset_Click(System::Object^  sender, System::EventArgs^  e)
		{
			nudSlots->Value = 0;
			cmbAmount1->SelectedIndex = 0;
			cmbAmount2->SelectedIndex = 0;
			cmbSkillFilters1->SelectedIndex = 0;
			cmbSkillFilters2->SelectedIndex = 0;
			cmbSkills1->SelectedIndex = 0;
			cmbSkills2->SelectedIndex = 0;

			cmbAmount1->Focus();
		}
		
};
}
